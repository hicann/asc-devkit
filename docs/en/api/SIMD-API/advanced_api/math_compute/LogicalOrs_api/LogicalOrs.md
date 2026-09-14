# LogicalOrs

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T08:55:54.128Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Each element in the input vector performs an OR operation with the scalar. When the data type of the input vector or the scalar is not bool, zero is treated as **False** and non-zero data is treated as **True**. The API supports two scenarios for the order of the vector and the scalar: scalar first and scalar last. The scalar can be configured as a single-point element of **LocalTensor**. The computation formula is as follows, where **idx** indicates the position coefficient of the single-point element of **LocalTensor**:

![](../../../../figures/zh-cn_formulaimage_0000002429156606.png)

## Prototype

```
template <const LogicalOrsConfig& config = DEFAULT_LOGICAL_ORS_CONFIG, typename T, typename U, typename S>
__aicore__ inline void LogicalOrs(const LocalTensor<T>& dst, const U& src0, const S& src1, const uint32_t count)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| config | Related configuration of the LogicalOrs algorithm. This parameter is optional and of the LogicalOrsConfig type. Its specific definition is shown in the following code, where the parameters mean:<br><br>**isReuseSource**: This parameter is reserved. Pass the default value **false**.<br><br>scalarTensorIndex: When the scalar is a single-point element of a LocalTensor, this parameter specifies whether the scalar serves as the left operand or the right operand in the OR operation. **0** indicates the left operand, and **1** (default) indicates the right operand. |
| T | Data type of the destination operand. The supported data type is: bool. |
| U | LocalTensor type or scalar type. The type is automatically deduced from the input parameter src0. You do not need to configure this parameter; just ensure that src0 meets the data type constraints.<br><br>The supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. |
| S | LocalTensor type or scalar type. The type is automatically deduced from the input parameter **src1**. You do not need to configure this parameter; just ensure that src1 meets the data type constraints.<br><br>The supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. |

```
struct LogicalOrsConfig {
    bool isReuseSource;
    int8_t scalarTensorIndex;
};
```

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Target operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands. The type is scalar or LocalTensor. When the type is LocalTensor, it can be a vector operand or a scalar single-point element, and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   At least one of the left operand and the right operand of this API or operation must be a vector. It is currently not supported for both the left and right operands to be scalars.
-   When single-point data of **LocalTensor** is passed as a scalar, the **scalarTensorIndex** parameter must be a compile-time constant. If a variable is passed, the variable must be declared as **constexpr**.

-   **Overlapping between the source operand and the destination operand addresses is not supported.**
-   For the operand address alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
AscendC::LocalTensor<bool> dst;
AscendC::LocalTensor<half> src0, src1;
uint32_t count = 512; // Number of elements involved in the computation.
// Scalar at the end.
AscendC::LogicalOrs(dst, src0, src1, count);
// Scalar at the beginning.
static constexpr AscendC::LogicalOrsConfig config= { false, 0 };
AscendC::LogicalOrs<config>(dst, src0, src1, count);
```

The result examples are as follows:

```
// Scalar at the end.
Input data (src0):
[1, 2, 0, -1, -2, 0, 3, 4, 0, -3, -4, 0, 5, 6, 0, -5, -6, 0, ... 0]
Input data (src1):
[0.0000]
Output data (dst):
[ True,  True,  False,  True,  True,  False,  True,  True,  False,  True,
  True,  False,  True,  True,  False,  True,  True,  False,  ...  False]
// Scalar at the beginning.
Input data (src1):
[1, 2, 0, -1, -2, 0, 3, 4, 0, -3, -4, 0, 5, 6, 0, -5, -6, 0, ... 0]
Input data (src0):
[0.0000]
Output data (dst):
[ True,  True,  False,  True,  True,  False,  True,  True,  False,  True,
  True,  False,  True,  True,  False,  True,  True,  False,  ...  False]
```
