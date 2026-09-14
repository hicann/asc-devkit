# GetGeluMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T11:09:41.479Z pushedAt=2026-09-09T07:16:16.883Z -->

## Description

The computation of the **Gelu**, **FasterGelu**, and **FasterGeluV2** APIs on the kernel side requires you to reserve/apply for temporary space. This API is used to obtain the maximum and minimum temporary space sizes to be reserved/applied on the host side. Based on this range, you select an appropriate space size and pass it to the kernel side as a Tiling parameter.

-   To ensure correct functionality, the reserved/applied temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side APIs improves to a certain extent. To achieve better performance, you can reserve/apply space based on the actual memory usage.

## Prototype

-   API for obtaining the maximum temporary space size required for kernel API computation.

    ```
    uint32_t GetGeluMaxTmpSize(const ge::Shape& srcShape, const uint32_t typeSize)
    ```

-   API for obtaining the minimum temporary space size required for kernel API computation.

    ```
    uint32_t GetGeluMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize)
    ```

-   API for obtaining the maximum/minimum temporary space required for kernel API computation.

    ```
    void GetGeluMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, uint32_t& maxValue, uint32_t& minValue)
    ```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. |
| typeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| maxValue | Output | Maximum temporary space size required for the **Gelu**, **FasterGelu**, and **FasterGeluV2** APIs to complete computation. Space exceeding this value is not used by these APIs. Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side APIs improves to a certain extent. To achieve better performance, you can reserve/apply space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>**maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, you need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the **Gelu**, **FasterGelu**, and **FasterGeluV2** APIs to complete computation. To ensure correct functionality, the temporary space reserved/applied during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |

## Return Value

**GetGeluMaxTmpSize** returns the maximum temporary space size required for the kernel-side APIs to complete computation.

**GetGeluMinTmpSize** returns the minimum required temporary space for the kernel-side API to complete the computation.

**GetGeluMaxMinTmpSize** returns nothing.

## Constraints

None

## Examples

```
// The input shape information is 1024; the data type of the operator input is half;
std::vector<int64_t> shape_vec = {1024};
ge::Shape srcShape(shape_vec);
uint32_t typeSize = 2;
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetGeluMaxMinTmpSize(srcShape, typeSize, maxValue, minValue);
```
