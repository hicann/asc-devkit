# Matmul AOT Example

## Overview

This example introduces **AOT (Ahead-of-Time) compilation optimization** on top of the Matmul high-level API implementation for matrix multiplication. By making Tiling parameters constant at compile time, the compiler can perform more aggressive optimizations on the kernel (loop unrolling, dead code elimination, constant propagation, and so on). At runtime, the system automatically matches the pre-compiled specialized version based on the actual Tiling, resulting in performance improvements.

The matrix multiplication uses the Matmul high-level API to compute `C = A × B`, where A and B are of type half, C is of type float, and the shape is fixed at `M=512, N=512, K=128`.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR&950DT products | >= CANN 9.1.0 |
| Atlas A3 products | >= CANN 9.0.0 |
| Atlas A2 products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_aot_compilation
│   ├── CMakeLists.txt          // Build project file
│   ├── aot_helper.h            // AOT framework core implementation (Holder, Registry, Dispatcher)
│   └── matmul_aot_example.asc  // Ascend C example implementation (kernel and host invocation)
```

## AOT Principles

### Core Concept

The core concept of AOT compilation is: **move Tiling parameters, which are determined at runtime, to compile time as constants**, enabling the compiler to perform more aggressive optimizations, such as:

- **Loop Unrolling**: Unroll internal Matmul loops based on constant boundaries.
- **Dead Code Elimination**: Remove unnecessary branches based on constant configurations.
- **Constant Propagation**: Evaluate constant expressions at compile time to reduce runtime computation.
- **Improved Instruction Scheduling**: Use constant information to optimize instruction layout for Cube computing units.

### Overall Workflow

```
┌─────────────────────────────────────────────────────────────┐
│                    Compile Time                               │
├─────────────────────────────────────────────────────────────┤
│  Predefined Tiling constant values (byte array format)        │
│    └── AOT_MatmulTiling_512x512x128_Value: M=512,N=512,K=128  │
│                                                              │
│  Register to AOT registry                                    │
│    └── using MatmulTilingAOTRegistry = aot::AOTRegistry<...>   │
│                                                              │
│  Generate specialized + generic versions at compile time      │
│    ├── matmul_custom<AOTHolder<...>> → optimized by constants │
│    └── matmul_custom<RuntimeHolder<...>> → generic runtime    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Runtime                                    │
├─────────────────────────────────────────────────────────────┤
│  1. Host side calls GenerateTiling() to generate runtime      │
│     Tiling data                                               │
│  2. AOTDispatcher compares runtime Tiling against all AOT     │
│     Holders in the registry via memcmp                        │
│  3. Match found → call the corresponding AOT specialized      │
│     kernel version                                            │
│     No match → fall back to RuntimeHolder generic version     │
└─────────────────────────────────────────────────────────────┘
```

## Code Details

### 1. AOT Helper Framework (aot_helper.h)

The AOT framework implementation in this example (`aot_helper.h`) contains the following core components. For details, see the [aot_helper.h](aot_helper.h) source code in the same directory.

| Component | Description |
|-----------|-------------|
| `aot::AOTHolder<T, ValuePtr>` | AOT constant holder: defines compile-time Tiling constants via `constexpr uint8_t[]` byte arrays and carries them in template parameters |
| `aot::RuntimeHolder<T>` | Runtime holder: a placeholder type that marks the generic (non-AOT) version |
| `aot::AOTRegistry<Holders...>` | AOT registry: registers all AOT Holders as a type parameter pack with O(1) compile-time overhead |
| `aot::AOTDispatcher<T, Registry>` | Runtime dispatcher: traverses the registry using C++17 fold expressions and calls the corresponding specialized version after `memcmp` matching |
| `aot::GetHolderDataRef<T, HT>(rt_data)` | Data retrieval helper function: the AOT version returns a compile-time constant reference; the generic version returns a runtime data reference |

### 2. AOT Tiling Constant Definition

```cpp
// Tiling data defined as a compile-time constant in raw byte array format
static constexpr uint8_t AOT_MatmulTiling_512x512x128_Value[] = {
    0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, ...
};
```

This byte array corresponds to the complete memory layout of `TCubeTiling` generated by `MultiCoreMatmulTiling::GetTiling()` for the shape `M=512, N=512, K=128`, including all tiling fields such as usedCoreNum, singleCoreM, singleCoreN, singleCoreKa, singleCoreKb, and so on.

> **How to obtain constant values**: The `GenerateTiling()` function on the host side prints the raw tiling byte data (in `0x%02x` format). Copy the runtime output directly as a `constexpr uint8_t[]` to use it as an AOT constant.

### 3. AOT Registration

```cpp
using MatmulTilingAOTRegistry = aot::AOTRegistry<
    // Multiple instances can be listed here as a comma-separated list
    aot::AOTHolder<AscendC::tiling::TCubeTiling, AOT_MatmulTiling_512x512x128_Value>
