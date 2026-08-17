# SIMT Programming Mode Implementation of 2D Gather Operator Sample

## Overview

This sample implements a simple scenario (fixed shape) 2D Gather operator using the Ascend C SIMT programming mode. It collects specified m rows of data from the input tensor, demonstrating the development method for discrete memory access operators in simplified scenarios.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Versions

- \>= CANN 9.0.0

## Directory Structure

```text
├── gather_2d
│   ├── CMakeLists.txt            # cmake build file
│   ├── gather_2d.asc             # SIMT implementation of 2D gather call sample
│   └── README.md
```

## Operator Description

- Operator Function:  
  The gather_2d operator retrieves 12288 rows of data at specified indices from a 2D vector with shape [100000,128]. The calculation formula for the i-th row of the operator output is:
  
  ```text
  output[i] = input[index[i]]
  ```

- Operator Specification:  
  <table>
  <tr><td rowspan="1" align="center">Operator Type(OpType)</td><td colspan="4" align="center">gather_2d</td></tr>
  <tr><td rowspan="3" align="center">Operator Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[100000,128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[12288]</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Operator Output</td><td align="center">output</td><td align="center">[12288,128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">gather_2d_custom</td></tr>
  </table>

- Data Partitioning:  
  * Number of cores: 48 cores
  * Threads per core: 256 threads
  * Processing per thread: 1 row (128 columns)
  * Total processing capacity: 48×256=12288 rows (covering index length)

- Operator Implementation:  
  The gather_2d operator implementation process retrieves data at specified indices from the input (Global Memory). Based on the data partitioning above, it first calculates the index of data that the thread should process, then stores one row of data to Global Memory through an assignment operation.

- Invocation Implementation:  
  Use the kernel call operator <<<>>> to invoke the kernel function.

## Compilation and Execution

Execute the following steps in the root directory of this sample to compile and run the operator.

- Configure Environment Variables  
  Configure the environment variables according to the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Sample Execution

  Execute the following commands in this sample directory.

  ```bash
  mkdir -p build && cd build;   # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # Build the project
  ./demo                        # Run the sample
  ```

  Compilation Option Description

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU Architecture: This sample only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  The execution result is as follows, indicating that the accuracy comparison passed.

  ```text
  [Success] Case accuracy is verification passed.
  ```
