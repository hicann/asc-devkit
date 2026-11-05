# SetSingleShape

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:54:10.368Z pushedAt=2026-10-27T11:31:19.817Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetSingleShape_res.md#id1 -->

## Description

Sets the single-core computation shapes **singleCoreM**, **singleCoreN**, and **singleCoreK** of the Matmul kernel, in elements. It is used to modify the shape at runtime, for example, to reuse a Matmul object to process the tail block. Its functionality is the same as that of the **SetTail** API, and this API is recommended.

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
// If this is a tail core, the remaining elements may be fewer than the elements that a single core needs to compute. In this case, use SetSingleShape to reset the number of elements to be computed this time.
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
