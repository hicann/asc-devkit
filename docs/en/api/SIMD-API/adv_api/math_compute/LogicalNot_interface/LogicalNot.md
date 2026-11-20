# LogicalNot

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:14:41.566Z pushedAt=2026-11-18T08:54:29.554Z nl=2 -->

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
- AI Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/LogicalNot_interface/LogicalNot_res.md#id1 -->

## Description

Performs element-wise logical negation. When the input data type is not bool, zero is treated as False and non-zero data is treated as True.

![](../../../../figures/formulaimage_0000002419746457.png)

## Prototype

```
template <const LogicalNotConfig& config = DEFAULT_LOGICAL_NOT_CONFIG, typename T, typename U>
__aicore__ inline void LogicalNot(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration related to the LogicalNot algorithm. This parameter is optional and is of the LogicalNotConfig type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>isReuseSource: This parameter is reserved. Pass the default value false. |
| T | Data type of the destination operand. Supported data type: bool. |
| U | Data type of the source operands. Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |

```
struct LogicalNotConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
AscendC::LocalTensor<bool> dst;
AscendC::LocalTensor<half> src;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::LogicalNot(dst, src, count);
```

The result is as follows:

```
Input data (src):
[1, 2, 0, -1, -2, 0, 3, 4, 0, -3, -4, 0, 5, 6, 0, -5, -6, 0, ... 0]
Output data (dst):
[False, False, True, False, False, True, False, False, True, False,
 False, True, False, False, True, False, False, True, ... True]
```
