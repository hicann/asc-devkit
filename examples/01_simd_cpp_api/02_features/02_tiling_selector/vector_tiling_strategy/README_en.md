# Vector Computing Multi-Core Tiling Split Strategy Sample

## Overview

This sample uses two one-dimensional Tensors as inputs for addition. It introduces a multi-core Tiling split strategy for vector computing. The strategy calculates different inter-core and intra-core split parameters based on the input data volume. It distributes data across multiple AI Cores as evenly as possible to improve multi-core computing efficiency.

The sample uses 8 Cores. The main tile data volume in each core is 3200 `half` elements, which equals 200 DataBlocks. Use the CMake build parameter `SCENARIO_NUM` to select a data volume scenario.

## Supported Products and CANN Software Versions

| Product | CANN software version |
|------|--------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 training series products/Atlas A3 inference series products | >= CANN 9.0.0 |
| Atlas A2 training series products/Atlas A2 inference series products | >= CANN 9.0.0 |

## Directory Structure

```text
├── vector_tiling_strategy
│   ├── scripts
│   │   ├── gen_data.py            // Generates input data and golden data.
│   │   └── verify_result.py       // Verifies the output result.
│   ├── CMakeLists.txt             // Build project file.
│   ├── vector_tiling_strategy.asc // Ascend C sample implementation and invocation sample.
│   ├── data_utils.h               // Data read and write functions.
│   └── README.md                  // Sample description document.
```

## Scenario Details

This sample adds two one-dimensional Tensors. The compute logic is `z = x + y`. Use the CMake build parameter `SCENARIO_NUM` to select a data volume scenario. Each scenario maps to a different Tiling split scenario. All scenarios use the ND data format. The input and output data type is `half`. The kernel function name is `add_custom`.

The sample uses 8 Cores. The main tile data volume is 3200 `half` elements. The main tile (`mainTileLength`) is the maximum data volume that one operation can process based on the available Unified Buffer (UB) space. It meets 32-byte alignment.

**Scenario 0: Main Tile Equal Split**

- Input: `x` and `y` each contain [1, 256000] `half` elements.
- Output: `z` contains [1, 256000] `half` elements.
- Tiling split: Each of the 8 cores processes 32000 elements. Each core contains 10 main tiles, and each main tile contains 3200 elements.
- Description: All cores process the same data volume. Each core contains only main tiles.

**Scenario 1: Tail Block Equal Split**

- Input: `x` and `y` each contain [1, 260096] `half` elements.
- Output: `z` contains [1, 260096] `half` elements.
- Tiling split: Each of the 8 cores processes 32512 elements. Each core contains 10 main tiles and a tail block with 512 elements.
- Description: All cores process the same data volume. Each core has an equal-length tail block after the main tiles.

**Scenario 2: Tail Core Split**

- Input: `x` and `y` each contain [1, 256064] `half` elements.
- Output: `z` contains [1, 256064] `half` elements.
- Tiling split: The first 4 former cores each process 32016 elements, and the remaining 4 tail cores each process 32000 elements. Each former core contains 10 main tiles and a tail block with 16 elements. Each tail core contains 10 main tiles.
- Description: The first 4 former cores each process one more DataBlock than the tail cores. The tail cores do not have tail blocks.

**Scenario 3: Tail Core Split with Tail Blocks**

- Input: `x` and `y` each contain [1, 258112] `half` elements.
- Output: `z` contains [1, 258112] `half` elements.
- Tiling split: The first 4 former cores each process 32272 elements, and the remaining 4 tail cores each process 32256 elements. Each former core contains 10 main tiles and a tail block with 272 elements. Each tail core contains 10 main tiles and a tail block with 256 elements.
- Description: All cores have tail blocks. The first 4 former cores each process one more DataBlock.

## Sample Specifications

<table border="2">
<caption>Table 1: Sample input and output specifications (scenario 0)</caption>
<tr><td rowspan="3" align="center">Sample input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 256000]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 256000]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Sample output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 256000]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel function name</td><td colspan="4" align="center">add_custom</td></tr>
</table>

<table border="2">
<caption>Table 2: Sample input and output specifications (scenario 1)</caption>
<tr><td rowspan="3" align="center">Sample input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 260096]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 260096]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Sample output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 260096]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel function name</td><td colspan="4" align="center">add_custom</td></tr>
</table>

