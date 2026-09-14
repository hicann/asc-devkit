# SetLocalWorkspace

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:57:02.436Z pushedAt=2026-09-12T09:55:18.132Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

In certain scenarios, **Matmul** internally requires additional **VECCALC** space. If you want to reuse this additionally occupied **VECCALC** space in the operator, you must reserve the space, apply for a **LocalTensor**, and pass its start physical address to **Matmul**. The specific size of the **VECCALC** temporary space to be applied for is provided by the tiling API [MatmulGetTmpBufSize](../get_matmul_compute_space/MatmulGetTmpBufSize.md). This API must be used to pass the UB temporary space if any of the following conditions is met:

-   The **Position** of the C matrix is **TPosition::GM**.
-   The **CubeFormat** of the C matrix is **CubeFormat::ND**.
-   The **CubeFormat** of the A matrix or the B matrix is **CubeFormat::ND**.
-   A **Bias** exists and the **Position** of the **Bias** is not **VECCALC**.

Call this API before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

The obtained UB temporary space size is in bytes.

## Prototype

```
__aicore__ inline void SetLocalWorkspace(const LocalTensor<uint8_t>& tmpBuffer)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **tmpBuffer** | Input | Temporary space, applied for and managed by the user, with **TPosition** set to **VECCALC**. |

## Return Value

None

## Constraints

When the MixDualMaster (dual-master) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetLocalWorkspace(mmFormatUb);    //Set the temporary VECCALC space.
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(biasGlobal);
}
mm.IterateAll(gm_c);
mm.End();
```
