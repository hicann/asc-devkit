# SoftmaxGrad Tiling APIs

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T13:53:00.656Z -->

## Description

Used to obtain the SoftmaxGrad tiling parameters.

## Prototype

-   APIs for obtaining the minimum/maximum temporary space required for kernel API computation

    ```
    uint32_t GetSoftMaxGradMaxTmpSize(const ge::Shape& srcShape, const uint32_t dataTypeSize, const bool isFront, const bool isReuseSource)
    ```

    ```
    uint32_t GetSoftMaxGradMinTmpSize(const ge::Shape& srcShape, const uint32_t dataTypeSize, const bool isFront, const bool isReuseSource)
    ```

-   Tiling computation API
    -   Computation APIs in the AscendC::optiling namespace

        ```
        void SoftMaxGradTilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxGradTiling, const bool isFront = false)
        ```

    -   Computation APIs in the AscendC namespace

        ```
        void SoftMaxGradTilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxGradTiling, const bool isFront = false)
        ```

## Parameters

**Table 1**  SoftmaxGrad GetSoftMaxGradMaxTmpSize/GetSoftMaxGradMinTmpSize API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. |
| dataTypeSize | Input | Data type used for computation, for example, half=2. |
| isFront | Input | Whether to compute only ![](../../../../figures/zh-cn_formulaimage_0000001722356465.png), consistent with the SoftmaxGrad API on the kernel side. The default value is **false**. |
| isReuseSource | Input | Keep consistent with the kernel-side API configuration. |

**Table 2**  SoftmaxGrad SoftMaxGradTilingFunc API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. |
| localWorkSpaceSize | Input | Remaining temporary space available for the SoftmaxGrad API computation. The unit is Byte. The value of localWorkSpaceSize must be greater than the minimum temporary space size required for computation returned by the GetSoftMaxGradMinTmpSize API. |
| dataTypeSize | Input | Data type used for computation, for example, half=2. |
| isFront | Input | Whether to compute only ![](../../../../figures/zh-cn_formulaimage_0000001723260621.png), consistent with the SoftmaxGrad API on the kernel side. The default value is **false**. |
| softmaxGradTiling | Output | Outputs the tiling information required by the SoftmaxGrad API. It supports the optiling::SoftMaxTiling form and the AscendC::tiling::SoftMaxTiling form as input parameters. |

## Return Value

GetSoftMaxGradMinTmpSize returns the minimum temporary space size required for the SoftmaxGrad API to complete computation. The unit is Byte.

**GetSoftMaxGradMaxTmpSize** returns the maximum temporary space size required for the **SoftmaxGrad** API to complete computation, and the unit is Byte.

## Constraints

None
