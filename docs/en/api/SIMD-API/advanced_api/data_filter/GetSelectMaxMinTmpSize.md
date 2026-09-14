# GetSelectMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T17:42:56.989Z -->

## Description

The Select API on the kernel side requires you to allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to allocate on the host side. Based on this range, select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correctness, the allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can allocate space based on the actual memory usage.

## Prototype

> [!NOTE]Note
> The **GetSelectWithBytesMaskMinTmpSize**, **GetSelectWithBytesMaskMaxTmpSize**, and **GetSelectWithBytesMaskMaxMinTmpSize** APIs are deprecated and will be removed in a later version. Do not use these APIs. Use the **GetSelectMinTmpSize**, **GetSelectMaxTmpSize**, and **GetSelectMaxMinTmpSize** APIs instead.

-   Obtain the minimum temporary space size.

    ```
    uint32_t GetSelectMinTmpSize(const ge::Shape& src0Shape, const ge::Shape& src1Shape, const uint32_t srcTypeSize, const ge::Shape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask)
    ```

    ```
    uint32_t GetSelectWithBytesMaskMinTmpSize(const ge::Shape& src0Shape, const ge::Shape& src1Shape, const uint32_t srcTypeSize, const ge::Shape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask)
    ```

-   Obtain the maximum temporary space size.

    ```
    uint32_t GetSelectMaxTmpSize(const ge::Shape& src0Shape, const ge::Shape& src1Shape, const uint32_t srcTypeSize, const ge::Shape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask)
    ```

    ```
    uint32_t GetSelectWithBytesMaskMaxTmpSize(const ge::Shape& src0Shape, const ge::Shape& src1Shape, const uint32_t srcTypeSize, const ge::Shape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask)
    ```

-   Obtain the maximum and minimum temporary space sizes.

    ```
    void GetSelectMaxMinTmpSize(const ge::Shape& src0Shape, const ge::Shape& src1Shape, const uint32_t srcTypeSize, const ge::Shape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask, uint32_t& maxValue, uint32_t& minValue)
    ```

    ```
    void GetSelectWithBytesMaskMaxMinTmpSize(const ge::Shape& src0Shape, const ge::Shape& src1Shape, const uint32_t srcTypeSize, const ge::Shape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask, uint32_t& maxValue, uint32_t& minValue)
    ```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0Shape | Input | Shape information of the input src0. When src0 is a scalar, the shape should be {1}. |
| src1Shape | Input | Shape information of the input src1. When src1 is a scalar, the shape should be {1}. |
| srcTypeSize | Input | Data type size of the input srcTensor. For example, if the data type is half, pass 2 here. |
| maskShape | Input | Shape information of the input maskTensor. |
| maskTypeSize | Input | Data type size of the input maskTensor. For example, if the data type is bool, pass 1 here. |
| isReuseMask | Input | Whether to reuse the space of the input maskTensor. Keep it consistent with the kernel side. |
| maxValue | Output | Maximum temporary space size required for the Select API to complete computation.<br> Note: maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the Select API to complete computation. |

## Return Value

**GetSelectMinTmpSize** returns the minimum temporary space size required for the **Select** API to complete the computation.

**GetSelectMaxTmpSize** returns the maximum temporary space size required for the **Select** API to complete the computation.

**GetSelectMaxMinTmpSize** has no return value.

## Constraints

None

## Examples

```
std::vector<int64_t> shape0Vec = {64, 128};
std::vector<int64_t> shape1Vec = {1};
std::vector<int64_t> mask1Vec = {64, 128};
ge::Shape src0Shape(shape0Vec);
ge::Shape src1Shape(shape1Vec);
ge::Shape maskShape(mask1Vec);
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetSelectMaxMinTmpSize(src0Shape, src1Shape, 2, maskShape, 1, false, maxValue, minValue);
```
