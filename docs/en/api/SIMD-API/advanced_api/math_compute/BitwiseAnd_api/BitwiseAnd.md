# BitwiseAnd

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T13:08:49.910Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs a bitwise AND operation on two inputs.

![](../../../../figures/zh-cn_formulaimage_0000002386108598.png)

## Prototype

```
template <const BitwiseAndConfig& config = DEFAULT_BITWISE_AND_CONFIG, typename T>
__aicore__ inline void BitwiseAnd(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Related configuration of the BitwiseAnd algorithm. This parameter is optional and of the **BitwiseAndConfig** type. Its specific definition is shown in the following code, where the parameter meanings are as follows:<br><br>**isReuseSource**: This parameter is reserved. Pass the default value **false**. |
| T | Data type of the operands. Supported data types are int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, and uint64_t. |

```
struct BitwiseAndConfig {
    bool isReuseSource;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operands must be consistent with that of the destination operand. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
AscendC::LocalTensor<uint16_t> dst, src0, src1;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::BitwiseAnd(dst, src0, src1, count);
```

The results are as follows:

```
Input data (src0):
[93, 87, 99, 1, 87, 58, 6, 16, 85, 66, 56, 65, 24, 98, 96, 50, 18, 37, 0, ... 66]
Input data (src1):
[81, 12, 33, 47, 47, 18, 82, 44, 91, 34, 17, 99, 7, 82, 42, 85, 88, 99, 61, ... 81]
Output data (dst):
[81, 4, 33, 1, 7, 18, 2, 0, 81, 2, 16, 65, 0, 66, 32, 16, 16, 33, 0, ... 64]
```
