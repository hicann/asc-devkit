# SetLocalWorkspace

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:44:34.232Z pushedAt=2026-11-18T04:53:35.930Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Not supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Not supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Not supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030 processor products: Not supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetLocalWorkspace_res.md#id1 -->

## Description

In certain scenarios, Matmul internally requires additional Unified Buffer (UB, VECCALC) space. If you want to reuse this additionally occupied UB (VECCALC) space in the operator, you must reserve the space, allocate a **LocalTensor**, and pass its starting physical address to Matmul. The specific size of the UB (VECCALC) temporary space to be allocated is provided by the tiling API [MatmulGetTmpBufSize](../get_Matmul_compute_space/MatmulGetTmpBufSize.md). This API must be used to pass the UB temporary space if any of the following conditions is met:

-   The position of cube C is **TPosition::GM**;
-   The CubeFormat of cube C is CubeFormat::ND.
-   The CubeFormat of cube A or cube B is CubeFormat::ND.
-   Bias exists and the Position of Bias is not VECCALC.

Call this API before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

The size of the obtained UB temporary space is in bytes.

## Prototype

```
__aicore__ inline void SetLocalWorkspace(const LocalTensor<uint8_t>& tmpBuffer)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tmpBuffer | Input | Temporary space, allocated and managed by the user, with TPosition set to VECCALC. |

## Return Value

None

## Constraints

When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetLocalWorkspace(mmFormatUb);    // Set the temporary VECCALC space.
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(biasGlobal);
}
mm.IterateAll(gm_c);
mm.End();
```
