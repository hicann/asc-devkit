# load_data_2dv2_l12l0 Example

## Overview

This example demonstrates how to call the `LoadData` interface and configure `LoadData2DParamsV2` structure parameters (abbreviated as `Load2Dv2` interface in this example) to transfer A / B matrices from L1 to L0A / L0B.

The example covers three input data types: int8_t, half, and float, as well as A / B matrix transposed and non-transposed input scenarios. L0A data layout is Nz format, L0B data layout is Zn format.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── load_data_2dv2_l12l0
│   ├── figures                          // Illustrations
│   ├── scripts
│   │   ├── gen_data.py                  // Script for generating input data and ground truth data
│   │   └── verify_result.py             // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                   // Build project file
│   ├── data_utils.h                     // Data read/write functions
│   ├── load_data_2dv2_l12l0.asc         // Ascend C example implementation & invocation example
│   └── README.md                        // Example documentation
```

## Example Description

### 1. Overall Flow

The overall flow of the example is as follows:

```text
GM(ND) -> L1(Nz) -> L0A(Nz)/L0B(Zn) -> L0C(Nz) -> GM(ND)
       │         │                  │          │
    DataCopy  Load2Dv2            Mmad        Fixpipe
```

**Step details**:

1. **GM -> L1**: Use `DataCopy` to implement ND to Nz format conversion.
2. **L1 -> L0A / L0B**: Use `Load2Dv2` interface with `LoadData2DParamsV2` structure parameters, controlling transpose via `ifTranspose`.
3. **Matrix multiplication**: Use `Mmad` interface to perform matrix multiplication.
4. **L0C -> GM**: Use `Fixpipe` interface to transfer results out.

This example uses a fixed matrix multiplication specification of `[m, n, k] = [40, 50, 70]`, with output C matrix `[40, 50]` in ND format. The kernel function selects different scenarios through `scenarioNum`.

### 2. Key Scenario Differences

The input shape, transpose flags, and L1 -> L0 transfer methods for each scenario are shown in [Table 1](#table1).

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Meaning of Different scenarioNum Values</span></caption>
  <tr>
    <td><span style="font-weight: bold;">scenarioNum</span></td>
    <td><span style="font-weight: bold;">Input Data Type</span></td>
    <td><span style="font-weight: bold;">Output Data Type</span></td>
    <td><span style="font-weight: bold;">Input Shape</span></td>
    <td><span style="font-weight: bold;">isAtranspose</span></td>
    <td><span style="font-weight: bold;">isBtranspose</span></td>
    <td><span style="font-weight: bold">Extra Dirty Fractals</span></td>
    <td><span style="font-weight: bold;">L1 -> L0 Transfer Method</span></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">1</span></td>
    <td rowspan="2">int8_t</td>
    <td rowspan="2">int32_t</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>No</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>Yes</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td rowspan="2">half</td>
    <td rowspan="2">float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>No</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>No</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td rowspan="2">float</td>
    <td rowspan="2">float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>No</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>No</td>
    <td><code>Load2Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">7</span></td>
    <td rowspan="1">int8_t</td>
    <td rowspan="1">int32_t</td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>No</td>
    <td>for loop + <code>Load2Dv2</code></td>
  </tr>
</table>

Key differences by transfer mode:

**`Load2Dv2` non-transpose transfer: Scenarios 1 / 3 / 5**

- A matrix non-transposed input `[m, k]`, B matrix transposed input `[n, k]`.
- L1 -> L0A / L0B both do not need transpose, `loadDataParams.ifTranspose = false`.

**`Load2Dv2` transpose transfer, single call: Scenarios 2 / 4 / 6**

- A matrix transposed input `[k, m]`, B matrix non-transposed input `[k, n]`.
- L1 -> L0A / L0B both need transpose, `loadDataParams.ifTranspose = true`.
- In scenario 2, int8_t transpose single call causes extra dirty data fractals in m direction; `Fixpipe` transfers out only the valid region via `fixpipeParams.mSize = m`.

**`Load2Dv2` transpose transfer, for loop call: Scenario 7**

- Scenario 7 is also int8_t transposed input, same as scenario 2.
- A matrix uses for loop to call `Load2Dv2`, skipping dirty data fractals at the tail of m direction when writing to L0A, so no extra dirty data fractals participate in matrix computation.

### 3. Parameter Description

The subsequent code and parameter descriptions repeatedly use fractal and alignment related variables. This section defines these concepts first.

- `fractalShape`: Small fractal shape. B8 / B16 / B32 input data types have shape `[16, 32 / sizeof(T)]`, where `T` represents the input data type. Fractal information for data types involved in this example is shown in [Table 2](#table2).
- `fractalSize`: Number of elements in 1 small fractal, see [Table 2](#table2).
- `fractalNum`: When L1 -> L0A / L0B requires transpose, `Load2Dv2` transposes by square matrix. When multiple consecutive small fractals need to merge into a square matrix, `fractalNum` represents the number of small fractals in that square matrix, see [Table 2](#table2).

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: Fractal Information for Different Data Types</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">fractalShape</span></td>
    <td align="center"><span style="font-weight: bold;">fractalSize</span></td>
    <td align="center"><span style="font-weight: bold;">fractalNum</span></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B8</span></td>
    <td align="center">[16, 32]</td>
    <td align="center">512</td>
    <td align="center">2</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center">[16, 16]</td>
    <td align="center">256</td>
    <td align="center">1</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B32</span></td>
    <td align="center">[16, 8]</td>
    <td align="center">128</td>
    <td align="center">2</td>
  </tr>
</table>

- `CeilAlign`: Ceiling alignment operation. For example, when `m = 30`, `CeilAlign(30, 16) = 32`, meaning that the m-axis is aligned to 16 and its aligned length is 32.

  ```cpp
  __aicore__ inline uint16_t CeilAlign(uint16_t size, uint16_t alignValue) {
      return (size + alignValue - 1) / alignValue * alignValue;
  }
  ```

- `CeilDivision`: Ceiling division, generally used to compute loop counts after ceiling alignment.
- `mAlignValue`: m axis aligns to `mAlignValue`. For example, when `mAlignValue = 32`, the m axis is aligned to 32. Similarly `nAlignValue`, `kaAlignValue`, `kbAlignValue`.
- `mAlignL1` and `mAlignL0`: Aligned values of m axis when A matrix is on L1 and L0A respectively. Similarly `nAlignL1`, `nAlignL0`, `kaAlignL1`, `kaAlignL0`, `kbAlignL1`, `kbAlignL0`.

### 4. Alignment Requirements

A / B matrices have different alignment requirements on each axis on L1 and L0. These values are used when configuring `Load2Dv2`, `Mmad`, and `Fixpipe` parameters. [Table 3](#table3) and [Table 4](#table4) summarize alignment requirements for A / B matrices on L1 and L0 when scenarioNum is 1-6; [Table 5](#table5) and [Table 6](#table6) summarize alignment requirements when scenarioNum is 7.

<a name="table3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 3: Alignment Requirements for A and B Matrices on L1 (L1 Layout Format is Nz)</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A matrix non-transposed input [m, k]</span></td>
    <td colspan="3" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kaAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A matrix transposed input [k, m]</span></td>
    <td colspan="2" align="center">kaAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kaAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >mAlignValue = fractalShape[1]</td>
    <td colspan="1" align="center" >mAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B matrix non-transposed input [k, n]</span></td>
    <td colspan="2" align="center">kbAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kbAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >nAlignValue = fractalShape[1]</td>
    <td colspan="1" align="center" >nAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B matrix transposed input [n, k]</span></td>
    <td colspan="3" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kbAlignValue = fractalShape[1]</td>
  </tr>
</table>

<a name="table4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 4: Alignment Requirements for A and B Matrices on L0</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A matrix non-transposed input [m, k], L1 -> L0A no transpose needed</span></td>
    <td colspan="3" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kaAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A matrix transposed input [k, m], L1 -> L0A transpose needed</span></td>
    <td colspan="2" align="center">kaAlignValue = fractalShape[1]</td>
    <td >kaAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >mAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >mAlignValue = fractalShape[0]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B matrix non-transposed input [k, n], L1 -> L0B transpose needed</span></td>
    <td colspan="2" align="center">kbAlignValue = fractalShape[1]</td>
      <td align="center">kbAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="2" align="center">nAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center" >nAlignValue = fractalShape[0]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B matrix transposed input [n, k], L1 -> L0B no transpose needed</span></td>
    <td colspan="3" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kbAlignValue = fractalShape[1]</td>
  </tr>
</table>

When scenarioNum=7, A matrix uses for loop + `Load2Dv2` for L1 -> L0A transfer, and L0A only writes the region aligned by valid data fractals.

<a name="table5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 5: Alignment Requirements for A and B Matrices on L1 when scenarioNum=7</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractalNum=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A matrix transposed input [k, m]</span></td>
    <td align="center" >kaAlignValue = fractalShape[0] * fractalNum</td>
  </tr>
    <tr>
    <td align="center" >mAlignValue = fractalShape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B matrix non-transposed input [k, n]</span></td>
    <td align="center" >kbAlignValue = fractalShape[0] * fractalNum</td>
  </tr>
    <tr>
    <td align="center" >nAlignValue = fractalShape[1]</td>
  </tr>
</table>

<a name="table6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 6: Alignment Requirements for A and B Matrices on L0 when scenarioNum=7</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractalNum=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A matrix transposed input [k, m], L1 -> L0A transpose needed</span></td>
    <td align="center" >mAlignValue = fractalShape[0]</td>
  </tr>
    <tr>
    <td align="center" >kaAlignValue = fractalShape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B matrix non-transposed input [k, n], L1 -> L0B transpose needed</span></td>
    <td align="center" >kbAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td align="center" >nAlignValue = fractalShape[0] * fractalNum</td>
  </tr>
</table>

### 5. `Load2Dv2`

A / B matrices on L1 are all in Nz layout, on L0A and L0B they are in Nz and Zn layouts respectively. The L1 -> L0 phase calls the `LoadData` interface with `LoadData2DParamsV2` structure parameters, abbreviated as `Load2Dv2` in this document.

When reading this section, first determine the scenario from [Table 1](#table1), then select alignment values from [Table 3](#table3)-[Table 6](#table6). Key parameters when configuring `LoadData2DParamsV2`:

- `ifTranspose`: Controls whether to transpose during L1 -> L0 transfer. Set to `false` when A matrix non-transposed input `[m, k]` and B matrix transposed input `[n, k]`; set to `true` when A matrix transposed input `[k, m]` and B matrix non-transposed input `[k, n]`.
- `mStep`, `kStep`: Represent the number of small fractals covered in row and col directions respectively. During transpose transfer, B8 / B32 need to transfer in square fractal combinations, related step sizes must satisfy `fractalNum` requirements.
- `srcStride`, `dstStride`: Represent intervals between adjacent small fractals in row direction of L1 source matrix and L0 destination matrix respectively.
- `mStartPosition`, `kStartPosition`: Represent the starting small fractal position in the L1 source matrix for this transfer. In scenario 7, `mStartPosition` is modified in the loop to transfer A matrix in segments.

#### 5.1. A Matrix L1 -> L0A Non-Transpose

When L1 -> L0A does not require transpose, the parameter configurations for B8 / B16 / B32 data types are essentially the same, differing only in `fractalShape`, see [Table 2](#table2). The following figure uses int8_t as an example.

<div align="center">
  <img src="figures/B8_A_l1_l0A_Load2dv2.png" width="800"><br>
  Figure 1: int8_t data type, L1 -> L0A non-transpose, Load2Dv2 data layout diagram
</div>

Parameter configuration key points:

- `mStep = CeilDivision(mAlignL1, fractalShape[0])`, representing the number of small fractals in the row direction for the m axis.
- `kStep = CeilDivision(kaAlignL1, fractalShape[1])`, representing the number of small fractals in the col direction for the k axis.
- `ifTranspose = false`, meaning only L1 Nz to L0A Nz layout transfer is performed, no transpose.

```cpp
mAlignL1 = CeilAlign(m, fractalShape[0]); // 48
kaAlignL1 = CeilAlign(k, fractalShape[1]); // 96
mAlignL0 = CeilAlign(m, fractalShape[0]); // 48
kaAlignL0 = CeilAlign(k, fractalShape[1]); // 96
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = CeilDivision(mAlignL1, fractalShape[0]); // 3
loadDataParams.kStep = CeilDivision(kaAlignL1, fractalShape[1]); // 3
loadDataParams.srcStride = CeilDivision(mAlignL1, fractalShape[0]); // 3
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]); // 3
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

