# Mmad Example

## Overview

This example introduces matrix multiplication with ND format input, covering B4 / B8 / B16 / B32 input data types (specifically int4_t / int8_t / bfloat16 / float as examples), demonstrating how to implement matrix multiplication computation (C = A x B + Bias) through the Mmad instruction.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── mmad
│   ├── figures                     // Illustrations
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   ├── mmad.py                 // Matrix multiplication helper script
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── mmad.asc                    // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

A complete matrix multiplication involves the following data transfer process: GM -> L1, L1 -> L0A / L0B, L1 -> BT (BiasTable Buffer), L0C -> GM. The data layout formats in different storage units are shown in Table 1 below:

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Data Layout Formats in Different Storage Units</span></caption>
  <tr>
    <td>Storage Unit</td>
    <td>Data Layout Format</td>
  </tr>
  <tr>
    <td>GM</td>
    <td>Input A, B matrices and output C matrix are in ND layout.</td>
  </tr>
  <tr>
    <td>L1</td>
    <td>A, B matrices are in Nz layout.</td>
  </tr>
  <tr>
    <td>L0A</td>
    <td>For Ascend 950PR/Ascend 950DT products, A matrix is in Nz layout;<br>For Atlas A3 Training Series Products/Atlas A3 Inference Series Products and Atlas A2 Training Series Products/Atlas A2 Inference Series Products, A matrix is in Zz layout.</td>
  </tr>
  <tr>
    <td>L0B</td>
    <td>B matrix is in Zn layout.</td>
  </tr>
  <tr>
    <td>BT (BiasTable Buffer)</td>
    <td>Bias is a 1D Tensor with shape [N].</td>
  </tr>
  <tr>
    <td>L0C</td>
    <td>C matrix is in Nz layout.</td>
  </tr>
</table>

The general matrix multiplication formula: C = A x B + Bias, where A, B, Bias, C matrices must satisfy shapes [M,K], [K,N], [N] and [M,N] respectively. The correspondence between Bias data type and C matrix data type is shown in Table 2, where combinations using bfloat16 for Bias on GM/L1 are only supported on Ascend 950PR/Ascend 950DT.
<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: Data Type Correspondence Between L0C and Input Bias</span></caption>
  <tr>
    <td>Bias Data Type on GM/L1</td>
    <td>Bias Data Type on BT (BiasTable Buffer)</td>
    <td>Matrix Computation Output Data Type on L0C</td>
  </tr>
  <tr>
    <td>int32_t</td>
    <td>int32_t</td>
    <td>int32_t</td>
  </tr>
  <tr>
    <td>bfloat16</td>
    <td rowspan="3">float</td>
    <td rowspan="3">float</td>
  </tr>
  <tr>
    <td>half</td>
  </tr>
    <tr>
    <td>float</td>
  </tr>
</table>

The scenarios corresponding to different values of the scenarioNum parameter in the program are shown in Table 3 below:

<a name="table3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 3: Meaning of Different scenarioNum Values</span></caption>
  <tr>
    <td>scenarioNum</td>
    <td>Input Data Type</td>
    <td>Output Data Type</td>
    <td>A Matrix</td>
    <td>B Matrix</td>
    <td>Bias</td>
  </tr>
  <tr>
    <td>1</td>
    <td>int8_t</td>
    <td>int32_t</td>
    <td>Not transposed</td>
    <td>Not transposed</td>
    <td>With Bias and biasTensor not passed, C matrix initial value from C2</td>
  </tr>
  <tr>
    <td>2</td>
    <td>bfloat16</td>
    <td>bfloat16</td>
    <td>Not transposed</td>
    <td>Transposed</td>
    <td>Without Bias, C matrix accumulation from CO1 initial value</td>
  </tr>
  <tr>
    <td>3</td>
    <td>float</td>
    <td>float</td>
    <td>Transposed</td>
    <td>Transposed</td>
    <td>With Bias and biasTensor passed</td>
  </tr>
  <tr>
    <td>4</td>
    <td>int4b_t</td>
    <td>int32_t</td>
    <td>Not transposed</td>
    <td>Transposed</td>
    <td>Bias not enabled, C matrix initial value is 0</td>
  </tr>
</table>

### Scenario Details

  This example selects different output scenarios through the compilation parameter `SCENARIO_NUM`. All scenarios are based on the same matrix multiplication specification: [M, N, K] = [30, 40, 70], with kernel function name `mmad_custom`.

**Scenario 1: int8_t input, int32_t output, C matrix initial value from C2**
- Input:
  - A not transposed [30, 70] int8_t type, ND format
  - B not transposed [70, 40] int8_t type, ND format
  - Bias [1, 40] int32_t type
- Output: C [30, 40] int32_t type, ND format
- Implementation: Use `Mmad` for matrix multiplication, without passing biasTensor, set C matrix initial value from C2 through parameters: `mmadParams.cmatrixInitVal = false, mmadParams.cmatrixSource = true`.
- Description: For int8_t type input with B matrix not transposed, the N axis aligns to 2 * 16, filling a 32 * 16 fractal with all invalid data. As shown in Figure 1 below, if `mmadParams.n = N` is set, it would read fractals numbered 3 and 7 while failing to read fractals numbered 9 and 10 that contain valid data. Therefore, set: `mmadParams.n = CeilAlign(N, BLOCK_CUBE * fractalNum)`, which reads all fractals. Although the matrix computation result includes results from invalid data participation, the Fixpipe instruction ensures that results from invalid data are not transferred out by setting `fixpipeParams.nSize = N` during data transfer.
<p align="center">
  <img src="figures/mmad_s8_L0B_转置.png" width="700">
