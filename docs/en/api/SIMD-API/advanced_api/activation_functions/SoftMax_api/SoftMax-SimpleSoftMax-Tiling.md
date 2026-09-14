# SoftMax/SimpleSoftMax Tiling

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T13:46:11.535Z -->

## Description

Used to obtain the SoftMax/SimpleSoftMax Tiling parameters.

## Prototype

-   APIs for obtaining the maximum/minimum temporary space required for kernel API computation

    ```
    uint32_t GetSoftMaxMaxTmpSize(const ge::Shape& srcShape, const uint32_t dataTypeSize, const bool isReuseSource)
    ```

    ```
    uint32_t GetSoftMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t dataTypeSize, const bool isReuseSource)
    ```

-   Tiling computation API
    -   Computation API in the **AscendC::optiling** namespace

        ```
        void SoftMaxTilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxTiling)
        ```

    -   Computation API in the **AscendC** namespace

        ```
        void SoftMaxTilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxTiling)
        ```

## Parameters

**Table 1** Parameter list of the SoftMax/SimpleSoftMax **GetSoftMaxMaxTmpSize**/**GetSoftMaxMinTmpSize** APIs

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information of the input **srcTensor**. |
| **dataTypeSize** | Input | Data type of the max and sum values involved in the computation, for example, half=2. |
| **isReuseSource** | Input | Keep consistent with the kernel-side API configuration. |

**Table 2** Parameter list of the SoftMax/SimpleSoftMax **SoftMaxTilingFunc** API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information of the input **srcTensor**. |
| **dataTypeSize** | Input | Data type of the max and sum values involved in the computation, for example, half=2. |
| **localWorkSpaceSize** | Input | Remaining space size available for the SoftMax API computation, in Byte. The value of **localWorkSpaceSize** must be greater than the minimum temporary space size required for computation returned by the **GetSoftMaxMinTmpSize** API. |
| **softmaxTiling** | Output | Outputs the tiling information required by the SoftMax API. Both the **optiling::SoftMaxTiling** form and the **AscendC::tiling::SoftMaxTiling** form are supported as input parameters. |

## Return Value

**GetSoftMaxMaxTmpSize** returns the maximum temporary space size required for the SoftMax/SimpleSoftMax API to complete the computation, in Byte.

**GetSoftMaxMinTmpSize** returns the minimum temporary space size required for the SoftMax/SimpleSoftMax API to complete the computation, in Byte.

## Constraints

None
