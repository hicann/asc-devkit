# load_data_l12l0 Example

## Overview

This example demonstrates the complete flow for B4 / B8 / B16 / B32 input data types, covering A / B matrix transfer from GM to L1, L1 to L0, matrix multiplication, and result transfer out.

The example covers A / B matrix transposed and non-transposed input combinations, focusing on the usage of `LoadData`, `LoadDataWithTranspose`, `Mmad`, and `Fixpipe` interfaces.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```
├── load_data_l12l0
│   ├── figures                     // Illustrations
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── load_data_l12l0.asc         // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

### 1. Overall Flow

This example uses a fixed matrix multiplication specification of `[m, n, k] = [40, 50, 70]`, with output C matrix `[40, 50]` in ND format. The kernel function name is `KernelLoadDataL12L0`, selecting different scenarios through `scenarioNum`.

A complete matrix multiplication includes four phases: GM -> L1, L1 -> L0A / L0B, matrix multiplication, and L0C -> GM, as shown below:

```text
GM(ND) -> L1(Nz/Zz) -> L0A(Zz)/L0B(Zn) -> L0C(Nz) -> GM(ND)
       │            │                       │          │
    DataCopy     LoadData                 Mmad       Fixpipe
```

<div align="center">
  <img src="figures/cube.png" width="900"><br>
  Figure 1: Matrix multiplication overall flow diagram
</div>

Each phase processes the following:

1. **GM -> L1**: Call `DataCopy` interface with `Nd2NzParams` structure parameters to convert A / B matrices from ND layout to Nz or Zz layout.
2. **L1 -> L0A / L0B**: Based on data type, transpose flags, and L1 layout, select `LoadData (LoadData2DParams)`, `LoadDataWithTranspose`, or `LoadData (LoadData3DParamsV2)` to complete transfer and format transformation. For convenience, these are abbreviated as `Load2D`, `LoadDataWithTranspose`, and `Load3Dv2` respectively below.
3. **Matrix multiplication**: Call `Mmad` interface to perform matrix multiplication.
4. **L0C -> GM**: Call `Fixpipe` interface to transfer result C matrix out to GM.

The data layout formats of A / B matrices in different storage units are shown in [Table 1](#table1). L1 uses Nz layout in regular scenarios; in scenario 14, A / B matrices on L1 use Zz layout, demonstrating the special transfer method for float transposed input through `LoadDataWithTranspose`.

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Data Layout Formats in Different Storage Units</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">GM</span></td>
    <td align="center"><span style="font-weight: bold;">L1</span></td>
    <td align="center"><span style="font-weight: bold;">L0A</span></td>
    <td align="center"><span style="font-weight: bold;">L0B</span></td>
    <td align="center"><span style="font-weight: bold;">L0C</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">Data Layout Format</span></td>
    <td align="center">ND</td>
    <td align="center">Nz</td>
    <td align="center">Zz</td>
    <td align="center">Zn</td>
    <td align="center">Zz</td>
  </tr>
</table>

14 scenarios are derived from combinations of input data type, `isAtranspose`, `isBtranspose`, L1 layout format, and L1 -> L0 transfer method. [Table 2](#table2) first describes the callable interfaces for the L1 -> L0 phase based on input data type and transpose requirements.

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: L1 -> L0, Callable <code>LoadData</code> Instructions in Different Scenarios</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4</span></td>
    <td align="center"><span style="font-weight: bold;">B8</span></td>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center"><span style="font-weight: bold;">B32</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A non-transposed input [m, k]<br>L1 -> L0A no transpose needed</span></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">A transposed input [k, m]<br>L1 -> L0A transpose needed</span></td>
    <td align="center">Not supported</td>
    <td align="center"><code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code>, <code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load3Dv2</code>, <br><code>LoadDataWithTranspose</code> (L1 data layout is Zz)</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B non-transposed input [k, n]<br>L1 -> L0B transpose needed</span></td>
    <td align="center"><code>LoadDataWithTranspose</code></td>
    <td align="center"><code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code>, <code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load3Dv2</code>, <br><code>LoadDataWithTranspose</code> (L1 data layout is Zz)</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B transposed input [n, k]<br>L1 -> L0B no transpose needed</span></td>
    <td align="center"><code>Load2D</code></td>
    <td align="center"><code>Load2D</code></td>
    <td align="center"><code>Load2D</code></td>
    <td align="center"><code>Load2D</code></td>
  </tr>
</table>

`loadDataMode` is a general term used in this document to describe the L1 -> L0 transfer method, not an Ascend C interface parameter. In the code, A matrix and B matrix select the L1 -> L0 transfer method through template parameters `loadData2AMode` and `loadData2BMode` respectively:

- `0`: Use `Load2D`, that is `LoadData (LoadData2DParams)`.
- `1`: Use `LoadDataWithTranspose`.
- `2`: Use `Load3Dv2`, that is `LoadData (LoadData3DParamsV2)`.

### 2. Key Scenario Differences

The input shape, transpose flags, and L1 -> L0 transfer methods for each scenario are shown in [Table 3](#table3).

Note: When scenarioNum is 1 to 13, A and B matrices on L1 are all in Nz layout; when scenarioNum=14, A and B matrices on L1 are all in Zz layout.

<a name="table3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 3: Meaning of Different scenarioNum Values</span></caption>
  <tr>
    <td><span style="font-weight: bold;">scenarioNum</span></td>
    <td><span style="font-weight: bold;">Input Data Type</span></td>
    <td><span style="font-weight: bold;">Output Data Type</span></td>
    <td><span style="font-weight: bold;">Input Shape</span></td>
    <td><span style="font-weight: bold;">isAtranspose</span></td>
    <td><span style="font-weight: bold;">isBtranspose</span></td>
    <td><span style="font-weight: bold;">A Matrix <code>LoadData</code> Instruction Type</span></td>
    <td><span style="font-weight: bold;">B Matrix <code>LoadData</code> Instruction Type</span></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">1</span></td>
    <td rowspan="2">int4_t</td>
    <td rowspan="2">int32_t</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>A [40, 70]<br>B [70, 50]</td>
    <td>false</td>
    <td>false</td>
    <td><code>Load3Dv2</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td rowspan="3">int8_t</td>
    <td rowspan="3">int32_t</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>LoadDataWithTranspose</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td rowspan="5">half</td>
    <td rowspan="5">float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">7</span></td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">8</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">9</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>LoadDataWithTranspose</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">10</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load3Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">11</span></td>
    <td rowspan="3">float</td>
    <td rowspan="3">float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">12</span></td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">13</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load3Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">14</span></td>
    <td rowspan="1">float</td>
    <td rowspan="1">float</td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>LoadDataWithTranspose</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
</table>

This section describes key differences by transfer mode.

**`Load2D` non-transpose transfer: Scenarios 1 / 3 / 6 / 11**

- A matrix non-transposed input `[m, k]`, use `Load2D` to transfer to L0A, for loop along m direction, transferring `AscendC::Std::ceil_div(k, fractalShape[1])` fractals in k direction at once.
- B matrix transposed input `[n, k]`, use `Load2D` to transfer to L0B, for loop along k direction, transferring `AscendC::Std::ceil_div(n, fractalShape[0])` fractals in n direction at once.
- Both A / B matrices complete L1 -> L0 transfer and large fractal layout format changes by configuring `srcStride`, `dstGap` and other parameters.

**A-side `Load3Dv2` non-transpose transfer: Scenarios 2 / 4 / 7 / 12**

- A matrix non-transposed input `[m, k]`, use `Load3Dv2` to transfer from L1 to L0A, converting Nz layout on L1 to Zz layout needed by L0A.
- `Load3Dv2` configures N as 1, convolution kernel width and height as 1, padding as 0, stride as 1, dilation as 1. After image to column expansion, the data layout can be viewed as Nz fractal layout on L1.
- A matrix completes transfer by configuring `l1H`, `l1W`, `kExtension`, `mExtension` and other parameters.
- B matrix uses `LoadDataWithTranspose` or `Load2D` based on scenario: in scenario 2, B matrix non-transposed input `[k, n]` completes transposed transfer through `LoadDataWithTranspose`; in scenarios 4 / 7 / 12, B matrix transposed input `[n, k]` completes non-transposed transfer through `Load2D`.

**`LoadDataWithTranspose` transpose transfer: Scenarios 5 / 9 / 14**

- In scenarios 5 / 9, A / B matrices on L1 are all in Nz layout. A matrix transposed input `[k, m]`, B matrix non-transposed input `[k, n]`, both use `LoadDataWithTranspose` for transfer with transpose.
- In scenario 14, A / B matrices on L1 are all in Zz layout, both use `LoadDataWithTranspose` to complete L1 -> L0 transfer with transpose during the transfer process.

**`Load2D` transpose transfer: Scenario 8**

- Both A / B matrices use `Load2D` with `ifTranspose = true`.
- A matrix for loops along m direction, B matrix for loops along k direction; the number of fractals transferred per iteration is the same as in `Load2D` non-transpose scenarios.
- During L1 -> L0 transfer, `Load2D` completes small fractal transpose and large fractal layout format changes through `ifTranspose = true`.

**`Load3Dv2` transpose transfer: Scenarios 10 / 13**

- Both A / B matrices use `Load3Dv2`.
- A matrix completes L1 -> L0A transpose transfer by configuring `enTranspose`, `l1H`, `l1W`, `kExtension`, `mExtension` and other parameters.
- B matrix enables transpose by default when calling `Load3Dv2`, no need to configure `enTranspose`, other configurations are similar to A matrix transfer.

### 3. Parameter Description

The subsequent code and parameter descriptions repeatedly use fractal and alignment related variables. This section defines these concepts first.

- `fractalShape`: Small fractal shape. B8 / B16 / B32 input data types have shape `[16, 32 / sizeof(T)]`, where `T` represents the input data type; B4 input data type has shape `[16, 64]`. Fractal information for data types involved in this example is shown in [Table 4](#table4).
- `fractalSize`: Number of elements in 1 small fractal, see [Table 4](#table4).
- `fractalNum`: When L1 -> L0A / L0B requires transpose, the transfer interface transposes by square matrix. When multiple consecutive small fractals need to merge into a square matrix, `fractalNum` represents the number of small fractals in that square matrix, see [Table 4](#table4).

<a name="table4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 4: Fractal Information for Different Data Types</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">fractalShape</span></td>
    <td align="center"><span style="font-weight: bold;">fractalSize</span></td>
    <td align="center"><span style="font-weight: bold;">fractalNum</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B4</span></td>
    <td align="center">[16, 64]</td>
    <td align="center">1024</td>
    <td align="center">4</td>
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

- `CeilAlign`: Ceiling alignment operation. For example, when `m = 30`, `CeilAlign(30, 16) = 32`, meaning align m axis to 16, resulting in m axis length of 32.

  ```cpp
  __aicore__ inline uint16_t CeilAlign(uint16_t size, uint16_t alignValue) {
      return (size + alignValue - 1) / alignValue * alignValue;
  }
  ```

- `ceil_div`: Ceiling division, generally used to compute the number of loops after ceiling alignment.
- `mAlignValue`: m axis aligns to `mAlignValue`. For example, `mAlignValue = 32` means m axis aligns to 32; similarly `nAlignValue`, `kAlignValue`.
- `mAlignL1` and `mAlignL0`: Aligned values of m axis when A matrix is on L1 and L0A respectively. Similarly `kAlignL1`, `kAlignL0`, `nAlignL1`, `nAlignL0`.
- `srcoffset` and `dstoffset`: On L1, the LocalTensor address offset when A / B matrix outer axis loops once; on L0A / L0B, the LocalTensor address offset when A / B matrix outer axis loops once.

### 4. Alignment Requirements

Note: For ease of understanding, this example defaults to using the m axis of A matrix and k axis of B matrix as outer axis loops, without considering the scenario where the longer axis between m and k is used as the outer axis.

Additionally, A and B matrices have different alignment requirements in row and col directions on L1 and L0. The alignment requirements for the 13 scenarios corresponding to scenarioNum values 1-13 in [Table 3](#table3) (L1 layout format is Nz) are summarized in [Table 5](#table5) and [Table 6](#table6):

<a name="table5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 5: Alignment Requirements for A, B Matrices on L1 (L1 layout format is Nz)</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4 (fractalNum=4)</span></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A non-transposed [m, k]</span></td>
    <td colspan="4" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">kAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A transposed [k, m]</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">mAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B non-transposed [k, n]</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">nAlignValue = fractalShape[1]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B transposed [n, k]</span></td>
    <td colspan="4" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">kAlignValue = fractalShape[1]</td>
  </tr>
</table>

<a name="table6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 6: Alignment Requirements for A, B Matrices on L0</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4 (fractalNum=4)</span></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A non-transposed [m, k], L1 -> L0A no transpose</span></td>
    <td colspan="4" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">kAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A transposed [k, m], L1 -> L0A transpose needed</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[1]</td>
    <td>kAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="3" align="center">mAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center">mAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B non-transposed [k, n], L1 -> L0B transpose needed</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[1]</td>
      <td align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center">nAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center">nAlignValue = fractalShape[1]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B transposed [n, k], L1 -> L0B no transpose</span></td>
    <td colspan="4" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">kAlignValue = fractalShape[1]</td>
  </tr>
</table>

For scenarioNum=14, the L1 layout format is Zz. The alignment requirements for A and B matrices on L1 and L0 in height and width directions are shown in [Table 7](#table7).

<a name="table7"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 7: scenarioNum=14, Alignment Requirements for A, B Matrices on L1 (Zz layout) and L0</span></caption>
  <tr>
    <td align="center"><span style="font-weight: bold;">Matrix Input</span></td>
    <td align="center"><span style="font-weight: bold;">L1 (Zz layout)</span></td>
    <td align="center"><span style="font-weight: bold;">L0</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A transposed [k, m]</span><br>float (fractalNum=2)</td>
    <td align="center">kAlignValue = fractalShape[0]<br>mAlignValue = fractalShape[1] * fractalNum</td>
    <td align="center">mAlignValue = fractalShape[0]<br>kAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B non-transposed [k, n]</span><br>float (fractalNum=2)</td>
    <td align="center">kAlignValue = fractalShape[0]<br>nAlignValue = fractalShape[1] * fractalNum</td>
    <td align="center">kAlignValue = fractalShape[1] * fractalNum<br>nAlignValue = fractalShape[0]</td>
  </tr>
</table>

### 5. GM to L1 (`DataCopy`)

This section explains how to call the `DataCopy` interface and configure `Nd2NzParams` to move the A/B matrix from the ND layout on GM to L1. First determine the L1 target layout according to the scenario:

- **Scenario 1-13**: After the A/B matrix is moved to L1, it is arranged in Nz, which is a conventional ND -> Nz move.
- **Scenario 14**: After the A/B matrix is transferred to L1, it is arranged in Zz, which is used to complete the float transposition transfer through `LoadDataWithTranspose`.

When configuring `Nd2NzParams`, focus on the following parameters:

- `nValue`, `dValue`: logical shape of the source ND matrix.
- `dstNzC0Stride`: The length of the Nz matrix on L1 after alignment in the row direction, the unit is 32B.
- `ndNum`, `srcNdMatrixStride`, `dstNzMatrixStride`: It is only necessary to split multiple ND matrices according to the height axis when constructing the Zz layout.

#### 5.1. A matrix GM -> L1

A matrix has two types of GM input shapes: non-transposed input `[m, k]` and transposed input `[k, m]`. Scenarios 1-13 are Nz layouts after being transferred to L1; Scenario 14 needs to construct Zz layouts. The main difference between different data types is the value of `dstNzC0Stride`. For details, please refer to [Table 5](#table5) and [Table 7](#table7).

##### 5.1.1. The input on A matrix GM is [m, k]

When the A matrix GM input is not transposed to `[m, k]`, it is transferred to L1 according to an ND matrix and converted to Nz layout. The following takes half input as an example.

<div align="center">
  <img src="figures/GM_L1_FP16_A_input_m_k_to_Nz.png" width="600"><br>
  Figure 2: A matrix is input without transposition ([m,k]), under half data type, GM -> L1 data layout diagram
</div>

Key points of parameter configuration:

- `nValue = m`, `dValue = k`, corresponding to the source ND matrix shape.
- `dstNzC0Stride = CeilAlign(m, fractalShape[0])`, represents the length of the Nz matrix on L1 after alignment in the row direction, the unit is 32B.
- `ndNum = 1`, indicates that the source data is transferred according to an ND matrix.

```cpp
nd2nzA1Params.ndNum = 1;
nd2nzA1Params.nValue = m;
nd2nzA1Params.dValue = k;
nd2nzA1Params.srcNdMatrixStride = 0;
nd2nzA1Params.srcDValue = k;