>;
```

Currently, the `512×512×128` configuration is registered. To add more pre-compiled configurations, define a new byte array constant and append it to the registry:

```cpp
static constexpr uint8_t AOT_MatmulTiling_1024x1024x256_Value[] = { ... };
using AOT_MatmulTiling_1024x1024x256 = aot::AOTHolder<AscendC::tiling::TCubeTiling, AOT_MatmulTiling_1024x1024x256_Value>;

using MatmulTilingAOTRegistry = aot::AOTRegistry<
    aot::AOTHolder<AscendC::tiling::TCubeTiling, AOT_MatmulTiling_512x512x128_Value>,
    AOT_MatmulTiling_1024x1024x256
>;
```

No changes to the kernel invocation logic are needed. `AOTDispatcher` handles newly added versions automatically.

### 4. Kernel Template Design

```cpp
template <typename TH=aot::RuntimeHolder<AscendC::tiling::TCubeTiling>>
__global__ __cube__ void matmul_custom(
    __gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* c, __gm__ uint8_t* workspace,
    AscendC::tiling::TCubeTiling tilingInput)
{
    // AOT version compile-time check: ensure the value in the Holder is constant at compile time
    if constexpr (!std::is_same_v<TH, aot::RuntimeHolder<AscendC::tiling::TCubeTiling>>) {
        static_assert(TH::value.M != 0);
    }
    // Retrieve Tiling data: the AOT version returns a compile-time constant; the generic version returns the runtime-passed value
    const auto &tiling = aot::GetHolderDataRef<AscendC::tiling::TCubeTiling, TH>(tilingInput);
    // ... subsequent Matmul computation logic uses constants/variables from tiling ...
}
```

Key points:
- The template parameter `TH` defaults to `RuntimeHolder` and is replaced with a specific `AOTHolder` during AOT dispatch.
- `GetHolderDataRef` returns a `constexpr` constant reference directly for the AOT version, enabling the compiler to perform optimizations such as constant propagation and loop unrolling.
- `static_assert` provides a compile-time guard to ensure the value in the AOT Holder is correctly initialized.

### 5. Runtime Dispatch

```cpp
auto tilPtr = reinterpret_cast<const uint8_t*>(&tiling);
aot::AOTDispatcher<AscendC::tiling::TCubeTiling, MatmulTilingAOTRegistry>::template dispatch(
    tilPtr,                             // runtime Tiling byte pointer
    [&](auto tiling_holder, AscendC::tiling::TCubeTiling value) {
        using Holder = decltype(tiling_holder);
        matmul_custom<Holder><<<numBlocks, 0, stream>>>(aDevice, bDevice, cDevice, workspaceDevice, tiling);
        if constexpr (std::is_same_v<Holder, aot::RuntimeHolder<AscendC::tiling::TCubeTiling>>) {
            printf("##### Using runtime Tiling\n");
        } else {
            static_assert(Holder::value.M != 0);
            printf("##### AOT matched! M=%u, N=%u, Ka=%u, Kb=%u\n",
                   Holder::value.M, Holder::value.N, Holder::value.Ka, Holder::value.Kb);
        }
    }
);
```

Dispatch workflow:
1. `AOTDispatcher::dispatch()` receives the runtime Tiling byte pointer.
2. Using C++17 fold expressions, it performs byte-by-byte `memcmp` comparison against each `AOTHolder` in the registry.
3. `||` short-circuit semantics: stops at the first match; if no match is found, calls the `RuntimeHolder` generic version.
4. Each `try_one` is annotated with `__attribute__((always_inline))` to force inlining, avoiding N separate function bodies for N Holders.

## Example Description

### Matrix Multiplication (Brief)

This example uses the Ascend C Matmul high-level API to implement matrix multiplication `C = A × B`. Matrix A has shape `[M, K]`, matrix B has shape `[K, N]`, and output matrix C has shape `[M, N]`. The Matmul high-level API encapsulates details such as data transfer, Cube computation scheduling, and basic pipeline synchronization. The developer primarily handles matrix shape configuration, tiling generation, input/output tensor setup, and result write-back.

- Example shape:
  The parameters for this example are `M = 512, N = 512, K = 128`. Input matrices A and B are of type `half` with `ND` format, and output matrix C is of type `float` with `ND` format. The input/output specifications are shown in the following table:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_custom</td></tr>
  </table>

  This example is a pure Cube matrix computation scenario that generates tiling for a fixed 2 Cube cores. Under the shape used in this example, the tiling result divides `M = 512` evenly across 2 cores, with each core processing `singleCoreM = 256`, `singleCoreN = 512`, and `singleCoreKa = singleCoreKb = 128`.

### AOT Optimization Effect

When the runtime Tiling parameters match the `512×512×128` configuration in the AOT registry, the compiler has already completed the following at compile time:

- **Loop unrolling optimization**: The internal Matmul block loops are fully unrolled based on the constant singleCoreM/N/Ka/Kb values.
- **Constant propagation**: Address offset expressions such as `GetBlockIdx() * tiling.singleCoreM * tiling.Ka` can be computed at compile time.
- **Dead code elimination**: Conditional branches unrelated to the constant configuration are removed.
- **Instruction scheduling optimization**: The compiler can produce better instruction layouts based on the known block sizes.

Runtime output example:

```
##### AOT matched! M=512, N=512, Ka=128, Kb=128
```

If the runtime Tiling parameters change (for example, the K value is modified), the system automatically falls back to the generic version:

```
##### Using runtime Tiling
```

## Build and Run

Run the following steps in the root directory of this example to build and run it.

- Configure environment variables
  Configure environment variables based on the installation method of the CANN development kit on the current environment.
  ```bash
    source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
    mkdir -p build && cd build;                                               # Create and enter the build directory
    cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build the project (NPU mode by default)
    ./matmul_aot_example                                                      # Run the example
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
    cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clean the CMake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description
  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 products and Atlas A3 products; dav-3510 corresponds to Ascend 950PR&950DT products |

