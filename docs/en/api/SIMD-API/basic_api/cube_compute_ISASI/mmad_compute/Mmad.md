# Mmad<a name="ZH-CN_TOPIC_00000025382311899"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:27:42.403Z pushedAt=2026-10-26T07:41:44.195Z nl=2 -->

## Applicable Products

### Prototype without bias

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad_res.md#id1 -->

### Prototype with bias passed in

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products: supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 products: supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 products: supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference product: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad_res.md#id2 -->

## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

**Mmad** is the matrix multiply-add core computation API of Ascend C for Ascend AI chips. It is designed for high-performance operator development, encapsulates the matrix multiply-accumulate computation capability of Ascend NPU hardware, and is widely used in the development of neural network layers (such as fully connected layers and convolutional layers) and numerical computation operators.

**Mmad** implements the matrix multiplication computation capability of the Ascend NPU. Its mathematical expression is:

$$
C = A \times B + C
$$

<!-- npu="950" id19 -->
For Ascend 950PR&950DT products:

**Table 1** Explanation of matrices A, B, and C in matrix computation

| matrix computation logic | matrix computation physical location | dimension | input/output data format | data type |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M x K | Nz | <a href="#zh_cn_topic_mmad_section5">data type</a> |
| B | L0B Buffer | K x N | Zn | <a href="#zh_cn_topic_mmad_section5">data type</a> |
| C | L0C Buffer | M x N, which can be initialized with the bias matrix Bias, with a dimension of 1 x N | Nz | <a href="#zh_cn_topic_mmad_section5">data type</a> |

**Figure 1** Fractal diagram of the Mmad API matrix multiplication (Ascend 950PR&950DT products)<a id="zh_cn_topic_mmad_section2_figure1"></a>

![Mmad API computation fractal diagram](../../../../figures/mmad_demo_950.png)
<!-- end id19 -->

<!-- npu="A3,910b" id20 -->
For the following product models, the matrix descriptions are shown in the table below:
<!-- npu="A3" id21 -->
Atlas A3 products
<!-- end id21 -->
<!-- npu="910b" id22 -->
Atlas A2 products
<!-- end id22 -->
**Table 2** Description of matrices A, B, and C in matrix computation

  <table>
    <thead>
      <tr>
        <th>Matrix Computation Logic</th>
        <th>Matrix Computation Physical Location</th>
        <th>Dimension</th>
        <th>Input/Output Data Format</th>
        <th>Data Type</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>A</td>
        <td>L0A Buffer</td>
        <td>M x K</td>
        <td>Zz</td>
        <td rowspan="3"><a href="#zh_cn_topic_mmad_section5">Data Type</a></td>
      </tr>
      <tr>
        <td>B</td>
        <td>L0B Buffer</td>
        <td>K x N</td>
        <td>Zn</td>
      </tr>
      <tr>
        <td>C</td>
        <td>L0C Buffer</td>
        <td>M x N, which can be initialized using the bias matrix Bias, with dimensions of 1 x N</td>
        <td>Nz</td>
      </tr>
    </tbody>
  </table>
<!-- end id20 -->

## Prototype

- Without passing in bias

    ```cpp
    template <typename T, typename U, typename S>
    __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadParams& mmadParams)
    ```

- Passing in bias

    ```cpp
    template <typename T, typename U, typename S, typename V>
    __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadParams& mmadParams)
    ```

## Parameters

