# ClearBias

<!-- md-trans-meta sourceCommit=4f4939e215c7dcbcebb0b665d1de1aeb4c46f587 translatedAt=2026-09-09T06:22:34.769Z pushedAt=2026-09-12T09:55:18.085Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

The [DisableBias](DisableBias.md) API provides the same functionality as this API. You are advised to use [DisableBias](DisableBias.md).

Clears the bias flag, indicating that no bias is involved in Matmul computation. If the bias is enabled by configuring the **isBias** parameter in the **TCubeTiling** structure when calling [Init](Init-85.md), you can call this API to clear the bias flag and disable the bias.

## Prototype

```
__aicore__ inline void ClearBias()
```

## Parameter

None

## Return Value

None

## Constraints

None

## Calling Example

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.ClearBias();    // Clear the bias flag in tiling.
mm.IterateAll(gm_c);
```