</p>
<p align="center">
Figure 1: int8_t type, B not transposed, N axis actual alignment requirement differs from Mmad instruction default
</p>

**Scenario 2: bfloat16 input, float output, A not transposed, B transposed, C matrix initial value from CO1**
- Input:
  - A not transposed [30, 70] bfloat16 type, ND format
  - B transposed [40, 70] bfloat16 type, ND format
- Output: C [30, 40] float type, ND format
- Implementation: Use `Mmad` for matrix multiplication, set C matrix initial value from CO1 through parameters: `mmadParams.cmatrixInitVal = false, mmadParams.cmatrixSource = false`
- Description: This scenario performs two Mmad computations, with the first computation result stored in CO1 as the C matrix initial value for the next computation, ultimately accumulating both Mmad computation results.

**Scenario 3: float input, float output, A transposed, B transposed, biasTensor passed, kDirectionAlign set to true**
- Input:
  - A transposed [70, 30] float type, ND format
  - B transposed [40, 70] float type, ND format
  - Bias [1, 40] float type
- Output: C [30, 40] float type, ND format
- Implementation: Use `Mmad` for matrix multiplication, pass biasTensor, `mmadParams.cmatrixSource` parameter is not applicable in this scenario
- Description: For float type input with A matrix transposed, `mmadParams.kDirectionAlign` is needed to resolve the issue where the K axis actually aligns to `CeilAlign(K, 8*2)` but the Mmad instruction defaults to `CeilAlign(K, 8)`. In this scenario, this parameter is set to true, K axis aligns to `CeilAlign(K, 16)`, and the matrix computation unit skips filled invalid data when reading from L0A. In other scenarios, this parameter defaults to false, and K axis aligns to `CeilAlign(K, 8)`, as shown in Figure 2 below:

<p align="center">
  <img src="figures/mmad_f32_L0A_转置.png" width="1100">
</p>
<p align="center">
Figure 2: float type, A transposed, K axis actual alignment differs from Mmad instruction default requirement
</p>

**Scenario 4: int4b_t input, int32_t output, C matrix initial value is 0, only supported on Atlas A3 Training Series Products/Atlas A3 Inference Series Products and Atlas A2 Training Series Products/Atlas A2 Inference Series Products**

- Input:
  - A not transposed [30, 70] int4b_t type, ND format
  - B transposed [40, 70] int4b_t type, ND format
- Output: C [30, 40] int32_t type, ND format
- Implementation: Use `Mmad` for matrix multiplication, set C matrix initial value to 0 through parameter: `mmadParams.cmatrixInitVal = true`
- Description: This scenario does not support adding Bias by passing biasTensor (Scenario 3).

### Matrix Multiplication (Mmad)

The following describes how to configure the members of the MmadParams structure for the Mmad instruction. The specific meaning of each member variable is not repeated here.

Note that when executing the Mmad instruction, the matrix computation unit continuously reads multiple fractals from L0A/L0B to participate in matrix multiplication computation. The number of fractals read is determined by the values of MmadParams structure members m, n, k, and the alignment requirements of the Mmad instruction for A and B matrix axes on L0A/L0B.

Taking b16 input type as an example, the Mmad instruction reads data continuously according to A matrix fractal [16, 16] and B matrix fractal [16, 16]. At this point, the total number of fractals read by the matrix computation unit from L0A/L0B are 2 x 5 = 10 and 5 x 3 = 15 respectively, and the total number of fractals written to L0C is 2 x 3 = 6. As shown in the figures below, Figure 3 corresponds to Atlas A3 Training Series Products/Atlas A3 Inference Series Products and Atlas A2 Training Series Products/Atlas A2 Inference Series Products, Figure 4 corresponds to Ascend 950PR/Ascend 950DT; the two differ in data layout on L0A, the former being Zz and the latter being Nz.

<p align="center">
  <img src="figures/mmad_f16_A3.png" width="900">
</p>
<p align="center">
Figure 3: bfloat16 type, Zz layout on L0A, Mmad data layout diagram
</p>
<p align="center">
  <img src="figures/mmad_f16_A5.png" width="900">
</p>
<p align="center">
Figure 4: bfloat16 type, Nz layout on L0A, Mmad data layout diagram
</p>

The Mmad computation includes padded invalid data, which needs to be excluded during the L0C to GM transfer process by the Fixpipe instruction, eliminating the invalid data filled during Mmad computation.

## Build and Run

Run the following steps in the root directory of this example to build and run the operator.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim`, `cpu` | Run mode: NPU execution, NPU simulation, CPU debug |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products/Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` |  `1` (default), `2`, `3`, `4` | Scenario number, corresponding to int8_t / bfloat16 / float / int4b_t input data types respectively; `only supported when CMAKE_ASC_ARCHITECTURES=dav-2201 to set to 4` |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
