# GetBatchC

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:18:54.366Z pushedAt=2026-10-27T08:18:29.442Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetBatchC_res.md#id1 -->

## Description

The [GetBatchTensorC](GetBatchTensorC.md) API has the same functionality as this API. It is recommended to use [GetBatchTensorC](GetBatchTensorC.md).

Calling **GetBatchC** once obtains a C cube tile. This API can be used together with the asynchronous [IterateNBatch](IterateNBatch.md) API. It is used to obtain a cube tile of size std::max\(batchA, batchB\) \* **singleCoreM** \* **singleCoreN** after calling **IterateNBatch** for iterative computation.

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

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| sync | Sets the synchronous or asynchronous mode through this parameter: set to **true** for synchronous mode and **false** for asynchronous mode. The default is synchronous mode. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchA | Input | Batch number of the left cube. |
| batchB | Input | Batch number of the right cube. |
| enSequentialWrite | Input | Whether the output data is stored contiguously. The default is **false** (non-sequential write mode). |
| c | Input | C cube, used to store cube tiles. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). |

## Return Value

GlobalTensor<DstT\>, returns the computed cube tile.

## Constraints

When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.

## Examples

```
// Calculate the number of multi-batch computation loop iterations.
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