<table border="2">
<caption>Table 3: Sample input and output specifications (scenario 2)</caption>
<tr><td rowspan="3" align="center">Sample input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 256064]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 256064]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Sample output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 256064]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel function name</td><td colspan="4" align="center">add_custom</td></tr>
</table>

<table border="2">
<caption>Table 4: Sample input and output specifications (scenario 3)</caption>
<tr><td rowspan="3" align="center">Sample input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 258112]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 258112]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">Sample output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 258112]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel function name</td><td colspan="4" align="center">add_custom</td></tr>
</table>

## Host-Side Tiling Calculation Process

The Tiling calculation process of this sample is as follows. The host side receives `totalLength` as the input. It calculates the number of main tiles, tail blocks, and tail cores step by step. The output is the Tiling split result.

1. Align the total data volume by DataBlock.

   ```cpp
   totalLengthAligned = AlignUp(totalLength, alignNum);
   ```

2. Allocate the same number of main tiles, `mainTileLength`, to all cores.

   ```cpp
   mainTileNum = totalLengthAligned / (numBlocks * mainTileLength);
   mainTileRemainder = totalLengthAligned % (numBlocks * mainTileLength);
   ```

   At this point, each core processes at least `mainTileNum * mainTileLength` elements. `mainTileRemainder` indicates the remaining data volume after main tile allocation. If `mainTileRemainder` is 0, select the main-tiles-only scenario and finish the Tiling calculation.

   ```cpp
   if (mainTileRemainder == 0) {
       // Use MainTileOnlyTiling.
       return;
   }
   ```

3. If `mainTileRemainder` is not 0, allocate it to all cores as equal-length DataBlock tail blocks.

   ```cpp
   tailBlockNumEachCore = mainTileRemainder / (numBlocks * alignNum);
   remainingTailLength = mainTileRemainder % (numBlocks * alignNum);
   baseLength = mainTileNum * mainTileLength + tailBlockNumEachCore * alignNum;
   ```

   `baseLength` indicates the base data volume processed by each core. `remainingTailLength` indicates the number of elements that remain unallocated after each core receives equal-length DataBlock tail blocks. Its value is an integer multiple of `alignNum`.

4. Check for the tail block equal split scenario. If `remainingTailLength` is 0, all cores receive equal-length tail blocks, and this scenario is selected.

   ```cpp
   if (remainingTailLength == 0) {
       // Use MainTileWithTailBlockTiling.
       return;
   }
   ```

5. If `remainingTailLength` is not 0, allocate the remaining DataBlocks to the first several former cores and calculate the data lengths of former and tail cores.

   ```cpp
   formerNum = remainingTailLength / alignNum;
   formerLength = baseLength + alignNum;
   tailLength = baseLength;
   ```

6. Check for the tail core split scenario. If `tailBlockNumEachCore` is 0, tail cores contain only main tiles, and this scenario is selected. Otherwise, select the tail core split with tail blocks scenario.

   ```cpp
   if (tailBlockNumEachCore == 0) {
       // Use MainTileWithTailCoreTiling.
   } else {
       // Use TailBlockAndTailCoreTiling.
   }
   ```

## Build and Run

In the sample root directory, perform the following steps to build and run the sample.

- Configure environment variables.
  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit package in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If you do not specify an installation directory, the default directory is `/usr/local/Ascend`.

- Run the sample.

  In the sample directory, run the following commands.

  ```bash
  SCENARIO_NUM=0  # Set the scenario number.
  mkdir -p build && cd build;      # Create and enter the build directory.
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project.
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data.
  ./demo                           # Run the generated executable file.
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct.
  ```

  To use CPU debugging mode or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debugging mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Note:** Before you switch the build mode, clear the cmake cache. In the build directory, run `rm CMakeCache.txt`, and then run `cmake` again.

- Build option description.

  | Option | Optional value | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debugging, and NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: `dav-2201` maps to Atlas A2 training series products, Atlas A2 inference series products, Atlas A3 training series products, and Atlas A3 inference series products. `dav-3510` maps to Ascend 950PR and Ascend 950DT |
  | `SCENARIO_NUM` | `0` (default), `1`, `2`, `3` | Scenario number: 0 (main tile equal split), 1 (tail block equal split), 2 (tail core split), and 3 (tail core split with tail blocks) |

- Execution result.

  The output is as follows. This result indicates that the accuracy comparison succeeds.

  ```bash
  test pass!
  ```
