# SIMT assert Function Implementation Example

## Overview

This example demonstrates how to use the `assert()` and `ascendc_assert()` interfaces in the SIMT programming model for on-board functional debugging.

`ascendc_assert()` supports the message form: when the assertion condition fails, it prints the standard assert header followed by a user-defined formatted message (printf-style format string with variadic arguments), which helps locate problems with business context.

## Supported Products and CANN Versions

| Products | CANN Versions |
|---|---|
| Ascend 950PR/Ascend 950DT | \>= CANN 9.2.0 |

## Directory Structure

```text
├── 01_assert
│   ├── CMakeLists.txt         // CMake build file
│   ├── assert.asc             // Ascend C operator implementation with assert invocation example
│   └── README.md
```

## Operator Description

- Operator Function:

  This example demonstrates in detail the practice of using the `assert()` and `ascendc_assert()` interfaces in SIMT implementation functions to debug assertions during operator execution. The `ascendc_assert()` part demonstrates the message form (with format string and arguments).


- Operator Implementation:
  ```cpp
  __global__ void simt_assert(float* input, uint32_t in_shape)
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
          // Message form: on failure prints the assert header plus the user-defined message, then traps
          ascendc_assert(in_shape >= MIN_SHAPE, "in_shape %u must be >= %u\n", in_shape, MIN_SHAPE);
          ascendc_assert(input != nullptr, "input tensor of thread %u is nullptr\n", threadIdx.x);
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

  The example controls `in_shape` via a command-line argument to demonstrate different assert paths:

  - Without arguments (default `in_shape=128`): the `ascendc_assert()` condition holds and does not trigger; the `assert()` condition fails and triggers (message-less form, prints the assertion text only).
  - `./demo 32` (`in_shape=32 < 64`): the `assert()` condition holds and does not trigger; the `ascendc_assert()` condition fails and triggers the message-form assert, which prints the assert header and the user-defined message, then traps.

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU simulation mode
  ```

  > **Note:** Clear the cmake cache before switching build modes. Execute `rm CMakeCache.txt` in the build directory, then run cmake again.

  Build Options Description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  After execution, the following print information is displayed, indicating that the function works correctly.
  ```
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check start 1!
  [SIMT] trap check start 1!
  [SIMT] trap check start 2!
  [SIMT] trap check start 2!
  [SIMT] trap check start 3!
  [SIMT] trap check start 3!
  [ASSERT] xxx/assert.asc:37: void simt_assert(float *, uint32_t): Assertion `in_shape < 1' failed.
  [ASSERT] xxx/assert.asc:37: void simt_assert(float *, uint32_t): Assertion `in_shape < 1' failed.
  ```

  Passing an argument smaller than 64 (e.g. `./demo 32`) triggers the message-form assert, which prints the assert header followed by the user-defined message (the trailing `in_shape 32 must be >= 64`):
  ```
  [ASSERT] xxx/assert.asc:43: void simt_assert(float *, uint32_t): Assertion `in_shape >= MIN_SHAPE' failed. in_shape 32 must be >= 64
  ```
