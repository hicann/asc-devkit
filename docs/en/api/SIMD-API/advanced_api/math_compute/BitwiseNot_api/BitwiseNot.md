# BitwiseNot

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T13:18:45.674Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs bitwise inversion on the input.

![](../../../../figures/zh-cn_formulaimage_0000002477048097.png)

## Prototype

```
template <const BitwiseNotConfig& config = DEFAULT_BITWISE_NOT_CONFIG, typename T>
__aicore__ inline void BitwiseNot(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration related to the **BitwiseNot** algorithm. This parameter is optional and of the **BitwiseNotConfig** type. For details, see the code below. The meaning of the parameter is as follows:<br><br>**isReuseSource**: This parameter is reserved. Pass the default value **false**. |
| T | Data type of the source operand and destination operand. Supported data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t. |

```
struct BitwiseNotConfig {
    bool isReuseSource;
};
```

**Table 2**  Parameter description

| Parameter | Type | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type must be consistent with that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

```
AscendC::LocalTensor<uint32_t> dst, src;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::BitwiseNot(dst, src, count);
```

The results are as follows:

```
Input data (src):
[ 91, 48, -71, 112, 64, -107, 74, -121, -103, -50, 112, -23, 15, ... -74]
Output data (dst):
[-92, -49, 70, -113, -65, 106, -75, 120, 102, 49, -113, 22, -16, ... 73]
```
