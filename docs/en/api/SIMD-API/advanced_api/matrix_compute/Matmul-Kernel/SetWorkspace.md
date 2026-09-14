# SetWorkspace

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:08:04.057Z pushedAt=2026-09-12T09:55:18.154Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

In the [asynchronous scenario](GetTensorC.md) of [Iterate](Iterate.md) computation, call this API to apply for a temporary workspace to cache computation results. Then, when [GetTensorC](GetTensorC.md) is called, the matrix tile of C is obtained from this temporary workspace.

During [IterateNBatch](IterateNBatch.md) computation, call this API to apply for a temporary workspace to cache computation results, and then call other APIs according to the [synchronous or asynchronous scenario](IterateNBatch.md).

## Prototype

You are advised to pass the **GlobalTensor** type:

```
template <class T> __aicore__ inline void SetWorkspace(GlobalTensor<T>& addr)
```

```
template <class T> __aicore__ inline void SetWorkspace(__gm__ const T* addr, int size)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **addr** | Input | Workspace space on the GM passed by the user, of the GlobalTensor type. |
| **addr** | Input | Workspace space on the GM passed by the user, of the GM address type. |
| **size** | Input | Number of elements to be passed together with the GM address. |

## Return Value

None

## Constraints

When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetWorkspace(workspaceGM);    //Set the temporary workspace for asynchronous use.
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(biasGlobal);
}
mm.template Iterate<false>();
for (int i = 0; i < singleCoreM/baseM * singleCoreN/baseN; ++i) {
    mm.template GetTensorC<false>(ub_c);
}
```