// The following parameter takes the aligned length of the A matrix in the row direction on L1.
nd2nzA1Params.dstNzC0Stride = CeilAlign(m, fractalShape[0]);

nd2nzA1Params.dstNzNStride = 1;
nd2nzA1Params.dstNzMatrixStride = 0;
```

##### 5.1.2. The input on A matrix GM is [k, m]

**1. The format on L1 is Nz**

When the A matrix GM input is transposed to `[k, m]` and the L1 target layout is Nz, it is still transferred as an ND matrix. The following takes float input as an example.

<div align="center">
  <img src="figures/GM_L1_FP32_A_transInput_k_m_to_Nz.png" width="500"><br>
  Figure 3: A matrix transpose input, under float data type, GM -> L1, ND -> Nz
</div>

Key points of parameter configuration:

- `nValue = k`, `dValue = m`, corresponding to the source ND matrix shape.
- `dstNzC0Stride = CeilAlign(k, fractalShape[0])`, represents the length of the Nz matrix on L1 after alignment in the row direction, the unit is 32B.
- `ndNum = 1`, indicates that the source data is transferred according to an ND matrix.

```cpp
nd2nzA1Params.ndNum = 1;
nd2nzA1Params.nValue = k;
nd2nzA1Params.dValue = m;
nd2nzA1Params.srcNdMatrixStride = 0;
nd2nzA1Params.srcDValue = m;
nd2nzA1Params.dstNzNStride = 1;
nd2nzA1Params.dstNzMatrixStride = 0;
// The following parameter takes the aligned length of the A matrix in the row direction on L1.
if constexpr (AscendC::IsSameType<T, float>::value) {
  nd2nzA1Params.dstNzC0Stride = CeilAlign(k, fractalShape[0]);
}
```

**2. The format on L1 is Zz**

When the A matrix GM input is transposed to `[k, m]` and the L1 target layout is Zz, the source ND matrix needs to be divided into multiple small ND matrices along the height axis according to `fractalShape[0]`, and then transferred through `DataCopy` into multiple Nz matrices with only one height fractal, which is equivalent to the Zz layout.

<div align="center">
  <img src="figures/GM_L1_FP32_A_inputTrans_k_m_to_Zz.png" width="600"><br>
  Figure 4: A matrix transpose input, under float data type, GM -> L1, ND -> Zz
</div>

In scenario 14, if the float transposed input uses `LoadDataWithTranspose` to complete the L1 -> L0A transposed transfer, the A matrix on L1 needs to satisfy the Zz layout. Therefore the GM -> L1 stage needs to construct Zz with the following configuration:

- `ndNum = AscendC::Std::ceil_div(k, fractalShape[0])`, means splitting into 16 lines along the k-axis.
- `nValue = fractalShape[0]`, each small ND matrix height is 16.
- `dstNzMatrixStride = fractalShape[0] * CeilAlign(m, fractalShape[1] * fractalNum)`, controls the writing interval after adjacent small ND matrices are transferred.

```cpp
nd2nzA1Params.ndNum = AscendC::Std::ceil_div(k, fractalShape[0]);
nd2nzA1Params.nValue = fractalShape[0];
nd2nzA1Params.dValue = m;
nd2nzA1Params.srcNdMatrixStride = fractalShape[0] * m;
nd2nzA1Params.srcDValue = m;
nd2nzA1Params.dstNzC0Stride = fractalShape[0];
nd2nzA1Params.dstNzNStride = 1;
nd2nzA1Params.dstNzMatrixStride = fractalShape[0] * CeilAlign(m, fractalShape[1] * fractalNum);
```

#### 5.2. B matrix GM -> L1

The B matrix has two types of GM input shapes: non-transposed input `[k, n]` and transposed input `[n, k]`. Scenarios 1-13 are Nz layouts after being transferred to L1; Scenario 14 needs to construct Zz layouts. The main difference between different data types is the value of `dstNzC0Stride`. For details, please refer to [Table 5](#table5) and [Table 7](#table7).

##### 5.2.1. The input on B matrix GM is [k, n]

**1. The format on L1 is Nz**

When the B matrix GM input is not transposed `[k, n]` and the L1 target layout is Nz, it is transferred to L1 according to an ND matrix and converted to the Nz layout. The following takes float input as an example.

<div align="center">
  <img src="figures/GM_L1_FP32_B_input_k_n_to_Nz.png" width="500"><br>
  Figure 5: B matrix input without transposition, under float data type, GM -> L1, ND -> Nz
</div>

Key points of parameter configuration:

- `nValue = k`, `dValue = n`, corresponding to the source ND matrix shape.
- `dstNzC0Stride = CeilAlign(k, fractalShape[0])`, represents the length of the Nz matrix on L1 after alignment in the row direction, the unit is 32B.
- `ndNum = 1`, indicates that the source data is transferred according to an ND matrix.

```cpp
nd2nzB1Params.ndNum = 1;
nd2nzB1Params.nValue = k;
nd2nzB1Params.dValue = n;
nd2nzB1Params.srcNdMatrixStride = 0;
nd2nzB1Params.srcDValue = n;
nd2nzB1Params.dstNzNStride = 1;
nd2nzB1Params.dstNzMatrixStride = 0;
nd2nzB1Params.dstNzC0Stride = CeilAlign(k, fractalShape[0]);
```

**2. The format on L1 is Zz**

When the B matrix GM input is not transposed `[k, n]` and the L1 target layout is Zz, the idea of constructing the Zz layout is the same as the A matrix: divide the source ND matrix into multiple small ND matrices along the height axis according to `fractalShape[0]`, and then transfer them into an equivalent Zz layout.

<div align="center">
  <img src="figures/GM_L1_FP32_B_input_k_n_to_Zz.png" width="600"><br>
  Figure 6: B matrix input without transposition, under float data type, GM -> L1, ND -> Zz
</div>

In Scenario 14, if the float input is not transposed and `LoadDataWithTranspose` is used to complete the L1 -> L0B transposition transfer, the B matrix on L1 needs to satisfy the Zz layout. Therefore the GM -> L1 stage needs to construct Zz with the following configuration:

- `ndNum = AscendC::Std::ceil_div(k, fractalShape[0])`, means splitting into 16 lines along the k-axis.
- `nValue = fractalShape[0]`, each small ND matrix height is 16.
- `dstNzMatrixStride = fractalShape[0] * CeilAlign(n, fractalShape[1] * fractalNum)`, controls the writing interval after adjacent small ND matrices are transferred.

```cpp
nd2nzB1Params.ndNum = AscendC::Std::ceil_div(k, fractalShape[0]);
nd2nzB1Params.nValue = fractalShape[0];
nd2nzB1Params.dValue = n;
nd2nzB1Params.srcNdMatrixStride = fractalShape[0] * n;
nd2nzB1Params.srcDValue = n;
nd2nzB1Params.dstNzC0Stride = fractalShape[0];
nd2nzB1Params.dstNzNStride = 1;
nd2nzB1Params.dstNzMatrixStride = fractalShape[0] * CeilAlign(n, fractalShape[1] * fractalNum);
```

##### 5.2.2. The input on B matrix GM is [n, k]

When the B matrix GM input is transposed to `[n, k]`, it is transferred to L1 according to an ND matrix and converted to Nz layout. The following takes half input as an example.

<div align="center">
  <img src="figures/GM_L1_FP16_B_transInput_n_k_to_Nz.png" width="600"><br>
  Figure 7: B matrix transpose, under half data type, GM -> L1, ND -> Nz
</div>

Key points of parameter configuration:

- `nValue = n`, `dValue = k`, corresponding to the source ND matrix shape.
- `dstNzC0Stride = CeilAlign(n, fractalShape[0])`, represents the length of the Nz matrix on L1 after alignment in the row direction, the unit is 32B.
- `ndNum = 1`, indicates that the source data is transferred according to an ND matrix.

```cpp
nd2nzB1Params.ndNum = 1;
nd2nzB1Params.nValue = n;
nd2nzB1Params.dValue = k;
nd2nzB1Params.srcNdMatrixStride = 0;
nd2nzB1Params.srcDValue = k;