#### 5.2. A Matrix L1 -> L0A Transpose

When L1 -> L0A requires transpose, `loadDataParams.ifTranspose = true`. During transpose, `Load2Dv2` completes small fractal transpose and L0A Nz layout writing via square fractal combinations; different data types have different square fractal combination methods, described below for B8 / B16 / B32.

##### 5.2.1. B8 Input Data Type

B8 input data type fractal is 16 * 32. During L1 -> L0 transpose, 2 fractals of 16 * 32 in the row direction are combined into a 32 * 32 square for transposing. The following uses int8_t as an example to illustrate single call and for loop call methods.

**Single Call**

The following figure shows calling `Load2Dv2` once to complete L1 -> L0A transfer and transpose:

<div align="center">
  <img src="figures/B8_A_l1_l0A_trans_load2dv2.png" width="800"><br>
  Figure 2: int8_t data type, L1 -> L0A transpose, single Load2Dv2 call data layout diagram
</div>

In this example `m = 40`. During B8 transpose transfer, `mAlignL1 = CeilAlign(m, fractalShape[1]) = 64`, `mAlignL1 - m = 24 > 16`. When calling `Load2Dv2` once, 1 extra invalid fractal is transferred in the m direction; during `Mmad` computation, `mmadParams.m = CeilAlign(m, fractalShape[0] * fractalNum)` must be set to include that fractal in computation, and finally `Fixpipe` transfers out only the valid region via `fixpipeParams.mSize = m`.

