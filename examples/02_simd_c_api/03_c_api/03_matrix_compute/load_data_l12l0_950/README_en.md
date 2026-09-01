# load_data_l12l0_950 Example

## Overview

This example demonstrates how to call `asc_copy_l12l0a` / `asc_copy_l12l0b` and their `_transpose` variants to transfer A / B matrices from L1 to L0A / L0B Buffer.

The example covers three input data types: int8_t, half, and float, as well as A / B matrix transposed and non-transposed input scenarios. L0A data layout is Nz format, L0B data layout is Zn format.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── load_data_l12l0_950
│   ├── figures                          // Illustrations
│   ├── scripts
│   │   ├── gen_data.py                  // Script for generating input data and ground truth data
│   │   └── verify_result.py             // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                   // Build project file
│   ├── data_utils.h                     // Data read/write functions
│   ├── load_data_l12l0_950.asc          // SIMD C API implementation and invocation example
│   └── README.md                        // Example documentation
```

## Example Description

### 1. Overall Flow

The overall flow of the example is as follows:

```text
GM(ND) -> L1(Nz) -> L0A(Nz)/L0B(Zn) -> L0C(Nz) -> GM(ND)
       │         │                  │          │
asc_copy_gm2l1_nd2nz  asc_copy_l12l0*        asc_mmad  asc_copy_l0c2gm
```

**Step details**:

1. **GM -> L1**: Use `asc_set_gm2l1_nz_para` to configure the Nz layout, then use `asc_copy_gm2l1_nd2nz` to convert ND to Nz.
2. **L1 -> L0A / L0B**: Use `asc_copy_l12l0a` / `asc_copy_l12l0b` for non-transpose transfer and their `_transpose` variants for transpose transfer.
3. **Matrix multiplication**: Use `asc_mmad` to perform matrix multiplication.
4. **L0C -> GM**: Use `asc_set_l0c_copy_nz_para` to configure Nz-to-ND conversion, then use `asc_copy_l0c2gm` to transfer results out.

This example uses a fixed matrix multiplication specification of `[m, n, k] = [40, 50, 70]`, with output C matrix `[40, 50]` in ND format. The kernel function selects different scenarios through `scenario_num`.

### 2. Key Scenario Differences

The input shape, transpose flags, and L1 -> L0 transfer methods for each scenario are shown in [Table 1](#table1).

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Meaning of Different scenario_num Values</span></caption>
  <tr>
    <td><span style="font-weight: bold;">scenario_num</span></td>
    <td><span style="font-weight: bold;">Input Data Type</span></td>
    <td><span style="font-weight: bold;">Output Data Type</span></td>
    <td><span style="font-weight: bold;">Input Shape</span></td>
    <td><span style="font-weight: bold;">is_a_transpose</span></td>
    <td><span style="font-weight: bold;">is_b_transpose</span></td>
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
    <td><code>asc_copy_l12l0a</code> / <code>asc_copy_l12l0b</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>Yes</td>
    <td><code>asc_copy_l12l0a_transpose</code> / <code>asc_copy_l12l0b_transpose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td rowspan="2">half</td>
    <td rowspan="2">float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>No</td>
    <td><code>asc_copy_l12l0a</code> / <code>asc_copy_l12l0b</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>No</td>
    <td><code>asc_copy_l12l0a_transpose</code> / <code>asc_copy_l12l0b_transpose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td rowspan="2">float</td>
    <td rowspan="2">float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td>No</td>
    <td><code>asc_copy_l12l0a</code> / <code>asc_copy_l12l0b</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>No</td>
    <td><code>asc_copy_l12l0a_transpose</code> / <code>asc_copy_l12l0b_transpose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">7</span></td>
    <td rowspan="1">int8_t</td>
    <td rowspan="1">int32_t</td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td>No</td>
    <td>for loop + <code>asc_copy_l12l0a_transpose</code>; <code>asc_copy_l12l0b_transpose</code></td>
  </tr>
</table>

Key differences by transfer mode:

**C API non-transpose transfer: Scenarios 1 / 3 / 5**

- A matrix non-transposed input `[m, k]`, B matrix transposed input `[n, k]`.
- L1 -> L0A / L0B both do not need transpose. Call `asc_copy_l12l0a` / `asc_copy_l12l0b`, respectively.

**C API transpose transfer, single call: Scenarios 2 / 4 / 6**

- A matrix transposed input `[k, m]`, B matrix non-transposed input `[k, n]`.
- L1 -> L0A / L0B both need transpose. Call `asc_copy_l12l0a_transpose` / `asc_copy_l12l0b_transpose`, respectively.
- In scenario 2, a single int8_t transpose transfer causes extra dirty data fractals in the m direction. `asc_copy_l0c2gm` transfers out only the valid region with `m_size = m`.

**C API transpose transfer, for loop call: Scenario 7**

- Scenario 7 is also int8_t transposed input, same as scenario 2.
- A matrix uses a for loop to call `asc_copy_l12l0a_transpose`, skipping dirty data fractals at the tail of the m direction when writing to L0A, so no extra dirty data fractals participate in matrix computation.

### 3. Parameter Description

The subsequent code and parameter descriptions repeatedly use fractal and alignment related variables. This section defines these concepts first.

- `fractal_shape`: Small fractal shape. B8 / B16 / B32 input data types have shape `[16, 32 / sizeof(T)]`, where `T` represents the input data type. Fractal information for data types involved in this example is shown in [Table 2](#table2).
- `fractal_size`: Number of elements in 1 small fractal, see [Table 2](#table2).
- `fractal_num`: When L1 -> L0A / L0B requires transpose, `asc_copy_l12l0a_transpose` / `asc_copy_l12l0b_transpose` transposes by square matrix. When multiple consecutive small fractals need to merge into a square matrix, `fractal_num` represents the number of small fractals in that square matrix, see [Table 2](#table2).

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: Fractal Information for Different Data Types</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">fractal_shape</span></td>
    <td align="center"><span style="font-weight: bold;">fractal_size</span></td>
    <td align="center"><span style="font-weight: bold;">fractal_num</span></td>
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

- `ceil_align`: Ceiling alignment operation. For example, when `m = 30`, `ceil_align(30, 16) = 32`, meaning that the m-axis is aligned to 16 and its aligned length is 32.

  ```cpp
  #define ceil_div(value, align) (((value) + (align) - 1) / (align))
  #define ceil_align(value, align) (ceil_div((value), (align)) * (align))
  ```

- `ceil_div`: Ceiling division, generally used to compute loop counts after ceiling alignment.
- `m_align_value`: m axis aligns to `m_align_value`. For example, when `m_align_value = 32`, the m axis is aligned to 32. Similarly `n_align_value`, `ka_align_value`, `kb_align_value`.
- `m_align_l1` and `m_align_l0`: Aligned values of m axis when A matrix is on L1 and L0A respectively. Similarly `n_align_l1`, `n_align_l0`, `ka_align_l1`, `ka_align_l0`, `kb_align_l1`, `kb_align_l0`.

### 4. Alignment Requirements

A / B matrices have different alignment requirements on each axis on L1 and L0. These values are used when configuring `asc_copy_l12l0a` / `asc_copy_l12l0b`, `asc_mmad`, and `asc_copy_l0c2gm` parameters. [Table 3](#table3) and [Table 4](#table4) summarize alignment requirements for A / B matrices on L1 and L0 when scenario_num is 1-6; [Table 5](#table5) and [Table 6](#table6) summarize alignment requirements when scenario_num is 7.

<a name="table3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 3: Alignment Requirements for A and B Matrices on L1 (L1 Layout Format is Nz)</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractal_num=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractal_num=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractal_num=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A matrix non-transposed input [m, k]</span></td>
    <td colspan="3" align="center">m_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >ka_align_value = fractal_shape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A matrix transposed input [k, m]</span></td>
    <td colspan="2" align="center">ka_align_value = fractal_shape[0] * fractal_num</td>
    <td colspan="1" align="center">ka_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >m_align_value = fractal_shape[1]</td>
    <td colspan="1" align="center" >m_align_value = fractal_shape[1] * fractal_num</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B matrix non-transposed input [k, n]</span></td>
    <td colspan="2" align="center">kb_align_value = fractal_shape[0] * fractal_num</td>
    <td colspan="1" align="center">kb_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >n_align_value = fractal_shape[1]</td>
    <td colspan="1" align="center" >n_align_value = fractal_shape[1] * fractal_num</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B matrix transposed input [n, k]</span></td>
    <td colspan="3" align="center">n_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kb_align_value = fractal_shape[1]</td>
  </tr>
</table>

<a name="table4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 4: Alignment Requirements for A and B Matrices on L0</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractal_num=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractal_num=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractal_num=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A matrix non-transposed input [m, k], L1 -> L0A no transpose needed</span></td>
    <td colspan="3" align="center">m_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >ka_align_value = fractal_shape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A matrix transposed input [k, m], L1 -> L0A transpose needed</span></td>
    <td colspan="2" align="center">ka_align_value = fractal_shape[1]</td>
    <td >ka_align_value = fractal_shape[1] * fractal_num</td>
  </tr>
  <tr>
    <td colspan="2" align="center" >m_align_value = fractal_shape[0] * fractal_num</td>
    <td align="center" >m_align_value = fractal_shape[0]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B matrix non-transposed input [k, n], L1 -> L0B transpose needed</span></td>
    <td colspan="2" align="center">kb_align_value = fractal_shape[1]</td>
      <td align="center">kb_align_value = fractal_shape[1] * fractal_num</td>
  </tr>
  <tr>
    <td colspan="2" align="center">n_align_value = fractal_shape[0] * fractal_num</td>
    <td align="center" >n_align_value = fractal_shape[0]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B matrix transposed input [n, k], L1 -> L0B no transpose needed</span></td>
    <td colspan="3" align="center">n_align_value = fractal_shape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center" >kb_align_value = fractal_shape[1]</td>
  </tr>
</table>

When scenario_num=7, A matrix uses a for loop with `asc_copy_l12l0a_transpose` for L1 -> L0A transfer, and L0A only writes the region aligned by valid data fractals.

<a name="table5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 5: Alignment Requirements for A and B Matrices on L1 when scenario_num=7</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractal_num=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A matrix transposed input [k, m]</span></td>
    <td align="center" >ka_align_value = fractal_shape[0] * fractal_num</td>
  </tr>
    <tr>
    <td align="center" >m_align_value = fractal_shape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B matrix non-transposed input [k, n]</span></td>
    <td align="center" >kb_align_value = fractal_shape[0] * fractal_num</td>
  </tr>
    <tr>
    <td align="center" >n_align_value = fractal_shape[1]</td>
  </tr>
</table>

<a name="table6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 6: Alignment Requirements for A and B Matrices on L0 when scenario_num=7</span></caption>
  <tr>
    <td align="center" ></td>
    <td align="center" ><span style="font-weight: bold;">int8_t (fractal_num=2)</span></td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">A matrix transposed input [k, m], L1 -> L0A transpose needed</span></td>
    <td align="center" >m_align_value = fractal_shape[0]</td>
  </tr>
    <tr>
    <td align="center" >ka_align_value = fractal_shape[1]</td>
  </tr>
   <tr>
    <td rowspan="2"><span style="font-weight: bold;">B matrix non-transposed input [k, n], L1 -> L0B transpose needed</span></td>
    <td align="center" >kb_align_value = fractal_shape[1]</td>
  </tr>
    <tr>
    <td align="center" >n_align_value = fractal_shape[0] * fractal_num</td>
  </tr>
</table>

### 5. L1 -> L0 C API

A / B matrices on L1 are all in Nz layout, and on L0A and L0B they are in Nz and Zn layouts respectively. Based on the destination and whether transpose is required, the L1 -> L0 phase calls `asc_copy_l12l0a`, `asc_copy_l12l0b`, `asc_copy_l12l0a_transpose`, or `asc_copy_l12l0b_transpose`.

When reading this section, first determine the scenario from [Table 1](#table1), then select alignment values from [Table 3](#table3)-[Table 6](#table6). The key C API parameters are:

- API name: APIs without the `_transpose` suffix perform non-transpose transfer; APIs with the suffix perform transpose transfer.
- `m_step`, `k_step`: Represent the number of small fractals covered in row and col directions respectively. During transpose transfer, B8 / B32 need to transfer in square fractal combinations, related step sizes must satisfy `fractal_num` requirements.
- `src_stride`, `dst_stride`: Represent intervals between adjacent small fractals in the row direction of the L1 source matrix and L0 destination matrix respectively.
- `m_start_position`, `k_start_position`: Represent the starting small fractal position in the L1 source matrix for this transfer. In scenario 7, `m_start_position` is modified in the loop to transfer A matrix in segments.

#### 5.1. A Matrix L1 -> L0A Non-Transpose

When L1 -> L0A does not require transpose, the parameter configurations for B8 / B16 / B32 data types are essentially the same, differing only in `fractal_shape`, see [Table 2](#table2). The following figure uses int8_t as an example.

<div align="center">
  <img src="figures/B8_A_l1_l0A_copy_l12l0.png" width="800"><br>
  Figure 1: int8_t data type, L1 -> L0A non-transpose, asc_copy_l12l0a data layout diagram
</div>

Parameter configuration key points:

- `m_step = ceil_div(m_align_l1, fractal_shape[0])`, representing the number of small fractals in the row direction for the m axis.
- `k_step = ceil_div(ka_align_l1, fractal_shape[1])`, representing the number of small fractals in the col direction for the k axis.
- Call `asc_copy_l12l0a` to perform only the L1 Nz to L0A Nz layout transfer without transpose.

```cpp
m_align_l1 = ceil_align(m, fractal_shape[0]);  // 48
ka_align_l1 = ceil_align(k, fractal_shape[1]); // 96
m_align_l0 = ceil_align(m, fractal_shape[0]);  // 48
ka_align_l0 = ceil_align(k, fractal_shape[1]); // 96
asc_copy_l12l0a(a_l0, a_l1, 0, 0,
                ceil_div(m_align_l1, fractal_shape[0]),  // m_step = 3
                ceil_div(ka_align_l1, fractal_shape[1]), // k_step = 3
                ceil_div(m_align_l1, fractal_shape[0]),  // src_stride = 3
                ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 3
```

#### 5.2. A Matrix L1 -> L0A Transpose

When L1 -> L0A requires transpose, call `asc_copy_l12l0a_transpose`. It completes small fractal transpose and L0A Nz layout writing via square fractal combinations; different data types have different square fractal combination methods, described below for B8 / B16 / B32.

##### 5.2.1. B8 Input Data Type

B8 input data type fractal is 16 * 32. During L1 -> L0 transpose, 2 fractals of 16 * 32 in the row direction are combined into a 32 * 32 square for transposing. The following uses int8_t as an example to illustrate single call and for loop call methods.

**Single Call**

The following figure shows calling `asc_copy_l12l0a_transpose` once to complete L1 -> L0A transfer and transpose:

<div align="center">
  <img src="figures/B8_A_l1_l0A_trans_copy_l12l0.png" width="800"><br>
  Figure 2: int8_t data type, L1 -> L0A transpose, single asc_copy_l12l0a_transpose call data layout diagram
</div>

In this example `m = 40`. During B8 transpose transfer, `m_align_l1 = ceil_align(m, fractal_shape[1]) = 64`, `m_align_l1 - m = 24 > 16`. When calling `asc_copy_l12l0a_transpose` once, 1 extra invalid fractal is transferred in the m direction; `asc_mmad` uses `m = ceil_align(m, fractal_shape[0] * fractal_num)` to include that fractal in computation, and `asc_copy_l0c2gm` transfers out only the valid region with `m_size = m`.

```cpp
ka_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 96
m_align_l1 = ceil_align(m, fractal_shape[1]);                // 64
m_align_l0 = ceil_align(m, fractal_shape[0] * fractal_num);  // 64
ka_align_l0 = ceil_align(k, fractal_shape[1]);               // 96
asc_copy_l12l0a_transpose(a_l0, a_l1, 0, 0,
                          ceil_div(ka_align_l1, fractal_shape[0]), // m_step = 6
                          ceil_div(m_align_l1, fractal_shape[1]),  // k_step = 2
                          ceil_div(ka_align_l1, fractal_shape[0]), // src_stride = 6
                          ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 4
```

**For Loop Call**

The following figure shows calling `asc_copy_l12l0a_transpose` multiple times in a for loop to complete L1 -> L0A transfer and transpose:

<div align="center">
  <img src="figures/B8_A_l1_l0A_trans_for_copy_l12l0.png" width="800"><br>
  Figure 3: int8_t data type, L1 -> L0A transpose, for loop calling asc_copy_l12l0a_transpose multiple times
</div>

When using for loop, transfer is done in segments along the L1 row direction (A matrix k axis), transferring 2 fractals in the k axis direction and `ceil_div(m_align_l0, fractal_shape[1])` fractals in the m axis direction each time. `dst_stride` is configured with m direction valid data aligned to `fractal_shape[0]`, skipping dirty data fractals at the tail of m direction when writing to L0A due to transpose over-read, so no extra dirty data fractals participate in `asc_mmad` computation in the m direction.

```cpp
ka_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 96
m_align_l1 = ceil_align(m, fractal_shape[1]);                // 64
m_align_l0 = ceil_align(m, fractal_shape[0]);                // 48
ka_align_l0 = ceil_align(k, fractal_shape[1]);               // 96
// Input is int8 type, A matrix [k,m] transposed input, L1 -> L0A requires transpose
// The loop calls asc_copy_l12l0a_transpose along the k axis, transferring 2 fractals each time.
uint16_t a_loop_num = ceil_div(ka_align_l0, fractal_shape[0] * fractal_num); // 3
uint32_t dst_offset = 0;
for (uint16_t loopIdx = 0; loopIdx < a_loop_num; ++loopIdx) {
    asc_copy_l12l0a_transpose(a_l0 + dst_offset, a_l1,
                              int8_m_step_align * loopIdx, 0,          // m_start_position, k_start_position
                              int8_m_step_align,                       // m_step = 2
                              ceil_div(m_align_l0, fractal_shape[1]),  // k_step = 2
                              ceil_div(ka_align_l1, fractal_shape[0]), // src_stride = 6
                              ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 3
    dst_offset += ceil_align(m_align_l0, fractal_shape[0]) * fractal_shape[1]; // 1536
}
```

##### 5.2.2. B16 Input Data Type

B16 input data type fractal is 16 * 16, one fractal is already a square. During L1 -> L0 transpose, it transposes by small fractal, and a single `asc_copy_l12l0a_transpose` call completes L1 -> L0A data transfer and transpose. This example uses half as the data type.

<div align="center">
  <img src="figures/B16_A_l1_l0A_trans_copy_l12l0.png" width="800"><br>
  Figure 4: half data type, L1 -> L0A transpose, single asc_copy_l12l0a_transpose call data layout diagram
</div>

Parameter configuration key points:

- `m_step = ceil_div(ka_align_l1, fractal_shape[0])`, representing the number of small fractals in the row direction for the k axis.
- `k_step = ceil_div(m_align_l1, fractal_shape[1])`, representing the number of small fractals in the col direction for the m axis.
- Call `asc_copy_l12l0a_transpose` to complete transpose when transferring to L0A.

```cpp
ka_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 80
m_align_l1 = ceil_align(m, fractal_shape[1]);                // 48
m_align_l0 = ceil_align(m, fractal_shape[0] * fractal_num);  // 48
ka_align_l0 = ceil_align(k, fractal_shape[1]);               // 80
asc_copy_l12l0a_transpose(a_l0, a_l1, 0, 0,
                          ceil_div(ka_align_l1, fractal_shape[0]), // m_step = 5
                          ceil_div(m_align_l1, fractal_shape[1]),  // k_step = 3
                          ceil_div(ka_align_l1, fractal_shape[0]), // src_stride = 5
                          ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 3
```

##### 5.2.3. B32 Input Data Type

B32 input data type fractal is 16 * 8. During L1 -> L0 transpose, 2 fractals of 16 * 8 in the col direction are combined into a 16 * 16 square for transposing. This example uses float as the data type.

<div align="center">
  <img src="figures/B32_A_l1_l0A_trans_copy_l12l0.png" width="800"><br>
  Figure 5: float data type, L1 -> L0A transpose, single asc_copy_l12l0a_transpose call data layout diagram
</div>

In this example `m = 40`. B32 transpose transfer requires combining 2 fractals in the col direction into a square, **`k_step` must be a multiple of 2**, so L1 col direction (m direction) reads 1 extra invalid fractal. After writing to L0A, there is 1 extra invalid fractal at the tail of the k direction; since L0A is in Nz layout, configuring `k = matrix_k` in `asc_mmad` ensures only valid k axis data participates in computation.

```cpp
ka_align_l1 = ceil_align(k, fractal_shape[0]);               // 80
m_align_l1 = ceil_align(m, fractal_shape[1] * fractal_num);  // 48
m_align_l0 = ceil_align(m, fractal_shape[0]);                // 48
ka_align_l0 = ceil_align(k, fractal_shape[1] * fractal_num); // 80
asc_copy_l12l0a_transpose(a_l0, a_l1, 0, 0,
                          ceil_div(ka_align_l1, fractal_shape[0]), // m_step = 5
                          ceil_div(m_align_l1, fractal_shape[1]),  // k_step = 6
                          ceil_div(ka_align_l1, fractal_shape[0]), // src_stride = 5
                          ceil_div(m_align_l0, fractal_shape[0])); // dst_stride = 3
```

#### 5.3. B Matrix L1 -> L0B Non-Transpose

When L1 -> L0B does not require transpose, the parameter configurations for B8 / B16 / B32 data types are essentially the same, differing only in `fractal_shape`, see [Table 2](#table2). The following figure uses float as an example.

<div align="center">
  <img src="figures/B32_B_l1_l0B_copy_l12l0.png" width="1100"><br>
  Figure 6: float data type, L1 -> L0B non-transpose, single asc_copy_l12l0b call data layout diagram
</div>

Parameter configuration key points:

- `m_step = ceil_div(n_align_l1, fractal_shape[0])`, representing the number of small fractals in the row direction for the n axis.
- `k_step = ceil_div(kb_align_l1, fractal_shape[1])`, representing the number of small fractals in the col direction for the k axis.
- Call `asc_copy_l12l0b` to perform only the L1 Nz to L0B Zn layout transfer without transpose.

```cpp
n_align_l1 = ceil_align(n, fractal_shape[0]);  // 64
kb_align_l1 = ceil_align(k, fractal_shape[1]); // 72
kb_align_l0 = ceil_align(k, fractal_shape[1]); // 72
n_align_l0 = ceil_align(n, fractal_shape[0]);  // 64
asc_copy_l12l0b(b_l0, b_l1, 0, 0,
                ceil_div(n_align_l1, fractal_shape[0]),  // m_step = 4
                ceil_div(kb_align_l1, fractal_shape[1]), // k_step = 9
                ceil_div(n_align_l1, fractal_shape[0]),  // src_stride = 4
                ceil_div(n_align_l0, fractal_shape[0])); // dst_stride = 4
```

#### 5.4. B Matrix L1 -> L0B Transpose

When L1 -> L0B requires transpose, call `asc_copy_l12l0b_transpose`. It completes small fractal transpose and L0B Zn layout writing via square fractal combinations; different data types have different square fractal combination methods, described below for B8 / B16 / B32.

##### 5.4.1. B8 Input Data Type

B8 input data type fractal is 16 * 32. During L1 -> L0 transpose, 2 fractals of 16 * 32 in the row direction are combined into a 32 * 32 square for transposing. This example uses int8_t as the data type.

<div align="center">
  <img src="figures/B8_B_l1_l0B_trans_copy_l12l0.png" width="1100"><br>
  Figure 7: int8_t data type, L1 -> L0B transpose, single asc_copy_l12l0b_transpose call data layout diagram
</div>

In this example `k = 70`. B8 transpose transfer requires combining 2 fractals in the row direction into a square, **`m_step` must be a multiple of 2**, so L1 row direction (k direction) reads 1 extra invalid fractal.

```cpp
kb_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 96
n_align_l1 = ceil_align(n, fractal_shape[1]);                // 64
kb_align_l0 = ceil_align(k, fractal_shape[1]);               // 96
n_align_l0 = ceil_align(n, fractal_shape[0] * fractal_num);  // 64
asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0,
                          ceil_div(kb_align_l1, fractal_shape[0]), // m_step = 6
                          ceil_div(n_align_l1, fractal_shape[1]),  // k_step = 2
                          ceil_div(kb_align_l1, fractal_shape[0]), // src_stride = 6
                          ceil_div(n_align_l0, fractal_shape[0])); // dst_stride = 4
```

##### 5.4.2. B16 Input Data Type

B16 input data type fractal is 16 * 16, one fractal is already a square. During L1 -> L0 transpose, it transposes by small fractal, and a single `asc_copy_l12l0b_transpose` call completes L1 -> L0B data transfer and transpose. This example uses half as the data type.

<div align="center">
  <img src="figures/B16_B_l1_l0B_trans_copy_l12l0.png" width="800"><br>
  Figure 8: half data type, L1 -> L0B transpose, single asc_copy_l12l0b_transpose call data layout diagram
</div>

Parameter configuration key points:

- `m_step = ceil_div(kb_align_l1, fractal_shape[0])`, representing the number of small fractals in the row direction for the k axis.
- `k_step = ceil_div(n_align_l1, fractal_shape[1])`, representing the number of small fractals in the col direction for the n axis.
- Call `asc_copy_l12l0b_transpose` to complete transpose when transferring to L0B.

```cpp
kb_align_l1 = ceil_align(k, fractal_shape[0] * fractal_num); // 80
n_align_l1 = ceil_align(n, fractal_shape[1]);                // 64
kb_align_l0 = ceil_align(k, fractal_shape[1]);               // 80
n_align_l0 = ceil_align(n, fractal_shape[0] * fractal_num);  // 64
asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0,
                          ceil_div(kb_align_l1, fractal_shape[0]), // m_step = 5
                          ceil_div(n_align_l1, fractal_shape[1]),  // k_step = 4
                          ceil_div(kb_align_l1, fractal_shape[0]), // src_stride = 5
                          ceil_div(n_align_l0, fractal_shape[0])); // dst_stride = 4
```

##### 5.4.3. B32 Input Data Type

B32 input data type fractal is 16 * 8. During L1 -> L0 transpose, 2 fractals of 16 * 8 in the col direction are combined into a 16 * 16 square for transposing. This example uses float as the data type.

<div align="center">
  <img src="figures/B32_B_l1_l0B_trans_copy_l12l0.png" width="1100"><br>
  Figure 9: float data type, L1 -> L0B transpose, single asc_copy_l12l0b_transpose call data layout diagram
</div>

In this example `n = 50`. B32 transpose transfer requires combining 2 fractals in the col direction into a square, **`k_step` must be a multiple of 2**, so L1 col direction (n direction) reads 1 extra invalid fractal. After writing to L0B, there is 1 extra invalid fractal at the tail of the k direction; since L0B is in Zn layout, configuring `k = matrix_k` in `asc_mmad` ensures only valid k axis data participates in computation.

```cpp
kb_align_l1 = ceil_align(k, fractal_shape[0]);               // 80
n_align_l1 = ceil_align(n, fractal_shape[1] * fractal_num);  // 64
kb_align_l0 = ceil_align(k, fractal_shape[1] * fractal_num); // 80
n_align_l0 = ceil_align(n, fractal_shape[0]);                // 64
asc_copy_l12l0b_transpose(b_l0, b_l1, 0, 0,
                          ceil_div(kb_align_l1, fractal_shape[0]), // m_step = 5
                          ceil_div(n_align_l1, fractal_shape[1]),  // k_step = 8
                          ceil_div(kb_align_l1, fractal_shape[0]), // src_stride = 5
                          ceil_div(n_align_l0, fractal_shape[0])); // dst_stride = 4
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
  python3 ../scripts/gen_data.py -scenario_num=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py -scenario_num=$SCENARIO_NUM output/output.bin output/golden.bin
  ```
  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;   # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`-`7` | Scenario number: different data types and transpose combinations |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
