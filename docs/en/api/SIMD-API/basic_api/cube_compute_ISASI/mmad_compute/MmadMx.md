# MmadMx<a name="ZH-CN_TOPIC_000000253823118102"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:23:04.939Z pushedAt=2026-10-26T07:41:44.185Z nl=2 -->

## Applicable Products

### Prototype without bias

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx_res.md#id1 -->

### Prototype with bias

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products: supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 products: not supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 products: not supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: not supported
<!-- end id16 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx_res.md#id2 -->

## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

**MmadMx** (Microscaling Mmad) is a matrix multiplication with quantization coefficients, that is, both the left matrix and the right matrix have corresponding quantization coefficient matrices: the left quantization coefficient matrix **scaleA** and the right quantization coefficient matrix **scaleB**. In the MmadMx scenario, the left quantization coefficient matrix is multiplied by the left matrix, the right quantization coefficient matrix is multiplied by the right matrix, and then matrix multiplication is performed on the results of the two products.

The computation formula of MmadMx is C=(scaleA⊗A)*(scaleB⊗B)+Bias, where "⊗" denotes broadcast multiplication. When the left/right matrix is multiplied by the left/right quantization coefficient matrix, every 32 elements in the K direction share one quantization factor, as shown in [Figure 1](#zh_cn_topic_mmadmx_section2_figure1) (where both the A and B matrices use the fp4x2_e2m1_t data type as an example).

- **A**, **scaleA**, **B**, and **scaleB** are source operands. **A** is the left matrix with shape [M,K]; **scaleA** is the left quantization coefficient matrix with shape [M,K/32]; **B** is the right matrix with shape [K,N]; **scaleB** is the right quantization coefficient matrix with shape [K/32,N].
- **C** is the destination operand, which is the matrix storing the matrix multiplication result, with shape [M,N].
- **Bias** is the matrix multiplication bias with shape [1,N]. This **Bias** is applied to each row of the result matrix of (scaleA⊗A)*(scaleB⊗B).

**Figure 1** MmadMx API matrix multiplication fractal diagram<a id="zh_cn_topic_mmadmx_section2_figure1"></a>
![MmadMx API matrix multiplication fractal diagram](../../../../figures/mxmmad_demo_a5.png "MmadMx API matrix multiplication fractal diagram")

**Table 1** Explanation of matrices A, B, and C in matrix computation

<table>
    <thead>
      <tr>
        <th>Matrix computation logic</th>
        <th>Matrix computation physical location</th>
        <th>Dimensions</th>
        <th>Input/Output Data Format</th>
        <th>Data Type</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>A</td>
        <td>L0A Buffer</td>
        <td>M x K</td>
        <td>[Nz]</td>
        <td rowspan="5"><a href="#zh_cn_topic_mmadmx_section_datatype">data type</a></td>
      </tr>
      <tr>
        <td>scaleA</td>
        <td>L0A_MX Buffer</td>
        <td>M x K/32</td>
        <td>[Big Z small z] small z shape is (2, 16)</td>
      </tr>
      <tr>
        <td>B</td>
        <td>L0B Buffer</td>
        <td>K x N</td>
        <td>[Zn]</td>
      </tr>
      <tr>
        <td>scaleB</td>
        <td>L0B_MX Buffer</td>
        <td>K/32 x N</td>
        <td>[Large N small n] small n shape is (2, 16)</td>
      </tr>
      <tr>
        <td>C</td>
        <td>L0C Buffer</td>
        <td>M x N. It can be initialized using the bias matrix Bias, with a dimension of 1 x N.</td>
        <td>[Nz]</td>
      </tr>
    </tbody>
  </table>

## Prototype

- Without bias

  ```cpp
  template <typename T, typename U, typename S>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadParams& mmadParams)
  ```

- With bias

  ```cpp
  template <typename T, typename U, typename S, typename V>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadParams& mmadParams)
  ```

## Parameters

**Table 2** Parameters

| Parameter | Input/Output | Description |
| ---------- | --------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst | Output | Destination operand, result matrix C, of type LocalTensor. The supported physical storage location is L0C Buffer (TPosition:CO1).<br>The start address of LocalTensor must be 1024-byte aligned. |
| fm | Input | Source operand, left matrix A, of type LocalTensor. The supported physical storage location is L0A Buffer (TPosition:A2).<br>The start address of the scale matrix corresponding to left matrix A is: start address of matrix A / 16.<br>For the fp4 scenario, the start address of LocalTensor must be 512-byte aligned. For the fp8 scenario, the start address of LocalTensor must be 1024-byte aligned. |
| filter | Input | Source operand, right matrix B, of type LocalTensor. The supported physical storage location is L0B Buffer (TPosition:B2).<br>The start address of the scale matrix corresponding to right matrix B is: start address of matrix B / 16.<br>For the fp4 scenario, the start address of LocalTensor must be 512-byte aligned. For the fp8 scenario, the start address of LocalTensor must be 1024-byte aligned. |
| bias | Input | Source operand, bias matrix, of type LocalTensor. The supported physical storage location is BT Buffer (TPosition:C2).<br>The start address of LocalTensor must be 64-byte aligned. |
| mmadParams | Input | Matrix multiplication related parameters.<br>For the specific definition of this parameter type, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed.<br>For the description of MmadParams parameters, see [MmadParams Structure Parameter Description](Mmad.md#zh_cn_topic_mmad_section4_table4). |

## Data Types<a id="zh_cn_topic_mmadmx_section_datatype"></a>

**Table 3** Supported precision type combinations for the left matrix, right matrix, Scale matrix, Bias matrix, and result matrix of the MmadMx API (Ascend 950PR&950DT products)

| left matrix fm | right matrix filter | Scale matrix | Bias | result matrix dst |
| ------------ | ------------ | ---------- | -------- | ----------- |
| fp4x2_e1m2_t | fp4x2_e1m2_t | fp8_e8m0_t | float | float |
| fp4x2_e2m1_t | fp4x2_e1m2_t | fp8_e8m0_t | float | float |
| fp4x2_e1m2_t | fp4x2_e2m1_t | fp8_e8m0_t | float | float |
| fp4x2_e2m1_t | fp4x2_e2m1_t | fp8_e8m0_t | float | float |
| fp8_e4m3fn_t | fp8_e4m3fn_t | fp8_e8m0_t | float | float |
| fp8_e4m3fn_t | fp8_e5m2_t | fp8_e8m0_t | float | float |
| fp8_e5m2_t | fp8_e4m3fn_t | fp8_e8m0_t | float | float |
| fp8_e5m2_t | fp8_e5m2_t | fp8_e8m0_t | float | float |

## Return Value

None

## Constraints

- Storage location constraints for different matrices:
  - The result matrix C is supported only at the physical storage location L0C Buffer (TPosition:CO1), with a size of 256KB.
  - The left matrix A is supported only at the physical storage location L0A Buffer (TPosition:A2), with a size of 64KB.
  - The right matrix B is supported only at the physical storage location L0B Buffer (TPosition:B2), with a size of 64KB.
  - The Bias matrix is supported only at the physical storage location BT Buffer (TPosition:C2), with a size of 4KB.
- For address constraints, see Table 2.
- When any of **M**, **K**, or **N** is 0, the instruction is not executed, and the API is treated as a NOP (no operation).
- **K** must be a multiple of 64.
- For the fp4 scenario, the start addresses of matrices A/B must be aligned to 512 bytes. For the fp8 scenario, the start addresses of matrices A/B must be aligned to 1024 bytes.
- The start address of the scale matrix corresponding to the left matrix A/B is: the start address of the A/B matrix / 16.
- For other special scenario constraints, see [Mmad API constraints](Mmad.md#constraints).
- For boundary value calculation results, see [MmadMx API boundary value summary](../../../../appendix/mmad_mx_interface_boundary_value_summary.md).

## Example

For a complete usage example, see [MmadMx Best Practices](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_mxfp4_basic_api_high_performance).
