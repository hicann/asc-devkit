# MmadMx<a name="ZH-CN_TOPIC_000000253823118102"></a>

<!-- md-trans-meta sourceCommit=5afce9079f40f95eb7b22e64d4da4de38a1a5378 translatedAt=2026-08-27T16:52:47.557Z -->

## Product Support

### Prototype without bias

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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
### Prototype with bias

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

**MmadMx** (Microscaling Mmad) is a cube multiplication with quantization coefficients, that is, both the left cube and the right cube have corresponding quantization coefficient matrices, namely the left quantization coefficient cube **scaleA** and the right quantization coefficient cube **scaleB**. In the MmadMx scenario, the left quantization coefficient cube is multiplied by the left cube, the right quantization coefficient cube is multiplied by the right cube, and cube multiplication is performed on the results of the two products.

The computation formula of **MmadMx** is C=(scaleA⊗A)*(scaleB⊗B)+Bias, where "⊗" denotes broadcast multiplication. When the left/right cube is multiplied by the left/right quantization coefficient cube, every 32 elements in the K direction share one quantization factor, as shown in [Figure 1](#zh_cn_topic_mmadmx_section2_figure1) (where both matrices A and B use the fp4x2_e2m1_t data type as an example).

- **A**, **scaleA**, **B**, and **scaleB** are source operands. **A** is the left cube with the shape [M,K]; **scaleA** is the left quantization coefficient cube with the shape [M,K/32]; **B** is the right cube with the shape [K,N]; **scaleB** is the right quantization coefficient cube with the shape [K/32,N].
- **C** is the destination operand, which is the cube storing the cube multiplication result, with the shape [M,N].
- **Bias** is the cube multiplication bias with the shape [1,N]. This **Bias** is applied to each row of the (scaleA⊗A)*(scaleB⊗B) result cube.

**Figure 1** MmadMx API cube multiplication fractal diagram<a id="zh_cn_topic_mmadmx_section2_figure1"></a>
![MmadMx API cube multiplication fractal diagram](../../../../figures/mxmmad_demo_a5.png "MmadMx API cube multiplication fractal diagram")

**Table 1** Explanation of matrices A, B, and C in cube computation

<table>
    <thead>
      <tr>
        <th>Cube computation logic</th>
        <th>Cube computation physical location</th>
        <th>Dimension</th>
        <th>Input/Output data format</th>
        <th>Data type</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>A</td>
        <td>L0A Buffer</td>
        <td>M x K</td>
        <td>[Nz]</td>
        <td rowspan="5"><a href="#zh_cn_topic_mmadmx_section_datatype">Data type</a></td>
      </tr>
      <tr>
        <td>scaleA</td>
        <td>L0A_MX Buffer</td>
        <td>M x K/32</td>
        <td>[large Z small z] small z shape is (2, 16)</td>
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
        <td>M x N, can be initialized using the bias cube Bias, with a dimension of 1 x N</td>
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

| Parameter | Input/Output | Meaning |
| ---------- | --------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **dst** | Output | Destination operand, result cube C, of type **LocalTensor**, with the supported physical storage location being L0C Buffer (**TPosition:CO1**).<br>The start address of **LocalTensor** must be 1024-byte aligned. |
| **fm** | Input | Source operand, left cube A, of type **LocalTensor**, with the supported physical storage location being L0A Buffer (**TPosition:A2**).<br>The start address of the scale cube corresponding to left cube A is: A cube start corresponding address/16.<br>For the fp4 scenario, the start address of **LocalTensor** must be 512-byte aligned. For the fp8 scenario, the start address of **LocalTensor** must be 1024-byte aligned. |
| **filter** | Input | Source operand, right cube B, of type **LocalTensor**, with the supported physical storage location being L0B Buffer (**TPosition:B2**).<br>The start address of the scale cube corresponding to right cube B is: B cube start corresponding address/16.<br>For the fp4 scenario, the start address of **LocalTensor** must be 512-byte aligned. For the fp8 scenario, the start address of **LocalTensor** must be 1024-byte aligned. |
| **bias** | Input | Source operand, Bias cube, of type **LocalTensor**, with the supported physical storage location being BT Buffer (**TPosition:C2**).<br>The start address of **LocalTensor** must be 64-byte aligned. |
| **mmadParams** | Input | Cube multiplication related parameters.<br>For the specific definition of this parameter type, see ${INSTALL_DIR}/include/ascendc/basic_api/API/kernel_struct_mm.h, where ${INSTALL_DIR} should be replaced with the file storage path after CANN software installation.<br>For the description of **MmadParams** parameters, see [MmadParams structure parameter description](Mmad.md#zh_cn_topic_mmad_section4_table4). |

## Data Type<a id="zh_cn_topic_mmadmx_section_datatype"></a>

**Table 3** Supported precision type combinations for the left cube, right cube, Scale cube, Bias cube, and result cube of the MmadMx API (Ascend 950PR/Ascend 950DT)

| left cube fm | right cube filter | Scale cube | Bias | result cube dst |
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

- Constraints on the storage locations of different matrices:
  - The result cube C can only be located in the **L0C Buffer** (**TPosition:CO1**) with a size of 256 KB.
  - The left cube A can only be located in the **L0A Buffer** (**TPosition:A2**) with a size of 64 KB.
  - The right cube B can only be located in the **L0B Buffer** (**TPosition:B2**) with a size of 64 KB.
  - The Bias cube can only be located in the **BT Buffer** (**TPosition:C2**) with a size of 4 KB.
- For address constraints, see Table 2.
- When any of M, K, and N is 0, the instruction is not executed and the API is treated as a NOP (no operation).
- K must be a multiple of 64.
- For the fp4 scenario, the start addresses of matrices A and B must be aligned to 512 bytes. For the fp8 scenario, the start addresses of matrices A and B must be aligned to 1024 bytes.
- The start address of the scale cube corresponding to the left cube **A**/**B** is: the corresponding address of the **A**/**B** cube start divided by 16.
- For constraints in other special scenarios, see [Mmad API constraints](Mmad.md#Constraints).

## Example

For a complete usage example, see [MmadMx best practices](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/05_best_practices/01_cube_compute/matmul_mxfp4_basic_api_high_performance).
