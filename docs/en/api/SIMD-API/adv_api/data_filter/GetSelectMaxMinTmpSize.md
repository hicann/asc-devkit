# GetSelectMaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:30:10.279Z pushedAt=2026-11-18T11:07:26.751Z nl=2 -->

## Description

The Select API on the kernel-side requires you to allocate temporary space for computation. This API is used to obtain the maximum and minimum temporary space sizes to allocate on the host side. Based on this range, you can select an appropriate space size as a Tiling parameter to pass to the kernel-side.

-   To ensure correct functionality, the allocated temporary space size must not be smaller than the minimum temporary space size;
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can allocate space based on the actual memory usage.

## Prototype

> [!NOTE] Note
>The GetSelectWithBytesMaskMinTmpSize, GetSelectWithBytesMaskMaxTmpSize, and GetSelectWithBytesMaskMaxMinTmpSize APIs are deprecated and will be removed in a later version. Do not use these APIs. Use the GetSelectMinTmpSize, GetSelectMaxTmpSize, and GetSelectMaxMinTmpSize APIs instead.

-   Obtain the minimum temporary space size.

    ```
    uint32_t GetSelectMinTmpSize(const AscendC::TensorShape& src0Shape, const AscendC::TensorShape& src1Shape, const uint32_t srcTypeSize, const AscendC::TensorShape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask)
    ```

    ```
    uint32_t GetSelectWithBytesMaskMinTmpSize(const AscendC::TensorShape& src0Shape, const AscendC::TensorShape& src1Shape, const uint32_t srcTypeSize, const AscendC::TensorShape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask)
    ```

-   Obtain the maximum temporary space size.

    ```
    uint32_t GetSelectMaxTmpSize(const AscendC::TensorShape& src0Shape, const AscendC::TensorShape& src1Shape, const uint32_t srcTypeSize, const AscendC::TensorShape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask)
    ```

    ```
    uint32_t GetSelectWithBytesMaskMaxTmpSize(const AscendC::TensorShape& src0Shape, const AscendC::TensorShape& src1Shape, const uint32_t srcTypeSize, const AscendC::TensorShape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask)
    ```

-   Obtain the maximum and minimum temporary space sizes.

    ```
    void GetSelectMaxMinTmpSize(const AscendC::TensorShape& src0Shape, const AscendC::TensorShape& src1Shape, const uint32_t srcTypeSize, const AscendC::TensorShape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask, uint32_t& maxValue, uint32_t& minValue)
    ```

    ```
    void GetSelectWithBytesMaskMaxMinTmpSize(const AscendC::TensorShape& src0Shape, const AscendC::TensorShape& src1Shape, const uint32_t srcTypeSize, const AscendC::TensorShape& maskShape, const uint32_t maskTypeSize, const bool isReuseMask, uint32_t& maxValue, uint32_t& minValue)
    ```

## Parameters

**Table 1**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0Shape | Input | Shape information of the input src0. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). When src0 is a scalar, the shape should be {1}. |
| src1Shape | Input | Shape information of the input src1. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). When src1 is a scalar, the shape should be {1}. |
| srcTypeSize | Input | Data type size of the input srcTensor. For example, if the data type is half, pass 2. |
| maskShape | Input | Shape information of the input maskTensor. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| maskTypeSize | Input | Data type size of the input maskTensor. For example, if the data type is bool, pass 1. |
| isReuseMask | Input | Whether to reuse the space of the input maskTensor. Keep it consistent with the kernel-side. |
| maxValue | Output | Maximum temporary space size required for the Select API to complete computation, in bytes.<br> Note: maxValue is only a reference value and may be larger than the remaining UB space. In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Select API to complete computation, in bytes. |

## Return Value

GetSelectMinTmpSize returns the minimum temporary space size required for the Select API to complete computation.

GetSelectMaxTmpSize returns the maximum temporary space size required for the Select API to complete computation.

GetSelectMaxMinTmpSize returns no value.

## Constraints

None

## Examples

```
std::vector<int64_t> shape0Vec = {64, 128};
std::vector<int64_t> shape1Vec = {1};
std::vector<int64_t> mask1Vec = {64, 128};
AscendC::TensorShape src0Shape(shape0Vec);
AscendC::TensorShape src1Shape(shape1Vec);
AscendC::TensorShape maskShape(mask1Vec);
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetSelectMaxMinTmpSize(src0Shape, src1Shape, 2, maskShape, 1, false, maxValue, minValue);
```
