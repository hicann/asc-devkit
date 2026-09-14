# GetBatchTensorC

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:25:19.614Z pushedAt=2026-09-12T09:55:18.093Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

The slices of matrix C are obtained each time **GetBatchTensorC** is called. This API can be used together with the asynchronous API [IterateNBatch](IterateNBatch.md). After **IterateNBatch** is called for iterative computation, this API obtains a matrix slice with the size of **std::max\(batchA, batchB\) \* singleCoreM \* singleCoreN**.

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
| sync | Asynchronous mode, that is, this parameter can only be set to **false**. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| batchA | Input | Number of batches of the left matrix. |
| batchB | Input | Number of batches of the right matrix. |
| enSequentialWrite | Input | Reserved parameter. Keep the default value **false**. |
| c | Input | Address where matrix C is placed in the Local Memory, which is used to store the matrix slices. |

## Return Value

GlobalTensor<DstT\>, the computed matrix slice is returned.

## Constraints

-   When **MixDualMaster** (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.
-   When the C matrix slice is output to the Local Memory and the size of **singleCoreN** along the N axis in the single-core computation not 32 bytes, the **CubeFormat** of the C matrix supports only the **ND\_ALIGN** format. When the C matrix slice is output, the data in the **singleCoreN** direction is automatically supplemented to 32 bytes.

## Examples

```
// Calculate the number of loop iterations required for multi-batch computation.
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
