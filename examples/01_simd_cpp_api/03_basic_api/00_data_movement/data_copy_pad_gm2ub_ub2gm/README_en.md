# DataCopyPad Example

## Overview

This example implements non-32-byte-aligned data transfer and padding functionality based on the DataCopyPad API in a data transfer scenario. The DataCopyPad API supports unaligned data transfer from Global Memory to Local Memory, with the ability to pad specified values on the left or right side of the data.

The data transfer process includes: Global Memory (GM) -> Unified Buffer (UB) (using DataCopyPad for unaligned transfer and padding) -> Global Memory (GM). This example uses static Tensor allocation for UB memory and supports switching between different scenarios through build parameters, demonstrating various usage methods of DataCopyPad.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── data_copy_pad_gm2ub_ub2gm
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Verification script to check output data against ground truth
│   ├── CMakeLists.txt          // Build configuration file
│   ├── data_utils.h            // Data read/write functions
│   ├── figures                 // Illustrations
│   ├── data_copy_pad.asc       // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Scenario Description

This example selects different scenarios through the build parameter `SCENARIO_NUM`. All scenarios use ND data format, and the kernel function name is `data_copy_pad_custom`.

<table border="2">
<caption>Table 1: Scenario Configuration Comparison</caption>
<tr><th>scenarioNum</th><th>Input Shape</th><th>Output Shape</th><th>Data Type</th><th>Padding/Transfer Mode</th><th>Description</th></tr>
<tr><td>1</td><td>[1, 20]</td><td>[1, 32]</td><td>half</td><td>SetPadValue padding</td><td>Pad 12 elements on the right, requires SetPadValue to set padding value to 1</td></tr>
<tr><td>2</td><td>[32, 59]</td><td>[32, 64]</td><td>float</td><td>rightPadding padding</td><td>Pad 5 elements on the right, default padding value is 0, no SetPadValue needed</td></tr>
<tr><td>3</td><td>[3, 24]</td><td>[1, 80]</td><td>half</td><td>Compact padding</td><td>Compact mode, pad 16 bytes on the right of the last data block (only supports dav-3510)</td></tr>
<tr><td>4</td><td>[1, 320]</td><td>[1, 576]</td><td>int8</td><td>LoopMode transfer (Compact)</td><td>SetLoopModePara enables loop mode, Compact mode, implements GM->UB non-contiguous stride transfer (only supports dav-3510)</td></tr>
<tr><td>5</td><td>[1, 320]</td><td>[1, 576]</td><td>int8</td><td>LoopMode transfer (Normal)</td><td>SetLoopModePara enables loop mode, Normal mode, implements GM->UB non-contiguous stride transfer (only supports dav-3510)</td></tr>
<tr><td>6</td><td>[2, 4, 3, 128, 126]</td><td>[512, 128]</td><td>int8</td><td>LoopMode transfer (Normal)</td><td>Five-dimensional data transfer, transfers [2, 2, 2, 64, 126], pads 2 bytes per row to 128 bytes (only supports dav-3510)</td></tr>
</table>

### Detailed Scenario Description

**Scenario 1: Custom Padding Using SetPadValue**
- Input shape: [1, 20]
- Output shape: [1, 32]
- Data type: half
- Parameter configuration: isPad=false, leftPadding=0, rightPadding=12
- Description: Uses SetPadValue to set the padding value to 1, padding 12 elements on the right. **SetPadValue requires the user to make an additional call to set the padding value**, used with isPad=false.

**Scenario 2: Default Padding Using rightPadding**
- Input shape: [32, 59]
- Output shape: [32, 64]
- Data type: float
- Parameter configuration: isPad=true, leftPadding=0, rightPadding=5
- Description: **No need to use SetPadValue**. When isPad=true, the default padding value is 0, padding 5 elements on the right.

**Scenario 3: Data Transfer Using Compact Mode ----This scenario is only supported on Ascend 950PR/Ascend 950DT products**
- Input shape: [3, 24]
- Output shape: [1, 80]
- Data type: half
- Parameter configuration: blockLen=48, blockCount=3, leftPadding=0, rightPadding=16, isPad=false
- Description: Compact mode allows a single transfer to be unaligned, uniformly padding to 32-byte alignment at the end of the entire data block. In this example, leftPadding is 0, rightPadding is 16, padding 16 bytes on the right of the last data block. The destination operand data volume is 160 bytes.

