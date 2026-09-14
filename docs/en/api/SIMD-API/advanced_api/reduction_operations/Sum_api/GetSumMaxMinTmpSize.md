# GetSumMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T13:19:33.478Z -->

## Description

The Sum API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
inline void GetSumMaxMinTmpSize(const uint32_t n, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxSize, uint32_t& minSize)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| n | Input | Actual number of elements computed per row of the input data. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand input. This is reserved here, consistent with the Sum API. |
| maxValue | Output | Maximum temporary space size required for the Sum API to complete the computation. Space exceeding this value is not used by this API.<br> Description: <br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the Sum API to complete the computation. To ensure correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. |

## Return Value

None

## Constraints

None

## Examples

```
// The input shape is a 2*3 matrix, so n = 3; the operator input data type is half; pass the default value false for isReuseSource.
uint32_t n = 3;
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetSumMaxMinTmpSize(n, 2, false, maxValue, minValue);
```

