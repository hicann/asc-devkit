# SetTail

<!-- md-trans-meta sourceCommit=4f4939e215c7dcbcebb0b665d1de1aeb4c46f587 translatedAt=2026-09-09T07:02:44.053Z pushedAt=2026-09-12T09:55:18.146Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Without changing the Tiling, resets **singleCoreM**/**singleCoreN**/**singleCoreK** for this computation, in elements.

## Prototype

```
__aicore__ inline void SetTail(int tailM = -1, int tailN = -1, int tailK = -1)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **tailM** | Input | Reset **singleCoreM** value. |
| **tailN** | Input | Reset **singleCoreN** value. |
| **tailK** | Input | Reset **singleCoreK** value. |

## Return Value

None

## Constraints

None

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);

// tailM: number of remaining elements in the M direction, tailN: number of remaining elements in the N direction, tailK: number of remaining elements in the K direction.
// If this is a tail core, the remaining elements may be fewer than the elements that a single core needs to compute. In this case, use SetTail to reset the number of elements to be computed this time.
if (tailM < tiling.singleCoreM || tailN < tiling.singleCoreN || tailK < tiling.singleCoreK) {
    matmulObj.SetTail(tailM, tailN, tailK);
}

mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
```
