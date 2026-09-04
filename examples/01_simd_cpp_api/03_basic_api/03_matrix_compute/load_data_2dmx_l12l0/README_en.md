# load_data_2dmx_l12l0 Example

## Overview

This example demonstrates how to use the basic API `LoadData` to complete L1 to L0 data transfer in MX quantized matrix multiplication.

The example takes FP4 / FP8 type A / B matrices and fp8_e8m0_t type scaleA / scaleB matrices as input, covering A / B matrix transposed and non-transposed input scenarios. It mainly demonstrates:

1. Using `LoadData2DParamsV2` to control A / B matrix transfer to L0A / L0B;
2. Using `LoadData2DMxParams` to control scaleA / scaleB matrix transfer to L0A_MX / L0B_MX;
3. Combining with `MmadMx` to complete MX matrix multiplication, and transferring results out to GM through `Fixpipe`.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── load_data_2dmx_l12l0
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── figures                     // Illustrations
│   ├── load_data_2dmx_l12l0.asc    // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

### 1. Overall Flow

The overall MX matrix multiplication flow is as follows:

```
GM(ND) --DataCopy--> L1(Nz/Zz/Nn) --LoadData(LoadData2DParamsV2, LoadData2DMxParams)--> L0A(Nz)/L0B(Zn)/L0A_MX(Zz)/L0B_MX(Nn) --MmadMx--> L0C(Nz) --Fixpipe--> GM(ND)
```

**Step details**:

1. **GM -> L1**:
   - Call `DataCopy` instruction with `Nd2NzParams` structure parameters to implement A / B matrix format conversion from ND to Nz.
   - Call `DataCopy` instruction with `Nd2NzParams` or `Dn2NzParams` structure parameters, using B16 view to implement scaleA / scaleB matrix format conversion from ND to Zz / Nn.
   - Use `Fill` to zero out invalid data to prevent dirty data from participating in `MmadMx` computation.
2. **L1 -> L0**:
   - Call `LoadData` instruction with `LoadData2DParamsV2` and `LoadData2DMxParams` structure parameters.
   - `LoadData2DParamsV2` controls A / B matrix transfer, `LoadData2DMxParams` controls scaleA / scaleB matrix transfer.
3. **Matrix multiplication**: Use `MmadMx` interface to perform MX matrix multiplication.
4. **L0C -> GM**: Use `Fixpipe` interface to transfer results out.

Note that `LoadData2DMxParams` does not contain transpose control parameters like `ifTranspose`. Scale matrices transfer directly in `x` / `y` direction with stride during the L1 -> L0A_MX / L0B_MX phase. Therefore, scaleA / scaleB need to be pre-arranged in the GM -> L1 phase to the layout expected by L0A_MX / L0B_MX.