```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 96
mAlignL1 = CeilAlign(m, fractalShape[1]); // 64
mAlignL0 = CeilAlign(m, fractalShape[0] * fractalNum); // 64
kaAlignL0 = CeilAlign(k, fractalShape[1]); // 96
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kaAlignL1, fractalShape[0]); // 6
loadDataParams.kStep = CeilDivision(mAlignL1, fractalShape[1]); // 2
loadDataParams.srcStride = CeilDivision(kaAlignL1, fractalShape[0]); // 6
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

**For Loop Call**

The following figure shows calling `Load2Dv2` multiple times in a for loop to complete L1 -> L0A transfer and transpose:

<div align="center">
  <img src="figures/B8_A_l1_l0A_trans_for_load2dv2.png" width="800"><br>
  Figure 3: int8_t data type, L1 -> L0A transpose, for loop calling Load2Dv2 multiple times data layout diagram
</div>

When using for loop, transfer is done in segments along the L1 row direction (A matrix k axis), transferring 2 fractals in the k axis direction and `CeilDivision(mAlignL0, fractalShape[1])` fractals in the m axis direction each time. `dstStride` is configured with m direction valid data aligned to `fractalShape[0]`, skipping dirty data fractals at the tail of m direction when writing to L0A due to transpose over-read, so no extra dirty data fractals participate in `Mmad` computation in the m direction.

```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 96
mAlignL1 = CeilAlign(m, fractalShape[1]); // 64
mAlignL0 = CeilAlign(m, fractalShape[0]); // 48
kaAlignL0 = CeilAlign(k, fractalShape[1]); // 96
// Input is int8 type, A matrix [k,m] transposed input, L1 -> L0A requires transpose
// For loop calls Load2Dv2, iterating along k axis, each iteration transfers 2 fractals in k direction on L1, skips dirty data fractals at tail of m direction on L0A, extra transfer in m direction does not exceed 1 fractal
uint16_t L0ALoopNum = CeilDivision(kaAlignL0, fractalShape[0] * fractalNum); // 3
loadDataParams.mStep = INT8_M_STEP_ALIGN; // 2
loadDataParams.kStep = CeilDivision(mAlignL0, fractalShape[1]); // 2
loadDataParams.srcStride = CeilDivision(kaAlignL1, fractalShape[0]); // 6
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]); // 3
loadDataParams.ifTranspose = true;
uint32_t dstOffset = 0;
for (uint16_t loopIdx = 0; loopIdx < L0ALoopNum; ++loopIdx) {
    loadDataParams.mStartPosition = INT8_M_STEP_ALIGN * loopIdx;
    AscendC::LoadData(a2Local[dstOffset], a1Local, loadDataParams);
    dstOffset += CeilAlign(mAlignL0, fractalShape[0]) * fractalShape[1]; // 1536
}
```

##### 5.2.2. B16 Input Data Type

B16 input data type fractal is 16 * 16, one fractal is already a square. During L1 -> L0 transpose, it transposes by small fractal, and a single `Load2Dv2` call completes L1 -> L0A data transfer and transpose. This example uses half as the data type.

<div align="center">
  <img src="figures/B16_A_l1_l0A_trans_load2dv2.png" width="800"><br>
  Figure 4: half data type, L1 -> L0A transpose, single Load2Dv2 call data layout diagram
</div>

Parameter configuration key points:

- `mStep = CeilDivision(kaAlignL1, fractalShape[0])`, representing the number of small fractals in the row direction for the k axis.
- `kStep = CeilDivision(mAlignL1, fractalShape[1])`, representing the number of small fractals in the col direction for the m axis.
- `ifTranspose = true`, meaning transpose is completed when transferring to L0A.

```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 80
mAlignL1 = CeilAlign(m, fractalShape[1]); // 48
mAlignL0 = CeilAlign(m, fractalShape[0] * fractalNum); // 48
kaAlignL0 = CeilAlign(k, fractalShape[1]); // 80
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kaAlignL1, fractalShape[0]); // 5
loadDataParams.kStep = CeilDivision(mAlignL1, fractalShape[1]); // 3
loadDataParams.srcStride = CeilDivision(kaAlignL1, fractalShape[0]); // 5
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]); // 3
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

