# DataCopy Multi-dimensional Data Transfer Example

## Overview

This example introduces how to use the multi-dimensional data transfer API to implement data transfer on the GM (Global Memory) to UB (Unified Buffer) path. By freely configuring the transfer dimension information and corresponding Stride, it can be used for various data transformation operations such as Padding, Transpose, BroadCast, and Slice.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── data_copy_gm2ub_nddma
│   ├── scripts
│   │   ├── gen_data.py                         // Input data and ground truth generation script
│   │   └── verify_result.py                    // Verification script to check output data against ground truth
│   ├── CMakeLists.txt                          // Build configuration file
│   ├── data_utils.h                            // Data read/write functions
│   ├── multidimensional_data_movement.asc      // Ascend C example implementation & invocation example
│   └── README.md                               // Example description document
```

## Scenario Description

This example selects different scenarios through the build parameter `SCENARIO_NUM`. All scenarios use two-dimensional ND format, and the kernel function name is `datacopy_custom`.

<table border="2">
<caption>Table 1: Scenario Configuration Comparison</caption>
<tr><th>scenarioNum</th><th>Input Shape</th><th>Output Shape</th><th>Data Type</th><th>Description</th></tr>
<tr><td>1</td><td>[16, 32]</td><td>[32, 64]</td><td>float</td><td>Padding scenario</td></tr>
<tr><td>2</td><td>[28, 15]</td><td>[32, 32]</td><td>float</td><td>Padding scenario with nearest value fill mode enabled</td></tr>
<tr><td>3</td><td>[16, 64]</td><td>[64, 16]</td><td>float</td><td>Transpose scenario</td></tr>
<tr><td>4</td><td>[1, 16]</td><td>[3, 16]</td><td>float</td><td>BroadCast scenario</td></tr>
<tr><td>5</td><td>[32, 64]</td><td>[16, 16]</td><td>float</td><td>Slice scenario</td></tr>
</table>

### Detailed Scenario Description

**Scenario 1: Padding Scenario**
- Input: [16, 32] float elements
- Output: [32, 64] float elements
- Parameter configuration: NdDmaLoopInfo={{1, 32}, {1, 64}, {32, 16}, {15, 13}, {17, 3}}, paddingValue=0
- Description: Transfers [16, 32] data from GM to UB and pads it to [32, 64], with left padding of 15, top padding of 13, right padding of 17, and bottom padding of 3, filling padding values with 0

**Scenario 2: Padding Scenario with Nearest Value Fill Mode Enabled**
- Input: [28, 15] float elements
- Output: [32, 32] float elements
- Parameter configuration: NdDmaLoopInfo={{1, 15}, {1, 32}, {15, 28}, {11, 3}, {6, 1}}, isNearestValueMode=true
- Description: Transfers [28, 15] data from GM to UB and pads it to [32, 32], enabling nearest value fill mode where padding positions are filled with boundary data instead of 0

**Scenario 3: Transpose Scenario**
- Input: [16, 64] float elements
- Output: [64, 16] float elements
- Parameter configuration: NdDmaLoopInfo={{1, 64}, {16, 1}, {64, 16}, {0, 0}, {0, 0}}
- Description: Transfers [16, 64] data from GM to UB and transposes it to [64, 16], achieving row-column swap through stride configuration

**Scenario 4: BroadCast Scenario**
- Input: [1, 16] float elements
- Output: [3, 16] float elements
- Parameter configuration: NdDmaLoopInfo={{1, 0}, {1, 16}, {16, 3}, {0, 0}, {0, 0}}
- Description: Transfers [1, 16] data from GM to UB and broadcasts it to [3, 16], achieving row data replication by configuring stride to 0

**Scenario 5: Slice Scenario**
- Input: [32, 64] float elements
- Output: [16, 16] float elements
- Parameter configuration: NdDmaLoopInfo={{1, 64}, {1, 16}, {16, 16}, {0, 0}, {0, 0}}
- Description: Transfers [32, 64] data from GM to UB and extracts it to [16, 16], achieving the final data slice through transfer data volume configuration

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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu` | Run mode: NPU execution, CPU debug |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2`, `3`, `4`, `5` | Scenario number |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
