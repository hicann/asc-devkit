# MmadWithSparse<a name="ZH-CN_TOPIC_000000253823118110"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:50:56.336Z -->

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
- Atlas 200I/500 A2 inference product: Not supported
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

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

The **MmadWithSparse** API performs a special sparse cube multiply-add operation. A sparse cube is a special type of cube that contains many zero elements. 4:2 structured sparsity requires that in a group of four consecutive weights or activation values (usually a row or a column in a tensor), at most two values are nonzero, and the remaining two are forced to zero.

The left cube A passed to the **MmadWithSparse** API is a sparse cube, and the right cube B is a dense cube. Cube A is a full-size cube that is densified during the **MmadWithSparse** computation. Cube B is a dense cube obtained after filtering out zero values through 4:2 structured sparsity, and it must be densified during input data preparation before computation (densified according to the densification algorithm described below). The dense cube B must be loaded by calling **LoadDataWithSparse**, which also loads the index cube. The index cube is generated during the densification of cube B and is then used for the densification of cube A. The index cube is stored in an internal buffer, and its layout and layout size are the same as those of cube B. It is used to further compress cube A before the cube multiply-add operation.

Similar to the **Mmad** API, which implements the cube computation capability of Ascend NPUs, the mathematical expression of the **MmadWithSparse** API is:

$$
C = A \times B + C
$$

For a complete example, please refer to [MmadWithSparse sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/mmad_with_sparse).

**Table 1** Explanation of matrices A, B, and C in sparse cube computation

| Cube Computation Logic | Physical Location of Cube Computation | Dimension | Input/Output Data Format | Data Type |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M x K | Zz | int8_t |
| B | L0B Buffer | K/2 x N | Zn | int8_t |
| C | L0C Buffer | M x N | Nz | int32_t |

The following figure shows how the Cube computes the inner product of one row and one column:

**Figure 1** MmadWithSparse API computation process diagram
![MmadWithSparse API computation process diagram](../../../../figures/mmadwithsparse_workflow_demo.png "MmadWithSparse API computation process diagram")

