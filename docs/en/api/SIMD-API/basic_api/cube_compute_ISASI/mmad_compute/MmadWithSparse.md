# MmadWithSparse<a name="ZH-CN_TOPIC_000000253823118110"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:33:44.602Z pushedAt=2026-10-26T07:41:44.202Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

The MmadWithSparse API performs a special sparse matrix multiply-add operation. A sparse matrix is a special type of matrix that contains many zero elements. 4:2 structured sparsity requires that, in a consecutive group of four weight or activation values (usually a row or a column in a tensor), at most two values are nonzero, and the remaining two are forced to zero.

<a name="section19443175724215"></a><a name="section618mcpsimp"></a>The left matrix A passed to the MmadWithSparse API is a sparse matrix, and the right matrix B is a dense matrix. Matrix A is a full-size matrix and is densified during the MmadWithSparse computation. Matrix B is a dense matrix obtained after filtering out zero values through 4:2 structured sparsity, and it must be densified during input data preparation before computation (densified according to the densification algorithm described below). The dense matrix B must be loaded by calling LoadDataWithSparse, together with the index matrix. The index matrix is generated during the densification of matrix B and is then used for the densification of matrix A. The index matrix is stored in an internal buffer. Its layout and layout size are the same as those of matrix B, and it is used to further compress matrix A before the matrix multiply-add operation.

Similar to the Mmad API, which implements the Ascend NPU matrix computation capability, the mathematical expression of the MmadWithSparse API is:

$$
C = A \times B + C
$$

For a complete example, see [MmadWithSparse sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_with_sparse).

**Table 1** Explanation of matrices A, B, and C in sparse matrix computation

| matrix computation logic | physical location of matrix computation | dimension | input/output data format | data type |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M x K | Zz | int8_t |
| B | L0B Buffer | K/2 x N | Zn | int8_t |
| C | L0C Buffer | M x N | Nz | int32_t |

The following figure shows how the Cube computes the inner product of one row and one column:

**Figure 1** MmadWithSparse API computation process diagram
![MmadWithSparse API computation process diagram](../../../../figures/mmadwithsparse_workflow_demo.png "MmadWithSparse API computation process diagram")

