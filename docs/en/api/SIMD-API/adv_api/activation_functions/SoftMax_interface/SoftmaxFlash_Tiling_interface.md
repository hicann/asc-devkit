# SoftmaxFlash Tiling API

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-10T06:17:05.926Z pushedAt=2026-11-18T11:07:26.426Z nl=2 -->

## Description

**Note: This API will be deprecated soon. Do not use it in new development.**

Used to obtain SoftmaxFlash Tiling parameters.

## Prototype

-   API for obtaining the minimum/maximum temporary space required for the kernel-side API computation

    ```
    uint32_t GetSoftMaxFlashMaxTmpSize(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const bool isUpdate, const bool isReuseSource)
    ```

    ```
    uint32_t GetSoftMaxFlashMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const bool isUpdate, const bool isReuseSource)
    ```

-   Tiling computation API
    -   Computation API in the AscendC::optiling namespace

        ```
        void SoftMaxFlashTilingFunc(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate = false)
        ```

    -   Computation API in the AscendC namespace

        ```
        void SoftMaxFlashTilingFunc(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate = false)
        ```

## Parameters

**Table 1**  SoftmaxFlash GetSoftMaxFlashMaxTmpSize/GetSoftMaxFlashMinTmpSize API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| dataTypeSize | Input | Data type of maxTensor and sumTensor involved in the computation, for example, half=2. |
| isUpdate | Input | Whether to enable the refresh function, consistent with the SoftmaxFlash API on the kernel side. The default value is false. |
| isReuseSource | Input | Keep consistent with the configuration of the API on the kernel side. |

**Table 2**  SoftmaxFlash SoftMaxFlashTilingFunc API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| dataTypeSize | Input | Data type of maxTensor and sumTensor involved in the computation, for example, half=2. |
| localWorkSpaceSize | Input | Remaining space available for the SoftmaxFlash API computation, in bytes. The value of localWorkSpaceSize must be greater than the minimum temporary space size required for computation, which is returned by the GetSoftMaxFlashMinTmpSize API. |
| isUpdate | Input | Whether to enable the refresh function, consistent with the SoftmaxFlash API on the kernel side. The default value is false. |
| softmaxFlashTiling | Output | Outputs the tiling information required by the SoftmaxFlash API. Both the optiling::SoftMaxTiling form and the AscendC::tiling::SoftMaxTiling form are supported as input parameters. |

## Return Value

GetSoftMaxFlashMaxTmpSize returns the maximum temporary space size required for the SoftmaxFlash API to complete computation, in bytes.

GetSoftMaxFlashMinTmpSize returns the minimum temporary space size required for the SoftmaxFlash API to complete computation, in bytes.

## Constraints

None
