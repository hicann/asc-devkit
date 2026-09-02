# SIMT printf Function Implementation Example

## Overview

This example demonstrates how to use the `printf()` interface in SIMT programming to implement on-board printing for functional debugging.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version
- \>= CANN 9.1.0

## Directory Structure

```text
├── 00_printf
│   ├── CMakeLists.txt         // CMake build file
│   ├── printf.asc             // Ascend C operator implementation with printf printing invocation example
│   └── README.md
```

## Operator Description

- Operator Function:

  This example demonstrates in detail the practice of using the `printf()` interface in SIMT kernel functions to print variable information of threads during operator execution (the example limits printing to only some threads via `threadIdx.x < 3`).


- Operator Implementation:
  ```cpp
  __global__ void simt_printf(float* input, uint32_t in_shape)
  {
      // Calculate global thread ID
      int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
      if (threadIdx.x < 3) {
          printf("[SIMT %s] thread index[%u], input data shape: %u\n", "print 1", idx, in_shape);
          printf("[SIMT %s] input addr: %p value[%u]: %f\n", "print 2", input, idx, input[idx]);
      }
  }
  ```

## Build and Run

Run the following steps in the root directory of this example to build and execute the operator.

- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # Build the project
  ./demo                        # Run the example
  ```

  Build Options Description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  After execution, the following print information is displayed, indicating that the printing function works correctly.
  ```
  [SIMT print 1] thread index[34], input data shape: 128
  [SIMT print 1] thread index[2], input data shape: 128
  [SIMT print 1] thread index[33], input data shape: 128
  [SIMT print 1] thread index[1], input data shape: 128
  [SIMT print 1] thread index[32], input data shape: 128
  [SIMT print 1] thread index[0], input data shape: 128
  [SIMT print 2] input addr: 0x120000016000 value[2]: 3.118000
  [SIMT print 2] input addr: 0x120000016000 value[1]: 2.118000
  [SIMT print 2] input addr: 0x120000016000 value[0]: 1.118000
  [SIMT print 2] input addr: 0x120000016000 value[34]: 35.118000
  [SIMT print 2] input addr: 0x120000016000 value[33]: 34.118000
  [SIMT print 2] input addr: 0x120000016000 value[32]: 33.118000
  ```
