# GetBroadCastMaxMinTmpSize

<!-- md-trans-meta sourceCommit=29d3208111c9988196c0c653d3794b0477c81f15 translatedAt=2026-09-08T15:57:39.702Z -->

## Description

The Broadcast API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetBroadCastMaxMinTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const ge::Shape& srcShape,const ge::Shape& dstShape, uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Platform information for execution. |
| srcShape | Input | Input shape information. |
| dstShape | Input | Output shape information. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand input, consistent with the kernel-side API. |
| maxValue | Output | Maximum temporary space size required for the Broadcast API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the Broadcast API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

```
// Input shape information is (1024,1); the operator input data type is half; modifying the source operand is not allowed.
auto platformInfo = context_->GetPlatformInfo();
auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
std::vector<int64_t> srcShapeVec = {1024, 1};
std::vector<int64_t> dstShapeVec = {1024, 16};
ge::Shape srcShape(srcShapeVec);
ge::Shape dstShape(dstShapeVec);
uint32_t maxValue{0};
uint32_t minValue{0};
AscendC::GetBroadCastMaxMinTmpSize(ascendcPlatform, srcShape,dstShape, 2, false, maxValue, minValue);
```
