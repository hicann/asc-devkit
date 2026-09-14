# GetLogicalXorMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T08:59:32.765Z -->

## Description

The computation of the **LogicalXor** API on the kernel side requires you to reserve/allocate temporary space. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the Host side. Based on this range, you can select an appropriate space size as a Tiling parameter and pass it to the kernel side for use.

-   To ensure correct functionality, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetLogicalXorMaxMinTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1** API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Input platform information. For the definition of PlatformAscendC, see [Constructors and Destructors](../../../../Utils-API/Platform Information/platform_info/constructors_and_destructors.md). |
| srcShape | Input | Input shape information. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | This parameter is reserved. Pass the default value **false**. |
| maxValue | Output | Maximum temporary space size required for the LogicalXor API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br><br>Note that **maxValue** is only a reference value and may be larger than the remaining size of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining size of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the LogicalXor API to complete computation. To ensure correct functionality, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

For the complete sample, see [More Samples](../more_samples_83.md).

```
// Input shape information is 1024; the operator input data type is half; modifying the source operand is not allowed.
std::vector<int64_t> shape_vec = {1024};
ge::Shape shape(shape_vec);
uint32_t maxValue = 0;
uint32_t minValue = 0;
auto plat = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
AscendC::GetLogicalXorMaxMinTmpSize(plat, shape, 2, false, maxValue, minValue);
```

