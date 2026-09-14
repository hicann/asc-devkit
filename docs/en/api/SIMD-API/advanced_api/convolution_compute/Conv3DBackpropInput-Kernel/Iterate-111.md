# Iterate

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:31:48.545Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Each call to **Iterate** computes a baseM \* baseN result matrix and writes the result to the L0C Buffer. The API internally maintains the iteration progress, and after each call, the start address of the matrix is offset. If the input data is not aligned and a tail block exists, the computation result of the tail block is output in the last iteration. This API must be used together with the [GetTensorC](GetTensorC-112.md) API; after calling this API, call the GetTensorC API to write the data in the L0C Buffer to the destination address.

## Prototype

```
template <bool sync = true>
__aicore__ inline bool Iterate(bool enPartialSum = false)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enPartialSum | Input | Reserved parameter, which users do not need to be aware of. |

## Return Value

false: All data on the configured SingleShape has been computed.

true: The data is still being iteratively computed.

## Constraints

-   The **Iterate** API must be called after the initialization API and the input/output configuration API to complete the convolution backward computation. The calling sequence is as follows.

    ```
    Init(...);
    ... // Input/output configuration
    while (Iterate()) {
        GetTensorC();
    }
    End();
    ```

-   In a multi-round loop computation scenario, the data size set by the single-core SetSingleShape is computed in a single loop. After each single-core computation is completed, the ctx.isFirstIter\_ of the Conv3DBackpropInput object must be set to true to ensure that the single-core computation in the next loop can proceed correctly.

## Examples

```
while (gradInput_.Iterate()) {
    gradInput_.GetTensorC(gradInputGm_[offsetC_]);
}
// After the SingleShape computation is complete, set ctx.isFirstIter_ to true to ensure correct computation of the next SingleShape block.
gradInput_.ctx.isFirstIter_ = true;
```

