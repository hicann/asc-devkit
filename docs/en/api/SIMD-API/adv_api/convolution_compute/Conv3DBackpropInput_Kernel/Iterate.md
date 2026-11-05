# Iterate

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:39:45.705Z pushedAt=2026-10-29T02:14:34.316Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropInput_Kernel/Iterate_res.md#id1 -->

## Description

Each call to Iterate computes a baseM \* baseN result matrix and writes it to the L0C Buffer. The API internally maintains the iteration progress, and after each call, the start address of the matrix is offset. If the input data is not aligned and a tail block exists, the computation result of the tail block is output in the last iteration. This API must be used together with the [GetTensorC](GetTensorC.md) API. After calling this API, call the GetTensorC API to write the data in the L0C Buffer to the destination address.

## Prototype

```
template <bool sync = true>
__aicore__ inline bool Iterate(bool enPartialSum = false)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enPartialSum | Input | Reserved parameter, not perceivable by you. |

## Return Value

false: All data on the configured SingleShape has been computed.

true: The data is still being iteratively computed.

## Constraints

-   The Iterate API must be called after the initialization API and the input/output configuration API to complete the convolution backward computation. The calling sequence is as follows.

    ```
    Init(...);
    ... // Input/output configuration
    while (Iterate()) {
        GetTensorC();
    }
    End();
    ```

-   In a multi-round loop computation scenario, the data size set by SetSingleShape for a single core is computed in a single loop. After each single-core computation is completed, the ctx.isFirstIter\_ of the Conv3DBackpropInput object must be set to true to ensure that the single-core computation in the next loop can proceed correctly.

## Examples

```
while (gradInput_.Iterate()) {
    gradInput_.GetTensorC(gradInputGm_[offsetC_]);
}
// After the SingleShape computation is completed, set ctx.isFirstIter_ to true to ensure correct computation of the next SingleShape block.
gradInput_.ctx.isFirstIter_ = true;
```
