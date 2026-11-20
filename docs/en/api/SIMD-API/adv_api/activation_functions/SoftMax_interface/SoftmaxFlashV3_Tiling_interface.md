# SoftmaxFlashV3 Tiling API

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:18:11.860Z pushedAt=2026-11-18T11:07:26.440Z nl=2 -->

## Description

Used to obtain the Tiling parameters required by the SoftmaxFlashV3 API.

## Prototype

-   API for obtaining the minimum/maximum temporary space required for the kernel-side API computation

    ```
    void GetSoftMaxFlashV3MaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, uint32_t& maxValue, uint32_t& minValue, const bool isUpdate, const bool isBasicBlock = false)
    ```

-   Tiling computation API
    -   Computation API in the AscendC::optiling namespace

        ```
        void SoftMaxFlashV3TilingFunc(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxFlashV3Tiling, const bool isUpdate,const bool isBasicBlock = false)
        ```

    -   Computation API in the AscendC namespace

        ```
        void SoftMaxFlashV3TilingFunc(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxFlashV3Tiling, const bool isUpdate,const bool isBasicBlock = false)
        ```

## Parameters

**Table 1**  GetSoftMaxFlashV3MaxMinTmpSize API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| dataTypeSize1 | Input | Data type size of the input srcTensor, that is, the data type size of the template parameter T in the SoftMaxFlashV3 kernel-side API. Currently, the template parameter T supports only the half type, so this parameter supports only the value 2. |
| dataTypeSize2 | Input | Data type size of the input inMeanTensor, inExpSumTensor, and inMaxTensor, that is, the data type size of the template parameter U in the SoftMaxFlashV3 kernel-side API. Currently, the template parameter U supports only the float type, so this parameter supports only the value 4. |
| maxValue | Output | Maximum temporary space size required for the SoftMaxFlashV3 API to complete computation, in bytes. Space beyond this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, you can reserve/allocate space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer (UB). In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the SoftMaxFlashV3 API to complete computation, in bytes. To ensure functional correctness, the temporary space reserved/allocated during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |
| isUpdate | Input | Whether to enable the formula computation with SoftMaxFlashV3 update set to true. The value of this parameter must be consistent with the template parameter isUpdate of the SoftMaxFlashV3 kernel-side API. |
| isBasicBlock | Input | Reserved parameter, not enabled yet. The default value false must be used. |

**Table 2**  SoftMaxFlashV3TilingFunc API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| dataTypeSize1 | Input | Data type size of the input srcTensor, that is, the data type size of the template parameter T in the SoftMaxFlashV3 kernel-side API. Currently, the template parameter T supports only the half type, so this parameter supports only the value 2. |
| dataTypeSize2 | Input | Data type size of the input inMeanTensor, inExpSumTensor, and inMaxTensor, that is, the data type size of the template parameter U in the SoftMaxFlashV3 kernel-side API. Currently, the template parameter U supports only the float type, so this parameter supports only the value 4. |
| localWorkSpaceSize | Input | Remaining space available for the SoftmaxFlashV3 API computation. The value of localWorkSpaceSize must be greater than the minimum temporary space size required for computation returned by the GetSoftMaxFlashV3MaxMinTmpSize API. |
| isUpdate | Input | Whether to enable the formula computation with SoftMaxFlashV3 update set to true. This value must be consistent with the template parameter isUpdate of the SoftmaxFlashV3 kernel-side API. |
| isBasicBlock | Input | Reserved parameter, not enabled yet. The default value false must be used. |
| softmaxFlashV3Tiling | Output | Outputs the Tiling information required by the SoftMaxFlashV3 API. Supports the optiling::SoftMaxTiling form and the AscendC::tiling::SoftMaxTiling form as input parameters. |

## Return Value

None

## Constraints

None
