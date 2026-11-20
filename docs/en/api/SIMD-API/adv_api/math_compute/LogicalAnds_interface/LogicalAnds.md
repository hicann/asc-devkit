# LogicalAnds

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:14:25.911Z pushedAt=2026-11-18T08:54:29.547Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/LogicalAnds_interface/LogicalAnds_res.md#id1 -->

## Description

Each element in the input vector is ANDed with a scalar. When the data type of the input vector or the scalar is not bool, zero is treated as False and non-zero data is treated as True. The API supports both orders of the vector and the scalar, that is, the scalar before the vector and the scalar after the vector. The scalar can be configured as a single-point element of a LocalTensor. The computation formula is as follows, where idx indicates the position coefficient of the single-point element of the LocalTensor:

![](../../../../figures/formulaimage_0000002462754813.png)

## Prototype

```
template <const LogicalAndsConfig& config = DEFAULT_LOGICAL_ANDS_CONFIG, typename T, typename U, typename S>
__aicore__ inline void LogicalAnds(const LocalTensor<T>& dst, const U& src0, const S& src1, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration related to the LogicalAnds algorithm. This parameter is optional and of the LogicalAndsConfig type, defined in the following code, where the parameter meanings are as follows:<br><br>isReuseSource: This parameter is reserved. Pass the default value false.<br><br>scalarTensorIndex: When the scalar is a single-point element of LocalTensor, this parameter specifies whether the scalar serves as the left operand or the right operand in the AND operation. 0 indicates the left operand, and 1 (default) indicates the right operand. |
| T | Data type of the destination operand. Supported data type: bool. |
| U | LocalTensor type or scalar type. The type is automatically deduced from the input parameter src0. You do not need to configure this parameter; just ensure that src0 meets the data type constraints.<br><br>Supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |
| S | LocalTensor type or scalar type. The type is automatically deduced from the input parameter src1. You do not need to configure this parameter; just ensure that src1 meets the data type constraints.<br><br>Supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |

```
struct LogicalAndsConfig {
    bool isReuseSource;
    int8_t scalarTensorIndex;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0,src1 | Input | Source operands. The type is scalar or LocalTensor. When the type is LocalTensor, the operand can be a vector operand or a single-point scalar element, and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   For this API, at least one of the left operand and the right operand of the AND operation must be a vector. Currently, it is not supported for both the left and right operands to be scalars simultaneously.
-   When a single-point element of a LocalTensor is passed as a scalar, the scalarTensorIndex parameter must be a compile-time constant. If a variable is passed, the variable must be declared as constexpr.

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
AscendC::LocalTensor<bool> dst;
AscendC::LocalTensor<half> src0, src1;
uint32_t count = 512; // Number of elements involved in the computation.
// Scalar at the end.
AscendC::LogicalAnds(dst, src0, src1, count);
// Scalar at the beginning.
static constexpr AscendC::LogicalAndsConfig config = {false, 0};
AscendC::LogicalAnds<config>(dst, src0, src1, count);
```

The result is as follows:

```
// Scalar at the end.
// Input data (src0):
[1, 2, 0, -1, -2, 0, 3, 4, 0, -3, -4, 0, 5, 6, 0, -5, -6, 0, ... 0]
// Input data (src1):
[3.0]
// Output data (dst):
[ True,  True,  False,  True,  True,  False,  True,  True,  False,  True,
  True,  False,  True,  True,  False,  True,  True,  False,  ...  False]
// Scalar at the beginning.
Input data (src1):
[1, 2, 0, -1, -2, 0, 3, 4, 0, -3, -4, 0, 5, 6, 0, -5, -6, 0, ... 0]
Input data (src0):
[3.0]
Output data (dst):
[ True,  True,  False,  True,  True,  False,  True,  True,  False,  True,
  True,  False,  True,  True,  False,  True,  True,  False,  ...  False]
```
