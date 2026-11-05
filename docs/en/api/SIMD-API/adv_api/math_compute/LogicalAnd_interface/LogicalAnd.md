# LogicalAnd

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:23:07.854Z pushedAt=2026-10-29T02:15:02.836Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/LogicalAnd_interface/LogicalAnd_res.md#id1 -->

## Description

Performs a logical AND operation on the two source operands element by element. When the input data type is not bool, zero is treated as False and non-zero data is treated as True.

![](../../../../figures/zh-cn_formulaimage_0000002386250116.png)

## Prototype

```
template <const LogicalAndConfig& config = DEFAULT_LOGICAL_AND_CONFIG, typename T, typename U>
__aicore__ inline void LogicalAnd(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration of the LogicalAnd algorithm. This parameter is optional and of the LogicalAndConfig type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>isReuseSource: This parameter is reserved. Pass the default value false. |
| T | Data type of the destination operand. Supported data type: bool. |
| U | Data type of the source operand. Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |

```
struct LogicalAndConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operands must be the same as that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
AscendC::LocalTensor<bool> dst;
AscendC::LocalTensor<half> src0, src1;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::LogicalAnd(dst, src0, src1, count);
```

The result example is as follows:

```
Input data (src0):
[1, 2, 0, -1, -2, 0, 3, 4, 0, -3, -4, 0, 5, 6, 0, -5, -6, 0, ... 0]
Input data (src1):
[1, 0, 2, -1, 0, -2, 3, 0, 4, -3, 0, -4, 5, 0, 6, -5, 0, -6, ... 6]
Output data (dst):
[True, False, False, True, False, False, True, False, False, True,
 False, False, True, False, False, True, False, False, ... False]
```
