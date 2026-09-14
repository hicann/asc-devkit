# LogSoftMax Tiling

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-05T11:10:04.676Z pushedAt=2026-09-09T07:16:16.884Z -->

## Description

The computation of the **LogSoftMax** API on the kernel side requires you to reserve/allocate temporary space. The following APIs are used to obtain the maximum and minimum temporary space sizes for reservation/allocation on the host side. Based on this range, you select an appropriate space size and call the **LogSoftMaxTilingFunc** function to obtain parameters such as **reduceSize** and **splitSize**, which are then passed to the kernel side as tiling parameters.

-   To ensure correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API improves to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage.

## Prototype

-   APIs for obtaining the maximum/minimum temporary space required for kernel API computation

    ```
    uint32_t GetLogSoftMaxMaxTmpSize(const ge::Shape srcShape, const uint32_t dataTypeSize, const bool isReuseSource)
    ```

    ```
    uint32_t GetLogSoftMaxMinTmpSize(const ge::Shape srcShape, const uint32_t dataTypeSize, const bool isReuseSource)
    ```

-   Tiling computation API

    ```
    void LogSoftMaxTilingFunc(const ge::Shape srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, optiling::LogSoftMaxTiling& softmaxTiling)
    ```

    ```
    void LogSoftMaxTilingFunc(const ge::Shape srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, AscendC::tiling::LogSoftMaxTiling& softmaxTiling)
    ```

## Parameters

**Table 1** Parameters of the **GetLogSoftMaxMaxTmpSize**/**GetLogSoftMaxMinTmpSize** API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. |
| dataTypeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| isReuseSource | Input | Whether to reuse the input buffer of the source operand. The behavior is consistent with that in the **LogSoftMax** API. |

**Table 2** Parameters of the **LogSoftMaxTilingFunc** API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. |
| dataTypeSize | Input | Size of the input data type, in bytes. For example, if the input data type is half, pass 2 here. |
| localWorkSpaceSize | Input | Input temporary space size. |
| softmaxTiling | Output | Tiling parameters passed to the kernel side. |

## Return Value

The **GetLogSoftMaxMaxTmpSize**/**GetLogSoftMaxMinTmpSize** APIs return the maximum/minimum temporary space.

The **LogSoftMaxTilingFunc** API returns nothing.

## Constraints

None

## Examples

```
// shape: Input shape information.
// dtypesize: Size of the input data type, in bytes.

// Define an instance of the tiling parameter structure for the LogSoftMax operator.
AscendC::tiling::LogSoftMaxTiling tilingData;
// Whether to reuse the input buffer of the source operand. The behavior is consistent with that in the LogSoftMax API.
bool isReuseSource = false;

// Obtain the maximum temporary space required for computation.
const uint32_t tmpsize = AscendC::GetLogSoftMaxMaxTmpSize(shape, dtypesize, isReuseSource);
// Compute the optimal splitting strategy and populate it into the tilingData structure for use on the kernel side.
AscendC::LogSoftMaxTilingFunc(shape, dtypesize, tmpsize, tilingData);
```
