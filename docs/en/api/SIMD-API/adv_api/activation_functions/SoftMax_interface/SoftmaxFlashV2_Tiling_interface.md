# SoftmaxFlashV2 Tiling API

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-10T06:17:44.739Z pushedAt=2026-11-18T11:07:26.433Z nl=2 -->

## Description

Obtains the Tiling parameters required by the SoftmaxFlashV2 API.

## Prototype

-   API for obtaining the minimum/maximum temporary space required for the kernel-side API computation

    ```
    uint32_t GetSoftMaxFlashV2MinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, const bool isUpdate, const bool isBasicBlock = false, const bool isFlashOutputBrc = false)
    ```

    ```
    uint32_t GetSoftMaxFlashV2MaxTmpSize(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, const bool isUpdate, const bool isBasicBlock = false, const bool isFlashOutputBrc = false)
    ```

-   Tiling computation API
    -   Computation API in the AscendC::optiling namespace

        ```
        void SoftMaxFlashV2TilingFunc(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate, const bool isBasicBlock = false, const bool isFlashOutputBrc = false)
        ```

    -   Computation API in the AscendC namespace

        ```
        void SoftMaxFlashV2TilingFunc(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxFlashTiling, const bool isUpdate, const bool isBasicBlock = false, const bool isFlashOutputBrc = false)
        ```

## Parameters

**Table 1**  GetSoftMaxFlashV2MinTmpSize/GetSoftMaxFlashV2MaxTmpSize API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| dataTypeSize1 | Input | Data type size of the source data to be computed, for example, half=2. |
| dataTypeSize2 | Input | Data type size of expSumTensor and maxTensor involved in the computation, for example, half=2. |
| isUpdate | Input | Whether to enable the refresh function, consistent with the SoftmaxFlashV2 API on the kernel side. |
| isBasicBlock | Input | Whether to enable basic block computation. The isBasicBlock parameter can be obtained through the [isBasicBlockInSoftmax](IsBasicBlockInSoftMax.md) API and must be consistent with the template parameter of the kernel-side API. The default value is false. Note that if the kernel-side API enables the SoftmaxConfig template parameter, that is, in the shape constant scenario, the isBasicBlock parameter must be obtained through the [isBasicBlockInSoftmax](IsBasicBlockInSoftMax.md) API. |
| isFlashOutputBrc | Input | Whether to enable the non-expansion mode of the output shape. In the non-expansion mode, Broadcast is not performed on the output data, and the output shape is (m, 1). The parameter values are as follows:<br>false: The non-expansion mode is disabled. This is the default value. When the output data type is float, the shape is (m, 8); when the output data type is half, the shape is (m, 16).<br>true: The non-expansion mode is enabled, and the output shape is (m, 1). When this parameter is set to true, the mode in the SoftmaxConfig template parameter of the [kernel API](SoftmaxFlashV2.md) must be configured as SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC. |

**Table 2**  SoftMaxFlashV2TilingFunc API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| localWorkSpaceSize | Input | Remaining space available for SoftmaxFlashV2 API computation. The value of localWorkSpaceSize must be greater than the minimum temporary space size required for computation returned by the GetSoftMaxFlashV2MinTmpSize API. |
| dataTypeSize1 | Input | Data type of the source data to be computed, for example, half=2. |
| dataTypeSize2 | Input | Data type of maxTensor and sumTensor involved in the computation, for example, half=2. |
| isUpdate | Input | Whether to enable the refresh function, consistent with the SoftmaxFlashV2 API on the kernel side. |
| isBasicBlock | Input | Whether to enable basic block computation. The isBasicBlock parameter can be obtained through the [isBasicBlockInSoftmax](IsBasicBlockInSoftMax.md) API and must be consistent with the template parameter of the kernel-side API. The default value is false. Note that if the kernel-side API enables the SoftmaxConfig template parameter, that is, in the shape constant scenario, the isBasicBlock parameter must be obtained through the [isBasicBlockInSoftmax](IsBasicBlockInSoftMax.md) API. |
| isFlashOutputBrc | Input | Whether to enable the non-expansion mode of the output shape. In the non-expansion mode, Broadcast is not performed on the output data, and the output shape is (m, 1). The parameter values are as follows:<br>false: The non-expansion mode is disabled. This is the default value. When the output data type is float, the shape is (m, 8); when the output data type is half, the shape is (m, 16).<br>true: The non-expansion mode is enabled, and the output shape is (m, 1). When this parameter is set to true, the mode in the SoftmaxConfig template parameter of the [kernel API](SoftmaxFlashV2.md) must be configured as SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC. |
| softmaxFlashTiling | Output | Outputs the tiling information required by the SoftmaxFlashV2 API. Supports the optiling::SoftMaxTiling form and the AscendC::tiling::SoftMaxTiling form as input parameters. |

## Return Value

GetSoftMaxFlashV2MinTmpSize returns the minimum temporary space size required for the SoftmaxFlashV2 API to complete computation, in bytes.

GetSoftMaxFlashV2MaxTmpSize returns the maximum temporary space size required for the SoftmaxFlashV2 API to complete computation, in bytes.

## Constraints

None
