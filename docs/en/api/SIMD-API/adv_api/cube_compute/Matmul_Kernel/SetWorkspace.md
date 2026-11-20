# SetWorkspace

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:57:47.506Z pushedAt=2026-11-18T04:54:24.187Z nl=2 -->

## Applicable Products

<!-- npu="950" id2 -->
- Ascend 950PR&950DT products: Supported
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 products: Supported
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 products: Supported
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Supported
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetWorkspace_res.md#id1 -->

## Description

In the [asynchronous scenario](GetTensorC.md#li17508136205415) of [Iterate](Iterate.md) computation, call this API to allocate a temporary workspace to cache the computation results, and then when calling [GetTensorC](GetTensorC.md), the cube tile of C is obtained from this temporary workspace.

During [IterateNBatch](IterateNBatch.md) computation, call this API to allocate a temporary workspace to cache the computation results, and then call other APIs according to the [synchronous or asynchronous scenario](IterateNBatch.md).

## Prototype

It is recommended that users pass in the **GlobalTensor** type:

```
template <class T> __aicore__ inline void SetWorkspace(GlobalTensor<T>& addr)
```

```
template <class T> __aicore__ inline void SetWorkspace(__gm__ const T* addr, int size)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| addr | Input | Workspace space on the GM passed in by the user, of the **GlobalTensor** type. |
| addr | Input | Workspace space on the GM passed in by the user, of the GM address type. |
| size | Input | When a GM address is passed in, the number of elements must also be passed in. |

## Return Value

None

## Constraints

When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetWorkspace(workspaceGM);    // Set the temporary workspace used in asynchronous scenarios.
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
