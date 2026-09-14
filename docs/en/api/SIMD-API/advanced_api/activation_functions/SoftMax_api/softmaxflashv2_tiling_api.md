# SoftmaxFlashV2 Tiling APIs

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T13:49:55.109Z -->

## Description

Used to obtain the Tiling parameters required by the SoftmaxFlashV2 API.

## Prototype

-   APIs for obtaining the minimum/maximum temporary space required for kernel API computation

    ```
    uint32_t GetSoftMaxFlashV2MinTmpSize(const ge::Shape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, const bool isUpdate, const bool isBasicBlock = false, const bool isFlashOutputBrc = false)
    ```

    ```
    uint32_t GetSoftMaxFlashV2MaxTmpSize(const ge::Shape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, const bool isUpdate, const bool isBasicBlock = false, const bool isFlashOutputBrc = false)
    ```

-   Tiling computation API
    -   Computation APIs in the AscendC::optiling namespace

        ```
        void SoftMaxFlashV2TilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate, const bool isBasicBlock = false, const bool isFlashOutputBrc = false)
        ```

    -   Computation APIs in the AscendC namespace

        ```
        void SoftMaxFlashV2TilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate, const bool isBasicBlock = false, const bool isFlashOutputBrc = false)
        ```

## Parameters

**Table 1** Parameter list of the GetSoftMaxFlashV2MinTmpSize/GetSoftMaxFlashV2MaxTmpSize APIs

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information of the input **srcTensor**. |
| **dataTypeSize1** | Input | Data type size of the source data to be computed, for example, half=2. |
| **dataTypeSize2** | Input | Data type size of the **expSumTensor** and **maxTensor** involved in the computation, for example, half=2. |
| **isUpdate** | Input | Whether to enable the refresh function, consistent with the SoftmaxFlashV2 API on the kernel side. |
| **isBasicBlock** | Input | Whether to enable basic block computation. The **isBasicBlock** parameter can be obtained through the [isBasicBlockInSoftmax](IsBasicBlockInSoftMax.md) API and must be consistent with the template parameter of the kernel-side API, defaulting to **false**. Note that if the kernel-side API enables the template parameter **SoftmaxConfig**, that is, in the shape constant scenario, the **isBasicBlock** parameter must be obtained through the [isBasicBlockInSoftmax](IsBasicBlockInSoftMax.md) API. |
| **isFlashOutputBrc** | Input | Whether to enable the non-expanded mode of the output shape. In the non-expanded mode, no broadcast is performed on the output data, and the output shape is (m, 1). The parameter values are as follows:<br>**false**: The non-expanded mode is not enabled, which is the default value. When the output is of the float data type, the shape is (m, 8); when the output is of the half data type, the shape is (m, 16).<br>**true**: The non-expanded mode is enabled, and the output shape is (m, 1). When this parameter is set to **true**, the **mode** in the template parameter **SoftmaxConfig** of the [kernel API](SoftmaxFlashV2.md) must be configured as **SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC**. |

**Table 2** Parameter list of the SoftMaxFlashV2TilingFunc API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **srcShape** | Input | Shape information of the input **srcTensor**. |
| **localWorkSpaceSize** | Input | Remaining space available for the SoftmaxFlashV2 API computation. The value of **localWorkSpaceSize** must be greater than the minimum temporary space size required for computation returned by the GetSoftMaxFlashV2MinTmpSize API. |
| **dataTypeSize1** | Input | Data type of the source data to be computed, for example, half=2. |
| **dataTypeSize2** | Input | Data type of the **maxTensor** and **sumTensor** involved in the computation, for example, half=2. |
| **isUpdate** | Input | Whether to enable the refresh function, consistent with the SoftmaxFlashV2 API on the kernel side. |
| **isBasicBlock** | Input | Whether to enable basic block computation. The **isBasicBlock** parameter can be obtained through the [isBasicBlockInSoftmax](IsBasicBlockInSoftMax.md) API and must be consistent with the template parameter of the kernel-side API, defaulting to **false**. Note that if the kernel-side API enables the template parameter **SoftmaxConfig**, that is, in the shape constant scenario, the **isBasicBlock** parameter must be obtained through the [isBasicBlockInSoftmax](IsBasicBlockInSoftMax.md) API. |
| **isFlashOutputBrc** | Input | Whether to enable the non-expanded mode of the output shape. In the non-expanded mode, no broadcast is performed on the output data, and the output shape is (m, 1). The parameter values are as follows:<br>**false**: The non-expanded mode is not enabled, which is the default value. When the output is of the float data type, the shape is (m, 8); when the output is of the half data type, the shape is (m, 16).<br>**true**: The non-expanded mode is enabled, and the output shape is (m, 1). When this parameter is set to **true**, the **mode** in the template parameter **SoftmaxConfig** of the [kernel API](SoftmaxFlashV2.md) must be configured as **SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC**. |
| **softmaxFlashTiling** | Output | Outputs the tiling information required by the SoftmaxFlashV2 API, supporting both the **optiling::SoftMaxTiling** form and the **AscendC::tiling::SoftMaxTiling** form as input parameters. |

## Return Value

GetSoftMaxFlashV2MinTmpSize returns the minimum temporary space size required for the SoftmaxFlashV2 API to complete computation, in bytes.

**GetSoftMaxFlashV2MaxTmpSize** returns the maximum temporary space size required for the **SoftmaxFlashV2** API to complete computation, in bytes.

## Constraints

None
