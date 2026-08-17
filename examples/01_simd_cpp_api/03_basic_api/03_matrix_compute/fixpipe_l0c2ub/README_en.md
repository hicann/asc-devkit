# fixpipe_l0c2ub Example

## Overview

This example introduces how to use Fixpipe to transfer matrix multiplication results from L0C Buffer to UB (Unified Buffer), supporting multiple output formats (Nz, ND) and dual-target mode (split by M dimension or N dimension). These interfaces efficiently transfer matrix multiplication computation results from L0C to the unified buffer area, with support for various data format conversions and splitting capabilities.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── fixpipe_l0c2ub
│   ├── scripts
│   │   ├── gen_data.py                // Script for generating input data and ground truth data
│   │   └── verify_result.py           // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── figures                        // Illustrations
│   ├── fixpipe_l0c2ub.asc             // Ascend C example implementation & invocation example
│   └── README.md                      // Example documentation
```

## Scenario Description

This example selects different output scenarios through the compilation parameter `SCENARIO_NUM`. The meanings of different SCENARIO_NUM values are shown in the table below. All scenarios are based on the same matrix multiplication specification: [M, N, K] = [128, 256, 128], with kernel function name `fixpipe_l0c2ub`.

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Meaning of Different scenarioNum Values</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">L0C Data Type</td><td align="center">Output Data Type</td><td align="center">Output Format</td><td align="center">Dual-Target Mode</td><td align="center">Split Dimension</td></tr>
<tr><td align="center">1</td><td align="center">float</td><td align="center">float</td><td align="center">Nz</td><td align="center">No</td><td align="center">-</td></tr>
<tr><td align="center">2</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">No</td><td align="center">-</td></tr>
<tr><td align="center">3</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">Yes</td><td align="center">M dimension</td></tr>
<tr><td align="center">4</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">Yes</td><td align="center">N dimension</td></tr>
</table>

**Scenario 1: Output format Nz, output data type float**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] float type, Nz format
- Implementation: Use `Fixpipe<outputType, l0cType, CFG_NZ_UB>` to transfer data from L0C to UB, output in Nz format
- Description: L0C data in Nz format directly output to UB in Nz format, data remains unchanged

**Scenario 2: Output format ND, output data type float**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] float type, ND format
- Implementation: Use `Fixpipe<outputType, l0cType, CFG_ROW_MAJOR_UB>` to specify ROW_MAJOR format conversion
- Description: Convert Nz format data in CO1 to ND format output to UB

**Scenario 3: Output format ND, output data type float, dual-target mode enabled, split by M dimension, writing to two sub-blocks (SUB BLOCK) UB simultaneously**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [64, 256] float type per sub-block, ND format (dual-target mode, split by M dimension, each target outputs 64 rows)
- Implementation: Set `fixpipeParams.dualDstCtl = 0b01`, split by M dimension, M must be a multiple of 2
- Description: Use dual-target mode to split data output to UB, two cores split by M dimension each process half the data
<p align="center">
  <img src="figures/fixpipe_l0c2ub_split_m.png" width="500">
</p>

**Scenario 4: Output format ND, output data type float, dual-target mode enabled, split by N dimension, writing to two sub-blocks (SUB BLOCK) UB simultaneously**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 128] float type per sub-block, ND format (dual-target mode, split by N dimension, each target outputs 128 columns)
- Implementation: Set `fixpipeParams.dualDstCtl = 0b10`, split by N dimension, N must be a multiple of 32
- Description: Use dual-target mode to split data output to UB, two cores split by N dimension each process half the data
<p align="center">
  <img src="figures/fixpipe_l0c2ub_split_n.png" width="500">
</p>

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
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-4 | Scenario number |

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
