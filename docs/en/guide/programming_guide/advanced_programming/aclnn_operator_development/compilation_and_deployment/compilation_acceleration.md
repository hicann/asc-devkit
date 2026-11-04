# Compilation Acceleration

<!-- md-trans-meta sourceCommit=fbd15d5df1f452aabb0f40f2b83dd25017ebaa8f translatedAt=2026-10-23T13:37:59.795Z pushedAt=2026-11-03T07:05:13.396Z nl=2 -->

This document is an extension that describes how to reduce the compilation time of an operator project. The compilation time of an operator project mainly comes from the per-chip and per-TilingKey compilation of the kernel function code. This document provides acceleration strategies from four dimensions: cache reuse, selective compilation, parallelism, and incremental build.

## ccache Compilation Cache

ccache is a compilation cache tool. During the first compilation, it writes the compilation results to the cache. In subsequent compilations, if the inputs such as the source code, compilation commands, and compiler remain identical, the cache can be reused to reduce repeated compilation time.

In an operator project, ccache can be configured through the CMake variable `CMAKE_CXX_COMPILER_LAUNCHER`. This variable applies to Host-side C++ compilation. In a CANN operator project, the CMake module also passes the ccache path to the Kernel-side compilation script for the Ascend C Kernel compilation process.

### Standalone ccache

The commands for installing ccache and finding its path are as follows:
```bash
apt install ccache
command -v ccache
ccache --version
```

In subsequent configurations, the ccache path is determined by the output of `command -v ccache`. The command for clearing ccache is as follows:
```bash
ccache -z          # Clear only the statistics, without deleting the cache content
ccache -C          # Clear only the cache content, without clearing the statistics
ccache -Cz         # Clear both the cache content and the statistics
```

#### Enabling Method

**Temporary Enabling via Command Line**

During development and debugging, it is recommended to enable ccache via the command line at the CMake configuration stage, without modifying the project files:

```bash
cmake -S . -B build_out --preset=default \
    -DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)
```

This command writes the ccache configuration into `build_out/CMakeCache.txt`. When you continue to use the same `build_out` build directory later, this configuration is retained.

**Writing into the Top-Level CMakeLists.txt**

If you want the project to enable ccache by default in the long term, you can also write it into the top-level `CMakeLists.txt` of the operator project, placing it before the compilation target is created, for example:

```cmake
cmake_minimum_required(VERSION 3.19.0)
project(opp)

set(CMAKE_CXX_COMPILER_LAUNCHER /usr/bin/ccache)
```

If ccache is installed in another path, replace `/usr/bin/ccache` with the actual path found by `command -v ccache`.

#### Verifying Whether the Cache Takes Effect

Note the following when verifying: the first build usually writes to the cache, so the hit rate is low; cache hits should only be observed when a recompilation is triggered the second time.

**Step 1: Clear the cache and statistics, and perform the first build.**

```bash
ccache -Cz
cmake -S . -B build_out --preset=default -DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)
cmake --build build_out --target binary package -j$(nproc)
ccache --show-stats -v
```

After the first build, `Misses` is usually high, which is normal and indicates that the cache is being written.

**Step 2: Keep the cache, clear only the statistics, and trigger a second compilation.**

If the source code is unchanged and you directly run `cmake --build` again, CMake may determine that recompilation is unnecessary, in which case the ccache statistics will not change significantly. Therefore, when verifying the hit rate, you can first run the `clean` target to clean up the build artifacts, and then rebuild:

```bash
ccache -z
cmake --build build_out --target clean
cmake --build build_out --target binary package -j$(nproc)
ccache --show-stats -v
```

After the second build, `Hits` or `Hits` under `Primary storage` should increase significantly. If there are still no hits, check the following first:

- Whether `-DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)` was passed during the configure phase.
- Whether the compile commands in the build directory include ccache. You can search `build.make` under `build_out`.
- Whether the source code, compiler, and compile options are identical between the two compilations.

