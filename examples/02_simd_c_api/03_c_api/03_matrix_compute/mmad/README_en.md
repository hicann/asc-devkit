# Mmad Example

## Overview

This example uses int8_t and bfloat16 as two input data types to demonstrate how to implement matrix multiplication (C = A x B + Bias) through the C API. The pipeline functions for each scenario are split into separate header files (`mmad_s1.h` / `mmad_s2.h`), and the main file `mmad.asc` dispatches by scenario at the kernel function entry through `if constexpr (scenario_num)`.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── mmad
│   ├── figures                     // Illustrations
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   ├── mmad.py                 // Matrix multiplication helper script
│   │   └── verify_result.py        // Accuracy verification script
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── mmad_s1.h                   // Constants and complete pipeline function for Scenario 1
│   ├── mmad_s2.h                   // Constants and complete pipeline function for Scenario 2
│   ├── mmad.asc                    // Example main file (common constants, kernel function entry, Host-side main function)
│   ├── README_en.md                // English example documentation
│   └── README.md                   // Example documentation
```

## Example Description

A complete matrix multiplication involves the following data transfer process: Global Memory -> L1 Buffer, L1 Buffer -> L0A / L0B Buffer, L1 Buffer -> BiasTable Buffer, L0C Buffer -> Global Memory. The data layout formats in different storage units are shown in the table below.

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Data Layout Formats in Different Storage Units</span></caption>
  <tr>
    <td>Storage Unit</td>
    <td>Data Layout Format</td>
  </tr>
  <tr>
    <td>Global Memory (GM)</td>
    <td>Input A, B matrices and output C matrix are in ND layout.</td>
  </tr>
  <tr>
    <td>L1 Buffer (L1)</td>
    <td>A, B matrices are in Nz layout.</td>
  </tr>
  <tr>
    <td>L0A Buffer (L0A)</td>
    <td>A matrix is in Nz layout.</td>
  </tr>
  <tr>
    <td>L0B Buffer (L0B)</td>
    <td>B matrix is in Zn layout.</td>
  </tr>
  <tr>
    <td>BiasTable Buffer (BT)</td>
    <td>Bias is a 1D Tensor with shape [N].</td>
  </tr>
  <tr>
    <td>L0C Buffer (L0C)</td>
    <td>C matrix is in Nz layout.</td>
  </tr>
</table>

The matrix multiplication formula: C = A x B + Bias, where A, B, Bias, C matrices must satisfy shapes [M, K], [K, N], [N] and [M, N] respectively. Bias is only enabled in Scenario 1, and its data type correspondence with the C matrix is shown in the table below.

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
    <td>bfloat16_t</td>
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

The scenarios corresponding to different values of the compilation parameter `SCENARIO_NUM` are shown below:

<a name="table3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 3: Meaning of Different SCENARIO_NUM Values</span></caption>
  <tr>
    <td>SCENARIO_NUM</td>
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
    <td>With Bias, C matrix initial value from BT</td>
  </tr>
  <tr>
    <td>2</td>
    <td>bfloat16_t</td>
    <td>float</td>
    <td>Not transposed</td>
    <td>Transposed</td>
    <td>Without Bias, C matrix initial value from CO1 (accumulated from two Mmad operations)</td>
  </tr>
</table>

### Scenario Details

This example selects scenarios through the compilation parameter `SCENARIO_NUM`. All scenarios are based on the same matrix multiplication specification: [M, N, K] = [30, 40, 70], with kernel function name `mmad_custom`.

**Scenario 1: int8_t input, int32_t output, with Bias**

- Input:
  - A not transposed [30, 70] int8_t, ND format;
  - B not transposed [70, 40] int8_t, ND format;
  - Bias [1, 40] int32_t;
- Output: C [30, 40] int32_t, ND format;
- Implementation (see `mmad_s1.h` for details):
  1. `asc_copy_gm2l1_nd2nz` + `asc_set_gm2l1_nz_para`: Transfer A, B, Bias from GM to L1 (ND -> Nz);
  2. `asc_copy_l12l0a`: Transfer A from L1 to L0A; `asc_copy_l12l0b_trans`: Transfer B from L1 to L0B with transposition; `asc_copy_l12bt`: Transfer Bias from L1 to BT;
  3. `asc_mmad`: Matrix multiply-add, C matrix initial value from BT;
  4. `asc_copy_l0c2gm` + `asc_set_l0c2gm_nz2nd`: Transfer result from L0C to GM (Nz -> ND);
- Description: For int8_t type input with B matrix not transposed, the N axis aligns to 2 * 16, filling a 32 * 16 fractal with all invalid data. As shown in Figure 1 below, if `right_width = N` is set, it would read fractals numbered 3 and 7 while failing to read fractals numbered 9 and 10 that contain valid data. Therefore, set: `right_width = CeilAlign(N, BLOCK_CUBE * fractalNum)`, which reads all fractals. Although the matrix computation result includes results from invalid data participation, the `asc_copy_l0c2gm` instruction ensures that results from invalid data are not transferred out by setting `n_size = N` during data transfer.
<p align="center">
  <img src="figures/mmad_s8_L0B_转置.png" width="700">
</p>
<p align="center">
Figure 1: int8_t type, B not transposed, N axis actual alignment requirement differs from Mmad instruction default
</p>

**Scenario 2: bfloat16_t input, float output, without Bias, two Mmad accumulations**

- Input:
  - A not transposed [30, 70] bfloat16_t, ND format;
  - B transposed [40, 70] bfloat16_t, ND format;
- Output: C [30, 40] float, ND format;
- Implementation (see `mmad_s2.h` for details):
  1. `asc_copy_gm2l1_nd2nz` + `asc_set_gm2l1_nz_para`: Transfer A, B from GM to L1 (B is stored in GM with transposed ND layout);
  2. `asc_copy_l12l0a`: Transfer A from L1 to L0A; `asc_copy_l12l0b`: Transfer B directly from L1 to L0B (B is already transposed to Nz layout in L1);
  3. `asc_mmad`: Called twice. The first call with `c_matrix_init_val = true` initializes C to 0 and computes A x B; the second call with `c_matrix_init_val = false` and `c_matrix_source = false` uses CO1 as initial value to accumulate the second A x B;
  4. `asc_copy_l0c2gm` + `asc_set_l0c2gm_nz2nd`: Transfer result from L0C to GM;

### Matrix Multiplication (Mmad)

The following describes how to configure the parameters of the `asc_mmad` instruction. The specific meaning of each parameter is not repeated here.

Note that when executing the `asc_mmad` instruction, the matrix computation unit continuously reads multiple fractals from L0A/L0B to participate in matrix multiplication computation. The number of fractals read is determined by the values of `left_height`, `n_dim`, `right_width`, and the alignment requirements of the `asc_mmad` instruction for A and B matrix axes on L0A/L0B.

As shown in Figure 2, taking b16 input type as an example, the `asc_mmad` instruction reads data continuously according to A matrix fractal [16, 16] and B matrix fractal [16, 16]. At this point, the total number of fractals read by the matrix computation unit from L0A/L0B are 2 x 5 = 10 and 5 x 3 = 15 respectively, and the total number of fractals written to L0C is 2 x 3 = 6.

<p align="center">
  <img src="figures/mmad_f16_A5.png" width="900">
</p>
<p align="center">
Figure 2: bfloat16 type, Nz layout on L0A, Mmad data layout diagram
</p>

The Mmad computation includes padded invalid data, which needs to be excluded during the L0C to GM transfer process by the Fixpipe instruction, eliminating the invalid data filled during Mmad computation.

## Build and Run

Run the following steps in the root directory of this example to build and execute the operator.

- Configure environment variables
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path for root users is `/usr/local/Ascend`, and for non-root users it is `$HOME/Ascend`.

- Run the example

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM .. && make -j
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM .. && make -j
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture, corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2` | Scenario number, corresponding to int8_t and bfloat16 input data types respectively |

- Execution result

  Output upon successful accuracy comparison:

  ```bash
  test pass!
  ```
