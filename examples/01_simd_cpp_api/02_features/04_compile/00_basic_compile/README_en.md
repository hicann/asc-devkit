# Basic Compile Example

## Overview

This example demonstrates, based on the Add operator, the basic compilation flow in which a Host-side `.cpp` file calls a Host + Device mixed `.asc` file (containing Kernel definitions and `<<<>>>` kernel launch calls). The `<<<>>>` launch function in the `.asc` file is exposed to the pure Host-side `.cpp` through `extern`. The code structure and call relationship are shown in Figure 1:

<p align="center">
  <img src="./figures/basic_compile.png" width="50%">
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
├── 00_basic_compile
│   ├── figures             // Illustrations
│   ├── add_kernel.asc      // Mixed file (containing Kernel definitions and <<<>>> kernel launch calls)
│   ├── CMakeLists.txt      // Build project file
│   ├── main.cpp            // Pure Host-side code (calls functions exposed by .asc through extern)
│   └── README.md           // Example documentation
```

## Operator Description

The Add operator implements element-wise addition of two vectors. The input vector length is 16384, and the output vector length is 16384.

## Heterogeneous Compilation Introduction

This example uses **heterogeneous compilation**, where Host-side `.cpp` and Device-side `.asc` are compiled separately and then linked. The basic compilation flow of AI Core SIMD is as follows: Host code is compiled into a Host binary using the Host compiler, AI Core SIMD code is compiled into a SIMD binary using the Device compiler, the SIMD binary is first linked into a Fatbin file, and then merged with the Host binary to generate the executable binary.

### bisheng Command-Line Compilation

Basic flow: first compile the `.asc` mixed file and the Host-side `.cpp` into `.o` object files respectively, then link them to generate the executable binary. The compilation commands are as follows:

```shell
# Step-by-step compilation
bisheng -c add_kernel.asc -o add_kernel.o --npu-arch=dav-2201
bisheng -c main.cpp -o main.o -I${ASCEND_HOME_PATH}/include
bisheng add_kernel.o main.o -o demo

# Or a single compilation command
bisheng main.cpp add_kernel.asc -o demo --npu-arch=dav-2201 -I${ASCEND_HOME_PATH}/include
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
    add_kernel.asc
    main.cpp)

# 5. Set the linker language to ASC
set_target_properties(demo PROPERTIES LINKER_LANGUAGE ASC)

# 6. Configure compilation options: --npu-arch specifies the architecture version
target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>)
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
