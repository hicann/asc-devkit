# GetSwiGLUMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T13:56:44.024Z -->

## Description

The SwiGLU API on the kernel side requires you to reserve/allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to reserve/allocate on the host side. Based on this range, you can select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

```
void GetSwiGLUMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue, const bool isReuseSource)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Input shape information. |
| **typeSize** | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| **maxValue** | Output | Maximum temporary space size required for the SwiGLU API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| **minValue** | Output | Minimum temporary space size required for the SwiGLU API to complete computation. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |
| **isReuseSource** | Input | Whether to reuse the space of the source operand input, consistent with the kernel-side API. |

## Return Value

None

## Constraints

None

## Examples

```
// Input shape information is 1024; the input data type of the operator is half; the source operand cannot be modified.
std::vector<int64_t> shape_vec = {1024};
ge::Shape shape(shape_vec);
uint32_t max;
uint32_t min;
AscendC::GetSwiGLUMaxMinTmpSize(shape, 2, max, min, false);
```
