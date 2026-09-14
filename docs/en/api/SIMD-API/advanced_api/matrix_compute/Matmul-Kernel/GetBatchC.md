# GetBatchC

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:24:55.066Z pushedAt=2026-09-12T09:55:18.090Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

The [GetBatchTensorC](GetBatchTensorC.md) API provides the same functionality as this API. You are advised to use [GetBatchTensorC](GetBatchTensorC.md).

The slices of matrix C are obtained each time **GetBatchC** is called. This API can be used together with the asynchronous API [IterateNBatch](IterateNBatch.md). After **IterateNBatch** is called for iterative computation, this API obtains a matrix slice with the size of **std::max\(batchA, batchB\) \* singleCoreM \* singleCoreN**.

## Prototype

```
template <bool sync = true>
__aicore__ inline GlobalTensor<DstT> GetBatchC(uint32_t batchA, uint32_t batchB, bool enSequentialWrite = false)
```

```
template <bool sync = true>
__aicore__ inline void GetBatchC(const LocalTensor<DstT>& c, uint32_t batchA, uint32_t batchB, bool enSequentialWrite = false)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| sync | Synchronous or asynchronous mode. The value **true** indicates synchronous mode and **false** indicates asynchronous mode. The default value is **true**. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchA | Input | Batch count of the left matrix. |
| batchB | Input | Batch count of the right matrix. |
| enSequentialWrite | Input | Whether the output data is stored sequentially. The default value is **false**, indicating non-sequential write mode. |
| c | Input | C matrix, used to store the matrix slices. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). |

## Return Value

GlobalTensor<DstT\>, the computed matrix slice is returned.

## Constraints

When **MixDualMaster** (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.

## Examples

```
// Compute the number of iterations required for multi-batch computation.
int g_lay = tiling.ALayoutInfoG > tiling.BLayoutInfoG ? tiling.ALayoutInfoG : tiling.BLayoutInfoG;
int for_exent = tiling.ALayoutInfoB * tiling.ALayoutInfoN * g_lay / tiling.BatchNum;
mm1.SetTensorA(gm_a[0], isTransposeAIn);
mm1.SetTensorB(gm_b[0], isTransposeBIn);
if (tiling.isBias) {
    mm1.SetBias(gm_bias[0]);
}
// Perform multi-batch Matmul computation.
mm1.template IterateNBatch<false>(for_exent, batchA, batchB, false);
// ...other compute
for (int i = 0; i < for_exent ; ++i) {
    mm1.template GetBatchC<false>(ubCmatrix, batchA, batchB);
    // ...other compute
}
```
