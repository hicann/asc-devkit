# Where

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:06:59.648Z pushedAt=2026-10-29T02:15:03.034Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Where_interface/Where_res.md#id1 -->

## Description

Selects elements from two source operands based on the specified condition to generate the destination operand. Both source operands can be a LocalTensor or a scalar.

![](../../../../figures/zh-cn_formulaimage_0000002425808613.png)

## Prototype

```
template <typename T, typename U, typename S, typename V>
__aicore__ inline void Where(const LocalTensor<T>& dst, const U& src0, const S& src1, const LocalTensor<V>& condition, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the destination operand. Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |
| U | LocalTensor type or scalar type. The type is automatically deduced from the input parameter src0. You do not need to configure this parameter, but ensure that the data type of src0 is consistent with that of the destination operand. |
| S | LocalTensor type or scalar type. The type is automatically deduced from the input parameter src1. You do not need to configure this parameter, but ensure that the data type of src1 is consistent with that of the destination operand. |
| V | Data type of the condition. Currently supported data type: bool. |

**Table 2**  Parameter description

| Parameter Name | Type | Description |
| --- | --- | --- |
| dst | Output | Target operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands. Type: scalar or LocalTensor. When the type is LocalTensor, the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type must be consistent with that of the destination operand. |
| condition | Input | Condition operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlapping between the source operands and the destination operand is not supported.**
-   For the operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

```
AscendC::LocalTensor<half> dst, src0, src1;
AscendC::LocalTensor<bool> condition;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::Where(dst, src0, src1, condition, count);
```

The result is as follows:

```
Input data (src0):
[1, 2, 3, ... 511, 512]
Input data (src1):
[-1, -2, -3, ... -511, -512]
Condition input data (condition). When it is 0, src1 is selected; when it is 1, src0 is selected:
[0, 1, 0, ... 0, 1]
Output data (dst):
[-1, 2, -3, ... -511, 512]
```
