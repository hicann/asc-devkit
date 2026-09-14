# Arange

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T17:49:58.033Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Given a start value, a difference value, and a length, returns an arithmetic sequence.

## Implementation Principle

Taking **float** type, **ND** format, and **firstValue** and **diffValue** input as **Scalar** as an example, the internal algorithm block diagram of the **Arange** high-order API is described as shown in the following figure.

**Figure 1**  Arange algorithm block diagram  
![](../../../figures/Arange_algorithm_block_diagram.png "Arange algorithm block diagram")

The computation process consists of the following steps, all performed on the vector:

1.  Steps for an arithmetic sequence with a length within 8: Based on the values of **firstValue** and **diffValue**, use **SetValue** to expand the arithmetic sequence, with a maximum expansion length of 8. If the arithmetic sequence length is less than 8, the algorithm ends.
2.  Steps for an arithmetic sequence with a length from 8 to 64: Use **Adds** to expand the arithmetic sequence result from step 1, looping up to 7 times to expand it to 64. If the arithmetic sequence length is less than 64, the algorithm ends.
3.  Steps for an arithmetic sequence with a length greater than 64: Use **Adds** to expand the arithmetic sequence result from step 2, looping continuously until the arithmetic sequence length is reached.

## Prototype

```
template <typename T>
__aicore__ inline void Arange(const LocalTensor<T>& dst, const T firstValue, const T diffValue, const int32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [Supported Data Types](#li73338460199). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand. The size of **dst** must be greater than or equal to count * sizeof(T).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| firstValue | Input | First element value of the arithmetic sequence. |
| diffValue | Input | Difference between elements of the arithmetic sequence, which must be greater than or equal to 0. |
| count | Input | Length of the arithmetic sequence. count>0. |

## Return Value

None

## Constraints

-   Currently, only ND-format input is supported, and other formats are not supported.
-   Supported data types<a id="li73338460199"></a>

    Ascend 950PR/Ascend 950DT, the supported data types are: int16\_t, half, int32\_t, float, and int64\_t.

    Atlas A3 training products/Atlas A3 inference products, the supported data types are: int16\_t, half, int32\_t, and float.

    Atlas A2 training products/Atlas A2 inference products, the supported data types are: int16\_t, half, int32\_t, and float.

    Atlas inference products AI Core, the supported data types are: int16\_t, half, int32\_t, and float.

## Examples

For the complete sample, see the [Arange example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/06_index/arange).

```
// dst: Output tensor.
// firstValue_: First element value of the arithmetic sequence.
// diffValue_: Common difference of the arithmetic sequence.
// count_: Length of the arithmetic sequence.
AscendC::Arange<T>(dst, static_cast<T>(firstValue_), static_cast<T>(diffValue_), count_);
```
