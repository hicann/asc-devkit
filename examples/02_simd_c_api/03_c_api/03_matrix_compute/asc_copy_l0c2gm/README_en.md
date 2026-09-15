# asc_copy_l0c2gm Example

## Overview

This example demonstrates how to use the SIMD C API `asc_copy_l0c2gm` to transfer matrix multiplication results from L0C Buffer to GM (Global Memory), supporting multiple output formats (Nz, ND, and DN), data type conversion, inline quantization, ReLU, and ChannelSplit. This API efficiently transfers matrix multiplication results from L0C Buffer to global memory and supports various data format conversions and preprocessing capabilities.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── asc_copy_l0c2gm
│   ├── figures                        // Illustrations
│   ├── scripts
│   │   ├── gen_data.py                // Script for generating input data and ground truth data
│   │   └── verify_result.py           // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── asc_copy_l0c2gm.asc             // SIMD C API implementation and invocation
│   └── README.md                      // Example documentation
```

## `asc_copy_l0c2gm` Parameters

This `dav-3510` example describes each transfer through the addresses, shape, strides, and feature controls of [`asc_copy_l0c2gm`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md). Layout conversion and quantization settings that require additional state are configured by `asc_set_l0c_copy_*` APIs.

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Main Parameters and Configuration APIs</span></caption>
<tr><td align="center">Parameter or API</td><td align="center">Description</td></tr>
<tr><td align="center"><code>n_size</code> / <code>m_size</code></td><td>Source Nz matrix size in the N and M directions</td></tr>
<tr><td align="center"><code>src_stride</code></td><td>Offset between adjacent Z layouts in the source Nz matrix, in 64-byte units</td></tr>
<tr><td align="center"><code>dst_stride</code></td><td>Element offset between Z layouts for Nz output, or elements per row for ND/DN output</td></tr>
<tr><td align="center"><code>quant_pre_mode</code></td><td>Preprocessing quantization mode; Scenarios 4 and 5 use <code>QF322B8_PRE</code> and <code>VQF322B8_PRE</code></td></tr>
<tr><td align="center"><code>relu_pre_mode</code></td><td>Preprocessing ReLU mode; Scenario 6 uses <code>asc_relu_pre_mode::NORMAL</code></td></tr>
<tr><td align="center"><code>enable_channel_split</code></td><td>ChannelSplit switch, enabled in Scenario 7</td></tr>
<tr><td align="center"><code>enable_nz2nd</code> / <code>enable_nz2dn</code></td><td>Nz-to-ND and Nz-to-DN conversion switches</td></tr>
<tr><td align="center"><code>asc_set_l0c_copy_nz_para</code></td><td>Configures the matrix count and inter-matrix strides for ND/DN output</td></tr>
<tr><td align="center"><code>asc_set_l0c_copy_channel_para</code></td><td>Configures the DN output channel count; Scenario 3 uses 1</td></tr>
<tr><td align="center"><code>asc_set_l0c_copy_prequant</code></td><td>Configures the scalar quantization value; Scenario 4 uses scale 2.0</td></tr>
<tr><td align="center"><code>asc_set_l0c_copy_config</code></td><td>Configures the vector quantization parameter address in Fixpipe Buffer</td></tr>
</table>

## Scenario Description

This example selects different output scenarios through the compilation parameter `SCENARIO_NUM`. The meanings of different SCENARIO_NUM values are shown in the table below. All scenarios are based on the same matrix multiplication specification: [M, N, K] = [128, 256, 128], with kernel function name `asc_copy_l0c2gm_custom`.

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: Meaning of Different scenarioNum Values</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">L0C Buffer Data Type</td><td align="center">Output Data Type</td><td align="center">Output Format</td><td align="center">Quantization Enabled</td><td align="center">ReLU Enabled</td><td align="center">ChannelSplit Enabled</td></tr>
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
- Implementation: Call `asc_copy_l0c2gm` with Nz2ND, Nz2DN, quantization, ReLU, and ChannelSplit disabled to write Nz directly
- Description: L0C Buffer data in Nz format directly output to GM in Nz format, data remains unchanged
<p align="center">
  <img src="figures/asc_copy_l0c2gm_NZ2NZ.png" width="800">
</p>

**Scenario 2: Output format ND, output data type float**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] float type, ND format
- Implementation: Call `asc_set_l0c_copy_nz_para(1, 0, 0)` for a single matrix, then call `asc_copy_l0c2gm` with `enable_nz2nd = true`
- Description: Convert Nz format data in CO1 to ND format output to GM. ND format has no alignment requirements like Nz format; parameters should be configured based on actual size during output
<p align="center">
  <img src="figures/asc_copy_l0c2gm_NZ2ND.png" width="800">
</p>

**Scenario 3: Output format DN, output data type float (only supported on Ascend 950PR/Ascend 950DT)**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [256, 128] float type, DN format
- Implementation: Call `asc_set_l0c_copy_channel_para(1)` and `asc_set_l0c_copy_nz_para(1, 0, 0)`, then call `asc_copy_l0c2gm` with `enable_nz2dn = true`
- Description: Convert Nz format data in CO1 to DN format output to GM
<p align="center">
  <img src="figures/asc_copy_l0c2gm_NZ2DN.png" width="800">
</p>

**Scenario 4: Output format ND, output data type int8_t, Scalar quantization enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] int8_t type, ND format
- Implementation: Call `asc_set_l0c_copy_prequant(2.0F, 0, true)` to configure scalar quantization, then call `asc_copy_l0c2gm` with `asc_quant_mode::QF322B8_PRE`
- Description: Quantize float type data to int8_t type, entire C matrix uses one quantization parameter

**Scenario 5: Output format ND, output data type int8_t, Vector quantization enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] int8_t type, ND format
- Implementation: Use `asc_copy_gm2l1` and `asc_copy_l12fb` to transfer per-column quantization values from GM through L1 Buffer to Fixpipe Buffer, configure the address with `asc_set_l0c_copy_config`, then call `asc_copy_l0c2gm` with `asc_quant_mode::VQF322B8_PRE`
- Description: Quantize float type data to int8_t type, each column of C matrix corresponds to one quantization parameter, quantization parameters need to be copied from GM to L1 Buffer

**Scenario 6: Output format ND, output data type float, ReLU enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 256] float type, ND format
- Implementation: Call `asc_copy_l0c2gm` with `relu_pre_mode = asc_relu_pre_mode::NORMAL`
- Description: Perform ReLU operation during data transfer from L0C Buffer to GM, setting negative values to 0

**Scenario 7: Output format Nz, output data type float, ChannelSplit enabled**
- Input: A [128, 128] half type, ND format; B [128, 256] half type, ND format
- Output: C [128, 512] float type, Nz format (channel split enabled)
- Implementation: Call `asc_copy_l0c2gm` with `enable_channel_split = true`, while layout conversion, quantization, and ReLU remain disabled
- Description: Enable channel split during L0C Buffer-to-GM transfer, splitting each 16x16 small-z fractal into two independent 16x8 small-z fractals. The `asc_copy_l0c2gm` input and output must both be float, and only Nz output is supported

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. The default installation path is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.
- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin ./output/golden.bin # Verify the output
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  Examples:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture for Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-7 | Scenario number |

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
