# Static Library Compile Example

## Overview

This example demonstrates, based on the Add operator, the static library compilation flow that compiles a Host + Device mixed `.asc` file (containing Kernel definitions and `<<<>>>` kernel launch calls) into a **static library (.a)**, which is then linked and called by the Host-side `.cpp`. The `<<<>>>` launch function in the `.asc` file is exposed to the pure Host-side `.cpp` through `extern`. The code structure and call relationship are shown in Figure 1:

<p align="center">
  <img src="./figures/static_library_compile.png" width="50%">
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
├── 03_static_library_compile
│   ├── figures             // Illustrations
│   ├── add_kernel.asc      // Mixed file (containing Kernel definitions and <<<>>> kernel launch calls)
│   ├── CMakeLists.txt      // Build project file
│   ├── main.cpp            // Pure Host-side code (calls functions exposed by .asc through extern)
│   └── README.md           // Example documentation
```

## Operator Description

The Add operator implements element-wise addition of two vectors. The input vector length is 16384, and the output vector length is 16384.

## Static Library Compilation Introduction

Through static library compilation, the `.asc` mixed file is compiled into an independent **static library (.a)**, and the Host-side `.cpp` completes the call by linking this `.a`.

### bisheng Command-Line Compilation

Use the `-lib` option to compile the `.asc` mixed file into a static library (.a). The compilation command is as follows:

```shell
bisheng -lib add_kernel.asc -o libadd_kernel.a --npu-arch=dav-2201
```

Where `--npu-arch` specifies the AI processor architecture at compile time. This example supports `dav-2201` and `dav-3510`. For the architecture version number corresponding to each product model, refer to the [mapping table](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

For more bisheng compilation options and usage, refer to [Basic Usage of AI Core Operator Compilation](../../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md).

### CMake Compilation

In a CMake project, use `add_library` with the `STATIC` keyword to compile the `.asc` file into a static library:

```cmake
# Compile the .asc file into a static library
add_library(add_kernel STATIC
    add_kernel.asc)
```

## Build and Run

Run the following steps in the root directory of this example to build and run the operator.

- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.
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
