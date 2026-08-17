# SIMT clock Interface Timestamp Function Implementation Example

## Overview

This example demonstrates how to use the `clock()` interface in SIMT programming to implement timestamp marking. The example implements Gather computation in SIMT mode, using `clock()` to record timestamps before and after kernel function execution to count execution cycles.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── 02_clock
│   ├── CMakeLists.txt         // CMake build file
│   ├── clock.asc              // Ascend C operator implementation with clock timestamp invocation example
│   └── README.md
```

## Operator Description

- Operator Function:

  This example performs Gather computation with the following formula:

  ```text
  output[i] = input[index[i]]
  ```

  The Host side directly constructs input data and golden data. After execution, result verification is completed within the process, without depending on additional data generation or verification scripts.

- Operator Specifications:

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">gather</td></tr>
  <tr><td align="center"></td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">input</td><td align="center">[100000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[12288]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[12288]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">simt_gather</td></tr>
  </table>

- Clock Timestamp Description:

  In the `simt_gather` kernel function, `clock()` is called once before the Gather computation to record the start timestamp, and called again after computation to record the end timestamp, outputting the cycle difference between the two timestamps. To avoid every SIMT thread printing a log line, only the first thread outputs the cycle statistics.

  ```text
  execute_cycle = clock_after_compute - clock_before_compute
  ```

- Operator Implementation:

  ```cpp
  __global__ void simt_gather(float* input, int32_t* index, float* output, uint64_t index_total_length)
  {
      uint64_t start = clock();
  
      int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
  
      // Maps to the index of output tensor
      if (idx >= index_total_length) {
          return;
      }
      output[idx] = input[index[idx]];
  
      uint64_t end = clock();
      if (blockIdx.x == 0 && threadIdx.x == 0) {
          printf("%s execute cycle : %lu\n", "simt_gather", end - start);
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

  After execution, the following print information is displayed, indicating that the timestamp function and accuracy verification are successful.

  ```text
  simt_gather execute cycle : 3479
  [Success] Case accuracy is verification passed.
  ```
