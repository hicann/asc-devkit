# Reduction Interface Multi-Scenario Example

## Overview

This example introduces the usage of reduction interfaces  `ReduceRepeat` in multiple scenarios. These interfaces perform reduction operations (find maximum, minimum, or sum) on all elements within each repeat of a LocalTensor, with reduction results stored in the destination LocalTensor.

Note: `ReduceRepeat` is the renamed API after CANN 9.1.0. For CANN 9.0.0 and earlier versions, use `WholeReduceMax`, `WholeReduceMin`, `WholeReduceSum`.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.1.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.1.0 |

## Directory Structure

``` text
├── reduce_repeat
│   ├── scripts
│   │   ├── gen_data.py                    // Script for generating input data and ground truth data
│   │   └── verify_result.py               // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                     // Build project file
│   ├── data_utils.h                       // Data read/write functions
│   ├── reduce_repeat.asc                  // Ascend C example implementation & invocation example
│   └── README.md                          // Example documentation
```

## Scenario Description

This example selects different reduction scenarios through the compilation parameter `SCENARIO_NUM`. All scenarios use ND data format, and the kernel function name is `reduce_repeat_custom`.

**Scenario 1: `ReduceRepeat<MAX>`**

- Input: [1, 1024] half elements, mask=128, repeat=8 (1024/128)
- Output: [1, 8] half elements (8 maximum values, no index returned)
- Implementation: `ReduceRepeat<ReduceType::MAX, half>(dstLocal, srcLocal, mask=128, repeat=8, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE)`
- Description: Using `ORDER_ONLY_VALUE` parameter, only returns maximum values without indices, output stored as [max0, max1, max2, ...]

**Scenario 2: `ReduceRepeat<MIN>`**

- Input: [1, 1024] half elements, mask=128, repeat=8 (1024/128)
- Output: [1, 16] half elements (8 minimum values + 8 indices, interleaved storage)
- Implementation: `ReduceRepeat<ReduceType::MIN, half>(dstLocal, srcLocal, mask=128, repeat=8, 1, 1, 8)`
- Description: Using default `order=ORDER_VALUE_INDEX`, output stored as [min0, idx0, min1, idx1, ...] interleaved

**Scenario 3: `ReduceRepeat<SUM>`**

- Input: [1, 2048] float elements, mask=64 (float type 32-bit, mask range [1,64]), repeat=32 (2048/64)
- Output: [1, 32] float elements (sum result for each repeat)
- Implementation: `ReduceRepeat<ReduceType::SUM, float>(dstLocal, srcLocal, mask=64, repeat=32, 1, 1, 8)`
- Description: Each repeat independently sums, outputting 32 sum results total

**Scenario 4: `ReduceRepeat<SUM>` unaligned scenario**

- Input: [13, 57] float elements (13 rows x 57 columns, column count 57x4 bytes=228 bytes, not 32-byte aligned)
- Output: [1, 13] float elements (sum result for each row)
- Implementation: Use `DataCopyPad` to transfer unaligned data, `ReduceRepeat<ReduceType::SUM, float>(dstLocal, srcLocal, mask=57, repeat=13, 1, 1, srcStride)` to sum each row
- Description: Demonstrates reduction operations in unaligned data scenarios. Each row has 57 float elements (228 bytes), padded to 232 bytes (58 float elements) using `DataCopyPad` for 32-byte alignment, with `ReduceRepeat<SUM>` srcRepeatStride calculated based on aligned block count

## Example Specifications

<table border="2">
<caption>Table 1: Example Input/Output Specifications (Scenario 1)</caption>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 8]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_repeat_custom</td></tr>
</table>

<table border="2">
<caption>Table 2: Example Input/Output Specifications (Scenario 2)</caption>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 16]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_repeat_custom</td></tr>
</table>

<table border="2">
<caption>Table 3: Example Input/Output Specifications (Scenario 3)</caption>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_repeat_custom</td></tr>
</table>

<table border="2">
<caption>Table 4: Example Input/Output Specifications (Scenario 4)</caption>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[13, 57]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 13]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_repeat_custom</td></tr>
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
  SCENARIO_NUM=1  # Set the scenario number
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2`, `3`, `4` | Scenario number: 1 (`ReduceRepeat<MAX>`), 2 (`ReduceRepeat<MIN>`), 3 (`ReduceRepeat<SUM>`), 4 (unaligned `ReduceRepeat<SUM>`) |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