A / B matrices and scaleA / scaleB matrices have different data layout formats in different storage units, as shown in [Table 1](#table1):

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Data Layout Formats in Different Storage Units</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">GM</span></td>
    <td align="center"><span style="font-weight: bold;">L1</span></td>
    <td align="center"><span style="font-weight: bold;">L0A</span></td>
    <td align="center"><span style="font-weight: bold;">L0A_MX</span></td>
    <td align="center"><span style="font-weight: bold;">L0B</span></td>
    <td align="center"><span style="font-weight: bold;">L0B_MX</span></td>
    <td align="center"><span style="font-weight: bold;">L0C</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A Matrix</span></td>
    <td align="center">ND</td>
    <td align="center">Nz</td>
    <td align="center">Nz</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B Matrix</span></td>
    <td align="center">ND</td>
    <td align="center">Nz</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">Zn</td>
    <td align="center">-</td>
    <td align="center">-</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">scaleA Matrix</span></td>
    <td align="center">ND</td>
    <td align="center">Zz</td>
    <td align="center">-</td>
    <td align="center">Zz</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">scaleB Matrix</span></td>
    <td align="center">ND</td>
    <td align="center">Nn</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">Nn</td>
    <td align="center">-</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">C Matrix</span></td>
    <td align="center">ND</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">-</td>
    <td align="center">Nz</td>
  </tr>
</table>

This example uses a fixed matrix multiplication specification of `[m, n, k] = [40, 50, 70]`, with output C matrix `[40, 50]`, float type, ND format. The kernel function selects different scenarios through `scenarioNum`. See [Table 2](#table2) for specific input shapes and L1 -> L0 transfer methods.

6 scenarios are derived from FP4 / FP8 data types, A / B transpose input, and whether for loop transfer is used when A is transposed. `isAtranspose` simultaneously indicates whether A matrix and scaleA matrix are transposed input; `isBtranspose` simultaneously indicates whether B matrix and scaleB matrix are transposed input. Scenarios 5 / 6 use for loop transfer for A matrix to avoid dirty data exceeding 1 fractal in single `LoadData` transfer when A is transposed.

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: Different scenarioNum Values, Input Shapes, and L1 -> L0 Transfer Methods</span></caption>
  <tr>
    <td><span style="font-weight: bold;">scenarioNum</span></td>
    <td><span style="font-weight: bold;">A / B Data Type</span></td>
    <td><span style="font-weight: bold;">A Matrix Type</span></td>
    <td><span style="font-weight: bold;">B Matrix Type</span></td>
    <td><span style="font-weight: bold;">Input Shape</span></td>
    <td><span style="font-weight: bold;">isAtranspose</span></td>
    <td><span style="font-weight: bold;">isBtranspose</span></td>
    <td><span style="font-weight: bold;">A Matrix Transfer</span></td>
    <td><span style="font-weight: bold;">B Matrix Transfer</span></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">1</span></td>
    <td>FP4</td>
    <td>fp4x2_e1m2_t</td>
    <td>fp4x2_e2m1_t</td>
    <td>A [40, 70]<br>B [50, 70]<br>scaleA [40, 4]<br>scaleB [50, 4]</td>
    <td align="center">false</td>
    <td align="center">true</td>
    <td>LoadData2DParamsV2 (no transpose)</td>
    <td>LoadData2DParamsV2 (no transpose)</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>FP4</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp4x2_e1m2_t</td>
    <td>A [70, 40]<br>B [70, 50]<br>scaleA [4, 40, 2]<br>scaleB [4, 50, 2]</td>
    <td align="center">true</td>
    <td align="center">false</td>
    <td>LoadData2DParamsV2 (transpose)</td>
    <td>LoadData2DParamsV2 (transpose)</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td>FP8</td>
    <td>fp8_e4m3fn_t</td>
    <td>fp8_e5m2_t</td>
    <td>A [40, 70]<br>B [50, 70]<br>scaleA [40, 4]<br>scaleB [50, 4]</td>
    <td align="center">false</td>
    <td align="center">true</td>
    <td>LoadData2DParamsV2 (no transpose)</td>
    <td>LoadData2DParamsV2 (no transpose)</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>FP8</td>
    <td>fp8_e5m2_t</td>
    <td>fp8_e4m3fn_t</td>
    <td>A [70, 40]<br>B [70, 50]<br>scaleA [4, 40, 2]<br>scaleB [4, 50, 2]</td>
    <td align="center">true</td>
    <td align="center">false</td>
    <td>LoadData2DParamsV2 (transpose)</td>
    <td>LoadData2DParamsV2 (transpose)</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td>FP4</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp4x2_e1m2_t</td>
    <td>A [70, 40]<br>B [70, 50]<br>scaleA [4, 40, 2]<br>scaleB [4, 50, 2]</td>
    <td align="center">true</td>
    <td align="center">false</td>
    <td>for loop + LoadData2DParamsV2 (transpose)</td>
    <td>LoadData2DParamsV2 (transpose)</td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td>FP8</td>
    <td>fp8_e5m2_t</td>
    <td>fp8_e4m3fn_t</td>
    <td>A [70, 40]<br>B [70, 50]<br>scaleA [4, 40, 2]<br>scaleB [4, 50, 2]</td>
    <td align="center">true</td>
    <td align="center">false</td>
    <td>for loop + LoadData2DParamsV2 (transpose)</td>
    <td>LoadData2DParamsV2 (transpose)</td>
  </tr>
</table>

### 2. Scenario Description

The input shapes for each scenario are shown in [Table 2](#table2). This section only describes the differences in L1 -> L0 transfer, zero padding, and subsequent computation parameters between scenarios.

**Scenario 1: FP4 data type input, isAtranspose=false, isBtranspose=true**

- A matrix non-transposed input `[m, k]`, L1 -> L0A does not need transpose, `LoadData2DParamsV2` `ifTranspose = false`.
- B matrix transposed input `[n, k]`, L1 -> L0B does not need transpose, `LoadData2DParamsV2` `ifTranspose = false`.
- scaleA input shape is `[m, scaleK]`, scaleB input shape is `[n, scaleK]`.

<div align="center">
  <img src="figures/whole_process/B4_A_scaleA_.png" width="1000"><br>
  Figure 1: Scenario 1 MX matrix multiplication GM -> L1 -> L0A / L0A_MX flow diagram
</div>

<div align="center">
  <img src="figures/whole_process/B4_B_scaleB_NK.png" width="1000"><br>
  Figure 2: Scenario 1 MX matrix multiplication GM -> L1 -> L0B / L0B_MX flow diagram
</div>

**Scenario 2: FP4 data type input, isAtranspose=true, isBtranspose=false**

- A matrix transposed input `[k, m]`, L1 -> L0A needs transpose, `LoadData2DParamsV2` `ifTranspose = true`.
- B matrix non-transposed input `[k, n]`, L1 -> L0B needs transpose, `LoadData2DParamsV2` `ifTranspose = true`.
- scaleA input shape is `[scaleK/2, m, 2]`, scaleB input shape is `[scaleK/2, n, 2]`.
- A matrix m direction transfers extra dirty data exceeding 1 fractal, `MmadMx` needs to set `mmadParams.m = CeilAlign(m, fractalShape[0] * fractalNum)`, `Fixpipe` skips results from invalid fractals during transfer out.

<div align="center">
  <img src="figures/whole_process/B4_A_scaleA_trans_KM.png" width="1000"><br>
  Figure 3: Scenario 2 MX matrix multiplication GM -> L1 -> L0A / L0A_MX flow diagram
</div>

<div align="center">
  <img src="figures/whole_process/B4_B_scaleB_trans_KN.png" width="1000"><br>
  Figure 4: Scenario 2 MX matrix multiplication GM -> L1 -> L0B / L0B_MX flow diagram
</div>

**Scenario 3: FP8 data type input, isAtranspose=false, isBtranspose=true**

Similar to scenario 1, but data type is FP8.

- A and B matrix L1 -> L0 transfers both do not need transpose, `LoadData2DParamsV2` `ifTranspose = false`.
- scaleA input shape is `[m, scaleK]`, scaleB input shape is `[n, scaleK]`.
- In FP8 data type, when A matrix non-transposed input `[m, k]` and B matrix transposed input `[n, k]`, k is in col direction. `DataCopy` only aligns k direction to 32B. Need to flush k direction tail data to 0 on L1 to prevent dirty data from participating in computation.

<div align="center">
  <img src="figures/whole_process/B8_A_scaleA_.png" width="1000"><br>
  Figure 5: Scenario 3 MX matrix multiplication GM -> L1 -> L0A / L0A_MX flow diagram
</div>

<div align="center">
  <img src="figures/whole_process/B8_B_scaleB_NK.png" width="1000"><br>
  Figure 6: Scenario 3 MX matrix multiplication GM -> L1 -> L0B / L0B_MX flow diagram
</div>

**Scenario 4: FP8 data type input, isAtranspose=true, isBtranspose=false**

Similar to scenario 2, but data type is FP8.

- A matrix transposed input `[k, m]`, B matrix non-transposed input `[k, n]`, L1 -> L0 transfers both need transpose, `LoadData2DParamsV2` `ifTranspose = true`.
- scaleA input shape is `[scaleK/2, m, 2]`, scaleB input shape is `[scaleK/2, n, 2]`.
- A matrix m direction transfers extra dirty data exceeding 1 fractal, `MmadMx` needs to set `mmadParams.m = CeilAlign(m, fractalShape[0] * fractalNum)`.
- When A matrix transposed input `[k, m]` and B matrix non-transposed input `[k, n]`, both need to flush remaining dirty data in k direction to 0 on L1.

<div align="center">
  <img src="figures/whole_process/B8_A_scaleA_trans_KM.png" width="1000"><br>
  Figure 7: Scenario 4 MX matrix multiplication GM -> L1 -> L0A / L0A_MX flow diagram
</div>

<div align="center">
  <img src="figures/whole_process/B8_B_scaleB_trans_KN.png" width="1000"><br>
  Figure 8: Scenario 4 MX matrix multiplication GM -> L1 -> L0B / L0B_MX flow diagram
</div>

**Scenario 5: FP4 data type input, isAtranspose=true, isBtranspose=false**

- A matrix transposed input `[k, m]`, L1 -> L0A needs transpose, uses for loop to call `LoadData` for transfer.
- For loop skips dirty data fractals at the tail of m direction on L0A each time, keeping extra transferred data in m direction within 1 fractal.
- B matrix non-transposed input `[k, n]`, L1 -> L0B needs transpose, single call `LoadData` for transfer, `LoadData2DParamsV2` `ifTranspose = true`.
- scaleA input shape is `[scaleK/2, m, 2]`, scaleB input shape is `[scaleK/2, n, 2]`.
- Both A and B matrices need to flush remaining dirty data in k direction to 0 on L1.

<div align="center">
  <img src="figures/whole_process/B4_A_scaleA_for_trans_KM.png" width="1000"><br>
  Figure 9: Scenario 5 MX matrix multiplication GM -> L1 -> L0A / L0A_MX flow diagram
</div>

<div align="center">
  <img src="figures/whole_process/B4_B_scaleB_trans_KN.png" width="1000"><br>
  Figure 10: Scenario 5 MX matrix multiplication GM -> L1 -> L0B / L0B_MX flow diagram
</div>

**Scenario 6: FP8 data type input, isAtranspose=true, isBtranspose=false**

Similar to scenario 5, but data type is FP8.

- A matrix transposed input `[k, m]`, L1 -> L0A needs transpose, uses for loop to call `LoadData` for transfer, avoiding dirty data exceeding 1 fractal in m direction.
- B matrix non-transposed input `[k, n]`, L1 -> L0B needs transpose, single call `LoadData` for transfer, `LoadData2DParamsV2` `ifTranspose = true`.
- scaleA input shape is `[scaleK/2, m, 2]`, scaleB input shape is `[scaleK/2, n, 2]`.
- Both A and B matrices need to flush remaining dirty data in k direction to 0 on L1.

<div align="center">
  <img src="figures/whole_process/B8_A_scaleA_for_trans_KM.png" width="1000"><br>
  Figure 11: Scenario 6 MX matrix multiplication GM -> L1 -> L0A / L0A_MX flow diagram
</div>

<div align="center">
  <img src="figures/whole_process/B8_B_scaleB_trans_KN.png" width="1000"><br>
  Figure 12: Scenario 6 MX matrix multiplication GM -> L1 -> L0B / L0B_MX flow diagram
</div>

### 3. Parameter Description

The subsequent code and parameter descriptions repeatedly use fractal, alignment, and scale related variables. This section defines these concepts first.

- `fractalShape`: Small fractal shape. This example involves FP4 and FP8 for A / B matrix input, scale matrices transferred using B16 view. Fractal information is shown in [Table 3](#table3).
- `fractalSize`: Number of elements in 1 small fractal, see [Table 3](#table3).
- `fractalNum`: When L1 -> L0A / L0B requires transpose, `LoadData` transposes by square matrix. `fractalNum` represents the number of small fractals in the square matrix, see [Table 3](#table3).
- `packedK`: Equivalent column count in `DataCopy` parameters when transferring A / B matrix k direction using B8 view. FP4 input still has k elements, but during ND -> Nz transfer it is processed as B8 view, every 2 FP4 elements correspond to 1 B8 view element, so `packedK = AscendC::Std::ceil_div(k, 2)`; FP8 input uses original element count, `packedK = k`.
- `scaleK`: Aligned length of scale matrix k axis. `scaleK = AscendC::Std::ceil_div(k, SCALE_BASE_FACTOR) * SCALE_EVEN_NUMBER`, where `SCALE_BASE_FACTOR = 64`, `SCALE_EVEN_NUMBER = 2`. In this example when `k = 70`, `scaleK = AscendC::Std::ceil_div(70, 64) * 2 = 4`.
- `alignK`: Aligned length of A / B matrix k axis. The `MmadMx` interface in MX matrix multiplication requires k direction alignment to `SCALE_BASE_FACTOR = 64`, so `alignK = CeilAlign(k, SCALE_BASE_FACTOR) = CeilAlign(k, 64)`. In this example when `k = 70`, `alignK = CeilAlign(70, 64) = 128`. This 64-alignment requirement only applies to k direction; m / n directions still align to the corresponding data type's small fractal or square transpose granularity.

<a name="table3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 3: Fractal Information for Different Data Types</span></caption>
  <tr>
    <td></td>
    <td align="center"><code>Usage in This Example</code></td>
    <td align="center"><code>fractalShape</code></td>
    <td align="center"><code>fractalSize</code></td>
    <td align="center"><code>fractalNum</code></td>
    <td align="center"><code>packedK</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">FP4</span></td>
    <td align="center">A / B matrix FP4 input</td>
    <td align="center">[16, 64]</td>
    <td align="center">1024</td>
    <td align="center">4</td>
    <td align="center"><code>AscendC::Std::ceil_div(k, 2)</code></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">FP8</span></td>
    <td align="center">A / B matrix FP8 input</td>
    <td align="center">[16, 32]</td>
    <td align="center">512</td>
    <td align="center">2</td>
    <td align="center"><code>k</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center">scale matrix transferred using B16 view</td>
    <td align="center">[16, 16]</td>
    <td align="center">256</td>
    <td align="center">1</td>
    <td align="center">-</td>
  </tr>
</table>

- `CeilAlign`: Ceiling alignment operation. For example, when `m = 30`, `CeilAlign(30, 16) = 32`, meaning that the m-axis is aligned to 16 and its aligned length is 32.

```cpp
__aicore__ inline uint16_t CeilAlign(uint16_t size, uint16_t alignValue) {
    return (size + alignValue - 1) / alignValue * alignValue;
}
```

- `ceil_div`: Ceiling division, generally used to compute loop counts after ceiling alignment.
- `mAlignValue`: m axis aligns to `mAlignValue`. For example, when `mAlignValue = 16`, the m axis is aligned to 16. Similarly `nAlignValue`, `kAlignValue`.
- `mAlignL1` and `mAlignL0`: Aligned values of m axis when A matrix is on L1 and L0A respectively. Similarly `kaAlignL1`, `kaAlignL0`, `nAlignL1`, `nAlignL0`, `kbAlignL1`, `kbAlignL0`.


### 4. Alignment Requirements

A / B / scaleA / scaleB matrices have different alignment requirements on each axis on L1 and L0. These values are used when configuring `DataCopy`, `LoadData`, `MmadMx`, and `Fixpipe` parameters. A / B matrix k axis in MX scenarios all align to `SCALE_BASE_FACTOR = 64`, that is `kAlignValue = 64`; [Table 4](#table4) and [Table 5](#table5) only list m axis / n axis alignment requirements that vary with input layout and transfer method.

- [Table 4](#table4) is used to determine the shape of A / B matrices on L1 after GM -> L1, and the subsequent `Fill` zero-padding range.
- [Table 5](#table5) is used to determine L0A / L0B alignment values after `LoadData` transfer in the L1 -> L0 phase, affecting subsequent `MmadMx` and `Fixpipe` parameter configuration.
- [Table 6](#table6) is used to determine alignment shapes of scaleA / scaleB matrices on L1, L0A_MX, and L0B_MX.

<a name="table4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 4: A / B Matrix m-axis / n-axis Alignment Requirements on L1 (L1 Layout Format is Nz)</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">FP4 (<code>fractalNum = 4</code>)</span></td>
    <td align="center"><span style="font-weight: bold;">FP8 (<code>fractalNum = 2</code>)</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A matrix non-transposed input [m, k]</span></td>
    <td colspan="2" align="center"><code>mAlignValue = fractalShape[0]</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A matrix transposed input [k, m]</span></td>
    <td align="center" ><code>mAlignValue = fractalShape[1] = 64</code></td>
    <td align="center" ><code>mAlignValue = fractalShape[1] = 32</code></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B matrix non-transposed input [k, n]</span></td>
    <td align="center" ><code>nAlignValue = fractalShape[1] = 64</code></td>
    <td align="center" ><code>nAlignValue = fractalShape[1] = 32</code></td>
  </tr>
 <tr>
    <td align="center"><span style="font-weight: bold;">B matrix transposed input [n, k]</span></td>
    <td colspan="2" align="center"><code>nAlignValue = fractalShape[0]</code></td>
  </tr>
</table>

<a name="table5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 5: A / B Matrix m-axis / n-axis Alignment Requirements on L0</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">FP4 (<code>fractalNum = 4</code>)</span></td>
    <td align="center"><span style="font-weight: bold;">FP8 (<code>fractalNum = 2</code>)</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A matrix non-transposed input [m, k], L1 -> L0A no transpose needed<br>(Scenario 1 / 3)</span></td>
    <td colspan="2" align="center"><code>mAlignValue = fractalShape[0]</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A matrix transposed input [k, m], L1 -> L0A transpose needed<br>(Scenario 2 / 4, single call)</span></td>
    <td align="center"><code>mAlignValue = fractalShape[0] * fractalNum = 64</code></td>
    <td align="center"><code>mAlignValue = fractalShape[0] * fractalNum = 32</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A matrix transposed input [k, m], L1 -> L0A transpose needed<br>(Scenario 5 / 6, for loop call)</span></td>
    <td colspan="2" align="center"><code>mAlignValue = fractalShape[0]</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B matrix non-transposed input [k, n], L1 -> L0B transpose needed<br>(Scenario 2 / 4 / 5 / 6)</span></td>
    <td align="center"><code>nAlignValue = fractalShape[0] * fractalNum = 64</code></td>
    <td align="center"><code>nAlignValue = fractalShape[0] * fractalNum = 32</code></td>
  </tr>
 <tr>
    <td align="center"><span style="font-weight: bold;">B matrix transposed input [n, k], L1 -> L0B no transpose needed<br>(Scenario 1 / 3)</span></td>
    <td colspan="2" align="center"><code>nAlignValue = fractalShape[0]</code></td>
  </tr>
</table>

<a name="table6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 6: scaleA / scaleB Matrix Alignment Requirements on Each Axis of L1 / L0</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">scaleA Matrix (L1 Layout Format is Zz)</span></td>
    <td align="center"><span style="font-weight: bold;">scaleB Matrix (L1 Layout Format is Nn)</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">m-axis / n-axis Alignment</span></td>
    <td align="center"><code>scaleMAlignL1 = CeilAlign(m, fractalShape[0])</code></td>
    <td align="center"><code>scaleNAlignL1 = CeilAlign(n, fractalShape[0])</code></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">k-axis Alignment</span></td>
    <td align="center"><code>scaleK = AscendC::Std::ceil_div(k, 64) * 2</code></td>
    <td align="center"><code>scaleK = AscendC::Std::ceil_div(k, 64) * 2</code></td>
  </tr>
</table>

### 5. GM to L1 (`DataCopy` and `Fill`)

This subsection describes `DataCopy` parameter configuration for the GM -> L1 phase for A matrix, B matrix, scaleA matrix, and scaleB matrix separately, as well as zero-padding operations needed on L1. Two types of constraints require focus:

1. scaleA / scaleB do not support transpose in the L1 -> L0A_MX / L0B_MX phase, so they need to be pre-converted to the layout format required by L0 in the GM -> L1 phase.
2. The `MmadMx` interface in MX matrix multiplication requires **k direction alignment to 64**, while `DataCopy` alignment behavior in the GM -> L1 phase depends on which axis k is on. Remaining k direction data needs to be flushed to 0 per scenario to prevent dirty data from participating in computation:
   - When k is in col direction, in FP8 data type `DataCopy` only aligns k direction to 32B (32 elements), need to continue padding to 64 element alignment.
   - When k is in row direction, `DataCopy` aligns k direction to 16, need to continue padding to 64 alignment.

The `DataCopy(Nd2NzParams)` notation below refers to calling the `DataCopy` instruction with `Nd2NzParams` structure parameters to convert ND layout to Nz large-small fractal layout.

#### 5.1. A Matrix GM -> L1

##### 5.1.1. A Matrix GM Input is [m, k]

<div align="center">
  <img src="figures/GM2L1/FP4_A_GM2L1_MK.png" width="900"><br>
  Figure 13: FP4 data type, A matrix [m, k] input, GM -> L1, ND -> Nz
</div>

<div align="center">
  <img src="figures/GM2L1/FP8_A_GM2L1_MK.png" width="900"><br>
  Figure 14: FP8 data type, A matrix [m, k] input, GM -> L1, ND -> Nz
</div>

**DataCopy (Nd2NzParams)**

- **dValue**: Takes `packedK`. FP4 input is transferred using B8 view, every 2 FP4 elements in k direction correspond to 1 B8 view element; FP8 input uses original k configuration.
- **dstNzC0Stride**: Unit is 32B, takes the aligned row count of the Nz matrix on L1, i.e., `mAlignL1`.
- **FP4 handling**: Per interface constraints, FP4 input is processed as B8 view during ND -> Nz, and parameters are configured accordingly.

```cpp
AscendC::Nd2NzParams nd2nzA1Params;
nd2nzA1Params.ndNum = 1; // Number of ND matrices
nd2nzA1Params.nValue = m; // Row count of source ND matrix
nd2nzA1Params.dValue = packedK; // Column count of source ND matrix configured as B8 view
nd2nzA1Params.srcNdMatrixStride = 0; // Start address offset between adjacent ND matrices in source operand
nd2nzA1Params.srcDValue = packedK;  // Start address offset between adjacent rows within the same ND matrix, configured as B8 view
nd2nzA1Params.dstNzC0Stride = mAlignL1; // After ND -> Nz, start address interval of segments split from the same row, unit 32B
nd2nzA1Params.dstNzNStride = 1;     // Offset in dst between row x and row x+1 of ND matrix after Nz conversion
nd2nzA1Params.dstNzMatrixStride = 0; // Offset between adjacent Nz matrix start addresses in destination Nz matrix, meaningless when set to 0
AscendC::DataCopy(a1Local, aGM, nd2nzA1Params);
```

**Fill**

- A matrix is `[m, k]` input, and input data type is FP8.
- **Zero-padding reason**: k is in col direction, `DataCopy` only aligns k direction to 32B; `MmadMx` interface in MX matrix multiplication requires k direction alignment to 64.
- **Zero-padding method**: Call `AscendC::Fill` to flush 1 block of data at the tail of k direction to 0, as shown in Figure 14.

```cpp
if constexpr (AscendC::IsSameType<TA, fp8_e4m3fn_t>::value || AscendC::IsSameType<TA, fp8_e5m2_t>::value) {
    // Fill A matrix L1 data as uint16 type with 0; when dst is on A1, Fill blockNum unit is 32B.
    const uint32_t heightAlign = CeilAlign(m, fractalShape[0]);
    auto padTensor = a1Local.template ReinterpretCast<uint16_t>();
    AscendC::InitConstValueParams<uint16_t> initConstValueParams;
    // repeatTimes represents iteration count; iterate in row direction, covering each row after m aligned to 16.
    initConstValueParams.repeatTimes = heightAlign;
    // blockNum represents the number of data blocks (32B) initialized per iteration; here only fill 1 32B block at col direction tail.
    initConstValueParams.blockNum = 1;
    // initValue represents initialization value; fill invalid data with 0 to prevent participation in MmadMx computation.
    initConstValueParams.initValue = 0;
    // dstOffset locates to the end of currently transferred data in col direction, subsequent Fill fills 1 32B block at the tail row by row.
    uint32_t dstOffset = heightAlign * (CeilAlign(packedK, SCALE_CEIL_NUMBER) / 2);
    AscendC::Fill(padTensor[dstOffset], initConstValueParams);
}
```
##### 5.1.2. A Matrix GM Input is [k, m]

<div align="center">
  <img src="figures/GM2L1/FP4_A_GM2L1_TRANS_KM.png" width="700"><br>
  Figure 15: FP4 data type, A matrix [k, m] input, GM -> L1, ND -> Nz
</div>

<div align="center">
  <img src="figures/GM2L1/FP8_A_GM2L1_TRANS_KM.png" width="700"><br>
  Figure 16: FP8 data type, A matrix [k, m] input, GM -> L1, ND -> Nz
</div>

**DataCopy (Nd2NzParams)**

- **nValue**: Takes `k`.
- **dValue**: `AscendC::Std::ceil_div(m, 2)` for FP4 data type; `m` for FP8 data type.
- **dstNzC0Stride**: Unit is 32B, takes the aligned row count of the Nz matrix on L1, i.e., the aligned length in k direction `alignK`.
- **FP4 handling**: Per interface constraints, FP4 input is processed as B8 view during ND -> Nz, and parameters are configured accordingly.

```cpp
AscendC::Nd2NzParams nd2nzA1Params;
uint16_t aColValue = isFP4 ? AscendC::Std::ceil_div(m, 2) : m;
nd2nzA1Params.ndNum = 1; // Number of ND matrices
nd2nzA1Params.nValue = k; // Row count of source ND matrix
nd2nzA1Params.dValue = aColValue; // Column count of source ND matrix configured as B8 view, 2 elements correspond to 1 B8 view element for FP4 input
nd2nzA1Params.srcNdMatrixStride = 0; // Start address offset between adjacent ND matrices in source operand
nd2nzA1Params.srcDValue = aColValue; // Start address offset between adjacent rows within the same ND matrix, configured as B8 view
nd2nzA1Params.dstNzC0Stride = alignK; // After ND -> Nz, start address interval of segments split from the same row, unit 32B
nd2nzA1Params.dstNzNStride = 1;      // Offset in dst between row x and row x+1 of ND matrix after Nz conversion
nd2nzA1Params.dstNzMatrixStride = 0; // Offset between adjacent Nz matrix start addresses in destination Nz matrix, meaningless when set to 0
AscendC::DataCopy(a1Local, aGM, nd2nzA1Params);
```

**Fill**

- A matrix is `[k, m]` input.
- **Zero-padding reason**: k is in row direction, `DataCopy` aligns k direction to 16; `MmadMx` interface in MX matrix multiplication requires k direction alignment to 64.
- **Zero-padding method**: Call `AscendC::Fill` to flush dirty data exceeding the original length in k direction to 0, FP4 data type as shown in Figure 15, FP8 data type as shown in Figure 16.

```cpp
// Pad invalid data in row direction [k, alignK] range.
// Fill A matrix L1 data as uint16 type with 0; when dst is on A1, Fill blockNum and dstGap unit is 32B.
auto padTensor = a1Local.template ReinterpretCast<uint16_t>();
AscendC::InitConstValueParams<uint16_t> initConstValueParams;
// repeatTimes represents iteration count; iterate in col direction.
initConstValueParams.repeatTimes = AscendC::Std::ceil_div(m, FP8_C0SIZE);
// blockNum represents the number of data blocks (32B) initialized per iteration; here fill invalid rows at row direction tail.
initConstValueParams.blockNum = alignK - k;
// dstGap represents the distance from the end address of the previous iteration to the start address of the next iteration; skip valid data in row direction.
initConstValueParams.dstGap = k;
// initValue represents initialization value; fill invalid data with 0 to prevent participation in MmadMx computation.
initConstValueParams.initValue = 0;
// Start address locates to the first fractal in row direction that needs zero-padding.
AscendC::Fill(padTensor[k * fractalShape[0]], initConstValueParams);
```
#### 5.2. B Matrix GM -> L1

##### 5.2.1. B Matrix GM Input is [k, n]

<div align="center">
  <img src="figures/GM2L1/FP4_B_GM2L1_TRANS_KN.png" width="700"><br>
  Figure 17: FP4 data type, B matrix [k, n] input, GM -> L1, ND -> Nz
</div>

<div align="center">
  <img src="figures/GM2L1/FP8_B_GM2L1_TRANS_KN.png" width="700"><br>
  Figure 18: FP8 data type, B matrix [k, n] input, GM -> L1, ND -> Nz
</div>

**DataCopy (Nd2NzParams)**

- **nValue**: Takes `k`.
- **dValue**: `AscendC::Std::ceil_div(n, 2)` for FP4 data type; `n` for FP8 data type.
- **dstNzC0Stride**: Unit is 32B, takes the aligned row count of the Nz matrix on L1, i.e., the aligned length in k direction `alignK`.
- **FP4 handling**: Per interface constraints, FP4 input is processed as B8 view during ND -> Nz, and parameters are configured accordingly.

```cpp
AscendC::Nd2NzParams nd2nzB1Params;
uint16_t bColValue = isFP4 ? AscendC::Std::ceil_div(n, 2) : n;
nd2nzB1Params.ndNum = 1; // Number of ND matrices
nd2nzB1Params.nValue = k; // Row count of source ND matrix
nd2nzB1Params.dValue = bColValue; // Column count of source ND matrix configured as B8 view, 2 elements correspond to 1 B8 view element for FP4 input
nd2nzB1Params.srcNdMatrixStride = 0; // Start address offset between adjacent ND matrices in source operand
nd2nzB1Params.srcDValue = bColValue; // Start address offset between adjacent rows within the same ND matrix, configured as B8 view
nd2nzB1Params.dstNzC0Stride = alignK; // After ND -> Nz, start address interval of segments split from the same row, unit 32B
nd2nzB1Params.dstNzNStride = 1; // Offset in dst between row x and row x+1 of ND matrix after Nz conversion
nd2nzB1Params.dstNzMatrixStride = 0; // Offset between adjacent Nz matrix start addresses in destination Nz matrix, meaningless when set to 0
AscendC::DataCopy(b1Local, bGM, nd2nzB1Params);
```

**Fill**

- B matrix is `[k, n]` input.
- **Zero-padding reason**: k is in row direction, `DataCopy` aligns k direction to 16; `MmadMx` interface in MX matrix multiplication requires k direction alignment to 64.
- **Zero-padding method**: Call `AscendC::Fill` to flush dirty data exceeding the original length in k direction to 0, FP4 data type as shown in Figure 17, FP8 data type as shown in Figure 18.

```cpp
// Pad invalid data in row direction [k, alignK] range.
// Fill B matrix L1 data as uint16 type with 0; when dst is on B1, Fill blockNum and dstGap unit is 32B.
auto padTensor = b1Local.template ReinterpretCast<uint16_t>();
AscendC::InitConstValueParams<uint16_t> initConstValueParams;
// repeatTimes represents iteration count; iterate in col direction.
initConstValueParams.repeatTimes = AscendC::Std::ceil_div(n, FP8_C0SIZE);
// blockNum represents the number of data blocks (32B) initialized per iteration; here fill invalid rows at row direction tail.
initConstValueParams.blockNum = alignK - k;
// dstGap represents the distance from the end address of the previous iteration to the start address of the next iteration; skip valid data in row direction.
initConstValueParams.dstGap = k;
// initValue represents initialization value; fill invalid data with 0 to prevent participation in MmadMx computation.
initConstValueParams.initValue = 0;
// Start address locates to the first fractal in row direction that needs zero-padding.
AscendC::Fill(padTensor[k * fractalShape[0]], initConstValueParams);
```
##### 5.2.2. B Matrix GM Input is [n, k]

<div align="center">
  <img src="figures/GM2L1/FP4_B_GM2L1_NK.png" width="800"><br>
  Figure 19: FP4 data type, B matrix [n, k] input, GM -> L1, ND -> Nz
</div>

<div align="center">
  <img src="figures/GM2L1/FP8_B_GM2L1_NK.png" width="800"><br>
  Figure 20: FP8 data type, B matrix [n, k] input, GM -> L1, ND -> Nz
</div>

**DataCopy (Nd2NzParams)**

- **nValue**: Takes `n`.
- **dValue**: Takes `packedK`. FP4 input is transferred using B8 view, every 2 FP4 elements in k direction correspond to 1 B8 view element; FP8 input uses original k configuration.
- **dstNzC0Stride**: Unit is 32B, takes the aligned row count of the Nz matrix on L1, i.e., the aligned length in n direction `nAlignL1`.
- **FP4 handling**: Per interface constraints, FP4 input is processed as B8 view during ND -> Nz, and parameters are configured accordingly.

```cpp
AscendC::Nd2NzParams nd2nzB1Params;
nd2nzB1Params.ndNum = 1; // Number of ND matrices
nd2nzB1Params.nValue = n; // Row count of source ND matrix
nd2nzB1Params.dValue = packedK; // Column count of source ND matrix configured as B8 view
nd2nzB1Params.srcNdMatrixStride = 0; // Start address offset between adjacent ND matrices in source operand
nd2nzB1Params.srcDValue = packedK; // Start address offset between adjacent rows within the same ND matrix, configured as B8 view
nd2nzB1Params.dstNzC0Stride = nAlignL1; // After ND -> Nz, start address interval of segments split from the same row, unit 32B
nd2nzB1Params.dstNzNStride = 1; // Offset in dst between row x and row x+1 of ND matrix after Nz conversion
nd2nzB1Params.dstNzMatrixStride = 0; // Offset between adjacent Nz matrix start addresses in destination Nz matrix, meaningless when set to 0
AscendC::DataCopy(b1Local, bGM, nd2nzB1Params);
```

**Fill**

- B matrix is `[n, k]` input, and input data type is FP8.
- **Zero-padding reason**: k is in col direction, `DataCopy` only aligns k direction to 32B; `MmadMx` interface in MX matrix multiplication requires k direction alignment to 64.
- **Zero-padding method**: Call `AscendC::Fill` to flush 1 block of data at the tail of k direction to 0, as shown in Figure 20.

```cpp
if constexpr (AscendC::IsSameType<TB, fp8_e4m3fn_t>::value || AscendC::IsSameType<TB, fp8_e5m2_t>::value) {
    // Fill B matrix L1 data as uint16 type with 0; when dst is on B1, Fill blockNum unit is 32B.
    const uint32_t heightAlign = CeilAlign(n, fractalShape[0]);
    auto padTensor = b1Local.template ReinterpretCast<uint16_t>();
    AscendC::InitConstValueParams<uint16_t> initConstValueParams;
    // repeatTimes represents iteration count; iterate in row direction, covering each row after n aligned to 16.
    initConstValueParams.repeatTimes = heightAlign;
    // blockNum represents the number of data blocks (32B) initialized per iteration; here only fill 1 32B block at col direction tail.
    initConstValueParams.blockNum = 1;
    // initValue represents initialization value; fill invalid data with 0 to prevent participation in MmadMx computation.
    initConstValueParams.initValue = 0;
    // dstOffset locates to the end of currently transferred data in col direction, subsequent Fill fills 1 32B block at the tail row by row.
    uint32_t dstOffset = heightAlign * (CeilAlign(packedK, SCALE_CEIL_NUMBER) / 2);
    AscendC::Fill(padTensor[dstOffset], initConstValueParams);
}
```
#### 5.3. scaleA Matrix GM -> L1

The scaleA matrix is fp8_e8m0_t data type. When arranged by the true fp8_e8m0_t data type, scaleA has Zz format on L1. Due to hardware constraints, scale matrices require 2-byte continuity in k direction. During `DataCopy`, fp8_e8m0_t needs to be transferred using B16 (half) view (every 2 fp8_e8m0_t elements correspond to 1 half element), resulting in B16 data type Nz layout on L1. The transfer method depends on the value of isAtranspose.

##### 5.3.1. scaleA Matrix GM Input is [m, scaleK] (isAtranspose=false)

Use `Dn2NzParams` structure parameters (B16 view) for transfer.

<div align="center">
  <img src="figures/GM2L1/scaleA_GM2L1_MK.png" width="900"><br>
  Figure 21: scaleA matrix [m, scaleK] input, GM -> L1, ND -> Zz
</div>

- **B16 view**: Every 2 `fp8_e8m0_t` elements are transferred as 1 half element.
- **Key parameters**: `dValue = m`, `nValue = scaleK / 2`, `dstNzC0Stride = scaleK / 2`.

```cpp
AscendC::GlobalTensor<half> scaleAGMB16;
scaleAGMB16.SetGlobalBuffer((__gm__ half *)(scaleAGM.GetPhyAddr()), m * scaleK / 2);
auto scaleA1LocalB16 = scaleA1Local.ReinterpretCast<half>();

AscendC::Dn2NzParams dn2nzParams;
dn2nzParams.dnNum = 1; // Number of DN matrices in source operand
dn2nzParams.dValue = m; // Row count of source DN matrix
dn2nzParams.nValue = scaleK / 2; // Column count of source DN matrix, 2 fp8_e8m0_t combined into 1 half in B16 view
dn2nzParams.srcDnMatrixStride = 0; // Start address offset between adjacent DN matrices in source operand
dn2nzParams.srcDValue = scaleK / 2; // Start address offset between adjacent rows within the same DN matrix
dn2nzParams.dstNzC0Stride = scaleK / 2; // After DN to Nz, start address interval of segments split from the same row, unit 32B
dn2nzParams.dstNzNStride = 1; // Offset in dst between row x and row x+1 of DN matrix after Nz conversion
dn2nzParams.dstNzMatrixStride = 0; // Start address offset between adjacent Nz matrices
AscendC::DataCopy(scaleA1LocalB16, scaleAGMB16, dn2nzParams);
```

##### 5.3.2. scaleA Matrix GM Input is [scaleK, m, 2] (isAtranspose=true)

Use `Nd2NzParams` structure parameters (B16 view) for transfer.

<div align="center">
  <img src="figures/GM2L1/scaleA_GM2L1_KM.png" width="1000"><br>
  Figure 22: scaleA matrix [scaleK, m, 2] input, GM -> L1, ND -> Zz
</div>

- **B16 view**: Every 2 `fp8_e8m0_t` elements are transferred as 1 half element.
- **Key parameters**: `nValue = scaleK / 2`, `dValue = m`, `dstNzC0Stride = scaleK / 2`.

```cpp
AscendC::GlobalTensor<half> scaleAGMB16;
scaleAGMB16.SetGlobalBuffer((__gm__ half *)(scaleAGM.GetPhyAddr()), m * scaleK / 2);
auto scaleA1LocalB16 = scaleA1Local.ReinterpretCast<half>();

AscendC::Nd2NzParams nd2nzParams;
nd2nzParams.ndNum = 1; // Number of ND matrices in source operand
nd2nzParams.nValue = scaleK / 2; // Row count of source ND matrix, 2 fp8 scale combined into 1 half in B16 view
nd2nzParams.dValue = m; // Column count of source ND matrix
nd2nzParams.srcDValue = m; // Start address offset between adjacent rows within the same ND matrix
nd2nzParams.dstNzC0Stride = scaleK / 2; // After ND -> Nz, start address interval of segments split from the same row, unit 32B
nd2nzParams.dstNzNStride = 1; // Offset in dst between row x and row x+1 of ND matrix after Nz conversion
nd2nzParams.dstNzMatrixStride = 0; // Start address offset between adjacent Nz matrices
AscendC::DataCopy(scaleA1LocalB16, scaleAGMB16, nd2nzParams);
```
#### 5.4. scaleB Matrix GM -> L1

The transfer method for scaleB matrix is similar to scaleA. When arranged by the true fp8_e8m0_t data type, scaleB has Nn format on L1. Due to hardware constraints, scale matrices require 2-byte continuity in k direction, also requiring B16 view for `DataCopy` transfer, resulting in B16 data type Nz layout on L1. The transfer method depends on the value of isBtranspose.

##### 5.4.1. scaleB Matrix GM Input is [scaleK, n, 2] (isBtranspose=false)

Use `Nd2NzParams` structure parameters (B16 view) for transfer.

<div align="center">
  <img src="figures/GM2L1/scaleB_GM2L1_KN.png" width="1000"><br>
  Figure 23: scaleB matrix [scaleK, n, 2] input, GM -> L1, ND -> Nn
</div>

- **B16 view**: Every 2 `fp8_e8m0_t` elements are transferred as 1 half element.
- **Key parameters**: `nValue = scaleK / 2`, `dValue = n`, `dstNzC0Stride = scaleK / 2`.

```cpp
AscendC::GlobalTensor<half> scaleBGMB16;
scaleBGMB16.SetGlobalBuffer((__gm__ half *)(scaleBGM.GetPhyAddr()), n * scaleK / 2);
auto scaleB1LocalB16 = scaleB1Local.ReinterpretCast<half>();

AscendC::Nd2NzParams nd2nzParams;
nd2nzParams.ndNum = 1; // Number of ND matrices in source operand
nd2nzParams.nValue = scaleK / 2; // Row count of source ND matrix, 2 fp8 scale combined into 1 half in B16 view
nd2nzParams.dValue = n; // Column count of source ND matrix
nd2nzParams.srcDValue = n; // Start address offset between adjacent rows within the same ND matrix
nd2nzParams.dstNzC0Stride = scaleK / 2; // After ND -> Nz, start address interval of segments split from the same row, unit 32B
nd2nzParams.dstNzNStride = 1; // Offset in dst between row x and row x+1 of ND matrix after Nz conversion
nd2nzParams.dstNzMatrixStride = 0; // Start address offset between adjacent Nz matrices
AscendC::DataCopy(scaleB1LocalB16, scaleBGMB16, nd2nzParams);
```

##### 5.4.2. scaleB Matrix GM Input is [n, scaleK] (isBtranspose=true)

Use `Dn2NzParams` structure parameters (B16 view) for transfer.

<div align="center">
  <img src="figures/GM2L1/scaleB_GM2L1_NK.png" width="1000"><br>
  Figure 24: scaleB matrix [n, scaleK] input, GM -> L1, ND -> Nn
</div>

- **B16 view**: Every 2 `fp8_e8m0_t` elements are transferred as 1 half element.
- **Key parameters**: `dValue = n`, `nValue = scaleK / 2`, `dstNzC0Stride = scaleK / 2`.

```cpp
AscendC::GlobalTensor<half> scaleBGMB16;
scaleBGMB16.SetGlobalBuffer((__gm__ half *)(scaleBGM.GetPhyAddr()), n * scaleK / 2);
auto scaleB1LocalB16 = scaleB1Local.ReinterpretCast<half>();

AscendC::Dn2NzParams dn2nzParams;
dn2nzParams.dnNum = 1; // Number of DN matrices in source operand
dn2nzParams.dValue = n; // Row count of source DN matrix
dn2nzParams.nValue = scaleK / 2; // Column count of source DN matrix, 2 fp8 scale combined into 1 half in B16 view
dn2nzParams.srcDnMatrixStride = 0; // Start address offset between adjacent DN matrices in source operand
dn2nzParams.srcDValue = scaleK / 2; // Start address offset between adjacent rows within the same DN matrix
dn2nzParams.dstNzC0Stride = scaleK / 2; // After DN to Nz, start address interval of segments split from the same row, unit 32B
dn2nzParams.dstNzNStride = 1; // Offset in dst between row x and row x+1 of DN matrix after Nz conversion
dn2nzParams.dstNzMatrixStride = 0; // Start address offset between adjacent Nz matrices
AscendC::DataCopy(scaleB1LocalB16, scaleBGMB16, dn2nzParams);
```


### 6. L1 to L0 (`LoadData`)

This subsection describes how to call the `LoadData` instruction and complete data transfer and format transformation through `LoadData2DParamsV2` and `LoadData2DMxParams` structure parameters when transferring A / B matrices from L1 to L0A / L0B and scaleA / scaleB matrices from L1 to L0A_MX / L0B_MX.

#### 6.1. `LoadData2DParamsV2` Structure Parameter Description

`LoadData2DParamsV2` structure parameters control A / B matrix data transfer from L1 to L0A / L0B (transpose can occur during this process), including:

- **sid**: Source matrix identifier, default 0
- **mStartPosition**: Source matrix row direction start position, unit is 16 elements
- **kStartPosition**: Source matrix col direction start position, unit is 32B
- **mStep**: Source matrix row direction transfer length, unit is 16 elements
- **kStep**: Source matrix col direction transfer length, unit is 32B
- **srcStride**: Source matrix col direction adjacent fractal start address interval, unit is 512B
- **dstStride**: Destination matrix col direction adjacent fractal start address interval, unit is 512B
- **ifTranspose**: Whether to enable transpose function, transposing each fractal matrix, default false. When set to true, completes small fractal transpose and synchronously adjusts large fractal layout.

Note: A / B matrix data fractal size is 512B

#### 6.2. `LoadData2DMxParams` Structure Parameter Description

`LoadData2DMxParams` structure parameters control scale matrix data transfer from L1 to L0A_MX / L0B_MX (pure transfer, no layout format change), including:

- **xStartPosition**: Source matrix row direction start position, unit is 1 32B fractal
- **yStartPosition**: Source matrix col direction start position, unit is 32B
- **xStep**: Source matrix row direction transfer length, unit is 1 32B fractal
- **yStep**: Source matrix col direction transfer length, unit is 32B
- **srcStride**: Source matrix row direction adjacent fractal (16*2) start address interval, unit is 32B
- **dstStride**: Destination matrix row direction adjacent fractal (16*2) start address interval, unit is 32B

Note: scale matrix data fractal size is 16\*2\*1=32B

Calling `LoadData` once with both `LoadData2DParamsV2` and `LoadData2DMxParams` structure parameters simultaneously completes A matrix transfer to L0A and corresponding scale matrix transfer to L0A_MX. L0A_MX Buffer and L0A addresses have a fixed ratio relationship; the `LoadData` instruction derives L0A_MX address from L0A address automatically, no user configuration needed. B matrix and scaleB work similarly:

```cpp
AscendC::LoadData(a2Local, a1Local, scaleA1Local, loadDataParams, loadMxDataParams);
AscendC::LoadData(b2Local, b1Local, scaleB1Local, loadDataParams, loadMxDataParams);
```

#### 6.3. A Matrix L1 -> L0A, scaleA Matrix L1 -> L0A_MX

##### 6.3.1. A Matrix L1 -> L0A No Transpose (Scenario 1 / 3)

<div align="center">
  <img src="figures/L12L0/FP4_A_L12L0_MK.png" width="1000"><br>
  Figure 25: FP4 data type, A matrix [m, k] input, L1 -> L0A no transpose, loadDataParams.ifTranspose = false
</div>

<div align="center">
  <img src="figures/L12L0/FP8_A_L12L0_MK.png" width="1000"><br>
  Figure 26: FP8 data type, A matrix [m, k] input, L1 -> L0A no transpose, loadDataParams.ifTranspose = false
</div>

<div align="center">
  <img src="figures/L12L0/scaleA_l12l0_KM.png" width="500"><br>
  Figure 27: scaleA matrix L1 -> L0A_MX
</div>

**Key Configuration**

- A matrix L1 -> L0A no transpose, `loadDataParams.ifTranspose = false`.
- A matrix on L1 is in Nz layout, `mStep` configured by row direction fractal count, `kStep` configured by col direction 32B block count.
- `srcStride` / `dstStride` represent source / destination matrix col direction adjacent fractal start address interval, unit is 512B.
- scaleA matrix is transferred to L0A_MX via the same `LoadData` call, `xStep` corresponds to row direction transfer length, `yStep` corresponds to col direction transfer length.

```cpp
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// Start transfer from row direction fractal 0, col direction 32B block 0 of A matrix L1 source operand
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
// A matrix non-transposed input [m, k], L1 -> L0A no transpose needed
// mStep/kStep represent row direction fractal count and col direction 32B block count respectively
loadDataParams.mStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[0]);
loadDataParams.kStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[1]);
// srcStride/dstStride represent source/destination matrix col direction adjacent fractal start address interval, unit 512B
loadDataParams.srcStride = AscendC::Std::ceil_div(mAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = false;

AscendC::LoadData2DMxParams loadMxDataParams;
// scaleA synchronously starts transfer from row direction fractal 0, col direction 32B block 0 of L1 source operand
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// xStep/yStep configure scaleA row/col direction transfer length; stride configured by row direction adjacent fractal interval
loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleMAlignL1, fractalShape[0]);
loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

AscendC::LoadData(a2Local, a1Local, scaleA1Local, loadDataParams, loadMxDataParams);
```

##### 6.3.2. A Matrix L1 -> L0A Transpose, Single Call (Scenario 2 / 4)

<div align="center">
  <img src="figures/L12L0/FP4_A_L12L0_TRANS_KM.png" width="900"><br>
  Figure 28: FP4 data type, A matrix [k, m] input, L1 -> L0A transpose, loadDataParams.ifTranspose = true, single LoadData call
</div>

<div align="center">
  <img src="figures/L12L0/FP8_A_L12L0_TRANS_KM.png" width="900"><br>
  Figure 29: FP8 data type, A matrix [k, m] input, L1 -> L0A transpose, loadDataParams.ifTranspose = true, single LoadData call
</div>

**Key Configuration**

- A matrix L1 -> L0A needs transpose, `loadDataParams.ifTranspose = true`.
- A matrix shape on L1 is `[kaAlignL1, mAlignL1]`, row direction corresponds to logical k dimension, col direction corresponds to logical m dimension.
- `mStep` configures row direction transfer length, `kStep` configures col direction transfer length.
- In single call, A matrix m direction transfers extra data exceeding 1 fractal, subsequent `MmadMx` parameters need to be adjusted accordingly, see Section 7.
- scaleA matrix is still transferred to L0A_MX via the same `LoadData` call.

```cpp
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// Start transfer from row direction fractal 0, col direction 32B block 0 of A matrix L1 source operand
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
// A matrix transposed input [k, m], L1 -> L0A needs transpose
// During transpose transfer, A matrix shape on L1 is [kaAlignL1, mAlignL1], row direction corresponds to logical k dimension, col direction corresponds to logical m dimension
loadDataParams.mStep = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]);
loadDataParams.kStep = AscendC::Std::ceil_div(mAlignL1, fractalShape[1]);
// srcStride/dstStride represent source/destination matrix col direction adjacent fractal start address interval, unit 512B
loadDataParams.srcStride = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;

AscendC::LoadData2DMxParams loadMxDataParams;
// scaleA synchronously starts transfer from row direction fractal 0, col direction 32B block 0 of L1 source operand
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// xStep/yStep configure scaleA row/col direction transfer length; stride configured by row direction adjacent fractal interval
loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleMAlignL1, fractalShape[0]);
loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

AscendC::LoadData(a2Local, a1Local, scaleA1Local, loadDataParams, loadMxDataParams);
```

##### 6.3.3. A Matrix L1 -> L0A Transpose, For Loop Call (Scenario 5 / 6)

<div align="center">
  <img src="figures/L12L0/FP4_A_L12L0_for_TRANS_KM.png" width="900"><br>
  Figure 30: FP4 data type, A matrix [k, m] input, L1 -> L0A transpose, loadDataParams.ifTranspose = true, for loop LoadData call
</div>

<div align="center">
  <img src="figures/L12L0/FP8_A_L12L0_for_TRANS_KM.png" width="900"><br>
  Figure 31: FP8 data type, A matrix [k, m] input, L1 -> L0A transpose, loadDataParams.ifTranspose = true, for loop LoadData call
</div>

<div align="center">
  <img src="figures/L12L0/scaleA_l12l0_KM.png" width="500"><br>
  Figure 32: scaleA matrix L1 -> L0A_MX, for loop LoadData call
</div>

**Key Configuration**

- A matrix L1 -> L0A needs transpose, `loadDataParams.ifTranspose = true`.
- For loop is used to avoid dirty data exceeding 1 fractal in m direction during single call.
- `mStepAlign` represents the row direction fractal count per loop transfer, 4 for FP4, 2 for FP8.
- `L0ALoopNum = AscendC::Std::ceil_div(kaAlignL0, fractalShape[0] * fractalNum)`, `mStartPosition` increments with `loopIdx`.
- scaleA matrix is only transferred to L0A_MX in the first loop iteration; subsequent loops set `xStep = 0`, `yStep = 0` to skip transfer.
- `dstOffset` records the destination address offset on L0A for each loop iteration, each loop skips dirty data fractals at the tail of m direction.

```cpp
uint16_t mStepAlign = isFP4 ? FP4_M_STEP_ALIGN : FP8_M_STEP_ALIGN;
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// kStartPosition is fixed to 0, each loop selects the current fractal in A matrix source row direction via mStartPosition
loadDataParams.kStartPosition = 0;
// Transpose transfer requires mStep aligned by data type: 4 fractals for FP4, 2 fractals for FP8
loadDataParams.mStep = mStepAlign;
// kStep corresponds to 32B block count in A matrix source col direction
loadDataParams.kStep = AscendC::Std::ceil_div(mAlignL0, fractalShape[1]);
// srcStride/dstStride represent source/destination matrix col direction adjacent fractal start address interval, unit 512B
loadDataParams.srcStride = AscendC::Std::ceil_div(kaAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(mAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;

AscendC::LoadData2DMxParams loadMxDataParams;
// scaleA row direction start fractal and col direction start 32B block are fixed to 0
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// srcStride/dstStride represent scaleA source/destination matrix row direction adjacent fractal start address interval, unit 32B
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

uint32_t dstOffset = 0;
uint16_t L0ALoopNum = AscendC::Std::ceil_div(kaAlignL0, fractalShape[0] * fractalNum);
for (uint16_t loopIdx = 0; loopIdx < L0ALoopNum; ++loopIdx) {
    // mStartPosition increments, A matrix updates start address along m direction each iteration, scaleA matrix completes full transfer in first for loop, subsequent for loops skip transfer
    loadDataParams.mStartPosition = mStepAlign * loopIdx;
    if (loopIdx != 0) {
        loadMxDataParams.xStep = 0;
        loadMxDataParams.yStep = 0;
    } else {
        loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleMAlignL1, fractalShape[0]);
        loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
    }
    AscendC::LoadData(a2Local[dstOffset], a1Local, scaleA1Local, loadDataParams, loadMxDataParams);
    dstOffset += CeilAlign(mAlignL0, fractalShape[0]) * fractalShape[1];
}
```
#### 6.4. B Matrix L1 -> L0B, scaleB Matrix L1 -> L0B_MX

The transfer method for B / scaleB matrices is similar to A / scaleA matrices. The difference is that B matrix has Zn layout on L0B, and scaleB matrix has Nn layout on L0B_MX.

##### 6.4.1. B Matrix L1 -> L0B No Transpose (Scenario 1 / 3)

<div align="center">
  <img src="figures/L12L0/FP4_B_L12L0_NK.png" width="900"><br>
  Figure 33: FP4 data type, B matrix [n, k] input, L1 -> L0B no transpose, loadDataParams.ifTranspose = false
</div>

<div align="center">
  <img src="figures/L12L0/FP8_B_L12L0_NK.png" width="900"><br>
  Figure 34: FP8 data type, B matrix [n, k] input, L1 -> L0B no transpose, loadDataParams.ifTranspose = false
</div>

<div align="center">
  <img src="figures/L12L0/scaleB_l12l0_KN.png" width="700"><br>
  Figure 35: scaleB matrix L1 -> L0B_MX
</div>

**Key Configuration**

- B matrix L1 -> L0B no transpose, `loadDataParams.ifTranspose = false`.
- B matrix shape on L1 is `[nAlignL1, kbAlignL1]`, row direction corresponds to logical n dimension, col direction corresponds to logical k dimension.
- `mStep` configures row direction transfer length, `kStep` configures col direction transfer length.
- scaleB matrix is transferred to L0B_MX via the same `LoadData` call, `xStep` corresponds to row direction transfer length, `yStep` corresponds to col direction transfer length.

```cpp
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// Start transfer from row direction fractal 0, col direction 32B block 0 of B matrix L1 source operand
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
// B matrix transposed input [n, k], L1 -> L0B no transpose needed
// mStep/kStep represent row direction fractal count and col direction 32B block count respectively
loadDataParams.mStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[0]);
loadDataParams.kStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[1]);
// srcStride/dstStride represent source/destination matrix col direction adjacent fractal start address interval, unit 512B
loadDataParams.srcStride = AscendC::Std::ceil_div(nAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(nAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = false;

// When configuring scaleB L1 -> L0B_MX transfer parameters, treat scaleNAlignL1 direction as row and scaleK direction as col
// i.e., small fractal 16*2, 16 in row direction, 2 in col direction
AscendC::LoadData2DMxParams loadMxDataParams;
// scaleB synchronously starts transfer from row direction fractal 0, col direction 32B block 0 of L1 source operand
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// xStep/yStep configure scaleB row/col direction transfer length; stride configured by row direction adjacent fractal interval
loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleNAlignL1, fractalShape[0]);
loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

AscendC::LoadData(b2Local, b1Local, scaleB1Local, loadDataParams, loadMxDataParams);
```

##### 6.4.2. B Matrix L1 -> L0B Transpose (Scenario 2 / 4 / 5 / 6)

<div align="center">
  <img src="figures/L12L0/FP4_B_L12L0_TRANS_KN.png" width="1000"><br>
  Figure 36: FP4 data type, B matrix [k, n] input, L1 -> L0B transpose, loadDataParams.ifTranspose = true
</div>

<div align="center">
  <img src="figures/L12L0/FP8_B_L12L0_TRANS_KN.png" width="1000"><br>
  Figure 37: FP8 data type, B matrix [k, n] input, L1 -> L0B transpose, loadDataParams.ifTranspose = true
</div>

**Key Configuration**

- B matrix L1 -> L0B needs transpose, `loadDataParams.ifTranspose = true`.
- B matrix shape on L1 is `[kbAlignL1, nAlignL1]`, row direction corresponds to logical k dimension, col direction corresponds to logical n dimension.
- `mStep` configures row direction transfer length, `kStep` configures col direction transfer length.
- When B matrix non-transposed input `[k, n]`, n direction extra transferred data does not exceed 1 fractal.
- scaleB matrix is transferred to L0B_MX via the same `LoadData` call.

```cpp
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.sid = 0;
// Start transfer from row direction fractal 0, col direction 32B block 0 of B matrix L1 source operand
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
// B matrix non-transposed input [k, n], L1 -> L0B needs transpose
// During transpose transfer, B matrix shape on L1 is [kbAlignL1, nAlignL1], row direction corresponds to logical k dimension, col direction corresponds to logical n dimension
loadDataParams.mStep = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]);
loadDataParams.kStep = AscendC::Std::ceil_div(nAlignL1, fractalShape[1]);
// srcStride/dstStride represent source/destination matrix col direction adjacent fractal start address interval, unit 512B
loadDataParams.srcStride = AscendC::Std::ceil_div(kbAlignL1, fractalShape[0]);
loadDataParams.dstStride = AscendC::Std::ceil_div(nAlignL0, fractalShape[0]);
loadDataParams.ifTranspose = true;

// When configuring scaleB L1 -> L0B_MX transfer parameters, treat scaleNAlignL1 direction as row and scaleK direction as col
// i.e., small fractal 16*2, 16 in row direction, 2 in col direction
AscendC::LoadData2DMxParams loadMxDataParams;
// scaleB synchronously starts transfer from row direction fractal 0, col direction 32B block 0 of L1 source operand
loadMxDataParams.xStartPosition = 0;
loadMxDataParams.yStartPosition = 0;
// xStep/yStep configure scaleB row/col direction transfer length; stride configured by row direction adjacent fractal interval
loadMxDataParams.xStep = AscendC::Std::ceil_div(scaleNAlignL1, fractalShape[0]);
loadMxDataParams.yStep = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.srcStride = scaleK / SCALE_EVEN_NUMBER;
loadMxDataParams.dstStride = scaleK / SCALE_EVEN_NUMBER;

AscendC::LoadData(b2Local, b1Local, scaleB1Local, loadDataParams, loadMxDataParams);
```



### 7. Matrix multiplication (`MmadMx`)

The MX matrix multiplication formula is `C = (scaleA ⊗ A) * (scaleB ⊗ B)`. The `MmadMx` interface will automatically complete the broadcast multiplication of the left/right matrix and the corresponding scale matrix. Every 32 elements in the K direction share a quantization factor.

**Key configuration**

- `mmadParams.k` is set to `alignK = CeilAlign(k, 64) = 128` to meet the requirement of 64 alignment in k direction for the `MmadMx` interface in MX matrix multiplication. This constraint comes from the MX matrix multiplied by the k-direction scale granularity: every 64 k elements correspond to 2 scale factors, `scaleK = AscendC::Std::ceil_div(k, 64) * 2`; the m/n direction does not need to be aligned by 64.
- `mmadParams.cmatrixInitVal = true` means initializing the C matrix.
- In scenario 2/4, when A matrix is ​​transposed and inputs `[k, m]` and `LoadData` is called in a single time, more than one fractal is moved in the m direction. Therefore, `mmadParams.m` is set to `CeilAlign(m, fractalShape[0] * fractalNum)`, allowing the multi-moved fractals to participate in the calculation, and the results corresponding to the invalid fractals will be skipped when `Fixpipe` is subsequently moved out.
- `mmadParams.m = m` in the remaining scenes. Scenario 5/6 Although the A matrix also transposes the input, the for loop method has avoided moving more than one fractal of dirty data in the m direction.

```cpp
AscendC::MmadParams mmadParams;
if constexpr (scenarioNum == 2 || scenarioNum == 4) {
    // Mmad defaults to aligning the m axis to 16, but due to the A transposition process, the m axis is aligned to fractalShape[0]*fractalNum,
    // Here, align according to fractalShape[0] * fractalNum, so that multiple fractals can participate in the calculation.
    // The results corresponding to invalid fractals will be skipped when Fixpipe moves out later.
    mmadParams.m = CeilAlign(m, fractalShape[0] * fractalNum);
} else {
    mmadParams.m = m;
}
mmadParams.n = n;
// MX matrix multiplication requires the k direction to be aligned by 64.
mmadParams.k = alignK;
//Initialize the C matrix.
mmadParams.cmatrixInitVal = true;
AscendC::MmadMx(c1Local, a2Local, b2Local, mmadParams);
```

### 8. L0C to GM (`Fixpipe`)

`FixpipeParamsArch3510` is a Fixpipe parameter structure dedicated to the dav-3510 architecture. `CO2Layout` is set to `ROW_MAJOR` to indicate that the output is in row-major ND format.

**Key configuration**

- `fixpipeParams.nSize = n`, `fixpipeParams.mSize = m`, only move out of the C matrix effective area.
- `fixpipeParams.dstStride = n`, represents the starting address offset of adjacent rows in the target ND matrix.
- The `fixpipeParams.srcStride` unit is an element and represents the starting address offset of adjacent Z layouts in the source Nz matrix.
- In scenario 2/4, the m alignment value of `srcStride` needs to be consistent with that of `MmadMx`, so take `CeilAlign(m, fractalShape[0] * fractalNum)`.
- `srcStride = CeilAlign(m, fractalShape[0])` in the remaining scenes.

```cpp
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.nSize = n;
fixpipeParams.mSize = m;
if constexpr (scenarioNum == 2 || scenarioNum == 4){
    // Scenario 2/4: Single call of A matrix transpose input [k, m], moving more dirty data in the m direction than 1 fractal
    fixpipeParams.srcStride = CeilAlign(m, fractalShape[0] * fractalNum);
} else {
    fixpipeParams.srcStride = CeilAlign(m, fractalShape[0]);
}
fixpipeParams.dstStride = n;
AscendC::Fixpipe<U, U, AscendC::CFG_ROW_MAJOR>(cGM, c1Local, fixpipeParams);
```

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
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

  If the preceding dependencies are not installed, the script cannot correctly generate FP4/FP8 input data and ground truth data, which may cause accuracy verification to fail.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # Build the project, default NPU mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the generated executable and execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify that the output is correct and confirm the algorithm logic
  ```
  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;   # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;   # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`-`6` | Scenario number |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
