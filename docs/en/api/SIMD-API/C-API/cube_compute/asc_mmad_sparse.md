# asc_mmad_sparse

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:33:40.298Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Performs a matrix multiply-add operation, where the input left matrix A is a dense matrix and the right matrix B is a sparse matrix. For matrix A, densification is performed during the computation of **asc_mmad_sparse**; for matrix B, densification is performed by the user during input data preparation before computation (densification is performed according to the dense algorithm described below), so the matrix A input to this API is a dense matrix. The dense matrix A must be loaded through [asc_copy_l12l0b_sparse](../cube_datamove/asc_copy_l12l0b_sparse.md), together with the index matrix. The index matrix is generated during the densification of matrix B and is then used for the densification of matrix A.

## Prototype

- Regular computation
```cpp
__aicore__ inline void asc_mmad_sparse(__cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag, bool c_matrix_source, bool c_matrix_init_val)
```

- Synchronous computation
```cpp
__aicore__ inline void asc_mmad_sparse_sync(__cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, uint16_t m, uint16_t k, uint16_t n, uint8_t unit_flag, bool c_matrix_source, bool c_matrix_init_val)
```


## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| **c**       | Output    | 	 Destination operand, result matrix C. The start address requires 1024-byte alignment.            |
| **a**       | Input    |    Source operand, left matrix A. The start address requires 512-byte alignment.|
| **b**       | Input    | 	 Source operand, right matrix B. The start address requires 512-byte alignment.      |
| **m**       | Input    | 	 Height of the left matrix. Value range: m∈[0,4095].      |
| **k**       | Input    | 	 Width of the left matrix and height of the right matrix. Value range: k∈[0,4095].      |
| **n**       | Input    | 	 Width of the right matrix. Value range: n∈[0,4095].      |
| **unit_flag**       | Input    | 	 **unit_flag** enables fine-grained parallelism between Mmad instructions and Fixpipe instructions. When this feature is enabled, the hardware moves out the computation result after each fractal is computed. This feature is not applicable to the L0C Buffer accumulation scenario. Value description: <br> **0**: Reserved value. <br> **2**: Enables **unit_flag**. After the hardware finishes executing the instruction, the **unit_flag** feature is not disabled.<br> **3**: Enables **unit_flag**. After the hardware finishes executing the instruction, the **unit_flag** feature is disabled.<br> When this feature is enabled, set **unit_flag** of the Mmad instruction to **3** for the last fractal and to **2** for the remaining fractals.     |
| **c_matrix_source**       | Input    | 	 Configures whether the initial value of matrix C comes from the BiasTable Buffer (the hardware cache area that stores the bias). Value description:  <br>&bull; **true**: Comes from the BiasTable Buffer. <br>&bull; **false**: Comes from the L0C Buffer.    |
| **c_matrix_init_val**       | Input    | 	 Configures whether the initial value of matrix C is 0. Value description:  <br>&bull; **true**: The initial value of matrix C is 0. <br>&bull; **false**: The initial value of matrix C is configured through the **c_matrix_source** parameter.     | 


## Return Value

None

## Pipeline Type

PIPE_M

## Constraints

- In the original sparse matrix B, at most 2 non-zero elements are guaranteed among every 4 elements. If there are 3 or more non-zero elements, only the first 2 non-zero elements are used.
- When any of **m**, **k**, or **n** is **0**, this instruction is not executed.

## Dense Algorithm

Assume that among every 4 elements of the original sparse matrix B, at least 2 are zeros. The densified matrix B is a dense matrix in which 2 zeros are filtered out among every 4 elements. During the densification of matrix B, an index matrix is generated as follows: for every 4 elements in the sparse matrix B, two 2-bit indices are generated in the index matrix and encoded according to the following rules. The indices must be within the range {0, 1, 2}.
<br>&bull;The first index indicates the relative position of the first non-zero element among the first 3 elements.
<br>&bull;The second index indicates the relative position of the second non-zero element among the last 3 elements.
<br>For details, refer to the following table. Here, "-" indicates that the algorithm does not care about the value at that position because it will be filtered out.
<table border="1" style="border-collapse: collapse; width: 100%; text-align: center;">
  <!-- Row 1: Do not merge. -->
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">Example</td>
    <td style="padding: 8px; border: 1px solid #333;">ele0</td>
    <td style="padding: 8px; border: 1px solid #333;">ele1</td>
    <td style="padding: 8px; border: 1px solid #333;">ele2</td>
    <td style="padding: 8px; border: 1px solid #333;">ele3</td>
    <td style="padding: 8px; border: 1px solid #333;">Index_a[i]</td>
    <td style="padding: 8px; border: 1px solid #333;">Index_b[i]</td>
  </tr>

  <!-- Rows 2–7: Merge the cells in the first column (6 rows in total). -->
  <tr>
    <td rowspan="6" style="padding: 8px; border: 1px solid #333; vertical-align: middle;">Two non-zero elements</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">-</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">-</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">Y</td>
    <td style="padding: 8px; border: 1px solid #333;">-</td>
    <td style="padding: 8px; border: 1px solid #333;">-</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>

  <!-- Rows 8–11: Merge the cells in the first column (4 rows in total). -->
  <tr>
    <td rowspan="4" style="padding: 8px; border: 1px solid #333; vertical-align: middle;">One non-zero element</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b10</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b01</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">X</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>

  <!-- Row 12: Do not merge. -->
  <tr>
    <td style="padding: 8px; border: 1px solid #333;">All zero</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">0</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
    <td style="padding: 8px; border: 1px solid #333;">2'b00</td>
  </tr>
</table>
This index matrix is used for the densification of matrix A. Based on the index matrix, 2 elements are selected from the 4 elements in **MatrixA** to participate in the computation, as shown in the following figure:

![Dense algorithm illustration](../figures/asc_mmad_sparse_dense_algorithm.png)

## Example

```cpp
constexpr uint32_t total_length = 128;
__ca__ int8_t a[total_length];
__cb__ int8_t b[total_length];
__cc__ int32_t c[total_length];
uint16_t m = 2;
uint16_t k = 2;
uint16_t n = 1;
uint8_t unit_flag = 2;
bool c_matrix_source = true;
bool c_matrix_init_val = true;
asc_mmad_sparse(c, a, b, m, k, n, unit_flag, c_matrix_source, c_matrix_init_val);
```
