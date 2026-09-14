# LogicalNot

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T16:55:30.324Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs a logical NOT operation element-wise. When the input data type is not bool, zero is treated as False and non-zero data is treated as True.

![](../../../../figures/zh-cn_formulaimage_0000002419746457.png)

## Prototype

```
template <const LogicalNotConfig& config = DEFAULT_LOGICAL_NOT_CONFIG, typename T, typename U>
__aicore__ inline void LogicalNot(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Related configuration of the LogicalNot algorithm. This parameter is optional and of the **LogicalNotConfig** type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>**isReuseSource**: This parameter is reserved. Pass the default value **false**. |
| T | Data type of the destination operand. The supported data type is bool. |
| U | Data type of the source operand. The supported data types are bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. |

```
struct LogicalNotConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
AscendC::LocalTensor<bool> dst;
AscendC::LocalTensor<half> src;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::LogicalNot(dst, src, count);
```

The results are as follows:

```
Input data (src):
[1, 2, 0, -1, -2, 0, 3, 4, 0, -3, -4, 0, 5, 6, 0, -5, -6, 0, ... 0]
Output data (dst):
[False, False, True, False, False, True, False, False, True, False,
 False, True, False, False, True, False, False, True, ... True]
```
