# BitwiseNot

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:04:55.471Z pushedAt=2026-11-18T08:54:29.239Z nl=2 -->

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
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/BitwiseNot_interface/BitwiseNot_res.md#id1 -->

## Description

Performs a bitwise NOT operation on the input.

![](../../../../figures/formulaimage_0000002477048097.png)

## Prototype

```
template <const BitwiseNotConfig& config = DEFAULT_BITWISE_NOT_CONFIG, typename T>
__aicore__ inline void BitwiseNot(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration of the BitwiseNot algorithm. This parameter is optional and is of the BitwiseNotConfig type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>isReuseSource: This parameter is reserved. Pass the default value false. |
| T | Data type of the source operands and the destination operand. Supported data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t. |

```
struct BitwiseNotConfig {
    bool isReuseSource;
};
```

**Table 2**  Parameter description

| Parameter | Type | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operands.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type must be consistent with that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For the operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

```
AscendC::LocalTensor<uint32_t> dst, src;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::BitwiseNot(dst, src, count);
```

The result is as follows:

```
Input data (src):
[ 91, 48, -71, 112, 64, -107, 74, -121, -103, -50, 112, -23, 15, ... -74]
Output data (dst):
[-92, -49, 70, -113, -65, 106, -75, 120, 102, 49, -113, 22, -16, ... 73]
```
