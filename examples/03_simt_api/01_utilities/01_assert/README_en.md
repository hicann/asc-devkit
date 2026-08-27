# SIMT assert Function Implementation Example

## Overview

This example demonstrates how to use the `assert()` interface in the SIMT programming model for on-board functional debugging.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version
- \>= CANN 9.1.0

## Directory Structure

```text
├── 01_assert
│   ├── CMakeLists.txt         // CMake build file
│   ├── assert.asc             // Ascend C operator implementation with assert invocation example
│   └── README.md
```

## Operator Description

- Operator Function:

  This example demonstrates in detail the practice of using the `assert()` interface in SIMT implementation functions to debug assertions during operator execution.


- Operator Implementation:
  ```cpp
  __global__ void  simt_assert(float* input, uint32_t in_shape)
  {
      // Calculate global thread ID
      int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
      if (threadIdx.x < 1) {
          printf("[SIMT] %s\n", "trap check start 1!");
          printf("[SIMT] %s\n", "trap check start 2!");
          printf("[SIMT] %s\n", "trap check start 3!");
          assert(in_shape < 1);
          printf("[SIMT] %s\n", "trap check 1!");
      } else if(threadIdx.x < 5) {
          printf("[SIMT] %s\n", "trap check 2!");
          assert(in_shape > 1);
          printf("[SIMT] %s\n", "trap check 3!");
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

  After execution, the following print information is displayed, indicating that the function works correctly.
  ```
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check start 1!
  [SIMT] trap check start 1!
  [SIMT] trap check start 2!
  [SIMT] trap check start 2!
  [SIMT] trap check start 3!
  [SIMT] trap check start 3!
  [ASSERT] xxx/assert.asc:32: void simt_assert(float *, uint32_t): Assertion `in_shape < 1' failed.
  [ASSERT] xxx/assert.asc:32: void simt_assert(float *, uint32_t): Assertion `in_shape < 1' failed.
  ```
