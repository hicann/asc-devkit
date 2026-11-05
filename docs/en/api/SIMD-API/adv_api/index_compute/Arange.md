# Arange

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:57:35.695Z pushedAt=2026-10-29T02:14:34.412Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/index_compute/Arange_res.md#id1 -->

## Description

Given a start value, a step value, and a length, returns an arithmetic sequence.

## Implementation Principles

The following figure shows the internal algorithm diagram of the Arange high-order API, using the float data type in ND format with firstValue and diffValue input as Scalars as an example.

**Figure 1** Arange algorithm block diagram  
![](../../../figures/arange_algorithm_diagram.png "Arange algorithm block diagram")

The computation process consists of the following steps, all performed on the Vector:

1.  Steps for an arithmetic sequence length within 8: Based on the values of firstValue and diffValue, use SetValue to expand the arithmetic sequence. The maximum expansion length is 8. If the arithmetic sequence length is less than 8, the algorithm ends.
2.  Steps for an arithmetic sequence length from 8 to 64: Use Adds to expand the arithmetic sequence result from step 1, looping up to 7 times to expand to 64. If the arithmetic sequence length is less than 64, the algorithm ends.
3.  Steps for an arithmetic sequence length greater than 64: Use Adds to expand the arithmetic sequence result from step 2, looping continuously until the arithmetic sequence length is reached.

## Prototype

```
template <typename T>
__aicore__ inline void Arange(const LocalTensor<T>& dst, const T firstValue, const T diffValue, const int32_t count)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. For the data types supported by different models, see [supported data types](#li73338460199). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand. The size of dst must be greater than or equal to count * sizeof(T).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| firstValue | Input | First element value of the arithmetic sequence. |
| diffValue | Input | Difference between elements of the arithmetic sequence, which must be greater than or equal to 0. |
| count | Input | Length of the arithmetic sequence. count>0. |

## Return Value

None

## Constraints

-   Currently, only the ND format is supported for input; other formats are not supported.
-   Supported data types<a id="li73338460199"></a>

    <!-- npu="950" id7 -->
    Ascend 950PR&950DT products, supported data types: int16\_t, half, int32\_t, float, int64\_t.
    <!-- end id7 -->
    <!-- npu="A3" id8 -->
    Atlas A3 products, supported data types: int16\_t, half, int32\_t, float.
    <!-- end id8 -->
    <!-- npu="910b" id9 -->
    Atlas A2 products, supported data types: int16\_t, half, int32\_t, float.
    <!-- end id9 -->
    <!-- npu="310p" id10 -->
    Atlas inference products AI Core, supported data types: int16\_t, half, int32\_t, float.
    <!-- end id10 -->

## Examples

For the complete example, see [Arange Examples](../../../../../../examples/01_simd_cpp_api/04_advanced_api/06_index/arange).

```
// dst: Output tensor.
// firstValue_: First element value of the arithmetic sequence.
// diffValue_: Common difference of the arithmetic sequence.
// count_: Length of the arithmetic sequence.
AscendC::Arange<T>(dst, static_cast<T>(firstValue_), static_cast<T>(diffValue_), count_);
```
