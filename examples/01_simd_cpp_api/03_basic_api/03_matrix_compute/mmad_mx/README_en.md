# Mmad MX Example

## Overview

This example introduces four types of matrix multiplication with quantization capabilities (MX matrix multiplication) where the input is in ND format, A and B matrices use FP4 (fp4x2_e1m2_t, fp4x2_e2m1_t) and FP8 (fp8_e4m3fn_t, fp8_e5m2_t) data types, quantization coefficient matrices scaleA and scaleB use fp8_e8m0_t data type, with the left matrix and left quantization coefficient matrix not transposed, and the right matrix and right quantization coefficient matrix transposed.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── mmad_mx
│   ├── figures                     // Illustrations
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── mmad_mx.asc                 // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

A complete MX matrix multiplication involves the following data transfer process: GM -> L1, L1 -> L0A / L0AMX / L0B / L0BMX, L1 -> BT (BiasTable Buffer), L0C -> GM. The data layout formats in different storage units are shown in Table 1 below:

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
    <td>L0A, L0AMX</td>
    <td>A, ScaleA matrices are in Nz, Zz layout respectively.</td>
  </tr>
  <tr>
    <td>L0B, L0BMX</td>
    <td>B, ScaleB matrices are in Zn, Nn layout respectively.</td>
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

MX matrix multiplication formula: $$ C = (ScaleA x A) x (ScaleB x B) + Bias $$
where A, ScaleA, B, ScaleB, Bias, C matrices must satisfy shapes [M,K], [M,ScaleK], [K,N], [ScaleK, N], [N] and [M,N] respectively, ScaleK = CeilAlign(CeilDiv(K, 32), 2).

The scenarios corresponding to different values of the scenarioNum parameter in the program are shown in Table 2 below:

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: Meaning of Different scenarioNum Values</span></caption>
  <tr>
    <td>scenarioNum</td>
    <td>A Type</td>
    <td>ScaleA Type</td>
    <td>B Type</td>
    <td>ScaleB Type</td>
    <td>Bias Type</td>
    <td>C Type</td>
    <td>Bias Description</td>
  </tr>
  <tr>
    <td>1</td>
    <td>fp4x2_e1m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>Without Bias, C matrix initial value is 0</td>
  </tr>
  <tr>
    <td>2</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp4x2_e1m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>With Bias, biasTensor not passed during matrix computation, C matrix initial value from C2 (BiasTable Buffer),<br>read from C2 start position 0</td>
  </tr>
  <tr>
    <td>3</td>
    <td>fp8_e4m3fn_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp8_e5m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>Without Bias, C matrix accumulation from CO1 initial value</td>
  </tr>
  <tr>
    <td>4</td>
    <td>fp8_e5m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp8_e4m3fn_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>With Bias, biasTensor passed during matrix computation, C matrix initial value from passed biasTensor,<br>read from biasTensor start position</td>
  </tr>
</table>

### Scenario Details
  This example selects different scenarios through the compilation parameter `SCENARIO_NUM`. All scenarios are based on the same matrix multiplication specification: [M, N, K] = [40, 50, 70], with kernel function name `KernelMmadMXCustom`.

**Scenario 1: C matrix initial value is 0**

- Input: A not transposed [40, 70] fp4x2_e1m2_t type, ND format; ScaleA not transposed [40, 4] fp8_e8m0_t type, ND format; B transposed [50, 70] fp4x2_e2m1_t type, ND format; ScaleB transposed [50, 4] fp8_e8m0_t type, ND format; without Bias
- Output: C [40, 50] float type, ND format
- Implementation: Use `MmadMx` for matrix multiplication, set C matrix initial value to 0 through parameter: `mmadParams.cmatrixInitVal = true`

**Scenario 2: C matrix initial value from C2**
- Input: A not transposed [40, 70] fp4x2_e2m1_t type, ND format; ScaleA not transposed [40, 4] fp8_e8m0_t type, ND format; B transposed [50, 70] fp4x2_e1m2_t type, ND format; ScaleB transposed [50, 4] fp8_e8m0_t type, ND format; with Bias, C matrix initial value from C2
- Output: C [40, 50] float type, ND format
- Implementation: Use `MmadMx` for matrix multiplication, without passing biasTensor, set C matrix initial value from C2 through parameters: `mmadParams.cmatrixInitVal = false, mmadParams.cmatrixSource = true`

**Scenario 3: C matrix initial value from CO1**
- Input: A not transposed [40, 70] fp8_e4m3fn_t type, ND format; ScaleA not transposed [40, 4] fp8_e8m0_t type, ND format; B transposed [50, 70] fp8_e5m2_t type, ND format; ScaleB transposed [50, 4] fp8_e8m0_t type, ND format; without Bias, C matrix initial value from CO1
- Output: C [40, 50] float type, ND format
- Implementation: Use `MmadMx` for matrix multiplication, set C matrix initial value from CO1 through parameters: `mmadParams.cmatrixInitVal = false, mmadParams.cmatrixSource = false`

**Scenario 4: C matrix initial value from biasTensor**
- Input: A not transposed [40, 70] fp8_e5m2_t type, ND format; ScaleA not transposed [40, 4] fp8_e8m0_t type, ND format; B transposed [50, 70] fp8_e4m3fn_t type, ND format; ScaleB transposed [50, 4] fp8_e8m0_t type, ND format; with Bias, C matrix initial value from passed biasTensor
- Output: C [40, 50] float type, ND format
- Implementation: Use `MmadMx` for matrix multiplication, pass biasTensor, `mmadParams.cmatrixSource` parameter is not applicable in this scenario

### Matrix Multiplication (MmadMx)

The `MmadMx` instruction automatically performs broadcast multiplication of left/right matrices with their corresponding scale matrices, with every 32 elements in the k direction sharing one quantization factor. Taking A and B matrices both as FP4 type as an example, the figure below shows the fractal layout format and quantization function principle of A, ScaleA, B, ScaleB:

<p align="center">
  <img src="figures/mmad-mx.png" width="1000">
</p>

<p align="center">
Figure 1: Matrix multiplication with quantization diagram
</p>

Note that mmadParams.k takes alignK = CeilAlign(k, 64) = 128, not the original k=70, because the Mmad instruction in MX matrix multiplication requires k direction alignment to 64.

## Build and Run
Run the following steps in the root directory of this example to build and run the operator.
- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Install Python dependencies

  The `gen_data.py` script in this example requires `ml_dtypes` to generate FP8 input data and `en_dtypes` to generate FP4 input data. The following versions are recommended:

  ```bash
  python3 -m pip install ml_dtypes==0.2.0 en_dtypes==0.0.4
  ```

  Without these dependencies installed, the script cannot correctly generate FP4/FP8 type input data and ground truth data, which may cause accuracy verification failures.
  
- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim`, `cpu` | Run mode: NPU execution, NPU simulation, CPU debug |
  | `SCENARIO_NUM` |  `1` (default), `2`, `3`, `4` | Scenario number, corresponding to the 4 scenarios in the scenario description |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```
