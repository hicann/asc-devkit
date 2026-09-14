# SoftmaxFlashV3 Tiling API

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T13:52:05.619Z -->

## Description

Obtains the Tiling parameters required by the SoftmaxFlashV3 API.

## Prototype

-   APIs for obtaining the minimum/maximum temporary space required for kernel API computation

    ```
    void GetSoftMaxFlashV3MaxMinTmpSize(const ge::Shape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, uint32_t& maxValue, uint32_t& minValue, const bool isUpdate, const bool isBasicBlock = false)
    ```

-   Tiling computation API
    -   Computation APIs in the AscendC::optiling namespace

        ```
        void SoftMaxFlashV3TilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxFlashV3Tiling, const bool isUpdate,const bool isBasicBlock = false)
        ```

    -   Computation APIs in the AscendC namespace

        ```
        void SoftMaxFlashV3TilingFunc(const ge::Shape& srcShape, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxFlashV3Tiling, const bool isUpdate,const bool isBasicBlock = false)
        ```

## Parameters

**Table 1** Parameter list of the GetSoftMaxFlashV3MaxMinTmpSize API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. |
| dataTypeSize1 | Input | Data type size of the input srcTensor, that is, the data type size of the template parameter T in the SoftMaxFlashV3 Kernel function. The current template parameter T supports only the half type, so this parameter supports only the value 2. |
| dataTypeSize2 | Input | Data type size of the input inMeanTensor, inExpSumTensor, and inMaxTensor, that is, the data type size of the template parameter U in the SoftMaxFlashV3 Kernel function. The current template parameter U supports only the float type, so this parameter supports only the value 4. |
| maxValue | Output | Maximum temporary space size required for the SoftMaxFlashV3 API to complete computation. Space exceeding this value is not used by this API. Within the range from the minimum temporary space to the maximum temporary space, as the temporary space increases, the kernel-side API computation performance is optimized to a certain extent. To achieve better performance, developers can reserve/apply for space based on the actual memory usage. A maximum space size of 0 indicates that no temporary space is required for computation.<br>maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, developers need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the SoftMaxFlashV3 API to complete computation. To ensure functional correctness, the temporary space reserved/applied for during API computation must not be smaller than this value. A minimum space size of 0 indicates that no temporary space is required for computation. |
| isUpdate | Input | Whether to enable the formula computation with update set to true in SoftMaxFlashV3. The value of this parameter must be consistent with the template parameter isUpdate of the SoftmaxFlashV3 Kernel API. |
| isBasicBlock | Input | Reserved parameter, not enabled yet. The default value false must be used. |

**Table 2** Parameter list of the SoftMaxFlashV3TilingFunc API

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. |
| dataTypeSize1 | Input | Data type size of the input srcTensor, that is, the data type size of the template parameter T in the SoftMaxFlashV3 Kernel function. The current template parameter T supports only the half type, so this parameter supports only the value 2. |
| dataTypeSize2 | Input | Data type size of the input inMeanTensor, inExpSumTensor, and inMaxTensor, that is, the data type size of the template parameter U in the SoftMaxFlashV3 Kernel function. The current template parameter U supports only the float type, so this parameter supports only the value 4. |
| localWorkSpaceSize | Input | Remaining space available for the SoftmaxFlashV3 API computation. The value of localWorkSpaceSize must be greater than the minimum temporary space size required for computation returned by the GetSoftMaxFlashV3MaxMinTmpSize API. |
| isUpdate | Input | Whether to enable the formula computation with update set to true in SoftMaxFlashV3. This value must be consistent with the template parameter isUpdate of the SoftmaxFlashV3 Kernel API. |
| isBasicBlock | Input | Reserved parameter, not enabled yet. The default value false must be used. |
| softmaxFlashV3Tiling | Output | Outputs the Tiling information required by the SoftMaxFlashV3 API. It supports the optiling::SoftMaxTiling form and the AscendC::tiling::SoftMaxTiling form as input parameters. |

## Return Value

None

## Constraints

None