##### 5.2.3. B32 Input Data Type

B32 input data type fractal is 16 * 8. During L1 -> L0 transpose, 2 fractals of 16 * 8 in the col direction are combined into a 16 * 16 square for transposing. This example uses float as the data type.

<div align="center">
  <img src="figures/B32_A_l1_l0A_trans_load2dv2.png" width="800"><br>
  Figure 5: float data type, L1 -> L0A transpose, single Load2Dv2 call data layout diagram
</div>

In this example `m = 40`. B32 transpose transfer requires combining 2 fractals in the col direction into a square, **`kStep` must be a multiple of 2**, so L1 col direction (m direction) reads 1 extra invalid fractal. After writing to L0A, there is 1 extra invalid fractal at the tail of the k direction; since L0A is in Nz layout, configuring `mmadParams.k = k` in `Mmad` ensures only valid k axis data participates in computation.

```cpp
kaAlignL1 = CeilAlign(k, fractalShape[0]); // 80
mAlignL1 = CeilAlign(m, fractalShape[1] * fractalNum); // 48
mAlignL0 = CeilAlign(m, fractalShape[0]); // 48
kaAlignL0 = CeilAlign(k, fractalShape[1] * fractalNum); // 80
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kaAlignL1, fractalShape[0]); // 5
loadDataParams.kStep = CeilDivision(mAlignL1, fractalShape[1]); // 6
loadDataParams.srcStride = CeilDivision(kaAlignL1, fractalShape[0]); // 5
loadDataParams.dstStride = CeilDivision(mAlignL0, fractalShape[0]); // 3
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

#### 5.3. B Matrix L1 -> L0B Non-Transpose

When L1 -> L0B does not require transpose, the parameter configurations for B8 / B16 / B32 data types are essentially the same, differing only in `fractalShape`, see [Table 2](#table2). The following figure uses float as an example.

<div align="center">
  <img src="figures/B32_B_l1_l0B_load2dv2.png" width="1100"><br>
  Figure 6: float data type, L1 -> L0B non-transpose, single Load2Dv2 call data layout diagram
</div>

Parameter configuration key points:

- `mStep = CeilDivision(nAlignL1, fractalShape[0])`, representing the number of small fractals in the row direction for the n axis.
- `kStep = CeilDivision(kbAlignL1, fractalShape[1])`, representing the number of small fractals in the col direction for the k axis.
- `ifTranspose = false`, meaning only L1 Nz to L0B Zn layout transfer is performed, no transpose.

```cpp
nAlignL1 = CeilAlign(n, fractalShape[0]); // 64
kbAlignL1 = CeilAlign(k, fractalShape[1]); // 72
kbAlignL0 = CeilAlign(k, fractalShape[1]); // 72
nAlignL0 = CeilAlign(n, fractalShape[0]); // 64
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = CeilDivision(nAlignL1, fractalShape[0]); // 4
loadDataParams.kStep = CeilDivision(kbAlignL1, fractalShape[1]); // 9
loadDataParams.srcStride = CeilDivision(nAlignL1, fractalShape[0]); // 4
loadDataParams.dstStride = CeilDivision(nAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

#### 5.4. B Matrix L1 -> L0B Transpose

When L1 -> L0B requires transpose, `loadDataParams.ifTranspose = true`. During transpose, `Load2Dv2` completes small fractal transpose and L0B Zn layout writing via square fractal combinations; different data types have different square fractal combination methods, described below for B8 / B16 / B32.

##### 5.4.1. B8 Input Data Type

B8 input data type fractal is 16 * 32. During L1 -> L0 transpose, 2 fractals of 16 * 32 in the row direction are combined into a 32 * 32 square for transposing. This example uses int8_t as the data type.

<div align="center">
  <img src="figures/B8_B_l1_l0B_trans_load2dv2.png" width="1100"><br>
  Figure 7: int8_t data type, L1 -> L0B transpose, single Load2Dv2 call data layout diagram
</div>

In this example `k = 70`. B8 transpose transfer requires combining 2 fractals in the row direction into a square, **`mStep` must be a multiple of 2**, so L1 row direction (k direction) reads 1 extra invalid fractal.

```cpp
kbAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 96
nAlignL1 = CeilAlign(n, fractalShape[1]); // 64
kbAlignL0 = CeilAlign(k, fractalShape[1]); // 96
nAlignL0 = CeilAlign(n, fractalShape[0] * fractalNum); // 64
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kbAlignL1, fractalShape[0]); // 6
loadDataParams.kStep = CeilDivision(nAlignL1, fractalShape[1]); // 2
loadDataParams.srcStride = CeilDivision(kbAlignL1, fractalShape[0]); // 6
loadDataParams.dstStride = CeilDivision(nAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = true;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

##### 5.4.2. B16 Input Data Type

B16 input data type fractal is 16 * 16, one fractal is already a square. During L1 -> L0 transpose, it transposes by small fractal, and a single `Load2Dv2` call completes L1 -> L0B data transfer and transpose. This example uses half as the data type.

<div align="center">
  <img src="figures/B16_B_l1_l0B_trans_load2dv2.png" width="800"><br>
  Figure 8: half data type, L1 -> L0B transpose, single Load2Dv2 call data layout diagram
</div>

Parameter configuration key points:

- `mStep = CeilDivision(kbAlignL1, fractalShape[0])`, representing the number of small fractals in the row direction for the k axis.
- `kStep = CeilDivision(nAlignL1, fractalShape[1])`, representing the number of small fractals in the col direction for the n axis.
- `ifTranspose = true`, meaning transpose is completed when transferring to L0B.

```cpp
kbAlignL1 = CeilAlign(k, fractalShape[0] * fractalNum); // 80
nAlignL1 = CeilAlign(n, fractalShape[1]); // 64
kbAlignL0 = CeilAlign(k, fractalShape[1]); // 80
nAlignL0 = CeilAlign(n, fractalShape[0] * fractalNum); // 64
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kbAlignL1, fractalShape[0]); // 5
loadDataParams.kStep = CeilDivision(nAlignL1, fractalShape[1]); // 4
loadDataParams.srcStride = CeilDivision(kbAlignL1, fractalShape[0]); // 5
loadDataParams.dstStride = CeilDivision(nAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = true;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

##### 5.4.3. B32 Input Data Type

B32 input data type fractal is 16 * 8. During L1 -> L0 transpose, 2 fractals of 16 * 8 in the col direction are combined into a 16 * 16 square for transposing. This example uses float as the data type.

<div align="center">
  <img src="figures/B32_B_l1_l0B_trans_load2dv2.png" width="1100"><br>
  Figure 9: float data type, L1 -> L0B transpose, single Load2Dv2 call data layout diagram
</div>

In this example `n = 50`. B32 transpose transfer requires combining 2 fractals in the col direction into a square, **`kStep` must be a multiple of 2**, so L1 col direction (n direction) reads 1 extra invalid fractal. After writing to L0B, there is 1 extra invalid fractal at the tail of the k direction; since L0B is in Zn layout, configuring `mmadParams.k = k` in `Mmad` ensures only valid k axis data participates in computation.

```cpp
kbAlignL1 = CeilAlign(k, fractalShape[0]); // 80
nAlignL1 = CeilAlign(n, fractalShape[1] * fractalNum); // 64
kbAlignL0 = CeilAlign(k, fractalShape[1] * fractalNum); // 80
nAlignL0 = CeilAlign(n, fractalShape[0]); // 64
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = CeilDivision(kbAlignL1, fractalShape[0]); // 5
loadDataParams.kStep = CeilDivision(nAlignL1, fractalShape[1]); // 8
loadDataParams.srcStride = CeilDivision(kbAlignL1, fractalShape[0]); // 5
loadDataParams.dstStride = CeilDivision(nAlignL0, fractalShape[0]); // 4
loadDataParams.ifTranspose = true;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

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
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`-`7` | Scenario number: different data types and transpose combinations |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
