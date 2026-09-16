# LoadData (GMToL1-2D Cube Transfer)<a id="ZH-CN_TOPIC_0000002538231138"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:20:35.138Z -->

## Applicable Products<a id="zh-cn_topic_0000002567745223_section796754519912"></a>

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
## Description<a id="zh-cn_topic_0000002567745223_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

It moves the 2D-format data required for ordinary cube computation from Global Memory to the L1 Buffer (with TPosition being A1/B1), in units of 512-byte data fractals.

## Prototype<a id="zh-cn_topic_0000002567745223_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

## Parameters<a id="zh-cn_topic_0000002567745223_section16128134420472"></a>

**Table 1** General parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type **LocalTensor**.<br>For the fractal constraint, see [Cube Computation Input Transfer Constraints](matrix_computation_input_movement_constraint.md).<br>For the start address alignment constraint, see [Alignment Constraint](matrix_computation_input_movement_constraint.md) in Cube Computation Input Transfer Constraints.<br>The data type is consistent with that of **src**.<br>The supported physical storage location is L1 Buffer (**TPosition** is **A1/B1**). |
| src | Input | Source operand, of type **GlobalTensor**.<br>For the fractal constraint, see [Cube Computation Input Transfer Constraints](matrix_computation_input_movement_constraint.md).<br>For the start address alignment constraint, see [Alignment Constraint](matrix_computation_input_movement_constraint.md) in Cube Computation Input Transfer Constraints.<br>The data type is consistent with that of **dst**.<br>The supported physical storage location is Global Memory (**TPosition** is **GM**). |
| loadDataParams | Input | LoadData parameter structure, of type **LoadData2DParams**. For details, see [Table 2](#zh-cn_topic_0000002567745223_table8955841508). |

**Table 2** Parameter description of the LoadData2DParams structure<a id="zh-cn_topic_0000002567745223_table8955841508"></a>

| Parameter | Description |
| ---------- | ---------- |
| startIndex | Fractal cube ID, indicating that the transfer start position is the Nth fractal in the source operand (0 indicates the first fractal cube in the source operand). Value range: startIndex∈[0, 65535]. Unit: 512 bytes. Default value: 0. |
| repeatTimes | Number of iterations. Each iteration processes 512 bytes of data. Value range: repeatTimes∈[0, 255].<br>**Note: repeatTimes = 0 indicates that no transfer is performed, and this API is treated as a NOP (no operation).** |
| srcStride | Interval between the **Start Address** of the previous fractal and that of the next fractal in the source operand between adjacent iterations. Unit: 512 bytes. Value range: srcStride∈[0, 65535]. Default value: 0.<br>**Note: srcStride = 0 indicates that the same fractal cube is fetched repeatedly between consecutive repeated execution cycles.** |
| sid | This parameter does not need to be concerned by users. Set it to 0. |
| dstGap | Interval between the **End Address** of the previous fractal and the **Start Address** of the next fractal in the destination operand between adjacent iterations. Unit: 512 bytes. Value range: dstGap∈[0, 65535]. Default value: 0.<br>**Note: dstGap = 0 indicates that the start addresses of the destination operands of adjacent repeats are separated by one data fractal, that is, stored contiguously.** |
| ifTranspose | Whether to enable the transpose function, which transposes each fractal cube. Default value: false:<br>&nbsp;&nbsp;&bull; true: Enabled.<br>&nbsp;&nbsp;&bull; false: Disabled.<br>**Note: Transpose is not supported in this path scenario, and this parameter is meaningless. Keep the default value.** |
| addrMode | Controls whether the fractal cube index ID of each iteration in the source operand increases or decreases in multi-iteration scenarios. 0 indicates increasing, where the next repeat index = startIndex + srcStride \* repeatTimes. 1 indicates decreasing, where the next repeat index = startIndex - srcStride \* repeatTimes. |

## Data Type<a id="zh-cn_topic_0000002567745223_section4219135304818"></a>

<!-- npu="950" id10 -->
For Ascend 950PR/Ascend 950DT, the supported data types are: uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, float.
<!-- end id10 -->

<!-- npu="A3" id11 -->
For Atlas A3 training products/Atlas A3 inference products, the supported data types are: b8, b16, b32.
<!-- end id11 -->

<!-- npu="910b" id12 -->
For Atlas A2 training products/Atlas A2 inference products, the supported data types are: b8, b16, b32.
<!-- end id12 -->

<!-- npu="310b" id13 -->
For Atlas 200I/500 A2 inference products, the supported data types are: uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, float.
<!-- end id13 -->

<!-- npu="310p" id14 -->
For Atlas inference products AI Core, the supported data types are: uint8_t, int8_t, uint16_t, int16_t, half.
<!-- end id14 -->

<!-- npu="910" id15 -->
For Atlas training products, the supported data types are: uint8_t, int8_t, uint16_t, int16_t, half.
<!-- end id15 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002567745223_section2045914466492"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- Transposition is not supported during the transfer on this path.
- The destination address must be 32-byte aligned. The source address must be 1-byte aligned. The pipeline occupied during instruction execution is **PIPE_MTE2**.
- When **srcStride** is 0, it indicates that the same block of data fractal in the source operand is read between consecutive repeats.
<!-- npu="910" id18 -->
- The **dstGap** parameter is not supported for Atlas training products.
<!-- end id18 -->

## Example<a id="zh-cn_topic_0000002567745223_section088124295117"></a>

In the following example, with the data type being half, the Load2D instruction is called to complete the Nz2Nz data transfer from GM to the L1 buffer.

The following figure shows the data layout change during the transfer:

![](../../../../figures/load2d_gm2l1_demo.png)

The sample code snippet is as follows:

```cpp
// m=32, k=32, fractalShape[0] = 16,fractalShape[1] = 16, fractalSize = 256;
uint32_t dstOffset = fractalSize;
uint32_t srcOffset = fractalSize;
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(m, fractalShape[0]);
loadDataParams.dstGap = CeilDivision(m, fractalShape[0]) - 1;
for (uint16_t i = 0; i < CeilDivision(m, fractalShape[0]); ++i) {
    AscendC::LoadData(a1Local[i * dstOffset], aGM[i * srcOffset], loadDataParams);
}
```
