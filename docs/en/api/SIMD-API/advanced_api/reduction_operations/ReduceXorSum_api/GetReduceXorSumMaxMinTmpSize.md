# GetReduceXorSumMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T13:15:01.500Z -->

## Description

The ReduceXorSum API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
void GetReduceXorSumMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is int16_t, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand input, consistent with the ReduceXorSum API. |
| maxValue | Output | Maximum temporary space size required by the ReduceXorSum API to complete computation. Space exceeding this value is not used by this API.<br> Note: <br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required by the ReduceXorSum API to complete computation. To ensure correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. |

## Return Value

None

## Constraints

None

## Examples

```
// Input shape information is 1024; the operator input data type is int16_t; modifying the source operand is not allowed.
std::vector<int64_t> shape_vec = {1024};
ge::Shape shape(shape_vec);
uint32_t maxValue = 0;
uint32_t minValue = 0;
uint32_t typeSize = sizeof(int16_t);
AscendC::GetReduceXorSumMaxMinTmpSize(shape, typeSize, false, maxValue, minValue);
```