For more `ccache` configuration and cache behavior details, see the [ccache official documentation](https://ccache.dev/documentation.html).

### Distributed Cache (ccache + Redis)

This applies to the multi-machine shared cache scenario: after machine A compiles, it pushes the results to Redis. Machine B can hit the cache from Redis under the same source code, the same compilation options, and the same toolchain version, reducing repeated compilation.

In the distributed scenario, `ccache` uses the local cache of each machine as the level-1 cache and Redis as the shared level-2 cache. When machine A compiles for the first time, it invokes the actual compiler and writes the results to both the local cache and Redis. When machine B compiles again under the same source code, the same compilation command, the same compiler, and the same build directory path, it can directly hit the shared cache from Redis, reducing repeated compilation actions. If the two machines have different compiler paths but identical compiler content, it is recommended to set `compiler_check=content`.


Networking requirements:

- Machine A: the machine that compiles first and writes the cache to Redis, with IP `<A_IP>`
- Machine B: the machine that compiles second and verifies the shared cache hit from Redis, with IP `<B_IP>`
- Machine C: the Redis server that stores the shared cache data, with IP `<C_IP>`
- Machines A, B, and C must be on the same network, and both machines A and B must be able to access `C_IP:6379`

It is recommended that machines A and B use the same source code content, the same compilation command, and the same compiler version, and keep consistent source code paths and build directory paths. Otherwise, cache misses may occur.

**1. Machine C: Deploy the Redis Service**
```bash
apt install redis-server
# Start the Redis service
redis-server --daemonize yes --bind 0.0.0.0 --port 6379 --requirepass <PASSWORD>
# Verify the Redis connection
redis-cli -h <C_IP> -p 6379 -a <PASSWORD> ping
```

> **Note:** The preceding configuration is intended only for a controlled test environment. For shared or production environments, enable access control, authentication, and network isolation.

**2. Machine A / Machine B: Configure ccache**
```bash
apt install redis-tools
# Verify the Redis connection
redis-cli -h <C_IP> -p 6379 -a <PASSWORD> ping
# Configure Redis as secondary storage, with password authentication format
# Format: redis://default:<PASSWORD>@<C_IP>:6379
ccache --set-config=secondary_storage=redis://default:<PASSWORD>@<C_IP>:6379
# Configure compiler content verification to avoid cache misses caused by path differences
ccache --set-config=compiler_check=content
```

**3. Machine A / Machine B: Run the build**

The build on Machine A writes to both the local cache and Redis:

```bash
ccache -Cz
cmake -S . -B build_out --preset=default -DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)
cmake --build build_out --target binary package -j$(nproc)
ccache --show-stats -v
```

The local cache on machine B is empty, so it can read the shared cache from Redis. After machine A completes the build, run the build on machine B:

```bash
ccache -z
cmake --build build_out --target clean
cmake --build build_out --target binary package -j$(nproc)
ccache --show-stats -v
```

By comparing the results of the two `ccache --show-stats -v` runs, you can see that the remote cache hit rate `Remote storage Hits` on machine B increases significantly.


<a id="selective-compilation"></a>
## Selective Compilation

The core idea of selective compilation is to compile only the parts that actually need to be verified during the development and debugging phase, reducing the compilation overhead caused by irrelevant kernel function variants, irrelevant chips, and irrelevant artifacts. Common strategies are as follows:

| Strategy | Application Scenario | Configuration Method | Effect |
|------|---------|---------|------|
| `--tiling_key=1,2` | TilingKey programming operators, for debugging only specific branches. | The OPTIONS parameter of `npu_op_kernel_options`. | Compiles only the kernel functions (Kernel) of the specified TilingKey and skips the rest. |
| `--kernel-template-input="D_T_X=float"` | Template programming operators, for debugging only specific template parameter combinations. | The OPTIONS parameter of `npu_op_kernel_options`. | Compiles only the kernel functions (Kernel) of the specified template parameter combination. |
| Narrowing `ASCEND_SOC_SERIES` or `ASCEND_COMPUTE_UNIT` | Running only the current SOC during the development phase. | Modify `CMakePresets.json` or the CMake configuration parameters; choose either one. | Skips the compilation of kernel functions (Kernel) for other chips. |
| `ENABLE_BINARY_PACKAGE=False` | Source code release mode, where kernel function binaries are not required. | CMakeLists.txt or cmake parameters. | Skips the generation of kernel function `.o` files while retaining the kernel function source code and necessary configuration. |

### TilingKey Selective Compilation

Compile only the kernel function code related to the specified [TilingKey](../design_and_implementation/multi_branch_strategy.md#compile-selected-tiling-keys) to accelerate the compilation process. If no TilingKey is specified for compilation, all TilingKeys are compiled by default. When configuring multiple TilingKeys, no spaces are allowed between them. The example is as follows, where 1 and 2 are TilingKey values:

```bash
# Add the following to op_kernel/CMakeLists.txt
npu_op_kernel_options(ascendc_kernels AddCustom COMPUTE_UNIT Ascendxxyy OPTIONS --tiling_key=1,2)
```

### Template Parameter Selective Compilation

For multi-branch and templated operators, you can compile only the specified template parameter combinations. After the `--kernel-template-input` option is set, only the kernel function code related to the specified template parameter combinations is compiled, which accelerates the compilation process. If this option is not set, all template parameter combinations are compiled by default. The input parameter is a list of key-value pairs, and the whole list must be enclosed in double or single quotation marks. Different template parameters are separated by semicolons (;), and multiple values of the same template parameter are separated by commas (,). No spaces are allowed in the configuration. An example is as follows:

```bash
npu_op_kernel_options(ascendc_kernels AddCustomTemplate COMPUTE_UNIT Ascendxxyy OPTIONS --kernel-template-input="D_T_X=float;D_T_Y=float;D_T_Z=float")
```

When configuring a template parameter combination, the template parameter names must match the template parameter names defined at the kernel function entry and on the host side. For the values of the template parameter combination, if a custom type exists, it must be replaced with its corresponding numeric value; if it is a natively supported data type, it must be consistent with the input parameter at the kernel function entry. An example is as follows:

```CPP
// Template parameter definition on the host side.
#define ADD_TPL_FP16 10
#define ADD_TPL_FP32 20

ASCENDC_TPL_ARGS_DECL(AddCustomTemplateNativeDtype,
    ASCENDC_TPL_DATATYPE_DECL(D_T_X, C_DT_FLOAT, C_DT_FLOAT16, ASCENDC_TPL_INPUT(0)),
    ASCENDC_TPL_DTYPE_DECL(D_T_Y, ADD_TPL_FP16, ADD_TPL_FP32),
    ASCENDC_TPL_DATATYPE_DECL(D_T_Z, C_DT_FLOAT, C_DT_FLOAT16, ASCENDC_TPL_OUTPUT(0)),
    ASCENDC_TPL_UINT_DECL(TILE_NUM, ASCENDC_TPL_8_BW, ASCENDC_TPL_UI_MIX, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8),
    ASCENDC_TPL_BOOL_DECL(IS_SPLIT, 0, 1),
);

ASCENDC_TPL_SEL(
    ASCENDC_TPL_ARGS_SEL(
    ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT),
    ASCENDC_TPL_DTYPE_SEL(D_T_Y, ADD_TPL_FP32),
    ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT),
    ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
    ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
    ASCENDC_TPL_DETERMINISTIC_SEL(true),
    ASCENDC_TPL_KERNEL_TYPE_SEL(ASCENDC_TPL_AIV_ONLY),
    ),
    ASCENDC_TPL_ARGS_SEL(
    ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT16),
    ASCENDC_TPL_DTYPE_SEL(D_T_Y, ADD_TPL_FP16),
    ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT16),
    ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
    ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
    ASCENDC_TPL_DETERMINISTIC_SEL(false),
    ASCENDC_TPL_KERNEL_TYPE_SEL(ASCENDC_TPL_AIV_ONLY),
    ),
);
#endif

// Kernel function entry. The template parameter names must be consistent with the template parameter definition on the host side.
template <typename D_T_X, typename D_T_Y, typename D_T_Z, uint32_t TILE_NUM, bool IS_SPLIT>
__global__ __aicore__ void add_custom_template(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                               GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tiling_data, tiling);

    if constexpr (std::is_same_v<D_T_X, float> && std::is_same_v<D_T_Z, float>) {
        KernelAdd<D_T_X, float, D_T_Z> op;
        op.Init(x, y, z, tiling_data.totalLength, TILE_NUM);
        op.Process1();
    } else if constexpr (std::is_same_v<D_T_X, half> && std::is_same_v<D_T_Z, half>) {
        KernelAdd<D_T_X, half, D_T_Z> op;
        op.Init(x, y, z, tiling_data.totalLength, TILE_NUM);
        op.Process2();
    }
}
```

A compilation option example is as follows:

```bash
npu_op_kernel_options(ascendc_kernels AddCustomTemplate COMPUTE_UNIT Ascendxxyy OPTIONS --kernel-template-input="D_T_X=half;D_T_Y=10;D_T_Z=half")
```

### Narrowing the Chip Range

The number of chip models configured by `ASCEND_SOC_SERIES` or `ASCEND_COMPUTE_UNIT` determines how many chip models the kernel function is compiled for. During the development phase, if you only verify on the current machine, you can modify either configuration in CMakeLists.txt to keep only the current SOC, or temporarily build only a specific SOC during the CMake configuration phase. The following example builds only the ascendxxx SOC:

```bash
cmake -S . -B build_out --preset=default -DASCEND_SOC_SERIES=ascendxxx
```
You can also use the parameter `-DASCEND_COMPUTE_UNIT=ascendxxxyy`; the two parameters cannot be passed at the same time.
### Skip Kernel Function Binary Compilation

If you only need to generate a source package, or do not need the kernel function `.o` binaries for the time being, you can disable binary package generation. In the top-level `CMakeLists.txt`, `ENABLE_BINARY_PACKAGE` should be passed in variable form:

```cmake
npu_op_package(${package_name}
    TYPE RUN
    CONFIG
        ENABLE_SOURCE_PACKAGE ${ENABLE_SOURCE_PACKAGE}
        ENABLE_BINARY_PACKAGE ${ENABLE_BINARY_PACKAGE}
        INSTALL_PATH ${CMAKE_BINARY_DIR}/
)
```

Then disable binary compilation during the configure phase:

```bash
cmake -S . -B build_out --preset=default -DENABLE_BINARY_PACKAGE=False
cmake --build build_out --target binary package -j$(nproc)
```

In this mode, the Host-side library, Tiling library, and configuration file generation are still performed, but the `--enable-binary` passed to the kernel function compilation script is `False`, so no kernel function `.o` binary files are generated. The kernel function source code and other files are retained under `op_kernel/ascendc_kernels/binary/dynamic/`.

> [!NOTE] Note
>
> If the top-level `CMakeLists.txt` hardcodes `ENABLE_BINARY_PACKAGE True` in `npu_op_package(... CONFIG ...)`, the `-DENABLE_BINARY_PACKAGE=False` passed on the command line will not take effect. To allow command-line override, change it to the `${ENABLE_BINARY_PACKAGE}` variable form, or do not override this variable in CONFIG.

## Parallelism Control

`cmake --build ... -j` controls the number of parallel tasks in the build phase. Reasonably increasing the parallelism can shorten the compilation time, but an excessively high thread count causes contention for CPU, memory, and I/O resources. In severe cases, it may even slow down the build or trigger a compilation failure.

**Suggestions for selecting the thread count**:

- In general scenarios, you can first use `-j$(nproc)` to let the build system run in parallel based on the number of available CPU cores.
- If the machine has limited memory, or if the kernel function compilation phase shows obvious stalling or an excessively high system load, change it to `nproc / 2` or a fixed smaller value.

```bash
cmake --build build_out --target binary package -j$(nproc)
cmake --build build_out --target binary package -j8
```

## Incremental Compilation

Incremental compilation means that you do not delete `build_out` and continue to use the same build directory. CMake reuses the build rules, intermediate files, and some compiled artifacts generated by the previous configure. This avoids starting from scratch for every build and speeds up the build and compilation process.

For the first build, you need to run configure first and then build:

```bash
cmake -S . -B build_out --preset=default
cmake --build build_out --target binary package -j$(nproc)
```

**Incremental compilation**: If you only modify existing source files afterward, you usually only need to run build again. For targets whose dependencies can be tracked by CMake, such as host-side C++ files and Tiling implementations, the corresponding targets are generally recompiled on demand based on file changes.

```bash
cmake --build build_out --target binary package -j$(nproc)
```

**Full compilation**: However, if you modify the project structure, CMake configuration, or content that affects code generation, such as modifying the operator prototype, modifying the CMakeLists.txt configuration, or adding/deleting operator files, you need to run configure again first and then build:

```bash
cmake -S . -B build_out --preset=default
cmake --build build_out --target binary package -j$(nproc)
```

Most minor source code changes do not require clearing `build_out`. Only when you suspect that the build directory contains residual error states do you need to recreate a clean build directory:

```bash
rm -rf build_out
cmake -S . -B build_out --preset=default
cmake --build build_out --target binary package -j$(nproc)
```

**Incremental compilation and ccache** can be used together. Incremental compilation reduces the targets that need to be reprocessed. When some files are still triggered for recompilation, ccache can reuse the cache when the source code, compilation command, and compiler are identical:

```bash
cmake -S . -B build_out --preset=default -DCMAKE_CXX_COMPILER_LAUNCHER=$(command -v ccache)
cmake --build build_out --target binary package -j$(nproc)
```

When you continue to use the same `build_out`, `CMAKE_CXX_COMPILER_LAUNCHER` is saved in `build_out/CMakeCache.txt`, so you do not need to pass it again for every build.
