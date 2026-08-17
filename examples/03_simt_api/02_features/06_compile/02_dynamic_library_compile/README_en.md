# Dynamic Library Compile Example

## Overview

This example, based on the Gather operator, demonstrates the dynamic library compilation flow where a Host + Device mixed .asc file (containing Kernel definition and <<<>>> kernel invocation) is compiled into a **dynamic shared library (.so)**, which is then linked by a Host-side .cpp. The <<<>>> invocation function in the .asc file is exposed to the pure Host-side .cpp through extern. The code structure and call relationship are shown in Figure 1:

<p align="center">
  <img src="./figures/dynamic_library_compile.png" width="50%">
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
├── 02_dynamic_library_compile
│   ├── CMakeLists.txt      // Build project file
│   ├── gather_kernel.asc   // Mixed file (containing Kernel definition, <<<>>> kernel invocation)
│   ├── main.cpp            // Pure Host-side code (calls functions exposed by .asc through extern)
│   └── figures             // Image resources for README
```

## Operator Description
The gather operator implements the function of obtaining 12288 data elements at specified indices from a 1D input vector of length 100000.

## Dynamic Library Compilation Introduction

Through dynamic library compilation, the `.asc` mixed file is compiled into an independent **dynamic shared library (.so)**, and the Host-side `.cpp` completes the call by linking this `.so`.

### bisheng Command Line Compilation

Use the `-shared` option to compile the `.asc` mixed file into a dynamic shared library (.so). The specific compilation command is as follows:

```shell
bisheng -shared kernel.asc -o libkernel.so -fPIC --npu-arch=dav-3510 --enable-simt
```

Where `--npu-arch` specifies the AI processor architecture at compile time. This example only supports `dav-3510`. For the architecture version corresponding to each product model, refer to the [correspondence table](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch). `--enable-simt` is used to specify SIMT compilation mode.

For more bisheng compilation options and usage, refer to [AI-Core Operator Compilation Basic Usage](../../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md).

### CMake Compilation

In a CMake project, use `add_library` with the `SHARED` keyword to compile `.asc` files into a dynamic shared library:

```cmake
# Compile .asc file into a dynamic shared library
add_library(gather_kernel SHARED
    gather_kernel.asc
)
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
    bisheng -shared gather_kernel.asc -o libgather_kernel.so -fPIC --npu-arch=dav-3510 --enable-simt    # Compile .asc to dynamic shared library
    bisheng main.cpp -o demo -L./ -L${ASCEND_HOME_PATH}/lib64 -I${ASCEND_HOME_PATH}/include \
      -lgather_kernel -lascendcl -lstdc++                                                              # Link to generate executable file
    export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH                                                          # Set dynamic library search path
    ./demo                                                                                             # Run the example
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
