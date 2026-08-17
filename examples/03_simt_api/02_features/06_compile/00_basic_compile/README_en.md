# Basic Compile Example

## Overview

This example, based on the Gather operator, demonstrates the basic compilation flow where a Host-side .cpp file calls a Host + Device mixed .asc file (containing Kernel definition and <<<>>> kernel invocation). The .asc file contains a Host-side entry function with <<<>>> kernel function invocation exposed to the pure Host-side .cpp through extern. The code structure and call relationship are shown in Figure 1:

<p align="center">
  <img src="./figures/basic_compile.png" width="50%">
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
├── 00_basic_compile
│   ├── CMakeLists.txt      // Build project file
│   ├── gather_kernel.asc   // Mixed file (containing Kernel definition, <<<>>> kernel invocation)
│   ├── main.cpp            // Pure Host-side code (calls functions exposed by .asc through extern)
│   └── figures             // Image resources for README
```

## Operator Description
The gather operator implements the function of obtaining 12288 data elements at specified indices from a 1D input vector of length 100000.

## Heterogeneous Compilation Introduction

This example uses **heterogeneous compilation**, where the Host-side `.cpp` and Device-side `.asc` are compiled separately and then linked. The basic compilation flow for AI Core SIMT is as follows: Host code is compiled into Host binary using the Host compiler, AI Core SIMT code is compiled into SIMT binary by the Device compiler, the SIMT binary is first linked into a Fatbin file, and then merged with the Host binary to generate the executable binary.

### bisheng Command Line Compilation

Basic flow: First compile the `.asc` mixed file and Host-side `.cpp` into `.o` object files separately, then link to generate the executable binary. The specific compilation commands are as follows:

```shell
# Step-by-step compilation
bisheng -c kernel.asc -o kernel.o --npu-arch=dav-3510 --enable-simt
bisheng -c host.cpp -o host.o -I${ASCEND_HOME_PATH}/include
bisheng kernel.o host.o -o demo

# Or a single compilation command
bisheng host.cpp kernel.asc -o demo --npu-arch=dav-3510 --enable-simt -I${ASCEND_HOME_PATH}/include
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
    kernel.asc
    host.cpp)

# 5. Set linker language to ASC
set_target_properties(demo PROPERTIES LINKER_LANGUAGE ASC)

# 6. Configure compilation options: --npu-arch specifies architecture version, --enable-simt enables SIMT mode
target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>)
```

In future versions, SIMT mode can be enabled by setting `CMAKE_ASC_ENABLE_SIMT` to ON instead of the `--enable-simt` compilation option in step 6 above:

```cmake
# Enable SIMT compilation mode
set(CMAKE_ASC_ENABLE_SIMT ON)
```

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  - Command Line Method

    ```bash
    bisheng -c gather_kernel.asc -o gather_kernel.o --npu-arch=dav-3510 --enable-simt  # Compile .asc mixed file to object file
    bisheng -c main.cpp -o main.o -I${ASCEND_HOME_PATH}/include                        # Compile Host-side .cpp to object file
    bisheng gather_kernel.o main.o -o demo                                             # Link to generate executable file
    ./demo                                                                             # Run the example
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
