# SoftmaxGrad Tiling API

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-11-10T06:20:50.340Z pushedAt=2026-11-18T11:07:26.471Z nl=2 -->

## Description

Obtains the SoftmaxGrad Tiling parameters.

## Prototype

-   API for obtaining the minimum/maximum temporary space required for the kernel-side API computation

    ```
    uint32_t GetSoftMaxGradMaxTmpSize(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const bool isFront, const bool isReuseSource)
    ```

    ```
    uint32_t GetSoftMaxGradMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const bool isFront, const bool isReuseSource)
    ```

-   Tiling computation API
    -   Computation API in the AscendC::optiling namespace

        ```
        void SoftMaxGradTilingFunc(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, optiling::SoftMaxTiling& softmaxGradTiling, const bool isFront = false)
        ```

    -   Computation API in the AscendC namespace

        ```
        void SoftMaxGradTilingFunc(const AscendC::TensorShape& srcShape, const uint32_t dataTypeSize, const uint32_t localWorkSpaceSize, AscendC::tiling::SoftMaxTiling& softmaxGradTiling, const bool isFront = false)
        ```

## Parameters

**Table 1**  SoftmaxGrad GetSoftMaxGradMaxTmpSize/GetSoftMaxGradMinTmpSize API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| dataTypeSize | Input | Data type for computation, for example, half=2. |
| isFront | Input | Whether to compute only ![](../../../../figures/formulaimage_0000001722356465.png), consistent with the SoftmaxGrad API on the kernel side. The default value is false. |
| isReuseSource | Input | Keep consistent with the API configuration on the kernel side. |

**Table 2**  SoftmaxGrad SoftMaxGradTilingFunc API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Shape information of the input srcTensor. The parameter type is [AscendC::TensorShape](../../data_structures/TensorShape.md). |
| localWorkSpaceSize | Input | Remaining temporary space available for the SoftmaxGrad API computation. The unit is Byte. The value of localWorkSpaceSize must be greater than the minimum temporary space size required for computation returned by the GetSoftMaxGradMinTmpSize API. |
| dataTypeSize | Input | Data type for computation, for example, half=2. |
| isFront | Input | Whether to compute only ![](../../../../figures/formulaimage_0000001723260621.png), consistent with the SoftmaxGrad API on the kernel side. The default value is false. |
| softmaxGradTiling | Output | Outputs the tiling information required by the SoftmaxGrad API. Supports the optiling::SoftMaxTiling form and the AscendC::tiling::SoftMaxTiling form as input parameters. |

## Return Value

GetSoftMaxGradMinTmpSize returns the minimum temporary space size required for the SoftmaxGrad API to complete computation. The unit is Byte.

GetSoftMaxGradMaxTmpSize returns the maximum temporary space size required for the SoftmaxGrad API to complete computation. The unit is Byte.

## Constraints

None
