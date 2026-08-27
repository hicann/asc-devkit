# HelloWorld Example

## Overview

This example is a SIMT programming introductory example. It uses the `<<<>>>` kernel launch operator to complete the basic process of running and verifying the example kernel function on the NPU side. The kernel function prints output results using `printf()`.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Versions
- \>= CANN 9.1.0

## Directory Structure

```
├── hello_world_simt
│   ├── CMakeLists.txt      // Build project file
│   └── hello_world.asc     // Ascend C SIMT programming example implementation and invocation example
```

## Compile and Run

Execute the following steps in the root directory of this example to compile and run the example.

- Configure Environment Variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development toolkit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, it is installed to `/usr/local/Ascend` by default.

- Example Execution

  Execute the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Compile the project
  ./demo                        # Execute the example
  ```
  
  Compilation Options

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: This example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

- Execution Result
  The execution result is as follows, indicating successful execution.

  ```bash
  [blockIdx (0/2)][threadIdx (2/32)]: Hello World!
  [blockIdx (0/2)][threadIdx (1/32)]: Hello World!
  [blockIdx (0/2)][threadIdx (0/32)]: Hello World!
  [blockIdx (1/2)][threadIdx (2/32)]: Hello World!
  [blockIdx (1/2)][threadIdx (1/32)]: Hello World!
  [blockIdx (1/2)][threadIdx (0/32)]: Hello World!
  ```

  The output consists of 6 lines, each corresponding to the print result of one thread. `[blockIdx (X/2)]` indicates the X-th thread block (out of 2 thread blocks in total), and `[threadIdx (Y/32)]` indicates the Y-th thread within the thread block. This shows that the kernel function has executed successfully on the AIV Core.