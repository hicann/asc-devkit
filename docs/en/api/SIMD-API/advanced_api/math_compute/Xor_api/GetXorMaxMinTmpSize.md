# GetXorMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T10:57:48.973Z -->

## Description

The computation of the **Xor** API on the kernel side requires you to reserve/apply for temporary space. This API is used to obtain the maximum and minimum temporary space sizes to be reserved/applied for on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter and pass it to the kernel side for use.

-   To ensure functional correctness, the reserved/applied temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum temporary space size to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API is optimized and improved to a certain extent. To achieve better performance, you can reserve/apply for space based on the actual memory usage.

## Prototype

```
void GetXorMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1** API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Input shape information. |
| **typeSize** | Input | Size of the input data type, in bytes. For example, if the input data type is int16_t, pass 2. |
| **isReuseSource** | Input | Whether to reuse the space of the source operand input, consistent with the Xor API. |
| **maxValue** | Output | Maximum temporary space size required for the Xor API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/apply for space based on the actual memory usage.<br><br>Note that **maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| **minValue** | Output | Minimum temporary space size required for the Xor API to complete computation. To ensure correct functionality, the temporary space reserved/applied for during API computation must not be smaller than this value. |

## Return Value

None

## Constraints

None

## Examples

```
// Input shape information is 1024; the operator input data type is int16_t; the source operand cannot be modified.
std::vector<int64_t> shape_vec = {1024};
ge::Shape shape(shape_vec);
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetXorMaxMinTmpSize(shape, 2, false, maxValue, minValue);
```

