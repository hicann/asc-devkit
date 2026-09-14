# Where

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T10:55:20.491Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Selects elements from two source operands based on a specified condition to generate the target operand. Both source operands can be **LocalTensor** or a scalar.

![](../../../../figures/zh-cn_formulaimage_0000002425808613.png)

## Prototype

```
template <typename T, typename U, typename S, typename V>
__aicore__ inline void Where(const LocalTensor<T>& dst, const U& src0, const S& src1, const LocalTensor<V>& condition, const uint32_t count)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the target operand. Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. |
| U | LocalTensor type or scalar type. The type is automatically deduced from the input parameter src0. You do not need to configure this parameter; just ensure that the data type of src0 is consistent with that of the target operand. |
| S | LocalTensor type or scalar type. The type is automatically deduced from the input parameter src1. You do not need to configure this parameter; just ensure that the data type of src1 is consistent with that of the target operand. |
| V | Data type of the condition. The currently supported data type is bool. |

**Table 2** Parameter description

| Parameter | Type | Description |
| --- | --- | --- |
| dst | Output | Target operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands. The type is a scalar or LocalTensor. When the type is LocalTensor, the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type must be consistent with that of the target operand. |
| condition | Input | Condition operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the target operand is not supported.**
-   For the address offset alignment requirements of operands, see [General Description and Constraints](../../../general_description_and_constraints.md).

## Examples

```
AscendC::LocalTensor<half> dst, src0, src1;
AscendC::LocalTensor<bool> condition;
uint32_t count = 512; // Number of elements involved in the computation.
AscendC::Where(dst, src0, src1,  condition, count);
```

The result example is as follows:

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
