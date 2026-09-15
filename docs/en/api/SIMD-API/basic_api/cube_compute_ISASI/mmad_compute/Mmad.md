# Mmad<a name="ZH-CN_TOPIC_00000025382311899"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-27T16:51:42.516Z -->

## Applicable Products

### Prototype without bias

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->
### Prototype with bias passed in

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference product: Supported
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

**Mmad** is the core cube multiply-accumulate computation API of Ascend C for Ascend AI processors. It is designed for high-performance operator development and encapsulates the cube multiply-accumulate computation capability of Ascend NPU hardware. It is widely used in the development of neural network layers (such as fully connected layers and convolutional layers) and numerical computation operators.

**Mmad** implements the cube multiplication computation capability of Ascend NPU. Its mathematical expression is:

$$
C = A \times B + C
$$

<!-- npu="950" id19 -->
For Ascend 950PR/Ascend 950DT:

**Table 1** Description of matrices A, B, and C in cube computation

| Cube Computation Logic | Physical Location of Cube Computation | Dimension | Input/Output Data Format | Data Type |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M x K | Nz | <a href="#zh_cn_topic_mmad_section5">Data Type</a> |
| B | L0B Buffer | K x N | Zn | <a href="#zh_cn_topic_mmad_section5">Data Type</a> |
| C | L0C Buffer | M x N, which can be initialized with a bias cube of dimension 1 x N | Nz | <a href="#zh_cn_topic_mmad_section5">Data Type</a> |

**Figure 1** Fractal diagram of cube multiplication for the Mmad API (Ascend 950PR/Ascend 950DT)<a id="zh_cn_topic_mmad_section2_figure1"></a>

![Fractal diagram of Mmad API computation](../../../../figures/mmad_demo_a5.png)
<!-- end id19 -->