// The following parameter takes the aligned length of the B matrix in the row direction on L1.
nd2nzB1Params.dstNzC0Stride = CeilAlign(n, fractalShape[0]);
nd2nzB1Params.dstNzNStride = 1;
nd2nzB1Params.dstNzMatrixStride = 0;
```

### 6. L1 to L0 (`LoadData`)

This section explains how to transfer the A/B matrices from L1 to L0A/L0B. When reading, first determine the current scenario according to [Table 3](#table3), and then enter the corresponding section according to the following rules:

- **No transposition transfer**: Only the large fractal layout format changes. After the A matrix is moved to L0A, it becomes Zz, and after the B matrix is moved to L0B, it becomes Zn.
- **Transposition Movement**: Small fractal transposition and large fractal layout format changes occur at the same time.
- **L1 is Nz layout**: You can select `Load2D`, `LoadDataWithTranspose` or `Load3Dv2` according to the data type.
- **L1 is arranged for Zz**: Scenario 14 uses `LoadDataWithTranspose` to complete float transposition.

The code block focuses on parameters such as `repeatTimes`, `srcStride`, `dstGap`, `dstFracGap`, `ifTranspose`, and `enTranspose`, which determine the number of fractals transferred at one time, the interval between adjacent fractals, and whether to transpose.

#### 6.1. A matrix L1 -> L0A

After the A matrix is moved into L0A, the target layout is Zz. Depending on whether A matrix needs to be transposed, it is divided into the following two categories:

- **A matrix does not transpose input `[m, k]`**: L1 -> L0A does not need to be transposed, `Load2D` or `Load3Dv2` can be used.
- **A matrix transposition input `[k, m]`**: L1 -> L0A needs to be transposed, and the interface needs to be selected according to the data type and L1 layout. For details, see [Table 2](#table2).

##### 6.1.1. A matrix L1 -> L0A is not transposed

In this scenario, only the large fractal layout format changes: the A matrix on L1 is Nz layout, and after being transferred to L0A, it is Zz layout. B4/B8/B16/B32 input data types can use `Load2D` or `Load3Dv2`. The parameter configuration is basically the same. The main difference is `fractalShape`. Please refer to [Table 4](#table4). The following uses half as an example.

**1. `Load2D` interface**

The diagram of calling the `Load2D` interface is as follows:

<div align="center">
  <img src="figures/L1_L0A_FP16_A_Load2D.png" width="800"><br>
  Figure 8: Under the half data type, L1 -> L0A is not transposed, and the Load2D data layout is called
</div>

Key points of parameter configuration:

- Perform a for loop with the m-axis direction as the outer axis, and configure `repeatTimes` with the k-axis direction as the inner axis.
- `srcOffset` represents the LocalTensor address offset in the m-axis direction of the A matrix on L1 every cycle.
- `dstOffset` represents the LocalTensor address offset in the m-axis direction of the A matrix on L0A every cycle.
- `ifTranspose = false`, indicating that only the large fractal layout format change of Nz -> Zz is carried out.

```cpp
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[1]) * fractalSize;
uint32_t srcOffset = fractalSize;
// Nz -> Zz
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[1]);
loadDataParams.srcStride = AscendC::Std::ceil_div(m, fractalShape[0]);
// Between adjacent iterations in the K-axis direction, the distance between the end address of the previous fractal and the start address of the next fractal of the destination operand
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = false;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[0]); ++i) {
    AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

**2. `Load3Dv2` interface**

Calling the `Load3Dv2` interface is as follows:

<div align="center">
  <img src="figures/L1_L0A_F16_A_Load3DV2.png" width="800"><br>
  Figure 9: Under the half data type, L1 -> L0A is not transposed, calling Load3Dv2 data layout diagram
</div>

The essence of `Load3D` is to expand the Feature Map in NC1HWC0 format through Image to Column, and then select the specified data block from the expanded two-dimensional matrix and move it into the corresponding memory location. When configured according to the parameters shown in the following code, `Load3Dv2` can be called once to convert the data layout format from Nz to Zz when L1 is transferred to L0A. According to the process of completing img2col by `Load3Dv2`, it can be seen that the height of the A matrix after img2col is ho * wo. According to the calculation formulas of ho and wo, substituting the convolution kernel width, convolution kernel sliding step, convolution kernel expansion coefficient and other parameters, it can be seen that the height of the A matrix is CeilAlign (m, fractalShape[0]); the width of the A matrix after img2col is ho * wo, ci * kh * kw, substituting kh=1, kw=1, it can be seen that the width of A matrix is CeilAlign(k, fractalShape[1]).

```cpp
// Load3Dv2: Nz -> Zz
AscendC::LoadData3DParamsV2<T> loadDataParams;
// Source operand height
loadDataParams.l1H = 1;
// Source operand width
loadDataParams.l1W = CeilAlign(m, fractalShape[0]);
// The number of channels of the source operand,
// The result matrix height of img2col is ho * wo. According to the calculation formulas of ho and wo, by substituting the convolution kernel width, convolution kernel sliding step size, convolution kernel expansion coefficient and other parameters, we can know: ho * wo = loadDataParams.l1H * loadDataParams.l1w
// The width of the result matrix of img2col is ci * kh * kw. Substituting kh=1, kw=1, it can be seen that the width of the result matrix is ci=loadDataParams.channelSize = m
loadDataParams.channelSize = CeilAlign(k, fractalShape[1]);
// The transmission length of this instruction in the width dimension of the destination operand, if the rightmost fractal is not covered, should be a multiple of 16 for the half type and a multiple of 32 for int8_t/uint8_t; in the case of coverage, there is no multiple requirement.
loadDataParams.kExtension = CeilAlign(k, fractalShape[1]);
// The transmission length of this instruction in the height dimension of the destination operand, if the lowermost fractal is not covered, should be a multiple of 16 for half/int8_t/uint8_t; in the case of coverage, there is no multiple requirement.
loadDataParams.mExtension = CeilAlign(m, fractalShape[0]);
// The step size of the convolution kernel sliding in the source operand width dimension
loadDataParams.strideW = 1;
// The step size of the convolution kernel sliding in the source operand height dimension
loadDataParams.strideH = 1;
// Convolution kernel width
loadDataParams.filterW = 1;
// Convolution kernel height
loadDataParams.filterH = 1;
// Convolution kernel width expansion coefficient
loadDataParams.dilationFilterW = 1;
// Convolution kernel height expansion coefficient
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = false;
loadDataParams.fMatrixCtrl = false;
```

##### 6.1.2. Transpose of A matrix L1 -> L0A

The scenario undergoes small fractal transposition and large fractal layout format changes at the same time. Atlas A3 training series products/Atlas A3 inference series products and Atlas A2 training series products/Atlas A2 inference series products do not support the B4 data type; the available interfaces for B8 / B16 / B32 input data types are different. The following uses int8_t, half, and float as examples respectively.

- **int8_t**: Use `LoadDataWithTranspose`.
- **half**: `Load2D`, `LoadDataWithTranspose` or `Load3Dv2` can be used.
- **float**: Use `Load3Dv2` when L1 is Nz; use `LoadDataWithTranspose` when L1 is Zz.

###### 6.1.2.1. int8_t input data type

Calling the `LoadDataWithTranspose` interface is as follows:

<div align="center">
  <img src="figures/L1_L0A_B8_A_trans_LoadDataWithTranspose.png" width="800"><br>
  Figure 10: Under the int8_t data type, L1 -> L0A transpose, calling LoadDataWithTranspose data layout diagram
</div>

Key points of parameter configuration:

- Perform a for loop with the m-axis direction as the outer axis, and configure `repeatTimes` with the k-axis direction as the inner axis.
- When transposed, multiple consecutive fractals are merged into a square matrix, so `repeatTimes = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum)`.
- `dstFracGap` is used to control the interval between fractals within the same transposition.

```cpp
// LoadDataWithTranspose: Nz-> Zz
// According to the following function prototype, the data type of offset is uint32_t
// __aicore__ inline LocalTensor operator[](const uint32_t offset) const
// dstoffset should be solved based on the alignment of the A matrix on L0 in the width direction.
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[1]) * fractalSize * fractalNum;
// srcoffset should be solved based on the alignment of the A matrix on L1 in the height direction.
uint32_t srcOffset = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;

AscendC::LoadData2dTransposeParams loadDataParams;
// The starting position of the transfer is the square matrix in the source operand (0 is the first square matrix in the source operand)
loadDataParams.startIndex = 0;
// Number of iterations, each iteration transposes a square matrix
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
// Between adjacent iterations, the interval between the starting address of the previous fractal and the next fractal of the source operand. The unit is the size of the square matrix
loadDataParams.srcStride = 1;
// Between adjacent iterations, the interval between the end address of the first fractal of the previous iteration of the destination operand and the start address of the first fractal of the next iteration, unit: 512B
loadDataParams.dstGap = 0;
// The destination operand in each iteration transposes the interval between the end address of the previous fractal and the start address of the next fractal, the unit is 512B
loadDataParams.dstFracGap = AscendC::Std::ceil_div(k, fractalShape[1]) - 1;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[1]); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

###### 6.1.2.2. half input data type

**1. `Load2D` interface**

Calling the `Load2D` interface is as follows:

<div align="center">
  <img src="figures/L1_L0A_F16_A_trans_Load2D.png" width="800"><br>
  Figure 11: Under the half data type, L1 -> L0A transpose, calling Load2D data layout diagram
</div>

Key points of parameter configuration:

- Perform a for loop with the m-axis direction as the outer axis, and configure `repeatTimes` with the k-axis direction as the inner axis.
- `srcOffset` represents the LocalTensor address offset in the m-axis direction of the A matrix on L1 every cycle.
- `dstOffset` represents the LocalTensor address offset in the m-axis direction of the A matrix on L0A every cycle.
- `ifTranspose = true`, means transpose each small fractal from L1 -> L0A.

```cpp
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[0]) * fractalSize;
uint32_t srcOffset = AscendC::Std::ceil_div(k, fractalShape[0]) * fractalSize;
AscendC::LoadData2DParams loadDataParams;
// Number of iterations, each iteration can process 512B data
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[0]);
// Between adjacent iterations, the interval between the starting address of the previous fractal and the next fractal of the source operand, unit: 512B
loadDataParams.srcStride = 1;
// Between adjacent iterations, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand, unit: 512B
loadDataParams.dstGap = 0;
// Whether to enable the transpose function to transpose each fractal matrix, the default is false
loadDataParams.ifTranspose = true;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[1]); ++i) {
    AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

**2. `LoadDataWithTranspose` interface**

The diagram of calling the `LoadDataWithTranspose` interface is consistent with `Load2D`. The m-axis direction is used as the outer axis to perform a for loop (red box in Figure 11), and the k-axis direction is used as the inner axis to configure loadDataParams.repeatTimes. The default is small fractal transposition.

```cpp
// dstoffset should be solved based on the alignment of the A matrix on L0 in the width direction.
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[1]) * fractalSize * fractalNum;
// srcoffset should be solved based on the alignment of the A matrix on L1 in the height direction.
uint32_t srcOffset = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;

AscendC::LoadData2dTransposeParams loadDataParams;
// The starting position of the transfer is the square matrix in the source operand (0 is the first square matrix in the source operand)
loadDataParams.startIndex = 0;
// Number of iterations, each iteration transposes a square matrix
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
// Between adjacent iterations, the interval between the starting address of the previous fractal and the next fractal of the source operand. The unit is the size of the square matrix
loadDataParams.srcStride = 1;
// Between adjacent iterations, the interval between the end address of the first fractal of the previous iteration of the destination operand and the start address of the first fractal of the next iteration, unit: 512B
loadDataParams.dstGap = 0;
// The destination operand in each iteration transposes the interval between the end address of the previous fractal and the start address of the next fractal, the unit is 512B
loadDataParams.dstFracGap = AscendC::Std::ceil_div(k, fractalShape[1]) - 1;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[1]); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

**3. `Load3Dv2` interface**

The essence of `Load3D` is to expand the Feature Map in NC1HWC0 format through Image to Column, then select the specified data block from the expanded two-dimensional matrix and move it into the corresponding memory location. Configure the loadDataParams.enTranspose parameter to true to complete the small fractal transposition during the transportation process. Complete the process of img2col according to the `Load3Dv2` instruction. It can be seen that the height of the A matrix after img2col is ho * wo. According to the calculation formulas of ho and wo, substituting the convolution kernel width, convolution kernel sliding step size, convolution kernel expansion coefficient and other parameters, we can know: the height of the A matrix is CeilAlign (k, fractalShape[0]); the width of the A matrix after img2col is ho * wo, ci * kh * kw, substitute kh=1, kw=1, it can be seen that the width of A matrix is CeilAlign(m, fractalShape[1]). As shown in the picture:

<div align="center">
  <img src="figures/L1_L0A_FP16_A_trans_Load3DV2.png" width="800"><br>
  Figure 12: Under the half data type, L1 -> L0A transpose, calling Load3Dv2 data layout diagram
</div>

```cpp
// Source operand height
loadDataParams.l1H = 1;
// Source operand width
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
// The number of channels of the source operand,
// The result matrix height of img2col is ho * wo. According to the calculation formulas of ho and wo, by substituting the convolution kernel width, convolution kernel sliding step size, convolution kernel expansion coefficient and other parameters, we can know: ho * wo = loadDataParams.l1H * loadDataParams.l1w
// The width of the result matrix of img2col is ci * kh * kw. Substituting kh=1, kw=1, it can be seen that the width of the result matrix is ci=loadDataParams.channelSize = m
loadDataParams.channelSize = CeilAlign(m, fractalShape[1]);
// The transmission length of this instruction in the width dimension of the destination operand, if the rightmost fractal is not covered, should be a multiple of 16 for the half type and a multiple of 32 for int8_t/uint8_t; in the case of coverage, there is no multiple requirement.
loadDataParams.kExtension = CeilAlign(m, fractalShape[1]);
// The transmission length of this instruction in the height dimension of the destination operand, if the lowermost fractal is not covered, should be a multiple of 16 for half/int8_t/uint8_t; in the case of coverage, there is no multiple requirement.
loadDataParams.mExtension = CeilAlign(k, fractalShape[1] * fractalNum);
// The step size of the convolution kernel sliding in the source operand width dimension
loadDataParams.strideW = 1;
// The step size of the convolution kernel sliding in the source operand height dimension
loadDataParams.strideH = 1;
// Convolution kernel width
loadDataParams.filterW = 1;
// Convolution kernel height
loadDataParams.filterH = 1;
// Convolution kernel width expansion coefficient
loadDataParams.dilationFilterW = 1;
// Convolution kernel height expansion coefficient
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = true;
loadDataParams.fMatrixCtrl = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

###### 6.1.2.3. float input data type

The fractal of float is [16, 8], and two fractals of [16, 8] need to be combined into a square shape for transposition. When the data layout of A matrix on L1 is Nz, it does not meet the requirements of the `LoadDataWithTranspose` instruction to merge two continuous fractals into a square matrix, as shown in Figure 13. When the layout on L1 is Nz, `LoadDataWithTranspose` will read 2 consecutive small fractals, as shown in the red box on the left side of the figure, forming a 32*8 fractal, which does not meet the requirements of transposition according to the square shape. At this time, `Load3Dv2` can be used to implement L1 -> L0A transposition and transfer; when the A matrix data is arranged as Zz on L1, `LoadDataWithTranspose` will read two consecutive small fractals, as shown in the red box on the right side of the figure, and form a 16 * 16 square shape, which meets the transposition requirements. The `LoadDataWithTranspose` instruction can be used to implement L1 -> L0A transposition and transfer.

<div align="center">
  <img src="figures/A矩阵L1上为Nz和Zz的区别.png" width="600"><br>
  Figure 13: Float data type, when the data layout format on L1 is Nz and Zz, LoadDataWithTranspose reads data at one time comparison chart
</div>

**1. The data layout on L1 is Nz, L1 -> L0A calls `Load3Dv2`**

As shown in Figure 14, when calling the `Load3Dv2` instruction, the height and width of the A matrix will be aligned at 16 and 8 respectively before writing to L0A. During the transposition process, the `Load3Dv2` instruction will automatically fill in the dummy data to ensure that it is transposed according to the square shape. When writing to L0A, the space for the dummy data will be skipped, as shown in the white part of the figure, ensuring that the A matrix eventually written to L0A is arranged in Zz.

<div align="center">
  <img src="figures/L1_L0A_F32_A_trans_Load3DV2.png" width="800"><br>
  Figure 14: Under the float data type, L1 -> L0A transpose, calling Load3Dv2 data layout diagram
</div>

```cpp
// Source operand height
loadDataParams.l1H = 1;
// Source operand width
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
// The number of channels of the source operand,
// The result matrix height of img2col is ho * wo. According to the calculation formulas of ho and wo, by substituting the convolution kernel width, convolution kernel sliding step size, convolution kernel expansion coefficient and other parameters, we can know: ho * wo = loadDataParams.l1H * loadDataParams.l1w
// The width of the result matrix of img2col is ci * kh * kw. Substituting kh=1, kw=1, it can be seen that the width of the result matrix is ci=loadDataParams.channelSize = m
loadDataParams.channelSize = CeilAlign(m, fractalShape[1]);
// The transmission length of this instruction in the width dimension of the destination operand, if the rightmost fractal is not covered, should be a multiple of 16 for the half type and a multiple of 32 for int8_t/uint8_t; in the case of coverage, there is no multiple requirement.
loadDataParams.kExtension = CeilAlign(m, fractalShape[1]);
// The transmission length of this instruction in the height dimension of the destination operand, if the lowermost fractal is not covered, should be a multiple of 16 for half/int8_t/uint8_t/int4_t; in the case of coverage, there is no multiple requirement.
loadDataParams.mExtension = CeilAlign(k, fractalShape[1] * fractalNum);
// The step size of the convolution kernel sliding in the source operand width dimension
loadDataParams.strideW = 1;
// The step size of the convolution kernel sliding in the source operand height dimension
loadDataParams.strideH = 1;
// Convolution kernel width
loadDataParams.filterW = 1;
// Convolution kernel height
loadDataParams.filterH = 1;
// Convolution kernel width expansion coefficient
loadDataParams.dilationFilterW = 1;
// Convolution kernel height expansion coefficient
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = true;
loadDataParams.fMatrixCtrl = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```

**2. The data layout on L1 is Zz, L1 -> L0A calls `LoadDataWithTranspose`**

As shown in Figure 15, when the A matrix on L1 meets the Zz layout, it can meet the `LoadDataWithTranspose` instruction's requirement that two continuous fractals be merged into a square matrix.

<div align="center">
  <img src="figures/L1_L0A_F32_A_trans_LoadDataWithTranspose.png" width="800"><br>
  Figure 15: Under float data type, L1 -> L0A transpose, calling LoadDataWithTranspose data layout diagram
</div>

Use the m-axis direction as the outer axis to perform a for loop (red box in Figure 15), and use the k-axis direction as the inner axis to configure loadDataParams.repeatTimes. As shown in Figure 15, the meanings of srcoffset and dstoffset are: on L1, the address offset of LocalTensor every time the m-axis direction of the A matrix is cycled; on L0A, the address offset of LocalTensor every time the m-axis direction of the A matrix is cycled. When configuring the `LoadData2dTransposeParams` structure, special attention needs to be paid to the fact that during transposition, two consecutive fractals are merged into one square. As shown in the figure, the blue and green boxes represent 2 squares, so loadDataParams.repeatTimes=AscendC::Std::ceil_div(k, fractalShape[1] * fractalNum).

```cpp
// The shape of the A matrix on L0A is [m, k] and Zz layout, so the dstoffset needs to be solved based on the alignment of the A matrix in the K-axis direction.
uint32_t dstOffset = AscendC::Std::ceil_div(k, fractalShape[1] * fractalNum) * fractalSize * fractalNum;
// The shape of A matrix on L1 is [K, M] and Zz layout, so srcoffset is the number of elements contained in one fractal.
uint32_t srcOffset = fractalSize * fractalNum;

AscendC::LoadData2dTransposeParams loadDataParams;
// The starting position of the transfer is the square matrix in the source operand (0 is the first square matrix in the source operand)
loadDataParams.startIndex = 0;
// Number of iterations, each iteration transposes a square matrix
loadDataParams.repeatTimes = AscendC::Std::ceil_div(k, fractalShape[1] * fractalNum);
// Between adjacent iterations, the interval between the starting address of the previous fractal and the next fractal of the source operand. The unit is the size of the square matrix
loadDataParams.srcStride = AscendC::Std::ceil_div(m, fractalShape[1] * fractalNum);
// Between adjacent iterations, the interval between the end address of the first fractal of the previous iteration of the destination operand and the start address of the first fractal of the next iteration, unit: 512B
loadDataParams.dstGap = 1;
// The destination operand in each iteration transposes the interval between the end address of the previous fractal and the start address of the next fractal, the unit is 512B
loadDataParams.dstFracGap = 0;
for (int i = 0; i < AscendC::Std::ceil_div(m, fractalShape[1] * fractalNum); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

#### 6.2. B matrix L1 -> L0B

After the B matrix is moved into L0B, the target layout is Zn. According to whether the B matrix needs to be transposed, it is divided into the following two categories:

- **B matrix transpose input `[n, k]`**: L1 -> L0B does not need to be transposed, `Load2D` can be used.
- **B matrix does not transpose input `[k, n]`**: L1 -> L0B needs to be transposed, and the interface needs to be selected according to the data type and L1 layout. For details, see [Table 2](#table2).

##### 6.2.1. B matrix L1 -> L0B is not transposed

In this scenario, only the large fractal layout format changes: the B matrix on L1 is Nz layout, and after being transferred into L0B, it is Zn layout. `Load2D` can be used for B4/B8/B16/B32 input data types. The parameter configuration is basically the same. The main difference is `fractalShape`. Please refer to [Table 4](#table4). The following uses half as an example.

<div align="center">
  <img src="figures/L1_L0B_F16_B_Load2D.png" width="800"><br>
  Figure 16: Under the half data type, L1 -> L0B is not transposed, and the Load2D data layout is called
</div>

Key points of parameter configuration:

- Perform a for loop with the k-axis direction as the outer axis, and configure `repeatTimes` with the n-axis direction as the inner axis.
- `srcOffset` represents the LocalTensor address offset in the k-axis direction of the B matrix on L1 every cycle.
- `dstOffset` represents the LocalTensor address offset in the k-axis direction of the B matrix on L0B every cycle.
- In this scenario, the B matrix on L1 is equivalent to the B matrix on L0B, so `srcOffset` and `dstOffset` are the same.

```cpp
// srcOffset and dstOffset are the same
// n-axis aligned with fractalShape[0]
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0]) * fractalSize;
uint32_t srcOffset = AscendC::Std::ceil_div(n, fractalShape[0]) * fractalSize;
// Nz -> Zn
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[0]);
loadDataParams.srcStride = 1;
// Between adjacent iterations in the n-axis direction, the interval between the end address of the previous fractal and the start address of the next fractal of the destination operand
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = false;
// k-axis aligned to fractalShape[1]
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[1]); ++i) {
    AscendC::LoadData(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

##### 6.2.2. B matrix L1 -> L0B transpose

The scenario undergoes small fractal transposition and large fractal layout format changes at the same time. The available interfaces for B4/B8/B16/B32 input data types are different. The following uses int4_t/int8_t, half, and float as examples.

- **int4_t/int8_t**: Use `LoadDataWithTranspose`.
- **half**: `Load2D`, `LoadDataWithTranspose` or `Load3Dv2` can be used.
- **float**: Use `Load3Dv2` when L1 is Nz; use `LoadDataWithTranspose` when L1 is Zz.

###### 6.2.2.1. int4_t / int8_t input data type

When the input data type is int4_t/int8_t, and L1 -> L0B needs to be transposed, the available interfaces are `LoadDataWithTranspose`. The parameter configurations are basically similar, only fractalShape, fractalNum, and fractalSize are different. Please refer to [Table 4](#table4). This section uses int8_t as an example for graphical introduction.

<div align="center">
  <img src="figures/L1_L0B_B8_B_trans_LoadDataWithTranspose.png" width="700"><br>
  Figure 17: Under the int8_t data type, L1 -> L0B transpose, call LoadDataWithTranspose data layout diagram
</div>

Key points of parameter configuration:

- Perform a for loop with the k-axis direction as the outer axis, and configure `repeatTimes` with the n-axis direction as the inner axis.
- During transposition, multiple consecutive fractals are merged into a block matrix, so `repeatTimes` needs to be configured according to the block matrix granularity.
- Fractal addresses within the same square remain continuous before and after the transposition, so `dstFracGap = 0`.

```cpp
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffset = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[1]);
loadDataParams.srcStride = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
loadDataParams.dstGap = 1;
loadDataParams.dstFracGap = 0;
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

###### 6.2.2.2. half input data type

When the input data type is half and L1 -> L0B needs to be transposed, the interfaces that can be used are `Load2D`, `LoadDataWithTranspose`, and `Load3Dv2`. The calling methods and illustrations of these three interfaces are introduced below.

**1. Call `Load2D`**
<div align="center">
  <img src="figures/L1_L0B_FP16_B_trans_Load2D.png" width="700"><br>
  Figure 18: Under the half data type, L1 -> L0B transpose, calling Load2D data layout diagram
</div>

According to [Table 2](#table2), when the B matrix is transposed, the b matrix is also transposed. If and only if the input data type is half, the `LoadData` instruction can be called to complete the transposition.

As shown in Figure 18 above, use the k-axis direction as the outer axis to perform a for loop, and use the n-axis direction as the inner axis to configure loadDataParams.repeatTimes. Combined with the diagram and based on the aforementioned definitions of srcoffset and dstoffset, the following configuration information can be obtained.

```cpp
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffset = fractalSize * fractalNum;
// Nz -> Zn
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[0] * fractalNum);
loadDataParams.srcStride = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = true;
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum); ++i) {
    AscendC::LoadData(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

**2. Call `LoadDataWithTranspose`**

The diagram for calling the `LoadDataWithTranspose` interface is consistent with `Load2D`. The k-axis direction is used as the outer axis for the for loop, and the n-axis direction is used as the inner axis to configure loadDataParams.repeatTimes. The default is small fractal transposition.

```cpp
// LoadDataWithTranspose: Nz -> Zn
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffset = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[1]);
loadDataParams.srcStride = AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum);
loadDataParams.dstGap = 0;
loadDataParams.dstFracGap = 0;
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[0] * fractalNum); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

**3. Call `Load3Dv2`**

<div align="center">
  <img src="figures/L1_L0B_FP16_B_trans_Load3DV2.png" width="800"><br>
  Figure 19: Under the half data type, L1 -> L0B transposition, calling Load3Dv2 data layout diagram
</div>

The essence of `Load3D` is to expand the Feature Map in NC1HWC0 format through Image to Column, then select the specified data block from the expanded two-dimensional matrix and move it into the corresponding memory location. Configure the loadDataParams.enTranspose parameter to true to complete the small fractal transposition during the transportation process. According to the `Load3Dv2` instruction to complete the process of img2col, it can be seen that the height of the B matrix after img2col is ho * wo. According to the calculation formulas of ho and wo, substituting the convolution kernel width, convolution kernel sliding step, convolution kernel expansion coefficient and other parameters, it can be seen that the height of the B matrix is CeilAlign (k, fractalShape[0]); the width of the A matrix after img2col is ho * wo, ci * kh * kw, substituting kh=1, kw=1, it can be seen that the width of the B matrix is CeilAlign(n, fractalShape[1]).

```cpp
// Load3Dv2: Nz -> Zn
AscendC::LoadData3DParamsV2<T> loadDataParams;
loadDataParams.l1H = 1;
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
loadDataParams.channelSize = CeilAlign(n, fractalShape[1]);
loadDataParams.kExtension = CeilAlign(n, fractalShape[1]);
loadDataParams.mExtension = CeilAlign(k, fractalShape[0]);
loadDataParams.strideW = 1;
loadDataParams.strideH = 1;
loadDataParams.filterW = 1;
loadDataParams.filterH = 1;
loadDataParams.dilationFilterW = 1;
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
// For the Load3Dv2 interface, when the destination address is L0B, the B matrix will be automatically transposed
loadDataParams.enTranspose = true;
loadDataParams.fMatrixCtrl = false;
AscendC::LoadData(b2Local, b1Local, loadDataParams);
```

###### 6.2.2.3. float input data type

Similar to the A matrix float transposed input, when the input data type is float and the small fractal needs to be transposed, the layout on L1 as Nz or Zz will affect the callable interface.

**1. The data layout on L1 is Nz, L1 -> L0B calls `Load3Dv2`**

As shown in Figure 20, when calling the `Load3Dv2` instruction, the height and width of the B matrix will be aligned at 16 and 8 respectively before writing to L0B. During the transposition process, the `Load3Dv2` instruction will automatically fill in the dummy data to ensure that it is transposed in a square shape. When writing to L0B, the space for dummy data will be skipped, as shown in the white part of the figure, ensuring that the B matrix eventually written to L0B is arranged in Zn.

In terms of alignment requirements, the k-axis of the B matrix on L1 is aligned to `fractalShape[0] * fractalNum`, and the n-axis is aligned to `fractalShape[1]`; on L0B, the k-axis of the B matrix is aligned to `fractalShape[0] * fractalNum`, and the n-axis is aligned to `fractalShape[1]`.

<div align="center">
  <img src="figures/L1_L0B_FP32_B_trans_Load3DV2.png" width="800"><br>
  Figure 20: Under the float data type, L1 -> L0B transposition, calling Load3Dv2 data layout diagram
</div>

Key points of parameter configuration:

- After `Load3Dv2` completes img2col, the B matrix has a height of `CeilAlign(k, fractalShape[0])` and a width of `CeilAlign(n, fractalShape[1])`.
- When the destination address is L0B, the B matrix is transposed by default, and the value `enTranspose` does not affect the function.

```cpp
loadDataParams.l1H = 1;
loadDataParams.l1W = CeilAlign(k, fractalShape[0]);
loadDataParams.channelSize = CeilAlign(n, fractalShape[1]);
loadDataParams.kExtension = CeilAlign(n, fractalShape[1]);
loadDataParams.mExtension = CeilAlign(k, fractalShape[0]);
loadDataParams.strideW = 1;
loadDataParams.strideH = 1;
loadDataParams.filterW = 1;
loadDataParams.filterH = 1;
loadDataParams.dilationFilterW = 1;
loadDataParams.dilationFilterH = 1;
loadDataParams.filterSizeW = false;
loadDataParams.filterSizeH = false;
loadDataParams.enTranspose = true;
loadDataParams.fMatrixCtrl = false;
```

**2. The data layout on L1 is Zz, L1 -> L0B calls `LoadDataWithTranspose`**

As shown in Figure 21, when the B matrix on L1 meets the Zz layout, it can meet the `LoadDataWithTranspose` instruction's requirement that two continuous fractals be merged into a square matrix.

<div align="center">
  <img src="figures/L1_L0B_FP32_B_trans_LoadDataWithTranspose.png" width="800"><br>
  Figure 21: Under the float data type, L1 -> L0B transpose, calling LoadDataWithTranspose data layout diagram
</div>

When configuring the `LoadData2dTransposeParams` structure, special attention should be paid to the fact that two consecutive fractals are merged into one square during transposition, so loadDataParams.repeatTimes=AscendC::Std::ceil_div(n, fractalShape[1] * fractalNum). In addition, since the two fractal addresses belonging to the same square matrix in the destination operand are no longer continuous, the parameter loadDataParams.dstFracGap = AscendC::Std::ceil_div(n, fractalShape[0]) - 1 at this time.

```cpp
// The B matrix has a shape of [k, n] and a Zn layout on L0B, so the dstoffset must be solved based on the alignment of the B matrix in the N-axis direction.
uint32_t dstOffset = AscendC::Std::ceil_div(n, fractalShape[0]) * fractalSize * fractalNum;
// The B matrix has a shape of [K, N] and a ZZ layout on L1, so the srcoffset must be solved based on the alignment of the B matrix in the N-axis direction.
uint32_t srcOffset = AscendC::Std::ceil_div(n, fractalShape[1] * fractalNum) * fractalSize * fractalNum;

AscendC::LoadData2dTransposeParams loadDataParams;
// The starting position of the transfer is the square matrix in the source operand (0 is the first square matrix in the source operand)
loadDataParams.startIndex = 0;
// Number of iterations, each iteration transposes a square matrix
loadDataParams.repeatTimes = AscendC::Std::ceil_div(n, fractalShape[1] * fractalNum);
// Between adjacent iterations, the interval between the starting address of the previous fractal and the next fractal of the source operand. The unit is the size of the square matrix
loadDataParams.srcStride = 1;
// Between adjacent iterations, the interval between the end address of the first fractal of the previous iteration of the destination operand and the start address of the first fractal of the next iteration, unit: 512B
loadDataParams.dstGap = 0;
// The destination operand in each iteration transposes the interval between the end address of the previous fractal and the start address of the next fractal, the unit is 512B
loadDataParams.dstFracGap = AscendC::Std::ceil_div(n, fractalShape[0]) - 1;
for (int i = 0; i < AscendC::Std::ceil_div(k, fractalShape[0]); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffset], b1Local[i * srcOffset], loadDataParams);
}
```

### 7. Matrix multiplication (`Mmad`)

The following will describe how to configure
The members of the `MmadParams` structure of the `Mmad` instruction, and the specific meaning of each member variable will not be described again here.

It should be noted that when the `Mmad` instruction is executed,
The matrix calculation unit will continuously read multiple fractals from L0A/L0B to participate in the matrix multiplication calculation. The number of read fractals is calculated based on the values of the member variables m, n, and k of the `MmadParams` structure and the alignment requirements of the `Mmad` instructions on the axes of the A matrix and B matrix on L0A/L0B. Due to the `Mmad` instruction, that is, the A matrix fractal is [16, 32] and the B matrix fractal is [32, 16] to continuously read in fractals, that is to say, the total number of fractals continuously read in by the matrix calculation unit from L0A/L0B are: AscendC::Std::ceil_div(m, 16) * AscendC::Std::ceil_div(k, 32), AscendC::Std::ceil_div(k, 32) * AscendC::Std::ceil_div(n, 16).

Therefore, when the actual alignment requirements of the A matrix and the B matrix on each axis on L0A/L0B are inconsistent with the default alignment requirements of the `Mmad` instruction, it may result in continuous reading of fractals, incorrectly reading fractals completely filled with invalid data and ignoring fractals containing valid data.

As shown in Figure 22 below, taking scenario 5 as an example, the input data type is int8_t, A is transposed and B matrix is not transposed. As can be seen from the previous content: L0A is aligned to 16 * 2 and 32 on the m-axis and k-axis respectively, L0B is aligned to 32 and 16 * 2 on the k-axis and n-axis respectively, and the default alignment requirements of the `Mmad` instruction on the m, k, and n axes are to be aligned to 16, 32, and 16 respectively. Assuming n=70, according to the alignment requirements on L0 nAlignL0 = CeilAlign(70, 16 * 2) = 96, according to the default alignment requirements of the `Mmad` instruction, nAlignL0 = CeilAlign(70, 16) = 80. At this time, the actual alignment requirements of the n-axis are inconsistent with the default alignment requirements of the `Mmad` instruction. The same goes for the m-axis.

Taking the n-axis of the B matrix as an example, as shown in the left subfigure of Figure 22, if mmadParams.n = n = 70 is set, the fractal numbered 5 will be read, and the fractal numbered 10 that contains valid data will not be read.

As shown in the subfigure on the right side of Figure 22, if mmadParams.n = CeilAlign(n, fractalShape[0] * fractalNum) = 96 is set, all fractals will be read in. Although the matrix calculation results include the results of invalid data participating in the calculation, when the `Fixpipe` instruction moves out the data, fixpipeParams.nSize = n is set to ensure that the results of invalid data participating in the calculation will not be moved out. The m-axis parameter configuration is the same.

<div align="center">
  <img src="figures/B_B8_read_invalid_data.png" width="900"><br>
  Figure 22: B matrix GM does not transpose the input. Under the int8_t data type, the actual alignment requirements of the n-axis are inconsistent with the default alignment requirements of the Mmad instruction.
</div>

The above scenarios are similar, such as scenario 13. When the input data type is float and A matrix GM transpose input ([k, m]), the actual alignment requirements of the k-axis are also inconsistent with the default alignment requirements of the `Mmad` instruction. However, the solution in this scenario is different from the above scenario. The mmadParams.kDirectionAlign parameter needs to be introduced separately to solve the problem. The reasons are explained below.

According to the calculation formula of matrix multiplication, the k-axis is the common dimension of the A and B matrices. At this time, if mmadParams.k is set like the above scenario = CeilAlign(k, fractalShape[1] * fractalNum) = CeilAlign(70, 16) = 80, which will cause the value of each element in the C matrix to be affected by multiple invalid data read in, and it is also impossible to set the parameters of fixpipeParams to ensure that the results of invalid data participating in the calculation will not be moved out.

As shown in Figure 23 below, mmadParams.kDirectionAlign only takes effect when the input data type is float. When the A matrix is transposed, this parameter needs to be set to true. At this time, the A matrix on L0A is aligned to 16 in the k direction. The matrix calculation unit will skip the filled invalid data when reading data from L0A. In other scenarios, the default value of this parameter is false. At this time, the A matrix on L0A is aligned to 8 in the k direction.

<div align="center">
  <img src="figures/Mmad_FP32_A_trans_kDirectionAlign.png" width="900"><br>
  Figure 23: A matrix GM transpose input, under the float data type, the actual alignment requirements of the k-axis are inconsistent with the default alignment requirements of the Mmad instruction
</div>

```cpp
AscendC::MmadParams mmadParams;
// Left matrixHeight
mmadParams.m = m;
// Right matrix width
mmadParams.n = n;
if constexpr (AscendC::IsSameType<T, int8_t>::value && AscendC::IsSameType<U, int32_t>::value) {
    if constexpr (!isBtranspose) {
        // mmad defaults to aligning the n axis to 16, but due to the b transposition process, the n axis is aligned to 2 * 16, filling a 32 * 16 fractal composed entirely of invalid data.
        // If mmadParams.n = n is still set, the cube unit will read more fractals of invalid data and no fractals of valid data will be read.
        // At this time, you can set the n-direction 32 alignment to allow this fractal to participate in the calculation, and skip the invalid fractal to participate in the calculation when moving out.
        mmadParams.n = CeilAlign(n, fractalShape[0] * fractalNum);
    }
}
// Left matrix Width, right matrix Height
mmadParams.k = k;
if constexpr (AscendC::IsSameType<T, float>::value && AscendC::IsSameType<U, float>::value) {
    if constexpr (isAtranspose) {
        mmadParams.kDirectionAlign = true;
    }
}
```

### 8. L0C to GM (`Fixpipe`)

The following will describe how to configure
The members of the `FixpipeParamsV220` structure of the `Fixpipe` instruction, and the specific meaning of each member variable will not be described again here. Among them, the unit of fixpipeParams.srcStride is 32/sizeof(T) elements, which means the starting address offset of adjacent Z layouts in the source Nz matrix (elements in the same row in the ND matrix are in adjacent Z layouts in the source Nz matrix). The value of this parameter is the length of the C matrix m axis on L0C after alignment to 16.

```cpp
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.nSize = n;
fixpipeParams.mSize = m;

// The source operand comes from L0c, so m only needs to be aligned to 16, regardless of the data type.
// The starting address offset of adjacent Z layouts in the source NZ matrix
fixpipeParams.srcStride = CeilAlign(m, fractalShape[0]);
fixpipeParams.dstStride = n;

fixpipeParams.ndNum = 1;
fixpipeParams.srcNdStride = 0;
fixpipeParams.dstNdStride = 0;
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
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin
  ```
  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;   # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default) | NPU architecture: Atlas A2/A3 series products |
  | `SCENARIO_NUM` | `1`-`14` | Scenario number: different data types and transpose combinations |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