- Execution results
  After successful execution, the AOT matching status and the first 16 elements of the output matrix are printed. If the Tiling matches the AOT registry, the output is:
  ```bash
    ##### AOT matched! M=512, N=512, Ka=128, Kb=128
    =============== output c matrix[:16] ===============
    128.00 128.00 128.00 128.00 128.00 128.00 128.00 128.00 ...
  ```

## Extending with More AOT Configurations

To use AOT optimization for other matrix shapes, follow these steps:

1. **Obtain the Tiling byte data for the target shape**: Modify the M/N/K parameters in `GenerateTiling()`, run the example once, and locate the byte array in the `matmul raw tiling data` section of the output.
2. **Define the AOT constant**: Define the byte array as a `static constexpr uint8_t[]`.
3. **Create the AOT Holder**: `using AOT_MatmulTiling_<shape> = aot::AOTHolder<AscendC::tiling::TCubeTiling, <constant_name>>`.
4. **Update the registry**: Append the new Holder to the type parameter pack of `MatmulTilingAOTRegistry`.

## Notes

1. **Tiling byte compatibility**: The byte layout of `TCubeTiling` may differ across CANN versions or architectures. Ensure that the AOT constants match the current build environment.
2. **Compilation time**: Each AOT specialized version increases compilation time and binary size (the kernel side grows linearly with N and is more sensitive than the host dispatcher). It is recommended to pre-compile only frequently used configurations.
3. **Runtime overhead**: The matching process uses `memcmp` for sequential comparison. A registry with up to approximately 1000 Holders still operates at microsecond-level overhead.
4. **Static compile-time validation**: The AOT version uses `static_assert(Holder::value.M != 0)` to ensure constants are correctly initialized.
5. **Pure Cube scenario**: This example uses the `ASCENDC_CUBE_ONLY` macro, and the kernel runs on the `__cube__` computing unit. It is only applicable to pure matrix multiplication scenarios.
