# GetBroadCastMaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-02T04:55:38.847Z pushedAt=2026-11-18T04:59:15.979Z nl=2 -->

## Description

The Broadcast API computation on the kernel function (Kernel) side requires developers to reserve/allocate temporary space. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, developers select an appropriate space size as the Tiling parameter and pass it to the kernel function (Kernel) side for use.

-   To ensure functional correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel function (Kernel) side API improves to a certain extent. To achieve better performance, developers can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetBroadCastMaxMinTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const AscendC::TensorShape& srcShape,const AscendC::TensorShape& dstShape, uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Platform information to be executed. |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| dstShape | Input | Output shape information. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the space of the source operand input, consistent with the kernel function (Kernel) side API. |
| maxValue | Output | Maximum temporary space size required for the Broadcast API to complete the computation, in bytes. Space exceeding this value is not used by this API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel function (Kernel) side API improves to a certain extent. To achieve better performance, developers can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for the computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, developers need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Broadcast API to complete the computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for the computation. |

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
AscendC::TensorShape srcShape(srcShapeVec);
AscendC::TensorShape dstShape(dstShapeVec);
uint32_t maxValue{0};
uint32_t minValue{0};
AscendC::GetBroadCastMaxMinTmpSize(ascendcPlatform, srcShape, dstShape, 2, false, maxValue, minValue);
```
