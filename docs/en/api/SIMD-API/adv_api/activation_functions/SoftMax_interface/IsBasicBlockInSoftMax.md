# IsBasicBlockInSoftMax

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-10T06:16:11.773Z pushedAt=2026-11-18T11:07:26.406Z nl=2 -->

## Description

Determines whether a SoftMaxTiling structure conforms to the basic block characteristics.

## Prototype

-   Computation API in the AscendC::optiling namespace

    ```
    bool IsBasicBlockInSoftMax(optiling::SoftMaxTiling& tiling, const uint32_t dataTypeSize = 2)
    ```

-   Computation API in the AscendC namespace

    ```
    bool IsBasicBlockInSoftMax(AscendC::tiling::SoftMaxTiling& tiling, const uint32_t dataTypeSize = 2)
    ```

## Parameters

**Table 1**  Parameter List

| API | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | SoftMaxTiling structure to be determined, which supports the optiling::SoftMaxTiling form and the AscendC::tiling::SoftMaxTiling form. |
| dataTypeSize | Input | Data type size of the srcTensor involved in the computation, for example, half=2. |

## Return Value

-   Returns true if the SoftMaxTiling structure conforms to the basic block Tiling feature.
-   Returns false if the SoftMaxTiling structure does not conform to the basic block Tiling feature.

## Constraints

None
