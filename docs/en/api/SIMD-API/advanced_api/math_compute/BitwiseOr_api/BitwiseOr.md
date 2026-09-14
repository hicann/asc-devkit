# BitwiseOr

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T13:25:57.663Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs a bitwise OR operation on the two inputs.

![](../../../../figures/zh-cn_formulaimage_0000002442050326.png)

## Prototype

```
template <const BitwiseOrConfig& config = DEFAULT_BITWISE_OR_CONFIG, typename T>
__aicore__ inline void BitwiseOr(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Related configuration of the BitwiseOr algorithm. This parameter is optional and of the BitwiseOrConfig type. Its definition is shown in the following code, where the parameter meanings are as follows:<br><br>**isReuseSource**: This parameter is reserved. Pass the default value **false**. |
| T | Data type of the source operand and destination operand. Supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t. |

```
struct BitwiseOrConfig {
    bool isReuseSource;
};
```

**Table 2**  Parameter description

| Parameter | Type | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type must be consistent with that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

```
AscendC::LocalTensor<uint32_t> src0, src1;
AscendC::LocalTensor<uint32_t> dst;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::BitwiseOr(dst, src0, src1, count);
```

The results are as follows:

```
Input data (src0):
[-111, -83, 95, -52, ... -102]
Input data (src1):
[64, -44, 45, -117, ... -107]
Input data (dst):
[-47, -3, 127, -49, ... -97]
```
