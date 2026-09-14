# GetMeanMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T12:21:53.476Z -->

## Description

The Mean API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. For this API, **the maximum temporary space is currently equal to the minimum temporary space**.

## Prototype

```
void GetMeanMaxMinTmpSize(const uint32_t n, const uint32_t srcTypeSize, const uint32_t accTypeSize, const bool isReuseSource, uint32_t& maxSize, uint32_t& minSize)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| n | Input | Actual number of elements computed in each row of the input data. |
| srcTypeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| accTypeSize | Input | Size of the accType data type, in bytes. For details about the accType parameter, see [Mean API parameters](Mean.md). |
| isReuseSource | Input | Whether to reuse the space of the source operand input, consistent with the Mean API. This is a reserved parameter. |
| maxSize | Output | Maximum temporary space size required for the Mean API to complete computation. Space exceeding this value is not used by this API.<br> Note: <br>maxSize is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minSize | Output | Minimum temporary space size required for the Mean API to complete computation. To ensure correct functionality, the temporary space reserved/allocated during API computation must not be smaller than this value. |

## Return Value

None

## Constraints

None

## Examples

```
// The input data type T of the operator is half, accType is float, and isReuseSource uses the default value false.
uint32_t n = 3;
uint32_t srcTypeSize = 2;
uint32_t accTypeSize = 4;
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetMeanMaxMinTmpSize(n, srcTypeSize, accTypeSize, false, maxValue, minValue);
```

