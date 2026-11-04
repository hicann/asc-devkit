# AOT Compilation Optimization

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:28:28.981Z pushedAt=2026-11-03T07:05:13.389Z nl=2 -->

## Overview

AOT (Ahead-of-Time) compilation optimization is a performance optimization technique that moves runtime parameters to the compilation stage as constants. By performing additional constant-based compilation on the kernel function during the compilation stage, and automatically matching the precompiled specialized version based on Tiling parameters at runtime, the compiler is able to perform deeper optimizations (such as loop unrolling, dead code elimination, and constant propagation), thereby achieving execution performance that surpasses the generic version.

## Core Principles

In Ascend C operator development, Tiling parameters (such as cube block size, operation type, etc.) are usually computed by the Host side at runtime and passed into the kernel function. The generic kernel function uses these parameters as variables, which prevents deep optimization.

The core idea of AOT is: **fix a specific Tiling configuration into the template code as compile-time constants**, so that the compiler can see these values at the compilation stage and further achieve:

- **Loop unrolling**: fully unroll inner loops based on constant boundaries, eliminating loop control overhead.
- **Constant propagation**: compute all expressions that depend on Tiling constants at compile time, reducing scalar computation at runtime.
- **Dead code elimination**: prune conditional branches that will never execute based on constant operation types and block sizes, reducing the amount of executed code and lowering the ICache miss rate.

## Overall Process

### Kernel Constant Precompilation

- Write a Tiling constant template definition based on the typical shape and input of the operator.

  ```cpp
  struct TilingData;
  template <typename T>
  struct ConstTilingHolder { // Constant struct that holds a specific Tiling value
      static constexpr const uint8_t bytes[] = {0, 1, 2, 3, ...};  // Constant specialized Tiling value (array)
      static constexpr const T value = convert_from_bytes<T>(bytes); // Convert to a constant struct at compile time
  };
  template <typename T>
  struct RuntimeTilingHolder { // Runtime stub struct used to distinguish constant Tiling
      static constexpr const uint8_t bytes[sizeof(T)] = {};  // Data stub implementation; this value is meaningless
      static constexpr const T value = convert_from_bytes<T>(bytes);
  };
  ```

- Kernel implementation: code that unifies the constant Tiling and runtime Tiling variables

  ```cpp
  // Expose a template type for the Host to provide a constant Tiling value or a runtime stub in different scenarios
  template <typename TH>
  __global__ __vector__ void add(__gm__ uint8_t* x1, __gm__ uint8_t* x2, TilingData tiling) {
      const TilingData *td;
      if constexpr(!std::is_same_v<TH, RuntimeTilingHolder<TilingData>>) {
          td = &TH::value; // If a constant Tiling value is provided, obtain the constant value directly and pass it down
      } else {
          td = &tiling;  // If a runtime stub is obtained, get it from the parameter.
      }
  }
  ```

### Host Selects a Specific Kernel Function at Runtime

Add constant Tiling matching and invocation to the code where the Host calls the kernel function. The compiler performs mixed compilation of the specified constant Tiling template and the kernel function to instantiate the constant kernel function.

```cpp
// Match the constant Tiling based on the Tiling result computed at runtime.
if (memcmp(&tiling_data, ConstTilingHolder<TilingData>::bytes, sizeof(tiling_data)) == 0) {
    // After a match is found, invoke the kernel function implementation with constant Tiling.
    add<ConstTilingHolder<TilingData>><<<1, 0, stream>>>(x1, x2, tiling_data);
} else {
    // If no constant Tiling can be matched, fall back to the runtime Tiling result.
    add<RuntimeTilingHolder<TilingData>><<<1, 0, stream>>>(x1, x2, tiling_data);
}
```

### Complete Example

For a complete Matmul example of AOT compilation optimization, see [matmul_aot_compilation](../../../../../examples/01_simd_cpp_api/02_features/06_aot_compilation/matmul_aot_compilation). This example demonstrates the complete process of implementing AOT compilation in a cube-only scenario, and extracts the template method into a simple API.


## Precautions

1. **Tiling byte compatibility**: If the operator modifies the Tiling structure, ensure that the defined constants are consistent with the current Tiling structure definition.

2. **Compilation time and binary size**: Each AOT specialization increases compilation time and binary size. It is recommended to precompile AOT versions only for frequently used configurations to avoid over-specialization.

3. **Runtime overhead**: The matching process for constant Tiling adds extra Host-side overhead. Developers should adjust accordingly based on the operator usage scenarios.
