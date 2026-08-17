# Separate Compile Example

## Overview

This example, based on the Gather operator, demonstrates the separate compilation flow where Device code in a Host + Device mixed .asc file is split into **multiple .asc files**. The separate compilation code structure and call relationship are shown in Figure 1 below, where `gather_compute.asc` is the **Device-side execution function** (defines the `gather_element` function and exposes it through extern for cross-file invocation), `gather_kernel.asc` is the **Host + Device mixed file** (containing the Kernel definition and <<<>>> kernel invocation, and calling the Device-side execution function in gather_compute.asc through extern), and `main.cpp` is pure Host-side code (calling the <<<>>> kernel launch function exposed by gather_kernel.asc through extern).

<p align="center">
  <img src="./figures/separate_compile.png" width="50%">
   </p>
<p align="center">
Figure 1: Code structure and call relationship
</p>

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```
├── 01_separate_compile
│   ├── CMakeLists.txt       // Build project file
│   ├── gather_compute.asc   // Computation logic function definition
│   ├── gather_kernel.asc    // Mixed file (containing Kernel definition, <<<>>> kernel invocation)
│   ├── main.cpp             // Pure Host-side code (calls functions exposed by .asc through extern)
│   └── figures              // Image resources for README
```

## Operator Description

The gather operator implements the function of obtaining 12288 data elements at specified indices from a 1D input vector of length 100000.

## Separate Compilation Introduction

bisheng uses **whole program compilation** mode by default, which requires that the device program compiled in a single source file `X.asc` has no unresolved external device function and variable references. When cross-file device code invocation between multiple `.asc` files is needed, the **`-dc` option** must be used to enable **separate compilation** mode.

Separate compilation has the following considerations:
- Non-constant device variables defined in one compilation unit must be declared with the `extern` keyword when referenced in other compilation units;
- All constant device variables must use the `extern` keyword for both definition and cross-unit references;
- All Ascend C source files (`.asc`) must be compiled with the `-dc` option.

### bisheng Command Line Compilation

This example has cross-file device function calls and requires the `-dc` option to compile each `.asc` file and Host-side `.cpp` into `.o` object files separately, then link to generate the executable binary. The specific compilation commands are as follows:

```shell
bisheng -dc gather_compute.asc -o gather_compute.o --npu-arch=dav-3510 --enable-simt
bisheng -dc gather_kernel.asc -o gather_kernel.o --npu-arch=dav-3510 --enable-simt
bisheng -c main.cpp -o main.o -I${ASCEND_HOME_PATH}/include
bisheng gather_compute.o gather_kernel.o main.o -o demo
```

Where `--npu-arch` specifies the AI processor architecture at compile time. This example only supports `dav-3510`. For the architecture version corresponding to each product model, refer to the [correspondence table](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch). `--enable-simt` is used to specify SIMT compilation mode.

For more bisheng compilation options and usage, refer to [AI-Core Operator Compilation Basic Usage](../../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md).

### CMake Compilation

In a project, CMake can be used to more conveniently compile Ascend C SIMT operators using the Bisheng compiler and generate executable binaries. The core steps are as follows:

```cmake
# 1. Set compilation configuration variables
# CMAKE_ASC_ARCHITECTURES: Specify NPU architecture version, this example only supports dav-3510, configurable via -DCMAKE_ASC_ARCHITECTURES=dav-xxxx
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture")

# 2. find_package(ASC) is the CMake command for finding and configuring the Ascend C compilation toolchain
find_package(ASC)

# 3. Specify that the project supports ASC and CXX languages
project(kernel LANGUAGES ASC CXX)

# 4. Use CMake interface to compile the executable file
add_executable(demo
    gather_compute.asc
    gather_kernel.asc
    main.cpp)

# 5. Set linker language to ASC
set_target_properties(demo PROPERTIES LINKER_LANGUAGE ASC)

# 6. Configure compilation options: -dc enables separate compilation, --npu-arch specifies architecture version, --enable-simt enables SIMT mode
target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:-dc --npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>)
```

In future versions, SIMT mode can be enabled by setting `CMAKE_ASC_ENABLE_SIMT` to ON instead of the `--enable-simt` compilation option in step 6 above:

```cmake
# Enable SIMT compilation mode
set(CMAKE_ASC_ENABLE_SIMT ON)
```

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  - Command Line Method

    ```bash
    bisheng -dc gather_compute.asc -o gather_compute.o --npu-arch=dav-3510 --enable-simt   # Compile computation function .asc to object file
    bisheng -dc gather_kernel.asc -o gather_kernel.o --npu-arch=dav-3510 --enable-simt     # Compile Kernel mixed .asc to object file
    bisheng -c main.cpp -o main.o -I${ASCEND_HOME_PATH}/include                            # Compile Host-side .cpp to object file
    bisheng gather_compute.o gather_kernel.o main.o -o demo                                # Link to generate executable file
    ./demo                                                                                 # Run the example
    ```

  - CMake Method

    ```bash
    mkdir -p build && cd build;                                     # Create and enter the build directory
    cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;            # Build the project
    ./demo                                                          # Run the example
    ```

    When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter. Example:

    ```bash
    cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
    ```

    > **Notice:** Before switching build modes, clean the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Execution Result

  The following output indicates that the accuracy verification is successful.
  ```
  [Success] Case accuracy is verification passed.
  ```
