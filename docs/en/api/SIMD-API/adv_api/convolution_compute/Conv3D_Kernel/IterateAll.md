# IterateAll

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T06:21:38.789Z pushedAt=2026-11-18T11:07:26.504Z nl=2 -->

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
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/IterateAll_res.md#id1 -->

## Description

By setting the base address of the result matrix Output in GM, this API computes a data block of singleCo \* singleDo \* singleM in a single call and writes it to the result matrix Output.

This API provides convolution computation capability within a single core. singleCo is the output channel size of a single core after multi-core splitting; singleDo is the Dout size of a single core after multi-core splitting; singleM is the M size of a single core after multi-core splitting. The sizes of singleCo, singleDo, and singleM are set through the [SetSingleOutputShape](SetSingleOutputShape.md) API.

## Prototype

```
__aicore__ inline void IterateAll(const AscendC::GlobalTensor<OutputT>& output, bool enPartialSum = false)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| output | Input | Address of Output in GM. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The result matrix Output supports the following data types: half and bfloat16_t. |
| enPartialSum | Input | Reserved parameter. |

## Return Value

None

## Constraints

-   The IterateAll API supports processing only single-batch data. In multi-batch computation scenarios, call the IterateAll API in a loop for each batch to complete the computation.

    ```
    for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
        conv3dApi.SetInput(inputGm[batchIter * inputOneBatchSize]);
        conv3dApi.IterateAll(outputGm[batchIter * outputOneBatchSize]);
        conv3dApi.End();
    }
    ```

-   The IterateAll API must be called after the initialization API and the input/output configuration APIs to complete Conv3D computation. The call sequence is as follows.

    ```
    Init(...);
    ... // Input/output configuration.
    IterateAll(...);
    End();
    ```

## Examples

```
TPipe pipe;
conv3dApi.Init(&tiling);
conv3dApi.SetWeight(weightGm);
if (biasFlag) {
    conv3dApi.SetBias(biasGm);
}
conv3dApi.SetInputStartPosition(diIdxStart, mIdxStart);
conv3dApi.SetSingleOutputShape(singleCoreCout, singleCoreDout, singleCoreM);
for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
    conv3dApi.SetInput(inputGm[batchIter * inputOneBatchSize]);
    conv3dApi.IterateAll(outputGm[batchIter * outputOneBatchSize]);
    conv3dApi.End();
}
```
