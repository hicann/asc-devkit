# IsBasicBlockInSoftMax

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T13:44:56.409Z -->

## Description

Checks whether the **SoftMaxTiling** structure conforms to the basic block characteristics.

## Prototype

-   Computation API in the **AscendC::optiling** namespace

    ```
    bool IsBasicBlockInSoftMax(optiling::SoftMaxTiling& tiling, const uint32_t dataTypeSize = 2)
    ```

-   Computation API in the **AscendC** namespace

    ```
    bool IsBasicBlockInSoftMax(AscendC::tiling::SoftMaxTiling& tiling, const uint32_t dataTypeSize = 2)
    ```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **tiling** | Input | **SoftMaxTiling** structure to be checked, which supports both the **optiling::SoftMaxTiling** form and the **AscendC::tiling::SoftMaxTiling** form. |
| **dataTypeSize** | Input | Data type size of the **srcTensor** involved in the computation, for example, **half** = 2. |

## Return Value

-   Returns **true** if the **SoftMaxTiling** structure conforms to the basic block tiling characteristics.
-   Returns **false** if the **SoftMaxTiling** structure does not conform to the basic block tiling characteristics.

## Constraints

None

