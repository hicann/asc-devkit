# MrgSort Example

## Overview

This example implements multi-way merge sort functionality based on Sort32 and MrgSort in a sorting scenario. First, Sort32 is called to preprocess data into multiple sorted subsequences in parallel (every 32 elements are grouped for descending sort, forming sorted queues stored in alternating (score, index) structure). Then MrgSort is called to merge these subsequences into a globally sorted result.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── mrg_sort
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── mrg_sort.asc            // Ascend C example implementation & invocation
│   └── README.md               // Example documentation
```

## Scenario Description

This example selects different scenarios through the compilation parameter `SCENARIO_NUM`. All scenarios use ND data format, and the kernel function name is `vec_mrgsort_kernel`.

**Scenario 1: 4-queue sorting**
- Input: 128 float elements (score) + 128 uint32 elements (index)
- Output: [1, 256] float elements (128*2 sorted results)
- Implementation: Sort32 repeat=4, dividing 128 elements into 4 groups for descending sort; MrgSort validBit=0b1111, ifExhaustedSuspension=false, repeatTimes=1, merging 4 queues into 1 sorted queue
- Description: Demonstrates merging 4 queues completely into 1 sorted queue

**Scenario 2: 3-queue non-4-aligned merge**
- Input: 96 float elements (score) + 96 uint32 elements (index)
- Output: [1, 192] float elements (96*2 sorted results)
- Implementation: Sort32 repeat=3, dividing 96 elements into 3 groups for descending sort; MrgSort validBit=0b0111, ifExhaustedSuspension=false, repeatTimes=1, merging 3 queues into 1 sorted queue
- Description: Demonstrates merging in non-4-aligned cases, validBit=0b0111 indicates the first 3 queues are valid, the 4th queue length is set to 0

**Scenario 3: 32-queue multi-round merge sort**
- Input: 1024 float elements (score) + 1024 uint32 elements (index)
- Output: [1, 2048] float elements (1024*2 sorted results)
- Implementation: Sort32 repeat=32, dividing 1024 elements into 32 groups for descending sort; first round MrgSort repeatTimes=8, merging every 4 queues into 1, yielding 8 sorted queues; second round MrgSort repeatTimes=2, merging every 4 queues into 1, yielding 2 sorted queues; third round MrgSort2 merges 2 queues into 1 globally sorted queue
- Description: Demonstrates multi-way merge sort, merging 32 sorted queues into 1 globally sorted queue through multiple rounds

## Example Specifications

<table border="2">
<caption>Table 1: Example Input/Output Specifications (Scenario 1)</caption>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 128]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1, 256]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">vec_mrgsort_kernel</td></tr>
</table>

<table border="2">
<caption>Table 2: Example Input/Output Specifications (Scenario 2)</caption>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 96]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 96]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1, 192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">vec_mrgsort_kernel</td></tr>
</table>

<table border="2">
<caption>Table 3: Example Input/Output Specifications (Scenario 3)</caption>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[1, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">vec_mrgsort_kernel</td></tr>
</table>

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
    
- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;                             # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;         # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                                                   # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2`, `3` | Scenario number |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
