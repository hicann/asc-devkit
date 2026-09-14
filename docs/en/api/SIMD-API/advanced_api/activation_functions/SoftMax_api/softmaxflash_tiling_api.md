# SoftmaxFlash Tiling API

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T13:48:18.089Z -->

## Description

**Note: This API will be deprecated soon. Do not use it in new development.**

Used to obtain SoftmaxFlash Tiling parameters.

## Prototype

-   APIs for obtaining the minimum/maximum temporary space required for kernel API computation

    ```
    uint32_t GetSoftMaxFlashMaxTmpSize(const ge::Shape& srcShape, const uint32_t dataTypeSize, const bool isUpdate, const bool isReuseSource)
    ```

    ```
    uint32_t GetSoftMaxFlashMinTmpSize(const ge::Shape& srcShape, const uint32_t dataTypeSize, const bool isUpdate, const bool isReuseSource)
    ```

-   Tiling computation API
    -   Computation APIs in the AscendC::optiling namespace

        ```
        void SoftMaxFlashTilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate = false)
        ```

    -   Computation APIs in the AscendC namespace

        ```
        void SoftMaxFlashTilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate = false)
        ```

## Parameters

**Table 1**  Parameter list of the SoftmaxFlash GetSoftMaxFlashMaxTmpSize/GetSoftMaxFlashMinTmpSize APIs

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. |
| dataTypeSize | Input | Data type of the maxTensor and sumTensor involved in the computation, for example, half=2. |
| isUpdate | Input | Whether to enable the refresh function, consistent with the kernel-side SoftmaxFlash API. The default value is **false**. |
| isReuseSource | Input | Keep consistent with the kernel-side API configuration. |

**Table 2**  Parameter list of the SoftmaxFlash SoftMaxFlashTilingFunc API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. |
| dataTypeSize | Input | Data type of the maxTensor and sumTensor involved in the computation, for example, half=2. |
| localWorkSpaceSize | Input | Remaining space available for the SoftmaxFlash API computation, in bytes. The value of localWorkSpaceSize must be greater than the minimum temporary space size required for computation returned by the GetSoftMaxFlashMinTmpSize API. |
| isUpdate | Input | Whether to enable the refresh function, consistent with the kernel-side SoftmaxFlash API. The default value is **false**. |
| softmaxFlashTiling | Output | Outputs the tiling information required by the **SoftmaxFlash** API. Both **optiling::SoftMaxTiling** and **AscendC::tiling::SoftMaxTiling** are supported as the parameter type. |

## Return Value

**GetSoftMaxFlashMaxTmpSize** returns the maximum temporary space size required for the SoftmaxFlash API to complete computation, and the unit is Byte.

**GetSoftMaxFlashMinTmpSize** returns the minimum temporary space size required for the SoftmaxFlash API to complete computation, and the unit is Byte.

## Constraints

None