**Scenario 4: Enable Loop Mode Using SetLoopModePara (Compact Mode) ----This scenario is only supported on Ascend 950PR/Ascend 950DT products**
- Input shape: [1, 320], as shown in Figure 1
- Output shape: [1, 576], as shown in Figure 2
- Data type: int8
- Parameter configuration:
  - GM->UB: LoopModeParams{loop1Size=2, loop2Size=2, loop1SrcStride=80, loop1DstStride=128, loop2SrcStride=160, loop2DstStride=288}, DataCopyMVType::OUT_TO_UB
  - DataCopyExtParams: BLOCK_COUNT=2, BLOCK_LEN=40 (using constexpr constants)
  - DataCopyPadExtParams: isPad=true, leftPadding=0, rightPadding=0, padValue=-1
- Description: Enables loop mode through SetLoopModePara, using Compact mode to implement GM->UB non-contiguous stride data transfer. In Compact mode, after each inner loop transfers 80B, it pads 16B to make it 96B aligned, with the padding value set to -1.

**Figure 1**  Source operand transfer scenario example

<img src="figures/datacopypad1.png" width="80%">

**Figure 2**  Destination operand Compact mode transfer scenario example

<img src="figures/datacopypad2.png">

**Scenario 5: Enable Loop Mode Using SetLoopModePara (Normal Mode) ----This scenario is only supported on Ascend 950PR/Ascend 950DT products**
- Input shape: [1, 320], as shown in Figure 1
- Output shape: [1, 576], as shown in Figure 3
- Data type: int8
- Parameter configuration:
  - GM->UB: LoopModeParams{loop1Size=2, loop2Size=2, loop1SrcStride=80, loop1DstStride=128, loop2SrcStride=160, loop2DstStride=288}, DataCopyMVType::OUT_TO_UB
  - DataCopyExtParams: BLOCK_COUNT=2, BLOCK_LEN=40 (using constexpr constants)
  - DataCopyPadExtParams: isPad=true, leftPadding=0, rightPadding=0, padValue=-1
- Description: Enables loop mode through SetLoopModePara, using Normal mode to implement GM->UB non-contiguous stride data transfer. In Normal mode, after each block transfer, it pads 24B to make it 64B aligned, with the padding value set to -1.

**Figure 3**  Destination operand Normal mode transfer scenario example

<img src="figures/datacopypad3.png">

**Scenario 6: Five-Dimensional Data Transfer Using SetLoopModePara (Normal Mode) ----This scenario is only supported on Ascend 950PR/Ascend 950DT products**
- Input shape: [2, 4, 3, 128, 126], five-dimensional data
- Output shape: [512, 128], stored contiguously in UB
- Data type: int8
- Transfer specification: [2, 2, 2, 64, 126], each 126 bytes padded with 2 bytes to 128 bytes
- Parameter configuration:
  - GM->UB: LoopModeParams{loop1Size=2, loop2Size=2, loop1SrcStride=128\*126, loop1DstStride=64\*128, loop2SrcStride=3\*128\*126, loop2DstStride=2\*64\*128}, DataCopyMVType::OUT_TO_UB
  - DataCopyExtParams: blockCount=64, blockLen=126, srcStride=0, dstStride=0
  - DataCopyPadExtParams: isPad=true, leftPadding=0, rightPadding=0, padValue=0
  - Dimension 0 uses a for loop to transfer 2 times

## Build and Run

Run the following steps in the root directory of this example to build and run it.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
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
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin -scenarioNum=$SCENARIO_NUM  # Verify whether the output result is correct
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2`, `3`, `4`, `5`, `6` | Scenario number: 1 (SetPadValue padding), 2 (rightPadding), 3 (Compact mode), 4 (SetLoopModePara loop mode Compact), 5 (SetLoopModePara loop mode Normal), 6 (five-dimensional LoopMode Normal) |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
