# fixpipe_l0c2gm Example

## Overview

This example introduces how to use Fixpipe to transfer matrix multiplication results from L0C Buffer to GM (Global Memory), supporting multiple output formats (Nz, ND, DN), data type conversion, inline quantization, ReLU, and ChannelSplit features. These interfaces efficiently transfer matrix multiplication computation results from L0C to global memory, with support for various data format conversions and preprocessing capabilities.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── fixpipe_l0c2gm
│   ├── figures                        // Illustrations
│   ├── scripts
│   │   ├── gen_data.py                // Script for generating input data and ground truth data
│   │   └── verify_result.py           // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── fixpipe_l0c2gm.asc             // Ascend C example implementation & invocation example
│   └── README.md                      // Example documentation
```

## FixpipeParamsV220 vs FixpipeParamsArch3510 Structure Comparison

Different products support different parameter structures:
- **Ascend 950PR/Ascend 950DT**: Supports both `FixpipeParamsV220` and `FixpipeParamsArch3510` parameter structures, `FixpipeParamsArch3510` is recommended
- **Atlas A3 Training/Inference Series, Atlas A2 Training/Inference Series**: Only supports `FixpipeParamsV220`

This example selects different architectures through the compilation parameter `CMAKE_ASC_ARCHITECTURES`, automatically selecting the corresponding parameter structure based on architecture:
- `dav-2201` architecture: Uses `FixpipeParamsV220`
- `dav-3510` architecture: Uses `FixpipeParamsArch3510`

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Parameter Structure Comparison</span></caption>
<tr><td rowspan="1" align="center">Member Name</td><td align="center">FixpipeParamsV220</td><td align="center">FixpipeParamsArch3510</td><td align="center">Description</td></tr>
<tr><td align="center"><code>nSize</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Output matrix size in N direction</td></tr>
<tr><td align="center"><code>mSize</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Output matrix size in M direction</td></tr>
<tr><td align="center"><code>srcStride</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Start address offset of adjacent Z layouts in source Nz matrix</td></tr>
<tr><td align="center"><code>dstStride</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Start address offset of adjacent Z layouts in destination matrix (Nz format) or elements per row (ND/DN format)</td></tr>
<tr><td align="center"><code>quantPre</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Quantization mode control</td></tr>
<tr><td align="center"><code>deqScalar</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Scalar quantization parameter</td></tr>
<tr><td align="center"><code>reluEn</code></td><td align="center">Yes</td><td align="center">Yes</td><td>ReLU switch</td></tr>
<tr><td align="center"><code>unitFlag</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Mmad and Fixpipe fine-grained parallelism control</td></tr>
<tr><td align="center"><code>isChannelSplit</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Channel split switch</td></tr>
<tr><td align="center"><code>ndNum</code> / <code>srcNdStride</code> / <code>dstNdStride</code></td><td align="center">Yes</td><td align="center">Yes (in <code>TransformParams</code>)</td><td>NZ2ND scenario multi-matrix transfer parameters, independent members in V220, integrated into <code>TransformParams</code> in Arch3510</td></tr>
<tr><td align="center"><code>dnNum</code> / <code>srcNzMatrixStride</code> / <code>dstDnMatrixStride</code> / <code>srcNzC0Stride</code></td><td align="center">No</td><td align="center">Yes (in <code>TransformParams</code>)</td><td>NZ2DN scenario multi-matrix transfer parameters, only Arch3510 supported</td></tr>
<tr><td align="center"><code>TransformParams</code></td><td align="center">No</td><td align="center">Yes</td><td>Template parameter-based type selector, auto-selects parameter type based on CO2Layout</td></tr>
<tr><td align="center"><code>dualDstCtrl</code></td><td align="center">No</td><td align="center">Yes</td><td>Dual-target mode control, supports M or N dimension splitting</td></tr>
<tr><td align="center"><code>subBlockId</code></td><td align="center">No</td><td align="center">Yes</td><td>Target UB number in single-target mode</td></tr>
</table>

## Scenario Description

This example selects different output scenarios through the compilation parameter `SCENARIO_NUM`. The meanings of different SCENARIO_NUM values are shown in the table below. All scenarios are based on the same matrix multiplication specification: [M, N, K] = [128, 256, 128], with kernel function name `fixpipe_l0c2gm`.

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: Meaning of Different scenarioNum Values</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">L0C Data Type</td><td align="center">Output Data Type</td><td align="center">Output Format</td><td align="center">Quantization Enabled</td><td align="center">ReLU Enabled</td><td align="center">ChannelSplit Enabled</td></tr>
<tr><td align="center">1</td><td align="center">float</td><td align="center">float</td><td align="center">Nz</td><td align="center">No</td><td align="center">No</td><td align="center">No</td></tr>
<tr><td align="center">2</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">No</td><td align="center">No</td><td align="center">No</td></tr>
<tr><td align="center">3</td><td align="center">float</td><td align="center">float</td><td align="center">DN</td><td align="center">No</td><td align="center">No</td><td align="center">No</td></tr>
<tr><td align="center">4</td><td align="center">float</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">Yes</td><td align="center">No</td><td align="center">No</td></tr>
<tr><td align="center">5</td><td align="center">float</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">Yes</td><td align="center">No</td><td align="center">No</td></tr>
<tr><td align="center">6</td><td align="center">float</td><td align="center">float</td><td align="center">ND</td><td align="center">No</td><td align="center">Yes</td><td align="center">No</td></tr>
<tr><td align="center">7</td><td align="center">float</td><td align="center">float</td><td align="center">Nz</td><td align="center">No</td><td align="center">No</td><td align="center">Yes</td></tr>
</table>

**Scenario 1: Output format Nz, output data type float**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] float type, Nz format
- Implementation: Use `Fixpipe<outputType, l0cType, AscendC::CFG_NZ>` to transfer data from L0C to GM, output in Nz format
- Description: L0C data in Nz format directly output to GM in Nz format, data remains unchanged
<p align="center">
  <img src="figures/fixpipe_l0c2gm_NZ2NZ.png" width="800">
</p>

**Scenario 2: Output format ND, output data type float**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] float type, ND format
- Implementation: Use `Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>` to specify ROW_MAJOR format conversion
- Description: Convert Nz format data in CO1 to ND format output to GM. ND format has no alignment requirements like Nz format; parameters should be configured based on actual size during output
<p align="center">
  <img src="figures/fixpipe_l0c2gm_NZ2ND.png" width="800">
</p>

**Scenario 3: Output format DN, output data type float (only supported on Ascend 950PR/Ascend 950DT)**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [256, 128] float type, DN format
- Implementation: Use `Fixpipe<outputType, l0cType, AscendC::CFG_COLUMN_MAJOR>` to specify COLUMN_MAJOR format conversion
- Description: Convert Nz format data in CO1 to DN format output to GM
<p align="center">
  <img src="figures/fixpipe_l0c2gm_NZ2DN.png" width="800">
</p>

**Scenario 4: Output format ND, output data type int8_t, Scalar quantization enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] int8_t type, ND format
- Implementation: Set `fixpipeParams.quantPre = QuantMode_t::QF322B8_PRE`, use Scalar quantization mode
- Description: Quantize float type data to int8_t type, entire C matrix uses one quantization parameter

**Scenario 5: Output format ND, output data type int8_t, Vector quantization enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] int8_t type, ND format
- Implementation: Set `fixpipeParams.quantPre = QuantMode_t::VQF322B8_PRE`, use Vector quantization mode, pass per-column quantization parameters through quantAlphaTensor
- Description: Quantize float type data to int8_t type, each column of C matrix corresponds to one quantization parameter, quantization parameters need to be copied from GM to L1

**Scenario 6: Output format ND, output data type float, ReLU enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] float type, ND format
- Implementation: Set `fixpipeParams.reluEn = true` to enable ReLU feature
- Description: Perform ReLU operation during data transfer from L0C to GM, setting negative values to 0

**Scenario 7: Output format Nz, output data type float, ChannelSplit enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 512] float type, Nz format (channel split enabled)
- Implementation: Set `fixpipeParams.isChannelSplit = true` to enable ChannelSplit feature
- Description: Enable channel split during data transfer from L0C to GM, splitting 16x16 small z fractal matrices into two independent 16x8 small z fractal matrices output to GM. Fixpipe interface input and output must both be float type, and only Nz format is supported

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
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # Verify whether the output result is correct
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
  | `SCENARIO_NUM` | 1-7 | Scenario number |

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
