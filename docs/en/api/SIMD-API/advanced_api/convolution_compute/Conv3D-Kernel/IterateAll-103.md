# IterateAll

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T14:06:00.713Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

By setting the base address of the result matrix **Output** in GM, this API computes a data block of size **singleCo** \* **singleDo** \* **singleM** in one pass and writes it to the result matrix **Output**.

This API provides the convolution computation capability within a single core. **singleCo** is the output channel size within a single core after multi-core splitting; **singleDo** is the Dout size within a single core after multi-core splitting; and **singleM** is the M size within a single core after multi-core splitting. The sizes of **singleCo**, **singleDo**, and **singleM** are set through the [SetSingleOutputShape](SetSingleOutputShape.md) API.

## Prototype

```
__aicore__ inline void IterateAll(const AscendC::GlobalTensor<OutputT>& output, bool enPartialSum = false)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| output | Input | Address of **Output** in GM. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The result matrix **Output** supports the following data types: half and bfloat16_t. |
| enPartialSum | Input | Reserved parameter. |

## Return Value

None

## Constraints

-   The **IterateAll** API supports processing only single-batch data. In multi-batch computation scenarios, the computation must be completed by calling the **IterateAll** API in a loop for each batch.

    ```
    for (uint64_t batchIter = 0; batchIter < singleCoreBatch; ++batchIter) {
        conv3dApi.SetInput(inputGm[batchIter * inputOneBatchSize]);
        conv3dApi.IterateAll(outputGm[batchIter * outputOneBatchSize]);
        conv3dApi.End();
    }
    ```

-   The **IterateAll** API must be called after the initialization API and the input/output configuration API to complete the Conv3D computation. The calling sequence is as follows.

    ```
    Init(...);
    ... // Configure input and output.
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
