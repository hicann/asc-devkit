# asc_copy_l0c2l1 Example

## Overview

This example demonstrates how to use the SIMD C API `asc_copy_l0c2l1` to transfer matrix multiplication results from L0C Buffer to L1 Buffer, supporting multiple data type conversions, inline quantization, and ReLU. This API efficiently transfers matrix multiplication results from L0C Buffer to L1 Buffer and supports various data format conversions and preprocessing capabilities.

Note: Ascend 950PR/Ascend 950DT does not support direct L1 Buffer-to-GM transfer. After `asc_copy_l0c2l1` transfers the data from L0C Buffer to L1 Buffer, this example directly transfers the L0C Buffer result produced by the first matrix multiplication to GM. The generated `output.bin` contains the original float matrix multiplication result and is used to verify the matrix multiplication computation. It does not numerically verify the type conversion, quantization, or ReLU result in L1 Buffer.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── asc_copy_l0c2l1
│   ├── scripts
│   │   ├── gen_data.py                // Script for generating input data and ground truth data
│   │   └── verify_result.py           // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── asc_copy_l0c2l1.asc             // SIMD C API implementation and invocation
│   └── README.md                      // Example documentation
```

## `asc_copy_l0c2l1` Parameters

This `dav-3510` example describes each L0C Buffer-to-L1 Buffer transfer through the addresses, shape, strides, and feature controls of [`asc_copy_l0c2l1`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md). Scalar and vector quantization values are set through companion configuration APIs.

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Main Parameters and Configuration APIs</span></caption>
<tr><td align="center">Parameter or API</td><td align="center">Description</td></tr>
<tr><td align="center"><code>n_size</code> / <code>m_size</code></td><td>Source Nz matrix size in the N and M directions; both are 128 in this example</td></tr>
<tr><td align="center"><code>src_stride</code></td><td>Offset between adjacent Z layouts in the source Nz matrix, in 64-byte units</td></tr>
<tr><td align="center"><code>dst_stride</code></td><td>Element offset between adjacent Z layouts in destination L1 Buffer; half and int8_t use C0 sizes of 16 and 32 elements</td></tr>
<tr><td align="center"><code>unit_flag_mode</code></td><td>Fine-grained matrix compute and transfer control; this example uses <code>DISABLE</code></td></tr>
<tr><td align="center"><code>quant_pre_mode</code></td><td>Scenarios 1 and 4 use <code>F322F16</code>; Scenarios 2 and 3 use <code>QF322B8_PRE</code> and <code>VQF322B8_PRE</code></td></tr>
<tr><td align="center"><code>relu_pre_mode</code></td><td>Scenario 4 uses <code>asc_relu_pre_mode::NORMAL</code>; all other scenarios use <code>NONE</code></td></tr>
<tr><td align="center"><code>asc_set_l0c_copy_prequant</code></td><td>Configures the scalar quantization value; Scenario 2 uses scale 2.0</td></tr>
<tr><td align="center"><code>asc_set_l0c_copy_config</code></td><td>Configures the vector quantization address in Fixpipe Buffer for Scenario 3</td></tr>
</table>

## Scenario Description

This example selects different output scenarios through the compilation parameter `SCENARIO_NUM`. The meanings of different `SCENARIO_NUM` values are shown in the table below.
All scenarios are based on the same matrix multiplication specification: [M, N, K] = [128, 128, 128], with kernel function name `asc_copy_l0c2l1_custom`.

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: Meaning of Different scenarioNum Values</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">L0C Buffer Data Type</td><td align="center">L1 Buffer Data Type</td><td align="center">L1 Buffer Output Format</td><td align="center">Quantization Enabled</td><td align="center">ReLU Enabled</td></tr>
<tr><td align="center">1</td><td align="center">float</td><td align="center">half</td><td align="center">Nz</td><td align="center">No (cast)</td><td align="center">No</td></tr>
<tr><td align="center">2</td><td align="center">float</td><td align="center">int8_t</td><td align="center">Nz</td><td align="center">Yes (scalar)</td><td align="center">No</td></tr>
<tr><td align="center">3</td><td align="center">float</td><td align="center">int8_t</td><td align="center">Nz</td><td align="center">Yes (vector)</td><td align="center">No</td></tr>
<tr><td align="center">4</td><td align="center">float</td><td align="center">half</td><td align="center">Nz</td><td align="center">No (cast)</td><td align="center">Yes</td></tr>
</table>

**Scenario 1: Output format Nz, output to L1 Buffer data type half**
- Input: A [128, 128] half type, ND format; B [128, 128] half type, ND format
- L1 Buffer output: C [128, 128] half type, Nz format
- Implementation: Call `asc_copy_l0c2l1` with `asc_quant_mode::F322F16` to convert the float accumulation result to half Nz
- Description: L0C Buffer data in Nz format directly output to L1 Buffer in Nz format, data remains unchanged

**Scenario 2: Output format Nz, output to L1 Buffer data type int8_t, Scalar quantization enabled**
- Input: A [128, 128] half type, ND format; B [128, 128] half type, ND format
- L1 Buffer output: C [128, 128] int8_t type, Nz format
- Implementation: Call `asc_set_l0c_copy_prequant(2.0F, 0, true)`, then call `asc_copy_l0c2l1` with `asc_quant_mode::QF322B8_PRE`
- Description: Quantize float type data to int8_t type, entire C matrix uses one quantization parameter

**Scenario 3: Output format Nz, output to L1 Buffer data type int8_t, Vector quantization enabled**
- Input: A [128, 128] half type, ND format; B [128, 128] half type, ND format
- L1 Buffer output: C [128, 128] int8_t type, Nz format
- Implementation: Use `asc_copy_gm2l1` and `asc_copy_l12fb` to transfer per-column quantization values to Fixpipe Buffer, configure the address with `asc_set_l0c_copy_config`, then call `asc_copy_l0c2l1` with `asc_quant_mode::VQF322B8_PRE`
- Description: Quantize float type data to int8_t type, each column of C matrix corresponds to one quantization parameter, quantization parameters need to be copied from GM to L1 Buffer

**Scenario 4: Output format Nz, output to L1 Buffer data type half, ReLU enabled**
- Input: A [128, 128] half type, ND format; B [128, 128] half type, ND format
- L1 Buffer output: C [128, 128] half type, Nz format
- Implementation: Call `asc_copy_l0c2l1` with `asc_quant_mode::F322F16` and `relu_pre_mode = asc_relu_pre_mode::NORMAL`
- Description: Perform ReLU operation during data transfer from L0C Buffer to L1 Buffer, setting negative values to 0

After each L0C Buffer-to-L1 Buffer transfer, the example uses `asc_copy_l0c2gm` to transfer the L0C Buffer result produced by the first matrix multiplication directly to GM, generating `output.bin` in float and ND format. This file verifies the original matrix multiplication result, not the type conversion, quantization, or ReLU result in L1 Buffer.

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
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # Build the project (default NPU mode)
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM  # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin # Verify the original matrix multiplication result
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture for Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-4 | Scenario number |

  The following output indicates that the accuracy comparison of the original matrix multiplication result is successful.

  ```bash
  test pass!
  ```
