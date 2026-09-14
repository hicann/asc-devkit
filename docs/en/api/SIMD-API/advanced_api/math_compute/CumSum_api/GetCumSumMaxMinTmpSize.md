# GetCumSumMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T14:32:43.566Z -->

## Description

The CumSum API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetCumSumMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isLastAxis, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Input shape information. |
| **typeSize** | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| **isReuseSource** | Input | Whether to reuse the space of the source operand input. |
| **isLastAxis** | Input | Defines whether to process the first axis or the last axis. |
| **maxValue** | Output | Maximum temporary space size required for the CumSum API to complete the computation. Space exceeding this value is not used by this API.<br><br>Note that **maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| **minValue** | Output | Minimum temporary space size required for the CumSum API to complete the computation. To ensure correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   The input supports only a two-dimensional structure.

## Examples

```
// The input shape is a 32*32 matrix; the operator input data type is half; isLastAxis uses the default value true, and isReuseSource uses the default value false.
uint32_t firstDim = 32;
uint32_t lastDim = 32;
std::vector<int64_t> srcShapeDims = {firstDim, lastDim};
auto srcShape = ge::Shape(srcShapeDims);
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetCumSumMaxMinTmpSize(srcShape, 2, true, false, maxValue, minValue);
```

