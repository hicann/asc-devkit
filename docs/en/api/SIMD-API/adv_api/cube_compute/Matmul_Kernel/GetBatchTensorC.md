# GetBatchTensorC

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T06:08:09.182Z pushedAt=2026-11-18T04:52:28.668Z nl=2 -->

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
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetBatchTensorC_res.md#id1 -->

## Description

Calling **GetBatchTensorC** once obtains a C cube tile. This API can be used together with the [IterateNBatch](IterateNBatch.md) asynchronous API. It is used to obtain a cube tile of size std::max\(batchA, batchB\) \* singleCoreM \* singleCoreN after calling IterateNBatch for iterative computation.

## Prototype

```
template <bool sync = true>
__aicore__ inline GlobalTensor<DstT> GetBatchTensorC(uint32_t batchA, uint32_t batchB, bool enSequentialWrite = false)
```

```
template <bool sync = true>
__aicore__ inline void GetBatchTensorC(const LocalTensor<DstT>& c, uint32_t batchA, uint32_t batchB, bool enSequentialWrite = false)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| sync | Currently only the asynchronous mode is supported, that is, this parameter only supports the value **false**. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchA | Input | Number of batches of the left cube. |
| batchB | Input | Number of batches of the right cube. |
| enSequentialWrite | Input | Reserved parameter. Keep the default value **false**. |
| c | Input | Address of the C cube in Local Memory, used to store the cube tile. |

## Return Value

GlobalTensor<DstT\>, returns the computed cube tile.

## Constraints

-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
-   When the C cube tile is output to Local Memory and the N-direction size **singleCoreN** computed by a single core is not 32-byte aligned, the CubeFormat of the C cube supports only the ND\_ALIGN format. When outputting the C cube tile, the data in the **singleCoreN** direction is automatically padded to 32 bytes.

## Examples

```
// Calculate the number of multi-batch compute loop iterations.
int for_extent = tiling.ALayoutInfoB * tiling.ALayoutInfoN * g_lay / tiling.BatchNum;
mm1.SetTensorA(gm_a[0], isTransposeAIn);
mm1.SetTensorB(gm_b[0], isTransposeBIn);
if (tiling.isBias) {
    mm1.SetBias(gm_bias[0]);
}
// Perform multi-batch Matmul computation.
mm1.template IterateNBatch<false>(for_extent, batchA, batchB, false);
...other compute
for (int i = 0; i < for_extent; ++i) {
    mm1.template GetBatchTensorC<false>(ubCmatrix);
    ...other compute
}
```
