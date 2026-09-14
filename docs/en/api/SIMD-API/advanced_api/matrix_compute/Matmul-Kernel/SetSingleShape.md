# SetSingleShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:00:26.128Z pushedAt=2026-09-12T09:55:18.139Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Sets the shapes **singleCoreM**, **singleCoreN**, and **singleCoreK** for single-core Matmul computation, in elements. It is used to modify the shape at runtime, for example, to reuse a Matmul object to process the tail block. Its functionality is the same as that of the **SetTail** API, and this API is recommended.

## Prototype

```
__aicore__ inline void SetSingleShape(int singleM, int singleN, int singleK)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| singleM | Input | Size of **singleCoreM** to set, in elements. |
| singleN | Input | Size of **singleCoreN** to set, in elements. |
| singleK | Input | Size of **singleCoreK** to set, in elements. |

## Return Value

None

## Constraints

None

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);

// tailM: number of remaining elements in the M direction, tailN: number of remaining elements in the N direction, tailK: number of remaining elements in the K direction.
// If this is a tail core, the remaining elements may be fewer than the elements that a single core needs to compute. In this case, use SetSingleShape to reset the number of elements to compute this time.
if (tailM < tiling.singleCoreM || tailN < tiling.singleCoreN || tailK < tiling.singleCoreK) {
    matmulObj.SetSingleShape(tailM, tailN, tailK);
}

mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
```