The original fractal of matrix A is \(16, 2\*C0\), and the fractal of the index matrix Index is \(C0, 16\). The data of each row of matrix A undergoes 2-out-of-4 selection based on the corresponding column of data in the index matrix Index. For the fractal format and generation method of the index matrix, see [4:2 sparse index matrix](../cube_compute_fractal_intro/aux_cube_fractal_format_details.md#4-choose-2-sparse-index-matrix). For the selection algorithm, see [Matrix A sparse selection algorithm description](#zh-cn_topic_0000002535726174_li1829819426378). After the selection process, the fractal of matrix A becomes \(16, C0\), and the original fractal of matrix B is \(C0, 16\). Next, a normal Mmad operation is performed, in which one row of matrix A and one column of matrix B undergo an inner product operation to obtain the corresponding element in result matrix C.

- <a name="zh-cn_topic_0000002535726174_li1829819426378"></a>**Matrix A sparse selection algorithm description**

  After the LoadDataWithSparse instruction, the index matrix is stored in the dedicated buffer space built into the Cube. The data type is uint8, and the fractal format is small n large Z, corresponding to a fractal size of (32, 16) in the figure above. Each uint8 index element consists of four uint2 raw data values, and every two 2-bit index data values correspond to four elements of the raw matrix A. For each group of two index data values, the selection and filtering rules for the four elements of matrix A are shown in the following table:

  - The first index data 0 is used to indicate the relative position of the first nonzero element among the first three elements.
  - The second index data 1 is used to indicate the relative position of the second nonzero element among the last three elements.
  - Here, "-" indicates that the value at that position is not of concern, that is, it will be filtered out.

  **Table 2** Matrix A selection and filtering rules table

  | Index data 0 | Index data 1 | Element 0 | Element 1 | Element 2 | Element 3 |
  | --------- | --------- | ----- | ----- | ----- | ----- |
  | 2'b10 | 2'b10 | - | - | X | Y |
  | 2'b01 | 2'b10 | - | X | - | Y |
  | 2'b00 | 2'b10 | X | - | - | Y |
  | 2'b01 | 2'b01 | - | X | Y | - |
  | 2'b00 | 2'b01 | X | - | Y | - |
  | 2'b00 | 2'b00 | X | Y | - | - |
  | 2'b10 | 2'b00 | - | - | X/X | - |
  | 2'b01 | 2'b00 | - | X/X | - | - |

  **Figure 2** Matrix A 4:2 selection algorithm model<a name="zh-cn_topic_0000002535726174_fig14223210123816"></a>
  ![Matrix A 4:2 selection algorithm model](../../../../figures/sparse_mmad_4select2.png "Matrix A 4:2 selection algorithm model")

  [Figure 2](#zh-cn_topic_0000002535726174_fig14223210123816) shows an algorithm model in which one uint8 index element selects eight original matrix A elements, and finally outputs four selected matrix A elements.

  1. Under normal usage, the software should ensure that at most two nonzero elements exist. If an error occurs, that is, when three or more nonzero elements exist, only the first two nonzero elements at the least significant bit (LSB) positions are used.
  2. The "-" used in the table above means "do not care about the value at this position", implying that three or more nonzero elements may exist.

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
| U | Data type of **fm** and **filter**.<br>When **dst**, **fm**, and **filter** are basic data types, **T** must be of the int32_t type and **U** must be of the int8_t type; otherwise, compilation fails. |

**Table 4** Parameter description

| Parameter | Input/Output | Description |
| ---------- | --------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst | Output | Destination operand, result matrix C, of the LocalTensor type. The supported physical storage location is L0C Buffer (TPosition: CO1).<br>The start address of the LocalTensor must be aligned to 256 elements (1024 bytes). |
| fm | Input | Source operand, left matrix A, of the LocalTensor type. The supported physical storage location is L0A Buffer (TPosition: A2).<br>The start address of the LocalTensor must be aligned to 512 bytes. |
| filter | Input | Source operand, right matrix B, of the LocalTensor type. The supported physical storage location is L0B Buffer (TPosition: B2).<br>The start address of the LocalTensor must be aligned to 512 bytes. |
| mmadParams | Input | Matrix multiplication related parameters, of the MmadParams type.<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed.<br>For parameter description, see [Table 5](#zh_cn_topic_mmadsparse_section5_table5). |

**Table 5** Parameter description of the MmadParams structure (Sparse scenario)<a id="zh_cn_topic_mmadsparse_section5_table5"></a>

| Parameter | Description |
| --------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| m | Height of the left matrix. Value range: m∈[0, 4095]. The default value is 0. |
| n | Width of the right matrix. Value range: n∈[0, 4095]. The default value is 0. |
| k | Width of the left matrix and height of the right matrix. Value range: k∈[0, 4095]. The default value is 0. |
| cmatrixInitVal | Whether to enable the default zero-initialization of matrix C. The default value is true.<br>&nbsp;&nbsp;&bull; true: Matrix C is initialized to 0 by default.<br>&nbsp;&nbsp;&bull; false: No default operation is performed on matrix C, and it is initialized by setting the **cmatrixSource** parameter. |
| cmatrixSource | Whether the initial value of matrix C comes from the BT Buffer. The default value is false.<br>&nbsp;&nbsp;&bull; false: The L0C Buffer is not initialized.<br>&nbsp;&nbsp;&bull; true: The L0C Buffer is initialized with the data in the BT Buffer (TPosition: C2).<br>Atlas A2 products: supports configuration to true/false.<br>Atlas A3 products: supports configuration to true/false.<br>Atlas 200I/500 A2 inference product: supports configuration to true/false.<br>Note: This parameter is invalid for APIs with Bias input. Whether the initial value of matrix C comes from the BT Buffer is determined by the position of the Bias input. |
| isBias | This parameter is deprecated. Do not use it in new development. To accumulate the initial matrix, use the API with Bias, or configure the source of the initial value of matrix C through the **cmatrixInitVal** and **cmatrixSource** parameters. The API with Bias is recommended because it is simpler and more convenient than configuring the **cmatrixInitVal** and **cmatrixSource** parameters.<br>Whether to accumulate the initial matrix. The default value is false. The values are described as follows:<br>&nbsp;&nbsp;&bull; false: Matrix multiplication, without accumulating the initial matrix, C = A \* B.<br>&nbsp;&nbsp;&bull; true: Matrix multiplication and accumulation, with the initial matrix accumulated, C += A \* B. |
| unitFlag | **unitFlag** enables fine-grained parallelism between Mmad instructions and Fixpipe instructions. When this function is enabled, the computation result is moved out after the hardware finishes computing each fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): **unitFlag** is disabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): **unitFlag** is enabled, and the hardware does not reset the unit flag bit after executing the instruction.<br>&nbsp;&nbsp;&bull; 3 (2'b11): **unitFlag** is enabled, and the hardware resets the unit flag bit after executing the instruction.<br>When this function is enabled, set the **unitFlag** value of both the Mmad instruction and the Fixpipe instruction to 2 or 3.<br>This parameter is supported only on the following models:<br>Atlas A2 products.<br>Atlas A3 products.<br>For the parameter configuration scheme and feature details, see [UnitFlag](../../cube_compute_ISASI/mmad_compute_key_features/UnitFlag.md#ZH-CN_TOPIC_00000025690709788). |
| kDirectionAlign | In the Sparse scenario, this switch defaults to false and does not support configuration to true. The core function of K-direction alignment is to control, through the **kDirectionAlign** parameter, the alignment of the L0A Buffer and L0B Buffer matrices in the K direction when the float data type is used. |
| fmOffset | Offset of the left matrix (one value for the entire left matrix). Supports Scalar (which should be consistent with src_fm.dtype) or an immediate value. The default value is 0.<br>Note: Not used. It is passed in for compatibility with older models. Atlas A2 products and later products do not process it. |
| enSsparse | Enables the structured sparsity feature. The default value is false.<br>Note: Not used. It is passed in for compatibility with older models. Atlas A2 products and later products do not process it. |
| enWinogradA | Indicates whether matrix a is generated by winograd_feature_map_transform() to support the winograd feature. The type is bool. The default value is false.<br>Note: Not used. It is passed in for compatibility with older models. Atlas A2 products and later products do not process it. |
| enWinogradB | Indicates whether matrix b is generated by winograd_weight_transform() to support the winograd feature. The type is bool. The default value is false.<br>Note: Not used. It is passed in for compatibility with older models. Atlas A2 products and later products do not process it. |

## Data Type<a id="zh_cn_topic_mmadsparse_section_datatype"></a>

**Table 6** Supported precision type combinations of A, B, and C (Atlas 200I/500 A2 inference product) (Atlas A2 products) (Atlas A3 products)

| Left matrix A | Right matrix B | Result matrix C |
| ------- | ------- | --------- |
| int8_t | int8_t | int32_t |

## Return Value Description

None

## Constraints

- Storage location constraints for different matrices:

  - The result matrix C supports only the physical storage location L0C Buffer (**TPosition**: CO1).
  - The left matrix A supports only the physical storage location L0A Buffer (**TPosition**: A2).
  - The right matrix B supports only the physical storage location L0B Buffer (**TPosition**: B2).
- In the original sparse matrix B, at most 2 of every 4 elements can be nonzero. If there are 3 or more nonzero elements, only the first 2 nonzero elements are used.
- If any of M, K, or N is 0, the instruction is not executed and the API is treated as a NOP (no operation).
- The MmadWithSparse API does not support Gemv mode.
- For constraints on other special scenarios, see [Mmad API constraints](Mmad.md#constraints).

## Example

For a complete usage example, see [MmadWithSparse example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_with_sparse).

```cpp
AscendC::LocalTensor<int8_t> a1Local(AscendC::TPosition::A1, a1Addr, aSize);
AscendC::LocalTensor<int8_t> a2Local(AscendC::TPosition::A2, a2Addr, aSize);
AscendC::LocalTensor<int8_t> b1Local(AscendC::TPosition::B1, b1Addr, bSize);
AscendC::LocalTensor<uint8_t> idxB1Local(AscendC::TPosition::B1, idxB1Addr, bSize / 4);
AscendC::LocalTensor<int8_t> b2Local(AscendC::TPosition::B2, b2Addr, bSize);
AscendC::LocalTensor<int32_t> cLocal(AscendC::TPosition::CO1, cAddr, cSize);

// GM->L1 Buffer: Move the original matrix a, the densified matrix b, and the corresponding idx matrix to the L1 Buffer.
CopyIn(a1Local, b1Local, idxB1Local);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);

// L1 Buffer->L0A Buffer/L0B Buffer: Move the original matrix a, the densified matrix b, and the corresponding idx matrix to the L0A Buffer/L0B Buffer.
SplitA(a1Local, a2Local);
SplitB(b2Local, b1Local, idxB1Local);
AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);

// mmad requires the matrix dimensions to be specified for computation.
uint32_t m = 128;
uint32_t k = 64;
uint32_t n = 128;
AscendC::MmadWithSparse(c1Local, a2Local, b2Local, { m, n, k, false, 0, false, false, false });
```