**Table 3** Parameter description

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand, result matrix C, of type **LocalTensor**. The supported physical storage location is L0C Buffer (TPosition: CO1).<br>The start address of **LocalTensor** must be 1024-byte aligned. |
| fm | Input | Source operand, left matrix A, of type **LocalTensor**. The supported physical storage location is L0A Buffer (TPosition: A2).<br>The start address of **LocalTensor** must be 512-byte aligned. |
| filter | Input | Source operand, right matrix B, of type **LocalTensor**. The supported physical storage location is L0B Buffer (TPosition: B2).<br>The start address of **LocalTensor** must be 512-byte aligned. |
| bias | Input | Source operand, Bias matrix, of type **LocalTensor**. The supported physical storage location is BT Buffer (TPosition: C2).<br>The start address of **LocalTensor** must be 64-byte aligned. |
| mmadParams | Input | Matrix multiplication related parameters.<br>For the specific definition of this parameter type, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.<br>For the description of the **MmadParams** parameters, see [Table 4](#zh_cn_topic_mmad_section4_table4). |

**Table 4** Description of parameters in the MmadParams structure<a id="zh_cn_topic_mmad_section4_table4"></a>

| Parameter | Meaning |
| --- | --- |
| m | Height of the left matrix. Value range: m∈[0, 4095]. The default value is 0. |
| n | Width of the right matrix. Value range: n∈[0, 4095]. The default value is 0. |
| k | Width of the left matrix and height of the right matrix. Value range: k∈[0, 4095]. The default value is 0. |
| cmatrixInitVal | Whether to enable the default zero-initialization operation for matrix C. The default value is **true**.<br>&nbsp;&nbsp;&bull; **true**: Matrix C is initialized to 0 by default.<br>&nbsp;&nbsp;&bull; **false**: No default operation is performed on matrix C, and it is initialized by setting the **cmatrixSource** parameter. |
| cmatrixSource | Configures whether the initial value of matrix C comes from BT Buffer. The default value is **false**.<br>&nbsp;&nbsp;&bull; **false**: No initialization operation is performed on L0C Buffer.<br>&nbsp;&nbsp;&bull; **true**: L0C Buffer is initialized with data from BT Buffer (TPosition: C2).<br><br>Atlas training products support only **false**.<br><br>Atlas inference products AI Core support only **false**.<br><br>Atlas A2 products support **true**/**false**.<br><br>Atlas A3 products support **true**/**false**.<br><br>Atlas 200I/500 A2 inference products support **true**/**false**.<br><br>Ascend 950PR&950DT products support **true**/**false**.<br><br>Note: This parameter is invalid for APIs with Bias input. Whether the initial value of matrix C comes from BT Buffer is determined by the position of the bias input. |
| isBias | This parameter is deprecated. Do not use it in new development. To accumulate the initial matrix, use the API with Bias, or configure the source of the initial value of matrix C through the **cmatrixInitVal** and **cmatrixSource** parameters. The API with Bias is recommended, as it is simpler and more convenient than configuring the **cmatrixInitVal** and **cmatrixSource** parameters.<br><br>Configures whether to accumulate the initial matrix. The default value is **false**. The value description is as follows:<br>&nbsp;&nbsp;&bull; **false**: Matrix multiplication, without accumulating the initial matrix, C = A \* B.<br>&nbsp;&nbsp;&bull; **true**: Matrix multiplication and addition, with the initial matrix accumulated, C += A \* B. |
| disableGemv | When M=1, this parameter configures whether the Mmad computation enables the [GEMV](../mmad_compute_key_features/GEMV.md#ZH-CN_TOPIC_0000002538231187) mode.<br>&nbsp;&nbsp;&bull; **false**: Enables the GEMV mode.<br>&nbsp;&nbsp;&bull; **true**: Disables the GEMV mode.<br><br>This parameter is supported only on the following models:<br><br>Ascend 950PR&950DT products |
| unitFlag | **unitFlag** controls fine-grained parallelism between the Mmad instruction and the Fixpipe instruction. When this feature is enabled, the computation result is moved out after the hardware finishes computing each fractal. The value description is as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): **unitFlag** is disabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): **unitFlag** is enabled, and the hardware does not reset the unit flag bit after executing the instruction.<br>&nbsp;&nbsp;&bull; 3 (2'b11): **unitFlag** is enabled, and the hardware resets the unit flag bit after executing the instruction.<br><br>When this feature is enabled, set the **unitFlag** value of both the Mmad instruction and the Fixpipe instruction to 2 or 3.<br><br>This parameter is supported only on the following models:<br><br>Ascend 950PR&950DT products<br><br>Atlas A2 products<br><br>Atlas A3 products. For the parameter configuration scheme and feature details, see [UnitFlag feature description](../mmad_compute_key_features/UnitFlag.md#ZH-CN_TOPIC_00000025690709788). |
| kDirectionAlign | The core function of K-direction alignment is to control, through the **kDirectionAlign** parameter, the alignment of L0A Buffer and L0B Buffer matrices in the K direction when the float data type is used.<br><br>The value description is as follows:<br>&nbsp;&nbsp;&bull; **false**: Default value. The K direction is aligned to `ceil(K / 8) * 8`.<br>&nbsp;&nbsp;&bull; **true**: The K direction is aligned to `ceil(K/16)*16`.<br><br>Atlas training products support only **false**.<br><br>Atlas inference products AI Core support only **false**.<br><br>Atlas A2 products support only **true**/**false**.<br><br>Atlas A3 products support only **true**/**false**.<br><br>Atlas 200I/500 A2 inference products support only **false**.<br><br>Ascend 950PR&950DT products support only **false**.<br><br>For feature details, see [kDirectionAlign feature description](../mmad_compute_key_features/k_direction_alignment_constraint.md#ZH-CN_TOPIC_0000002569070973). |
| fmOffset | Offset of the left matrix (one value for the entire left matrix). Supports Scalar (which should be consistent with src_fm.dtype) or an immediate value. The default value is 0.<br><br>Note: Not used. Passed in for compatibility with older product APIs. Atlas A2 products and later products do not process it. |
| enSsparse | Enables the structured sparsity feature. The default value is **false**.<br><br>Note: Not used. Passed in for compatibility with older product APIs. Atlas A2 products and later products do not process it. |
| enWinogradA | Indicates whether matrix a is generated by winograd_feature_map_transform() to support the winograd feature. Type: bool. The default value is **false**.<br><br>Note: Not used. Passed in for compatibility with older product APIs. Atlas A2 products and later products do not process it. |
| enWinogradB | Indicates whether matrix b is generated by winograd_weight_transform() to support the winograd feature. Type: bool. The default value is **false**.<br><br>Note: Not used. Passed in for compatibility with older product APIs. Atlas A2 products and later products do not process it. |

## Data Type<a id="zh_cn_topic_mmad_section5"></a>

<!-- npu="950" id23 -->
**Table 5** Precision type combinations supported by dst, fm, and filter (Ascend 950PR&950DT products)

| left matrix fm type | right matrix filter type | result matrix dst type |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| half | half | float |
| float | float | float |
| bfloat16_t | bfloat16_t | float |
| fp8_e4m3fn_t | fp8_e4m3fn_t | float |
| fp8_e4m3fn_t | fp8_e5m2_t | float |
| fp8_e5m2_t | fp8_e4m3fn_t | float |
| fp8_e5m2_t | fp8_e5m2_t | float |
| hifloat8_t | hifloat8_t | float |

**Table 6** Precision type combinations supported by dst, fm, filter, and bias (Ascend 950PR&950DT products)

| left matrix fm type | right matrix filter type | bias type | result matrix dst type |
| --- | --- | --- | --- |
| int8_t | int8_t | int32_t | int32_t |
| half | half | float | float |
| float | float | float | float |
| bfloat16_t | bfloat16_t | float | float |
| fp8_e4m3fn_t | fp8_e4m3fn_t | float | float |
| fp8_e4m3fn_t | fp8_e5m2_t | float | float |
| fp8_e5m2_t | fp8_e4m3fn_t | float | float |
| fp8_e5m2_t | fp8_e5m2_t | float | float |
| hifloat8_t | hifloat8_t | float | float |

<!-- end id23 -->

<!-- npu="A3,910b,310b" id24 -->
**Table 7** Precision type combinations supported by dst, fm, and filter (Atlas A2 products) (Atlas A3 products) (Atlas 200I/500 A2 inference products)

| left matrix fm type | right matrix filter type | result matrix dst type |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| half | half | float |
| float | float | float |
| bfloat16_t | bfloat16_t | float |
| int4b_t | int4b_t | int32_t |

**Table 8** Precision type combinations supported by dst, fm, filter, and bias (Atlas A2 products) (Atlas A3 products) (Atlas 200I/500 A2 inference product)

| left matrix fm type | right matrix filter type | bias type | result matrix dst type |
| --- | --- | --- | --- |
| int8_t | int8_t | int32_t | int32_t |
| half | half | float | float |
| float | float | float | float |
| bfloat16_t | bfloat16_t | float | float |

<!-- end id24 -->

<!-- npu="310p" id25 -->
**Table 9** Precision type combinations supported by dst, fm, and filter (Atlas inference products AI Core)

| left matrix fm type | right matrix filter type | result matrix dst type |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| uint8_t | int8_t | int32_t |
| uint8_t | uint8_t | int32_t |
| half | half | half<br> Description: <br>For this precision type combination, the precision cannot reach dual-0.1%, and later processor versions do not support this type conversion. It is recommended to directly use half input and float output.<br><br>Dual-0.1% means that the error between each actual data value and the true data value does not exceed 0.1%, and the total number of data values whose error exceeds 0.1% does not exceed 0.1% of the total number of data values. |
| half | half | float |
| int4b_t | int4b_t | int32_t |

<!-- end id25 -->

<!-- npu="910" id26 -->
**Table 10** Precision type combinations supported by dst, fm, and filter (Atlas training products)

| left matrix fm type | right matrix filter type | result matrix dst type |
| --- | --- | --- |
| uint8_t | uint8_t | uint32_t |
| int8_t | int8_t | int32_t |
| uint8_t | int8_t | int32_t |
| half | half | half<br> Description: <br>For this precision type group, the precision cannot reach dual-0.1% (2‰), and later processor versions do not support this type conversion. It is recommended to directly use half input and float output.<br><br>Dual-0.1% means that the error between each actual data value and the true data value does not exceed 0.1%, and the total number of data values whose error exceeds 0.1% does not exceed 0.1% of the total number of data values. |
| half | half | float |

<!-- end id26 -->

## Return Value

None

## Constraints

- The result matrix C can only reside in the L0C Buffer (CO1), the left matrix A can only reside in the L0A Buffer (A2), and the right matrix B can only reside in the L0B Buffer (B2).
- When any of M, K, and N is 0, the instruction is not executed, and the API is treated as a NOP (no operation).
- When M = 1, the GEMV (General Matrix-Vector Multiplication) feature is enabled by default. In this case, when the Mmad API reads data from the L0A Buffer, it reads the data in the ND format instead of treating it as the ZZ or NZ format. Therefore, the left matrix must be laid out directly in the ND format ([GEMV feature description](../mmad_compute_key_features/GEMV.md#ZH-CN_TOPIC_0000002538231187)). Note that when GEMV is enabled, the start address of matrix A is still required to be 512-byte aligned.<!-- npu="950" id29 -->For Ascend 950PR&950DT products, you can disable this feature by setting the **disableGemv** parameter of **MmadParams** to **true**.<!-- end id29 -->
- Generally, one Mmad computation completes at least one A(16\*16\*half) \* B(16\*16\*half) data block computation. However, in actual computation, the effective values of M, K, and N may not be multiples of 16, so some data is invalid. The following uses M = 30, K = 70, and N = 40 as an example to describe the layout of invalid data and valid data.

    The data is of the half type. When M = 30, K = 70, and N = 40, the L0A Buffer (A2) contains 2x5 16x16 matrices, the L0B Buffer (B2) contains 5x3 16x16 matrices, and the L0C Buffer (CO1) contains 2x3 16x16 matrices. In this scenario, M, K, and N are not multiples of 16. The matrix in the lower-right corner of A2 actually has only 14x6 valid data elements, but it still occupies the space of one 16x16 matrix, and the other invalid data is ignored during computation. The layout of invalid data and valid data in a 16x16 fractal data block is illustrated as follows:

    ![Valid data multiplication diagram](../../../../figures/mmad_validshape_demo.png)

- **Synchronization optimization description**

    When matrix computation accumulates along the K axis, the intermediate results usually need to be written to the same L0C Buffer. In theory, between two consecutive Mmad API calls, a PipeBarrier\(PIPE\_M\) should be inserted to ensure that the write operation of the previous Mmad is completed before the read operation of the next Mmad begins, thereby ensuring data consistency.

    However, when the matrix dimensions m and n exceed a certain threshold, the computation workload is large enough and the data access pattern has good locality and pipeline parallelism potential, so the hardware scheduler can automatically handle the write-read dependency. In this case, there is no need to insert a PipeBarrier\(PIPE\_M\), and the Mmad instructions can be executed in parallel, thereby significantly improving computation performance.

    The threshold formula is as follows:

    $$
    (m / 16) * (n / 16) < 10
    $$

    When the computation amount is less than the threshold, PipeBarrier\(PIPE\_M\) must be inserted.

    ```cpp
    if ((m / 16) * (n / 16) < 10) {
        AscendC::PipeBarrier<PIPE_M>();
    }
    ```

    When the computation amount is greater than the threshold, PipeBarrier\(PIPE\_M\) does not need to be inserted.

- **UnitFlag Feature Constraints**

    Both the **Mmad** and **Fixpipe** APIs provide the \`unitFlag\` parameter to control the enabling of this feature. Ensure that both are enabled synchronously for the feature to take effect.

    To keep the same L0C Buffer memory space continuously operated only by multiple **Mmad** or multiple **Fixpipe** instructions, set the **unitFlag** value of the first n-1 instructions to 2 to maintain the occupied state of the operated memory space, and set the last instruction to 3 to release the occupied state.

    After the \`unitFlag\` feature is enabled, it is recommended that the computation data amount of **Mmad** be consistent with the data amount moved out by **Fixpipe**. If **Mmad** computes a large block of data (M × N = 128 × 128) but **Fixpipe** moves out only part of it (M × N = 64 × 64), execution exceptions may occur. You can reset the L0C Buffer state through the SetFixPipeConfig\(\) API. For details, see the example in [UnitFlag Feature Description](../mmad_compute_key_features/UnitFlag.md#ZH-CN_TOPIC_00000025690709788).

- **Special Value/Boundary Value Constraints**

  For floating-point inf/nan input and output, you can configure the CTRL\[48\] bit of the CTRL register (control register) to control the saturation mode during floating-point computation:

  - Non-saturation mode: Set CTRL\[48\] to 1'b1, and inf/nan are output as-is.
  - Saturation mode: Set CTRL\[48\] to 1'b0, and inf output is saturated to ±MAX, while nan output is saturated to 0.

    ```cpp
    // Set CTRL[48] to 0 to enable the saturation mode for floating-point computation.
    AscendC::AscendCUtils::SetOverflow(0);
    ```

    Note: NaN input should be avoided for the **Mmad** instruction; otherwise, an execution error may occur.

  For integer types, only the saturation mode is available.

## Example

For a matrix multiplication sample, see [Mmad sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad).