<!-- npu="A3,910b" id20 -->
For the following product models, the cube descriptions are shown in the table below:
<!-- npu="A3" id21 -->
Atlas A3 training products/Atlas A3 inference products
<!-- end id21 -->
<!-- npu="910b" id22 -->
Atlas A2 training products/Atlas A2 inference products
<!-- end id22 -->
**Table 2** Cube computation matrices A, B, and C descriptions

  <table>
    <thead>
      <tr>
        <th>cube computation logic</th>
        <th>cube computation physical location</th>
        <th>dimension</th>
        <th>input/output data format</th>
        <th>data type</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>A</td>
        <td>L0A Buffer</td>
        <td>M x K</td>
        <td>Zz</td>
        <td rowspan="3"><a href="#zh_cn_topic_mmad_section5">data type</a></td>
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
        <td>M x N, which can be initialized using the bias cube Bias, with a dimension of 1 x N.</td>
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

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, result cube C, of type LocalTensor. The supported physical storage location is L0C Buffer (TPosition:CO1).<br>The start address of LocalTensor must be 1024-byte aligned. |
| fm | Input | Source operand, left cube A, of type LocalTensor. The supported physical storage location is L0A Buffer (TPosition: A2).<br>The start address of LocalTensor must be 512-byte aligned. |
| filter | Input | Source operand, right cube B, of type LocalTensor. The supported physical storage location is L0B Buffer (TPosition: B2).<br>The start address of LocalTensor must be 512-byte aligned. |
| bias | Input | Source operand, Bias cube, of type LocalTensor. The supported physical storage location is BT Buffer (TPosition:C2).<br>The start address of LocalTensor must be 64-byte aligned. |
| mmadParams | Input | Cube multiplication related parameters.<br>For the specific definition of this parameter type, see \$\{INSTALL_DIR\}/include/ascendc/basic_api/API/kernel_struct_mm.h. Replace \$\{INSTALL_DIR\} with the file storage path after CANN software installation.<br>For the description of MmadParams parameters, see [Table 4](#zh_cn_topic_mmad_section4_table4). |

**Table 4** Parameter description of the MmadParams structure<a id="zh_cn_topic_mmad_section4_table4"></a>

| Parameter | Description |
| --- | --- |
| m | Height of the left cube. Value range: m∈[0, 4095]. The default value is 0. |
| n | Width of the right cube. Value range: n∈[0, 4095]. The default value is 0. |
| k | Width of the left cube and height of the right cube. Value range: k∈[0, 4095]. The default value is 0. |
| ccubeInitVal | Whether to enable the default zero-initialization of cube C. The default value is true.<br>&nbsp;&nbsp;&bull; true: Cube C is initialized to 0 by default.<br>&nbsp;&nbsp;&bull; false: Cube C is not initialized by default; it is initialized by setting the ccubeSource parameter. |
| ccubeSource | Configures whether the initial value of cube C comes from BT Buffer. The default value is false.<br>&nbsp;&nbsp;&bull; false: L0C Buffer is not initialized.<br>&nbsp;&nbsp;&bull; true: L0C Buffer is initialized with data from BT Buffer (TPosition:C2).<br><br>Atlas training products support only false.<br><br>Atlas inference products AI Core support only false.<br><br>Atlas A2 training products/Atlas A2 inference products support true/false.<br><br>Atlas A3 training products/Atlas A3 inference products support true/false.<br><br>Atlas 200I/500 A2 inference products support true/false.<br><br>Ascend 950PR/Ascend 950DT support true/false.<br><br>Note: For APIs with Bias input, this parameter is invalid. Whether the initial value of cube C comes from BT Buffer is determined by the position of the bias input. |
| isBias | This parameter is deprecated. Do not use it in new development. If you need to accumulate the initial cube, use the API with Bias. Alternatively, configure the source of the initial value of cube C through the ccubeInitVal and ccubeSource parameters. Using the API with Bias is recommended, as it is simpler and more convenient than configuring the ccubeInitVal and ccubeSource parameters.<br><br>Configures whether to accumulate the initial cube. The default value is false. The value description is as follows:<br>&nbsp;&nbsp;&bull; false: Cube multiplication without accumulating the initial cube, C = A \* B.<br>&nbsp;&nbsp;&bull; true: Cube multiply-accumulate, accumulating the initial cube, C += A \* B. |
| disableGemv | When M=1, this parameter configures whether to enable [GEMV](../mmad_compute_key_features/GEMV.md#ZH-CN_TOPIC_0000002538231187) mode for Mmad computation.<br>&nbsp;&nbsp;&bull; false: Enables GEMV mode.<br>&nbsp;&nbsp;&bull; true: Disables GEMV mode.<br><br>This parameter is supported only on the following models:<br><br>Ascend 950PR/Ascend 950DT |
| unitFlag | unitFlag controls fine-grained parallelism between Mmad and Fixpipe instructions. When this feature is enabled, the computation result is moved out after the hardware finishes computing each fractal. The value description is as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): unitFlag is disabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): unitFlag is enabled, and the unit flag bit is not reset after the hardware finishes executing the instruction.<br>&nbsp;&nbsp;&bull; 3 (2'b11): unitFlag is enabled, and the unit flag bit is reset after the hardware finishes executing the instruction.<br><br>When this feature is enabled, set the unitFlag value of both Mmad and Fixpipe instructions to 2 or 3.<br><br>This parameter is supported only on the following models:<br><br>Ascend 950PR/Ascend 950DT<br><br>Atlas A2 training products/Atlas A2 inference products<br><br>Atlas A3 training products/Atlas A3 inference products. For parameter configuration and feature details, see [UnitFlag feature description](../mmad_compute_key_features/UnitFlag.md#ZH-CN_TOPIC_00000025690709788). |
| kDirectionAlign | The core function of K-direction alignment is to control, through the kDirectionAlign parameter, the alignment of L0A Buffer and L0B Buffer matrices in the K direction when the float data type is used.<br><br>The value description is as follows:<br>&nbsp;&nbsp;&bull; false: Default value. The K direction is aligned to `ceil(K / 8) * 8`.<br>&nbsp;&nbsp;&bull; true: The K direction is aligned to `ceil(K/16)*16`.<br><br>Atlas training products support only false.<br><br>Atlas inference products AI Core support only false.<br><br>Atlas A2 training products/Atlas A2 inference products support only true/false.<br><br>Atlas A3 training products/Atlas A3 inference products support only true/false.<br><br>Atlas 200I/500 A2 inference products support only false.<br><br>Ascend 950PR/Ascend 950DT support only false.<br><br>For feature details, see [kDirectionAlign feature description](../mmad_compute_key_features/k_direction_alignment_constraint.md#ZH-CN_TOPIC_0000002569070973). |
| fmOffset | Offset of the left cube (one value for the entire left cube). Supports Scalar (which should be consistent with src_fm.dtype)/immediate value. The default value is 0.<br><br>Note: Not used. Passed in for compatibility with previous product APIs. Atlas A2 training products/Atlas A2 inference products and later products do not process it. |
| enSsparse | Enables the structured sparsity feature. The default value is false.<br><br>Note: Not used. Passed in for compatibility with previous product APIs. Atlas A2 training products/Atlas A2 inference products and later products do not process it. |
| enWinogradA | Whether cube a is generated by winograd_feature_map_transform(). Used to support the winograd feature. bool type. The default value is false.<br><br>Note: Not used. Passed in for compatibility with previous product APIs. Atlas A2 training products/Atlas A2 inference products and later products do not process it. |
| enWinogradB | Whether cube b is generated by winograd_weight_transform(). Used to support the winograd feature. bool type. The default value is false.<br><br>Note: Not used. Passed in for compatibility with previous product APIs. Atlas A2 training products/Atlas A2 inference products and later products do not process it. |

## Data Type<a id="zh_cn_topic_mmad_section5"></a>

<!-- npu="950" id23 -->
**Table 5** Supported precision type combinations for dst, fm, and filter (Ascend 950PR/Ascend 950DT)

| left cube fm type | right cube filter type | result cube dst type |
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

**Table 6** Supported precision type combinations for dst, fm, filter, and bias (Ascend 950PR/Ascend 950DT)

| left cube fm type | right cube filter type | bias type | result cube dst type |
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
**Table 7** Supported precision type combinations for dst, fm, and filter (Atlas A2 training products/Atlas A2 inference products) (Atlas A3 training products/Atlas A3 inference products) (Atlas 200I/500 A2 inference product)

| left cube fm type | right cube filter type | result cube dst type |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| half | half | float |
| float | float | float |
| bfloat16_t | bfloat16_t | float |
| int4b_t | int4b_t | int32_t |

**Table 8** Supported precision type combinations for dst, fm, filter, and bias (Atlas A2 training products/Atlas A2 inference products) (Atlas A3 training products/Atlas A3 inference products) (Atlas 200I/500 A2 inference product)

| left cube fm type | right cube filter type | bias type | result cube dst type |
| --- | --- | --- | --- |
| int8_t | int8_t | int32_t | int32_t |
| half | half | float | float |
| float | float | float | float |
| bfloat16_t | bfloat16_t | float | float |
<!-- end id24 -->

<!-- npu="310p" id25 -->
**Table 9** Supported precision type combinations for dst, fm, and filter (Atlas inference products AI Core)

| Left cube fm type | Right cube filter type | Result cube dst type |
| --- | --- | --- |
| int8_t | int8_t | int32_t |
| uint8_t | int8_t | int32_t |
| uint8_t | uint8_t | int32_t |
| half | half | half<br> Note: <br>For this precision type combination, the precision cannot reach two thousandths, and subsequent processor versions do not support this type conversion. It is recommended to directly use half input and float output.<br><br>Two thousandths means that the error between each actual data value and the true data value does not exceed one thousandth, and the total number of data values whose error exceeds one thousandth does not exceed one thousandth of the total number of data values. |
| half | half | float |
| int4b_t | int4b_t | int32_t |
<!-- end id25 -->

<!-- npu="910" id26 -->
**Table 10** Precision type combinations supported by dst, fm, and filter (Atlas training products)

| Left cube fm type | Right cube filter type | Result cube dst type |
| --- | --- | --- |
| uint8_t | uint8_t | uint32_t |
| int8_t | int8_t | int32_t |
| uint8_t | int8_t | int32_t |
| half | half | half<br> Note: <br>For this precision type combination, the precision cannot reach two thousandths, and subsequent processor versions do not support this type conversion. It is recommended to directly use half input and float output.<br><br>Two thousandths means that the error between each actual data value and the true data value does not exceed one thousandth, and the total number of data values whose error exceeds one thousandth does not exceed one thousandth of the total number of data values. |
| half | half | float |
<!-- end id26 -->

## Return Value

None

## Constraints

- The result cube C can only be located in CO1 (storage location: L0C Buffer), the left cube A can only be located in A2 (storage location: L0A Buffer), and the right cube B can only be located in B2 (storage location: L0B Buffer).
- When any of M, K, and N is 0, the instruction is not executed, and this API is treated as a NOP (no operation).
- When M = 1, the GEMV (General Cube-Vector Multiplication) feature is enabled by default. In this case, when the Mmad API reads data from the L0A Buffer, it reads the data in ND format instead of treating it as ZZ or NZ format. Therefore, the left cube must be laid out directly in ND format ([GEMV feature description](../mmad_compute_key_features/GEMV.md#ZH-CN_TOPIC_0000002538231187)). Note that when GEMV is enabled, the start address of cube A must still be 512-byte aligned. <!-- npu="950" id29 -->For Ascend 950PR/Ascend 950DT, this feature can be disabled by setting the **disableGemv** parameter of **MmadParams** to **true**. <!-- end id29 -->
- Generally, one Mmad computation completes at least one A(16\*16\*half) \* B(16\*16\*half) data block computation. However, in actual computation, the valid values of M, K, and N may not be multiples of 16, so some data is invalid. The layout of invalid data and valid data is described below using M=30, K=70, and N=40 as an example.

    When the data is of the half type and M=30, K=70, and N=40, A2 (L0A Buffer) contains 2x5 16x16 matrices, B2 (L0B Buffer) contains 5x3 16x16 matrices, and CO1 (L0C Buffer) contains 2x3 16x16 matrices. In this scenario, M, K, and N are not multiples of 16. The cube at the bottom-right corner of A2 actually has only 14x6 valid data elements, but it still occupies the space of one 16x16 cube. Other invalid data is ignored during computation. The layout of invalid data and valid data in a 16x16 fractal data block is illustrated as follows:

    ![Valid data multiplication diagram](../../../../figures/mmad_validshape_demo.png)

- **Synchronization optimization description**

    When cube computation accumulates along the K axis, intermediate results usually need to be written to the same L0C Buffer. In theory, a PipeBarrier\(PIPE\_M\) should be inserted between two consecutive Mmad API calls to ensure that the write operation of the previous Mmad is completed before the read operation of the next Mmad begins, thereby ensuring data consistency.

    However, when the cube dimensions m and n exceed a certain threshold, the computation workload is large enough that the data access pattern has good locality and pipeline parallelism potential, and the hardware scheduler can automatically handle the write-read dependency. In this case, there is no need to insert PipeBarrier\(PIPE\_M\), and the Mmad instructions can be executed in parallel, thereby significantly improving computation performance.

    Threshold formula:

    $$
    (m / 16) * (n / 16) < 10
    $$

    When the computation amount is less than the threshold, **PipeBarrier(PIPE_M)** must be inserted.

    ```cpp
    if ((m / 16) * (n / 16) < 10) {
        AscendC::PipeBarrier<PIPE_M>();
    }
    ```

    When the computation amount is greater than the threshold, **PipeBarrier(PIPE_M)** does not need to be inserted.

- **UnitFlag feature constraints**

    Both the Mmad and Fixpipe APIs provide the `unitFlag` parameter to control whether this feature is enabled. The two must be enabled synchronously for the feature to take effect.

    To keep the same L0C Buffer memory space continuously operated only by multiple Mmad or multiple Fixpipe instructions, set the `unitFlag` value of the first n-1 instructions to 2 to keep the operated memory space continuously occupied, and set the last instruction to 3 to release the occupied state.

    After the `unitFlag` feature is enabled, it is recommended that the amount of data computed by Mmad be consistent with the amount of data moved out by Fixpipe. If Mmad computes a large block of data (M × N = 128 × 128) but Fixpipe moves out only part of it (M × N = 64 × 64), execution exceptions may occur. You can reset the L0C Buffer state by calling the SetFixPipeConfig() API. For details, see the example in [UnitFlag feature description](../mmad_compute_key_features/UnitFlag.md#ZH-CN_TOPIC_00000025690709788).

- **Special value/boundary value constraints**

   For floating-point INF/NaN input and output, the CTRL[48] bit of the CTRL register (control register) can be set to control the saturation mode during floating-point computation:

  - Non-saturation mode: Set CTRL\[48\] to 1'b1, and INF/NaN are output as-is.
  - Saturation mode: Set CTRL\[48\] to 1'b0, and INF outputs are saturated to ±MAX while NaN outputs are saturated to 0.

    ```cpp
    // Set CTRL[48] to 0 to enable the floating-point computation saturation mode.
    AscendC::AscendCUtils::SetOverflow(0);
    ```

    Note: Avoid NaN inputs for the Mmad instruction; otherwise, an execution error may occur.

  For integer types, only the saturation mode is available.

## Example

For a cube multiplication sample, see [Mmad sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_cube_compute/mmad).
