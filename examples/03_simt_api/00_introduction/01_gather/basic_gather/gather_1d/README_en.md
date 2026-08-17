# One-Dimensional Gather Operator Sample Using SIMT Programming Mode

## Overview

This sample implements a one-dimensional Gather operator in a simple scenario (with fixed shape) using the Ascend C SIMT programming mode. It collects elements at specified indices from a one-dimensional input, demonstrating the development method for discrete memory access operators in simplified scenarios.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Versions

- \>= CANN 9.0.0

## Directory Structure

```text
├── gather_1d
│   ├── CMakeLists.txt         # CMake build file
│   ├── gather_1d.asc          # Sample for one-dimensional gather using SIMT
│   └── README.md
```

## Operator Description

- Operator Function:  
  The gather_1d operator retrieves one data element from a one-dimensional input tensor with shape [100000] based on each element in the index. The formula for calculating the i-th element of the operator output is:

  ```text
  output[i] = input[index[i]]
  ```

- Operator Specification:  
  <table>
  <tr><td rowspan="1" align="center">Operator Type (OpType)</td><td colspan="4" align="center">gather_1d</td></tr>
  <tr><td rowspan="3" align="center">Operator Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[100000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[12288]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Operator Output</td><td align="center">output</td><td align="center">[12288]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">gather_1d_custom</td></tr>
  </table>

- Data Partitioning:  
  * Number of cores: 48 cores
  * Threads per core: 256 threads
  * Elements processed per thread: 1 element
  * Total processing capacity: 48 * 256 = 12288 elements
  * Expected result: The i-th element of the output equals the index[i]-th element of the input

- Operator Implementation:  
  The implementation process of the gather_1d operator retrieves data at specified indices from the input (Global Memory). Based on the above data partitioning, it first calculates the index of data that the thread should process, then reads one element according to index[i] and writes it to output[i].

- Invocation Implementation:  
  Use the kernel call operator <<<>>> to launch the kernel.

## Build and Run

Execute the following steps in the root directory of this sample to build and run the operator.

- Configure Environment Variables  
  Configure the environment variables according to the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Sample Execution

  Execute the following commands in this sample directory.

  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # Build the project
  ./demo                        # Run the sample
  ```

  Build Option Description

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: This sample only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  The following execution result indicates that the accuracy comparison succeeds.

  ```text
  [Success] Case accuracy is verification passed.
  ```