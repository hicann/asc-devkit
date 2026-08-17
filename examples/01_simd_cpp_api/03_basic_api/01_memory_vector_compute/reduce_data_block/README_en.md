# ReduceDataBlock Interface Multi-Scenario Example

## Overview

This example implements multi-scenario reduction functionality based on the `ReduceDataBlock` interface in a reduction scenario, performing reduction operations (find maximum, minimum, or sum) on all elements within each DataBlock (32 bytes) of the input Tensor.

Note: `ReduceDataBlock` is the renamed API after CANN 9.1.0. For CANN 9.0.0 and earlier versions, use `BlockReduceMax`, `BlockReduceMin`, `BlockReduceSum`.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.1.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.1.0 |

## Directory Structure

``` text
├── reduce_data_block
│   ├── scripts
│   │   ├── gen_data.py                    // Script for generating input data and ground truth data
│   │   └── verify_result.py               // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                     // Build project file
│   ├── data_utils.h                       // Data read/write functions
│   ├── reduce_data_block.asc              // Ascend C example implementation & invocation example
│   └── README.md                          // Example documentation
```

## Scenario Description

This example selects different reduction scenarios through the compilation parameter `SCENARIO_NUM`. All scenarios use ND data format, and the kernel function name is `reduce_data_block_custom`.

**Scenario 1: `ReduceDataBlock<MAX, half>`**

- Input: [1, 256] half elements, mask=128 (256/sizeof(half)), repeat=2
- Output: [1, 16] half elements (maximum values for 16 datablocks)
- Implementation: `ReduceDataBlock<ReduceType::MAX, half>(dstLocal, srcLocal, mask=128, repeat=2, dstRepStride=1, srcBlkStride=1, srcRepStride=8)`
- Description: Find the maximum of all elements within each datablock. One datablock processes 32 bytes, that is 16 half elements. 256 elements total 16 datablocks, outputting 16 maximum values

**Scenario 2: `ReduceDataBlock<MIN, half>`**

- Input: [4, 128] half elements, mask=128 (256/sizeof(half)), repeat=4
- Output: [4, 8] half elements (minimum values for 32 datablocks)
- Implementation: `ReduceDataBlock<ReduceType::MIN, half>(dstLocal, srcLocal, mask=128, repeat=4, dstRepStride=1, srcBlkStride=1, srcRepStride=8)`
- Description: Find the minimum of all elements within each datablock. One datablock processes 32 bytes, that is 16 half elements. 512 elements total 32 datablocks, outputting 32 minimum values

**Scenario 3: `ReduceDataBlock<SUM, float>`**

- Input: [1, 128] float elements, mask=64 (256/sizeof(float)), repeat=2
- Output: [1, 16] float elements (sum results for 16 datablocks)
- Implementation: `ReduceDataBlock<ReduceType::SUM, float>(dstLocal, srcLocal, mask=64, repeat=2, dstRepStride=1, srcBlkStride=1, srcRepStride=8)`
- Description: Sum all elements within each datablock. Source operand addition uses binary tree pairwise addition. One datablock processes 32 bytes, that is 8 float elements. 128 elements total 16 datablocks, outputting 16 sum results

## Example Specifications

<table border="2">
<caption>Table 1: Example Input/Output Specifications (Scenario 1: ReduceDataBlock&lt;MAX&gt;)</caption>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">y</td><td align="center">[1, 16]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_data_block_custom</td></tr>
</table>

<table border="2">
<caption>Table 2: Example Input/Output Specifications (Scenario 2: ReduceDataBlock&lt;MIN&gt;)</caption>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[4, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">y</td><td align="center">[4, 8]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_data_block_custom</td></tr>
</table>

<table border="2">
<caption>Table 3: Example Input/Output Specifications (Scenario 3: ReduceDataBlock&lt;SUM&gt;)</caption>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">y</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_data_block_custom</td></tr>
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
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct
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
