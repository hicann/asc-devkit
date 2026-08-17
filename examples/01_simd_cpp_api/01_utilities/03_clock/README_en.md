# Timestamp Marking Implementation Example Based on SIMD&SIMT Hybrid Programming

## Overview

This example demonstrates the usage of the `clock()` API for timestamp marking in a SIMD&SIMT hybrid programming scenario, based on fused Gather and Adds computation. The example implements discrete memory access Gather in SIMT mode, contiguous memory access Adds in SIMD mode, and places timestamps before and after the SIMT Gather phase to record execution cycles.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.0.0-beta.2 |

## Directory Structure

```
├── 03_clock
│   ├── CMakeLists.txt              // Build project file
│   ├── clock.asc                   // Ascend C example implementation & invocation example
│   └── README.md
```

## Example Description

- Example features:

  This example references [SIMT and SIMD Hybrid Programming for Gather and Adds Computation](../../../05_simd_simt_hybrid/00_introduction/simd_simt_gather_and_adds/README_en.md) to perform fused Gather and Adds computation. The computation formula is:

  ```text
  output[i] = input[index[i]] + 1
  ```

  The host side constructs input data and golden data directly, performs result verification within the process after execution, and does not rely on additional data generation or verification scripts.

- Example specifications:

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">gather & adds</td></tr>
  <tr><td align="center"></td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">input</td><td align="center">[100000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[8192]</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">gather_and_adds_kernel</td></tr>
  </table>

- Hybrid programming workflow:

  The SIMT unit and SIMD unit in the Vector Core share on-chip storage, which enables hybrid programming of SIMT and SIMD using on-chip storage. In this example, the Shape of `index` is `[8192]`, the number of cores is `8`, and each core processes `1024` data elements. The thread count `THREAD_COUNT` is `1024`, each thread processes one data element, and a single core invocation of `simt_gather` completes the Gather computation.

  The fused Gather and Adds computation consists of three main steps:

  1. `simt_gather` uses SIMT programming to read data from discrete locations in Global Memory based on `index[i]` and write it to the Unified Buffer.

     ```cpp
     uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
     uint32_t gatherIdx = index[idx];
     gatherOutput[threadIdx.x] = input[gatherIdx];
     ```

  2. `simd_adds` uses SIMD programming to read contiguous data from the Unified Buffer, load it into registers through `Reg::LoadAlign`, perform the add-1 computation through `Reg::Adds`, and write back to the Unified Buffer through `Reg::StoreAlign`.

     ```cpp
     AscendC::Reg::LoadAlign(srcReg0, input + i * oneRepeatSize);
     AscendC::Reg::Adds(dstReg0, srcReg0, addsAddend, maskReg);
     AscendC::Reg::StoreAlign(output + i * oneRepeatSize, dstReg0, maskReg);
     ```

  3. `DataCopy` transfers the result from the Unified Buffer to Global Memory.

  The focus of this example is to demonstrate `clock()` timestamp marking, so the computation workflow is kept as two stages: SIMT Gather and SIMD Adds. In actual development, a simple add-1 operation can also be completed directly in the SIMT stage.

- Clock timestamp marking:

  In `simt_gather`, `clock()` is called once before the Gather computation to record the start timestamp, and called again after the Gather computation to record the end timestamp. The cycle difference between the two timestamps is output. To avoid each SIMT thread printing a log line, the example only outputs the cycle statistics from the first thread.

  ```text
  execute_cycle = clock_after_gather - clock_before_gather
  ```

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j
  ./demo
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j
  ```

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: `dav-3510` corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The execution result is as follows, indicating successful timestamp marking and accuracy comparison.

  ```text
  simt_gather execute cycle : 22289
  test pass!
  ```
