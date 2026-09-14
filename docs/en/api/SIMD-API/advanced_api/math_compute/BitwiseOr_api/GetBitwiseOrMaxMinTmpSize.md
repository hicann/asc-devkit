# GetBitwiseOrMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T13:29:06.082Z -->

## Description

The BitwiseOr API on the Kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the Host side. Based on this range, you can select an appropriate space size and pass it to the Kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the Kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetBitwiseOrMaxMinTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Input platform information. For the definition of PlatformAscendC, see [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| srcShape | Input | Input shape information. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is uint16_t, pass 2 here. |
| isReuseSource | Input | This parameter is reserved. Pass the default value **false**. |
| maxValue | Output | Maximum temporary space size required for the BitwiseOr API to complete computation. Space exceeding this value will not be used by this API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the Kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br><br>Note that **maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the BitwiseOr API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

None

## Constraints

None

## Examples

For the complete calling example, see [More Samples](../more_samples_83.md).

```
std::vector<int64_t> shape_vec = {1024};
ge::Shape shape(shape_vec);
uint32_t maxValue = 0;
uint32_t minValue = 0;
auto plat = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
AscendC::GetBitwiseOrMaxMinTmpSize(plat, shape, 2, false, maxValue, minValue);
```