Here, the original fractal of cube A is \(16, 2\*C0\), and the fractal of the index cube Index is \(C0, 16\). The data in each row of cube A is selected 4-to-2 based on the corresponding column of data in the index cube Index. For the fractal format and generation method of the index cube, please refer to [4-to-2 sparse index cube](../cube_compute_fractal_intro/auxiliary_matrix_fractal_format_details.md#4-choose-2-sparse-index-cube). For the selection algorithm, refer to [Cube A sparse selection algorithm description](#zh-cn_topic_0000002535726174_li1829819426378). After the selection process, the fractal of cube A becomes \(16, C0\), and the original fractal of cube B is \(C0, 16\). Then a normal Mmad operation is performed, that is, one row of cube A and one column of cube B complete an inner product operation to obtain the corresponding element in result cube C.

- <a name="zh-cn_topic_0000002535726174_li1829819426378"></a>**Cube A sparse selection algorithm description**

  After the **LoadDataWithSparse** instruction, the index cube is stored in the dedicated buffer space built into the Cube. Its data type is uint8, and its fractal format is small n large Z, corresponding to a fractal size of (32, 16) in the figure above. Each uint8 index element consists of four uint2 raw data values, and every two 2-bit index data values correspond to 4 bits of the original cube A. For each group of two index data values, the selection and filtering rules for the four elements of cube A are shown in the following table:

  - The first index data 0 indicates the relative position of the first nonzero element among the first three elements.
  - The second index data 1 indicates the relative position of the second nonzero element among the last three elements.
  - Here, "-" indicates that the value at that position is not of concern and will be filtered out.

  **Table 2** Cube A selection and filtering rules table

  | index data 0 | index data 1 | element 0 | element 1 | element 2 | element 3 |
  | --------- | --------- | ----- | ----- | ----- | ----- |
  | 2'b10 | 2'b10 | - | - | X | Y |
  | 2'b01 | 2'b10 | - | X | - | Y |
  | 2'b00 | 2'b10 | X | - | - | Y |
  | 2'b01 | 2'b01 | - | X | Y | - |
  | 2'b00 | 2'b01 | X | - | Y | - |
  | 2'b00 | 2'b00 | X | Y | - | - |
  | 2'b10 | 2'b00 | - | - | X/X | - |
  | 2'b01 | 2'b00 | - | X/X | - | - |

  **Figure 2** Cube A 4:2 selection algorithm model<a name="zh-cn_topic_0000002535726174_fig14223210123816"></a>
  ![Cube A 4:2 selection algorithm model](../../../../figures/sparse_mmad_4select2.png "Cube A 4:2 selection algorithm model")

  [Figure 2](#zh-cn_topic_0000002535726174_fig14223210123816) shows the algorithm model in which one **uint8** index element corresponds to selecting eight original cube A elements, and finally outputs four selected cube A elements.

  1. Under normal usage, the software should ensure that there are at most two nonzero elements. If an error occurs, that is, when there are three or more nonzero elements, only the first two nonzero elements at the least significant bit (LSB) positions are used.
  2. The "-" used in the table above indicates "the value at this position is not of concern", implying that there may be three or more nonzero elements.

## Function Prototype

```cpp
template <typename T = int32_t, typename U = int8_t, typename Std::enable_if<Std::is_same<PrimT<T>, int32_t>::value, bool>::type = true, typename Std::enable_if<Std::is_same<PrimT<U>, int8_t>::value, bool>::type = true>
__aicore__ inline void MmadWithSparse(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<U>& filter, const MmadParams& mmadParams)
```

## Parameters

**Table 3** Template parameter description

| Parameter | Description |
| ------ | ------------------------------------------------------------------------------------------------------------------ |
| T | Data type of **dst**. |
| U | Data type of **fm** and **filter**.<br>When **dst**, **fm**, and **filter** are basic data types, **T** must be **int32_t** and **U** must be **int8_t**; otherwise, compilation fails. |

**Table 4** Parameter description

| Parameter | Input/Output | Description |
| ---------- | --------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst | Output | Destination operand, result cube C, of type **LocalTensor**, with supported physical storage location **L0C Buffer** (**TPosition:CO1**).<br>The start address of **LocalTensor** must be aligned to 256 elements (1024 bytes). |
| fm | Input | Source operand, left cube A, of type **LocalTensor**, with supported physical storage location **L0A Buffer** (**TPosition:A2**).<br>The start address of **LocalTensor** must be aligned to 512 bytes. |
| filter | Input | Source operand, right cube B, of type **LocalTensor**, with supported physical storage location **L0B Buffer** (**TPosition:B2**).<br>The start address of **LocalTensor** must be aligned to 512 bytes. |
| mmadParams | Input | Cube multiplication related parameters, of type **MmadParams**.<br>For the specific definition, please refer to ${INSTALL_DIR}/include/ascendc/basic_api/API/kernel_struct_mm.h, where ${INSTALL_DIR} should be replaced with the file storage path after CANN software installation.<br>For parameter description, please refer to [Table 5](#zh_cn_topic_mmadsparse_section5_table5). |

**Table 5** Parameter description of the **MmadParams** structure (Sparse scenario)<a id="zh_cn_topic_mmadsparse_section5_table5"></a>

| Parameter | Description |
| --------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| m | Height of the left cube, value range: m∈[0, 4095]. Default value is 0. |
| n | Width of the right cube, value range: n∈[0, 4095]. Default value is 0. |
| k | Width of the left cube and height of the right cube, value range: k∈[0, 4095]. Default value is 0. |
| ccubeInitVal | Whether to enable the default zero-initialization of cube C. Default value is **true**.<br>&nbsp;&nbsp;&bull; **true**: Cube C is initialized to 0 by default.<br>&nbsp;&nbsp;&bull; **false**: No default operation is performed on cube C; it is initialized by setting the **ccubeSource** parameter. |
| ccubeSource | Configures whether the initial value of cube C comes from **BT Buffer**. Default value is **false**.<br>&nbsp;&nbsp;&bull; **false**: No initialization is performed on **L0C Buffer**.<br>&nbsp;&nbsp;&bull; **true**: The data in **BT Buffer** (**TPosition:C2**) is used to initialize **L0C Buffer**.<br>Atlas A2 training products/Atlas A2 inference products support configuration as **true**/**false**.<br>Atlas A3 training products/Atlas A3 inference products support configuration as **true**/**false**.<br>Atlas 200I/500 A2 inference product supports configuration as **true**/**false**.<br>Note: For APIs with **Bias** input, this parameter is invalid; whether the initial value of cube C comes from **BT Buffer** is determined by the position of the **Bias** input. |
| isBias | This parameter is deprecated. Do not use it in new development. To accumulate the initial cube, use the API with **Bias**; alternatively, configure the source of the initial value of cube C through the **ccubeInitVal** and **ccubeSource** parameters. The API with **Bias** is recommended, as it is simpler and more convenient than configuring the **ccubeInitVal** and **ccubeSource** parameters.<br>Configures whether to accumulate the initial cube. Default value is **false**. The values are described as follows:<br>&nbsp;&nbsp;&bull; **false**: Cube multiplication, no need to accumulate the initial cube, C = A \* B.<br>&nbsp;&nbsp;&bull; **true**: Cube multiply-accumulate, the initial cube needs to be accumulated, C += A \* B. |
| unitFlag | **unitFlag** enables fine-grained parallelism between **Mmad** and **Fixpipe** instructions. When this feature is enabled, the computation result is moved out after the hardware finishes computing each fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; **0** (2'b00): **unitFlag** is disabled.<br>&nbsp;&nbsp;&bull; **2** (2'b10): **unitFlag** is enabled, and the hardware does not reset the unit flag bit after executing the instruction.<br>&nbsp;&nbsp;&bull; **3** (2'b11): **unitFlag** is enabled, and the hardware resets the unit flag bit after executing the instruction.<br>When enabling this feature, set the **unitFlag** value of both **Mmad** and **Fixpipe** instructions to 2 or 3.<br>This parameter is supported only on the following models:<br>Atlas A2 training products/Atlas A2 inference products;<br>Atlas A3 training products/Atlas A3 inference products.<br>For parameter configuration schemes and feature details, see [UnitFlag](../mmad_compute_key_features/UnitFlag.md#ZH-CN_TOPIC_00000025690709788). |
| kDirectionAlign | In the Sparse scenario, this switch defaults to **false** and does not support configuration as **true**. The core function of K-direction alignment is to control, through the `kDirectionAlign` parameter, the alignment of the **L0A Buffer** and **L0B Buffer** matrices in the K direction when the **float** data type is used. |
| fmOffset | Offset of the left cube (one value for the entire left cube), supporting **Scalar** (which should be consistent with src_fm.dtype)/immediate value, defaulting to 0.<br>Note: Unused. Compatible with older models for API input; Atlas A2 training products/Atlas A2 inference products and later products do not handle it. |
| enSsparse | Enables the structured sparsity feature, defaulting to **false**.<br>Note: Unused. Compatible with older models for API input; Atlas A2 training products/Atlas A2 inference products and later products do not handle it. |
| enWinogradA | Whether cube a is generated by `winograd_feature_map_transform()`, used to support the winograd feature, of type **bool**, defaulting to **false**.<br>Note: Unused. Compatible with older models for API input; Atlas A2 training products/Atlas A2 inference products and later products do not handle it. |
| enWinogradB | Whether cube b is generated by `winograd_weight_transform()`, used to support the winograd feature, of type **bool**, defaulting to **false**.<br>Note: Unused. Compatible with older models for API input; Atlas A2 training products/Atlas A2 inference products and later products do not handle it. |

## data type<a id="zh_cn_topic_mmadsparse_section_datatype"></a>

**Table 6** Supported precision type combinations for A, B, and C (Atlas 200I/500 A2 inference product) (Atlas A2 training products/Atlas A2 inference products) (Atlas A3 training products/Atlas A3 inference products)

| left cube A | right cube B | result cube C |
| ------- | ------- | --------- |
| int8_t | int8_t | int32_t |

## Return Value

None

## Constraints

- Constraints on the storage locations of different matrices:

  - The result cube C only supports being located at the physical storage location L0C Buffer (TPosition: CO1).
  - The left cube A only supports being located at the physical storage location L0A Buffer (TPosition: A2).
  - The right cube B only supports being located at the physical storage location L0B Buffer (TPosition: B2).
- In the original sparse cube B, at most 2 of every 4 elements should be nonzero. If there are 3 or more nonzero elements, only the first 2 nonzero elements are used.
- When any of M, K, or N is 0, the instruction is not executed, and the API is treated as a NOP (no operation).
- The **MmadWithSparse** API does not support the Gemv mode.
- For constraints in other special scenarios, please refer to [Mmad API constraints](Mmad.md#constraints).

## Example

For a complete usage sample, see [MmadWithSparse sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/mmad_with_sparse).

```cpp
AscendC::LocalTensor<int8_t> a1Local(AscendC::TPosition::A1, a1Addr, aSize);
AscendC::LocalTensor<int8_t> a2Local(AscendC::TPosition::A2, a2Addr, aSize);
AscendC::LocalTensor<int8_t> b1Local(AscendC::TPosition::B1, b1Addr, bSize);
AscendC::LocalTensor<uint8_t> idxB1Local(AscendC::TPosition::B1, idxB1Addr, bSize / 4);
AscendC::LocalTensor<int8_t> b2Local(AscendC::TPosition::B2, b2Addr, bSize);
AscendC::LocalTensor<int32_t> cLocal(AscendC::TPosition::CO1, cAddr, cSize);

// GM->L1 Buffer. Copy the original cube a, the densified cube b, and the corresponding index cube to the L1 Buffer.
CopyIn(a1Local, b1Local, idxB1Local);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);

// L1 Buffer->L0A Buffer/L0B Buffer. Copy the original cube a, the densified cube b, and the corresponding index cube to the L0A Buffer/L0B Buffer.
SplitA(a1Local, a2Local);
SplitB(b2Local, b1Local, idxB1Local);
AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);

// mmad requires the cube dimensions to be specified for computation.
uint32_t m = 128;
uint32_t k = 64;
uint32_t n = 128;
AscendC::MmadWithSparse(c1Local, a2Local, b2Local, { m, n, k, false, 0, false, false, false });
```
