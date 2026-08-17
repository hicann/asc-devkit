# Separate Compile Example

## Overview

This example demonstrates, based on the Add operator, the separate compilation flow that splits Device code from a Host + Device mixed `.asc` file into **multiple `.asc` files**. The code structure and call relationship of separate compilation are shown in Figure 1. Specifically, `add_compute.asc` is the **Device-side execution function** file (defines the `add_compute` function and exposes it through `extern` for cross-file calls), `add_kernel.asc` is the **Host + Device mixed file** (contains Kernel definitions and `<<<>>>` kernel launch calls, and calls the Device-side execution function in `add_compute.asc` through `extern`), and `main.cpp` is the pure Host-side code (calls the `<<<>>>` kernel launch function exposed by `add_kernel.asc` through `extern`).

<p align="center">
  <img src="./figures/separate_compile.png" width="50%">
</p>
<p align="center">
Figure 1: Code structure and call relationship
</p>

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── 01_separate_compile
│   ├── figures              // Illustrations
│   ├── add_compute.asc      // Compute logic function definition
│   ├── add_kernel.asc       // Mixed file (containing Kernel definitions and <<<>>> kernel launch calls)
│   ├── CMakeLists.txt       // Build project file
│   ├── main.cpp             // Pure Host-side code (calls functions exposed by .asc through extern)
│   └── README.md            // Example documentation
```

## Operator Description

The Add operator implements element-wise addition of two vectors. The input vector length is 16384, and the output vector length is 16384.

## Separate Compilation Introduction

bisheng uses **whole-program compilation** mode by default, which requires that the device program compiled from a single source file `X.asc` has no unresolved external device function or variable references. When you need to make cross-file device code calls among multiple `.asc` files, use the **`-dc` option** to enable **separate compilation** mode.

The following should be noted during separate compilation:
- Non-constant device variables defined in one compilation unit must be declared with the `extern` keyword when referenced in other compilation units.
- All constant device variables must use the `extern` keyword for both definition and cross-unit reference.
- All Ascend C source files (`.asc`) must be compiled with the `-dc` option.

### bisheng Command-Line Compilation

This example involves cross-file device function calls. Use the `-dc` option to compile each `.asc` file and the Host-side `.cpp` into `.o` object files respectively, then link them to generate the executable binary. The compilation commands are as follows:

```shell
bisheng -dc add_compute.asc -o add_compute.o --npu-arch=dav-2201
bisheng -dc add_kernel.asc -o add_kernel.o --npu-arch=dav-2201
bisheng -c main.cpp -o main.o -I${ASCEND_HOME_PATH}/include
bisheng add_compute.o add_kernel.o main.o -o demo
```

Where `--npu-arch` specifies the AI processor architecture at compile time. This example supports `dav-2201` and `dav-3510`. For the architecture version number corresponding to each product model, refer to the [mapping table](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

For more bisheng compilation options and usage, refer to [Basic Usage of AI Core Operator Compilation](../../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md).

### CMake Compilation

CMake can be used in a project to more conveniently compile Ascend C SIMD operators using the BiSheng compiler and generate executable binaries. The core steps are as follows:

```cmake
# 1. Set compilation configuration variables
# CMAKE_ASC_ARCHITECTURES: specifies the NPU architecture version. This example supports dav-2201 and dav-3510,
#    configurable through -DCMAKE_ASC_ARCHITECTURES=dav-xxxx
set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture")

# 2. find_package(ASC) is the CMake command used to find and configure the Ascend C compilation toolchain
find_package(ASC)

# 3. Specify that the project supports ASC and CXX languages
project(kernel LANGUAGES ASC CXX)

# 4. Use CMake interface to compile the executable file
add_executable(demo
    add_compute.asc
    add_kernel.asc
    main.cpp)

# 5. Set the linker language to ASC
set_target_properties(demo PROPERTIES LINKER_LANGUAGE ASC)

# 6. Configure compilation options: -dc enables separate compilation, --npu-arch specifies the architecture version
target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:-dc --npu-arch=${CMAKE_ASC_ARCHITECTURES}>)
```

## Build and Run

Run the following steps in the root directory of this example to build and run the operator.

- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  ./demo                           # Run the compiled executable to execute the example
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products/Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
