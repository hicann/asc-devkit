# fixpipe_l0c2l1 Example

## Overview

This example introduces how to use Fixpipe to transfer matrix multiplication results from L0C (L0C Buffer) to L1 (L1 Buffer), supporting data type conversion, inline quantization, ReLU, and other features. These interfaces efficiently transfer matrix multiplication computation results from L0C to L1 Buffer, with support for various data format conversions and preprocessing capabilities.

Note:
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products and Atlas A2 Training Series Products/Atlas A2 Inference Series Products only support Nz output format in the L0C to L1 path, and do not support float output data type; it must be quantized to other data types.
- Ascend 950PR/Ascend 950DT does not support direct data transfer from L1 to GM. Therefore, in this example, the result matrix transferred from L0C to L1 serves as input for the next matrix multiplication, performing another matrix computation and outputting the result to GM. (Atlas A2/A3 series products support direct data transfer from L1 to GM; this example chooses direct transfer out)

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```
├── fixpipe_l0c2l1
│   ├── scripts
│   │   ├── gen_data.py                // Script for generating input data and ground truth data
│   │   └── verify_result.py           // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── fixpipe_l0c2l1.asc             // Ascend C example implementation & invocation example
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
<tr><td align="center"><code>dstStride</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Start address offset of adjacent Z layouts in destination matrix (Nz format, note unit differences between structures) or elements per row (ND/DN format)</td></tr>
<tr><td align="center"><code>quantPre</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Quantization mode control</td></tr>
<tr><td align="center"><code>deqScalar</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Scalar quantization parameter</td></tr>
<tr><td align="center"><code>reluEn</code></td><td align="center">Yes</td><td align="center">Yes</td><td>ReLU switch</td></tr>
<tr><td align="center"><code>unitFlag</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Mmad and Fixpipe fine-grained parallelism control</td></tr>
<tr><td align="center"><code>isChannelSplit</code></td><td align="center">Yes</td><td align="center">Yes</td><td>Channel split switch</td></tr>
<tr><td align="center"><code>ndNum</code> / <code>srcNdStride</code> / <code>dstNdStride</code></td><td align="center">Yes</td><td align="center">Yes (in <code>TransformParams</code>)</td><td>Parameters controlling multi-matrix transfer in Nz2ND scenarios, independent members in V220, integrated into <code>TransformParams</code> structure in Arch3510</td></tr>
<tr><td align="center"><code>dnNum</code> / <code>srcNzMatrixStride</code> / <code>dstDnMatrixStride</code> / <code>srcNzC0Stride</code></td><td align="center">No</td><td align="center">Yes (in <code>TransformParams</code>)</td><td>Parameters controlling multi-matrix transfer in Nz2DN scenarios, only Arch3510 supported</td></tr>
<tr><td align="center"><code>TransformParams</code></td><td align="center">No</td><td align="center">Yes</td><td>Template parameter-based type selector, automatically selects parameter type based on CO2Layout</td></tr>
<tr><td align="center"><code>dualDstCtrl</code></td><td align="center">No</td><td align="center">Yes</td><td>Dual-target mode control, supports M dimension or N dimension splitting</td></tr>
<tr><td align="center"><code>subBlockId</code></td><td align="center">No</td><td align="center">Yes</td><td>Target UB number indicator in single-target mode</td></tr>
</table>

## Scenario Description

This example selects different output scenarios through the compilation parameter `SCENARIO_NUM`. The meanings of different `SCENARIO_NUM` values are shown in the table below.
All scenarios are based on the same matrix multiplication specification: [M, N, K] = [128, 128, 128], with kernel function name `fixpipe_l0c2l1`.

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: Meaning of Different scenarioNum Values</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">L0C Data Type</td><td align="center">L1 Data Type</td><td align="center">Output Format</td><td align="center">Quantization Enabled</td><td align="center">ReLU Enabled</td></tr>
<tr><td align="center">1</td><td align="center">float</td><td align="center">half</td><td align="center">Nz</td><td align="center">No (cast)</td><td align="center">No</td></tr>
<tr><td align="center">2</td><td align="center">float</td><td align="center">int8_t</td><td align="center">Nz</td><td align="center">Yes (scalar)</td><td align="center">No</td></tr>
<tr><td align="center">3</td><td align="center">float</td><td align="center">int8_t</td><td align="center">Nz</td><td align="center">Yes (vector)</td><td align="center">No</td></tr>
<tr><td align="center">4</td><td align="center">float</td><td align="center">half</td><td align="center">Nz</td><td align="center">No (cast)</td><td align="center">Yes</td></tr>
</table>

**Scenario 1: Output format Nz, output to L1 data type half**
- Input: A [128, 128] half type, ND format; B [128, 128] half type, ND format
- Output: C [128, 128] half type, Nz format
- Implementation: Use `Fixpipe<outputType, l0cType, AscendC::CFG_Nz>` to transfer data from L0C to L1, output in Nz format
- Description: L0C data in Nz format directly output to L1 in Nz format, data remains unchanged

**Scenario 2: Output format Nz, output to L1 data type int8_t, Scalar quantization enabled**
- Input: A [128, 128] half type, ND format; B [128, 128] half type, ND format
- Output: C [128, 128] int8_t type, Nz format
- Implementation: Set `fixpipeParams.quantPre = QuantMode_t::QF322B8_PRE`, use Scalar quantization mode
- Description: Quantize float type data to int8_t type, entire C matrix uses one quantization parameter

**Scenario 3: Output format Nz, output to L1 data type int8_t, Vector quantization enabled**
- Input: A [128, 128] half type, ND format; B [128, 128] half type, ND format
- Output: C [128, 128] int8_t type, Nz format
- Implementation: Set `fixpipeParams.quantPre = QuantMode_t::VQF322B8_PRE`, use Vector quantization mode, pass per-column quantization parameters through quantAlphaTensor
- Description: Quantize float type data to int8_t type, each column of C matrix corresponds to one quantization parameter, quantization parameters need to be copied from GM to L1

**Scenario 4: Output format Nz, output to L1 data type half, ReLU enabled**
- Input: A [128, 128] half type, ND format; B [128, 128] half type, ND format
- Output: C [128, 128] half type, Nz format
- Implementation: Set `fixpipeParams.reluEn = true` to enable ReLU feature
- Description: Perform ReLU operation during data transfer from L0C to L1, setting negative values to 0

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
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project (default dav-2201 NPU mode)
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM  # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin $SCENARIO_NUM # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-4 | Scenario number |

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
