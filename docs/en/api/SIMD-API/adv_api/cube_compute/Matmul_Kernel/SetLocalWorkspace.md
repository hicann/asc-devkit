# SetLocalWorkspace

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:49:16.181Z pushedAt=2026-10-27T11:29:38.440Z nl=2 -->

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
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetLocalWorkspace_res.md#id1 -->

## Description

In certain scenarios, Matmul internally requires additional Unified Buffer (UB, VECCALC) space. If you want to reuse this additionally occupied UB (VECCALC) space in the operator, you must reserve the space, apply for a **LocalTensor**, and pass its starting physical address to Matmul. The specific size of the UB (VECCALC) temporary space to be applied for is provided by the tiling API [MatmulGetTmpBufSize](../get_Matmul_compute_space/MatmulGetTmpBufSize.md). This API must be used to pass the UB temporary space if any of the following conditions is met:

-   The Position of cube C is **TPosition::GM**;
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
| tmpBuffer | Input | Temporary space, applied for and managed by the user, with TPosition set to VECCALC. |

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
