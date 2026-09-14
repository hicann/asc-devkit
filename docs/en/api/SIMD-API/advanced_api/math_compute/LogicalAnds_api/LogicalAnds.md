# LogicalAnds

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T16:47:31.349Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs an AND operation between each element in the input vector and a scalar. When the data type of the input vector or scalar is not bool, zero is treated as False and non-zero data is treated as True. The API supports two scenarios for the order of the vector and the scalar: scalar first and scalar last. The scalar can be configured as a single-point element of a **LocalTensor**. The calculation formula is as follows, where idx represents the position coefficient of the single-point element of the **LocalTensor**:

![](../../../../figures/zh-cn_formulaimage_0000002462754813.png)

## Prototype

```
template <const LogicalAndsConfig& config = DEFAULT_LOGICAL_ANDS_CONFIG, typename T, typename U, typename S>
__aicore__ inline void LogicalAnds(const LocalTensor<T>& dst, const U& src0, const S& src1, const uint32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configuration of the LogicalAnds algorithm. This parameter is optional and of the **LogicalAndsConfig** type. Its definition is shown in the following code, and the meanings of its parameters are as follows:<br><br>**isReuseSource**: This parameter is reserved. Pass the default value **false**.<br><br>**scalarTensorIndex**: When the scalar is a single-point element of a **LocalTensor**, this parameter specifies whether the scalar serves as the left operand or the right operand in the AND operation. **0** indicates the left operand, and **1** (default value) indicates the right operand. |
| T | Data type of the destination operand. The supported data type is: bool. |
| U | **LocalTensor** type or scalar type. The type is automatically deduced from the input parameter **src0**. Developers do not need to configure this parameter, but must ensure that **src0** satisfies the data type constraints.<br><br>The supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |
| S | **LocalTensor** type or scalar type. The type is automatically deduced from the input parameter **src1**. Developers do not need to configure this parameter, but must ensure that **src1** satisfies the data type constraints.<br><br>The supported data types are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t. |

```
struct LogicalAndsConfig {
    bool isReuseSource;
    int8_t scalarTensorIndex;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands. The type is scalar or **LocalTensor**. When the type is **LocalTensor**, it can be a vector operand or a scalar single-point element, and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   At least one of the left operand and the right operand of the AND operation in this API must be a vector. Both operands being scalars is currently not supported.
-   When single-point data of a **LocalTensor** is passed as the scalar, the **scalarTensorIndex** parameter must be a compile-time constant. If a variable is passed, the variable must be declared as **constexpr**.

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
AscendC::LocalTensor<bool> dst;
AscendC::LocalTensor<half> src0, src1;
uint32_t count = 512; // Number of elements involved in the computation.
// Scalar at the end.
AscendC::LogicalAnds(dst, src0, src1, count);
// Scalar at the beginning.
static constexpr AscendC::LogicalAndsConfig config = { false, 0 };
AscendC::LogicalAnds<config>(dst, src0, src1, count);
```

The results are as follows:

```
// Scalar at the end.
Input data (src0):
[1, 2, 0, -1, -2, 0, 3, 4, 0, -3, -4, 0, 5, 6, 0, -5, -6, 0, ... 0]
Input data (src1):
[3.0]
Output data (dst):
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
