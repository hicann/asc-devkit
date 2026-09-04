# data_copy_gm2l1 Example

## Overview

This example introduces how to use DataCopy to transfer input data from GM (Global Memory) to L1 (L1 Buffer), supporting multiple input formats (Nz, ND, DN), Bias matrix load, and quantization matrix load functionalities.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```
├── data_copy_gm2l1
│   ├── figures                        // Illustrations
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   │   └── verify_result.py           // Verification script to check output data against ground truth
│   ├── data_utils.h                   // Data read/write functions
│   ├── CMakeLists.txt                 // cmake build file
│   ├── data_copy_gm2l1.asc            // Ascend C example implementation & invocation example
│   ├── README.md                      // Chinese example documentation
│   └── README_en.md                   // English example documentation
```

## Detailed Scenario Description

This example selects different input scenarios through the build parameter `SCENARIO_NUM`. The meanings of different SCENARIO_NUM values are shown in the following table. All scenarios are based on the same matrix multiplication specification: [M, K, N] = [128, 128, 256], and the kernel function name is `data_copy_gm2l1`.

<table>
<caption style="font-weight: normal;">
   	     <span style="font-weight: bold; font-size: 1.2em;">Table 1: Meanings of Different scenarioNum Values</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">Input Format</td><td align="center">Input Data Type</td><td align="center">Output Data Type</td><td align="center">Bias Enabled</td><td align="center">Vector Quantization Enabled</td></tr>
<tr><td align="center">1</td><td align="center">Nz</td><td align="center">half</td><td align="center">float</td><td align="center">No</td><td align="center">No</td></tr>
<tr><td align="center">2</td><td align="center">ND</td><td align="center">half</td><td align="center">float</td><td align="center">No</td><td align="center">No</td></tr>
<tr><td align="center">3</td><td align="center">DN</td><td align="center">half</td><td align="center">float</td><td align="center">No</td><td align="center">No</td></tr>
<tr><td align="center">4</td><td align="center">ND</td><td align="center">half</td><td align="center">float</td><td align="center">Yes</td><td align="center">No</td></tr>
<tr><td align="center">5</td><td align="center">ND</td><td align="center">half</td><td align="center">int8_t</td><td align="center">No</td><td align="center">Yes</td></tr>
</table>

**Scenario 1: Input Format Nz, Input Data Type half**
- Input: A [128, 128] half type, Nz format; B [128, 256] half type, Nz format
- Output: C [128, 256] float type, ND format
- Implementation: Uses `DataCopyParams` to transfer Nz format input data from GM to L1
- Description: Input data is in Nz format, transferred directly to L1 without format conversion
<p align="center">
  <img src="figures/data_copy_gm2l1_NZ2NZ.png" width="800">
</p>

**Scenario 2: Input Format ND, Input Data Type half**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] float type, ND format
- Implementation: Uses `Nd2NzParams` to transfer ND format input data from GM to L1, automatically converting to Nz format
- Description: Input data is in ND format, automatically converted to Nz format during transfer
<p align="center">
  <img src="figures/data_copy_gm2l1_ND2NZ.png" width="800">
</p>

**Scenario 3: Input Format DN, Input Data Type half (Only Supported on Ascend 950PR/Ascend 950DT)**
- Input: A [128, 128] half type, DN format; B [128, 256] half type, DN format
- Output: C [128, 256] float type, ND format
- Implementation: Uses `Dn2NzParams` to transfer DN format input data from GM to L1, automatically converting to Nz format
- Description: Input data is in DN format, automatically converted to Nz format during transfer. DN format is only supported on Ascend 950PR/Ascend 950DT
<p align="center">
  <img src="figures/data_copy_gm2l1_DN2NZ.png" width="800">
</p>

**Scenario 4: Input Format ND, Input Data Type half, Bias Enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format; Bias [256] float type
- Output: C [128, 256] float type, ND format
- Implementation: Uses `Nd2NzParams` to transfer input data and Bias from GM to L1, adding Bias during matrix multiplication computation through `Mmad(c, a, b, bias, mmadParams)`
- Description: Bias is added during matrix multiplication computation. Bias is transferred from C1 to C2 and then participates in the matrix multiplication computation

**Scenario 5: Input Format ND, Input Data Type half, Vector Quantization Enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format; Quantization parameters [256] uint64_t type
- Output: C [128, 256] int8_t type, ND format
- Implementation: Uses `Nd2NzParams` to transfer input data from GM to L1, performs matrix multiplication computation, transfers quantization parameters from GM to L1 via `DataCopy`, then uses `Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>(cGM, c, quantAlphaTensor, fixpipeParams)` for Vector quantization output
- Description: The matrix multiplication result is float type, converted to int8_t type through Vector quantization. Each column of the C matrix corresponds to one quantization parameter

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
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-5 | Scenario number, scenario 3 only supports dav-3510 architecture |

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
