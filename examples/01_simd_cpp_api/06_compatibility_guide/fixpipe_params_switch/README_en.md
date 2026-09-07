# Fixpipe Parameter Structure Switch Compatibility Example

## Overview

This example demonstrates the switch between different parameter structures for the Fixpipe interface across the 2201 and 3510 architectures, isolating different hardware implementations through compile-time macros. The example includes both NZ2ND and NZ2NZ scenarios, highlighting the dstStride unit difference.

The Fixpipe interface transfers matrix multiplication results from L0C Buffer to GM. The 3510 architecture introduces a new native parameter structure `FixpipeParamsArch3510`, which has key field differences from the `FixpipeParamsV220` used by the 2201 architecture.

- Atlas A2/A3 Training/Inference Series Products: Use the `FixpipeParamsV220` parameter structure.
- Ascend 950PR/950DT: Use the `FixpipeParamsArch3510` native parameter structure. The 3510 also supports `FixpipeParamsV220` (auto-converted internally), but the native structure is recommended for full capabilities.

### Key Differences Between the Two Parameter Structures

| Comparison Item | FixpipeParamsV220 (2201) | FixpipeParamsArch3510 (3510) |
|-----------------|-------------------------|------------------------------|
| NZ2ND parameters | `ndNum`/`srcNdStride`/`dstNdStride` as top-level fields | Enclosed in `params` sub-structure (type selected by `format` template) |
| dstStride unit (NZ2NZ scenario) | datablock (32 bytes) | element |
| dstStride unit (NZ2ND scenario) | element | element (same as V220) |
| Advanced ReLU | Not supported | Supports `preReluMode`/`preClipReluMode`/`reluScalar`/`vectorRelu` |
| NZ2DN format conversion | Not supported | Supports `CO2Layout::COLUMN_MAJOR` |
| UB pathway control | Not supported | Supports `dualDstCtl`/`subBlockId` |
| Quantization modes | Basic quantization modes | Extended FP8/HIF8/BF16 etc. |

> **Migration Note**: In NZ2NZ (non-NZ2ND) scenarios, the dstStride unit changes from datablock to element. Directly reusing V220 values would cause address calculation errors on 3510. In NZ2ND (CFG_ROW_MAJOR) scenarios, both structures use element as the dstStride unit, allowing unified assignment.

### Scenario Description

| Scenario | Name | Output Format | dstStride Unit Difference | Description |
|---------|------|--------------|--------------------------|-------------|
| 1 | NZ2ND | ND (CFG_ROW_MAJOR) | No difference (both element) | Common params can be unified |
| 2 | NZ2NZ | NZ (CFG_NZ) | Different (V220=datablock, Arch3510=element) | dstStride must be assigned separately |

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── fixpipe_params_switch
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Verify whether output data matches ground truth
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── fixpipe_params_switch.asc // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Specifications

| Category | name | shape | data type | format |
|----------|------|-------|-----------|--------|
| Example Input | x | [128, 128] | half | ND |
| Example Input | y | [128, 256] | half | ND |
| Example Output (Scenario 1) | z | [128, 256] | float | ND |
| Example Output (Scenario 2) | z | [128, 256] | float | NZ |
| Kernel Function Name | fixpipe_params_switch | | | |

## Example Implementation

Data flow: GM -> L1 Buffer -> L0A/L0B Buffer -> L0C Buffer -> GM.

1. Copy matrices A and B from GM to L1 Buffer (NZ format).
2. Copy matrix A from L1 to L0A Buffer: 2201 requires NZ→ZZ fractal conversion (legacy `LoadData2DParams` loop), 3510 directly uses NZ (`LoadData2DParamsV2`).
3. Copy matrix B from L1 to L0B Buffer (`LoadData2DParams` loop).
4. Call `Mmad` for matrix multiplication, results written to L0C Buffer.
5. Call `Fixpipe` to transfer results from L0C to GM, using different parameter structures by scenario and architecture:
   - **Scenario 1 (NZ2ND)**: 2201 uses `FixpipeParamsV220` (set top-level `ndNum`/`srcNdStride`/`dstNdStride`), while 3510 uses `FixpipeParamsArch3510<ROW_MAJOR>` (set `params.ndNum`/`params.srcNdStride`/`params.dstNdStride`). Common params have identical field names, dstStride unit is element for both, and can be unified.
   - **Scenario 2 (NZ2NZ)**: 2201 uses `FixpipeParamsV220` with dstStride unit=datablock(32B); 3510 uses `FixpipeParamsArch3510<NZ>` with dstStride unit=element. dstStride must be calculated separately.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory. Select scenario with `-DSCENARIO_NUM=1` or `-DSCENARIO_NUM=2`.

  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 ..;make -j;   # Scenario 1: NZ2ND
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin

  # Clear cmake cache before switching scenarios
  rm CMakeCache.txt
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=2 ..;make -j;   # Scenario 2: NZ2NZ
  python3 ../scripts/gen_data.py
  SCENARIO_NUM=2 ./demo
  SCENARIO_NUM=2 python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  To build for Ascend 950PR/950DT:

  ```bash
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1 ..;make -j;
  ```

  > **Notice:** Clear the cmake cache before switching build modes or scenarios. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 for Atlas A2/A3, dav-3510 for Ascend 950PR/950DT |
  | `SCENARIO_NUM` | `1` (default), `2` | Scenario: 1=NZ2ND (CFG_ROW_MAJOR), 2=NZ2NZ (CFG_NZ) |

- Execution results

  The following execution result indicates that the accuracy comparison succeeded:

  ```bash
  test pass!
  ```
