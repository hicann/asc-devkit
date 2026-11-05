# SetOrgShape

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:50:48.368Z pushedAt=2026-10-27T11:29:58.450Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetOrgShape_res.md#id1 -->

## Description

Sets the original complete shape M, N, and K for Matmul computation, in elements. Used to modify the shape at runtime, for example, reusing the same Matmul object to fetch data from different cube blocks for computation.

## Prototype

```
__aicore__ inline void SetOrgShape(int orgM, int orgN, int orgK)
```

```
__aicore__ inline void SetOrgShape(int orgM, int orgN, int orgKa, int orgKb, int orgKc = 0)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| orgM | Input | Sets the original complete shape M size, in elements. <!-- npu="950" id8 --><br><br>For the Ascend 950PR&950DT products, when using this API with a Matmul object created by the [MDL template](MatmulConfig.md), this parameter is used to set the complete shape M size on GM or L1, in elements. <!-- end id8 --> |
| orgN | Input | Sets the original complete shape N size, in elements. <!-- npu="950" id10 --><br><br>For the Ascend 950PR&950DT products, when using this API with a Matmul object created by the [MDL template](MatmulConfig.md), this parameter is used to set the complete shape N size on GM or L1, in elements. <!-- end id10 --> |
| orgK | Input | Sets the original complete shape K size, in elements. Can be set when the original complete shape Ka=Kb. <!-- npu="950" id11 --><br><br>For the Ascend 950PR&950DT products, when using this API with a Matmul object created by the [MDL template](MatmulConfig.md), this parameter is used to set the complete shape K size on GM or L1, in elements. Can be set when the original complete shape Ka=Kb. <!-- end id11 --> |
| orgKa | Input | Sets the original complete shape Ka size of cube A, in elements. <!-- npu="950" id12 --><br><br>For the Ascend 950PR&950DT products, when using this API with a Matmul object created by the [MDL template](MatmulConfig.md), this parameter is used to set the complete shape Ka size on GM or L1, in elements. <!-- end id12 --> |
| orgKb | Input | Sets the original complete shape Kb size of cube B, in elements. <!-- npu="950" id13 --><br><br>For the Ascend 950PR&950DT products, when using this API with a Matmul object created by the [MDL template](MatmulConfig.md), this parameter is used to set the complete shape Kb size on GM or L1, in elements. <!-- end id13 --> |
| orgKc | Input | Sets the N of output cube C, in elements. Can be set when the N of input cube B and the N of output cube C are different, defaulting to 0 (that is, using the N of cube B without modification). |

<!-- npu="950" id14 -->
> [!CAUTION]
>-   For a Matmul object created using the [MDL template](MatmulConfig.md) on Ascend 950PR&950DT products, when the shape of the data on L1 is consistent with orgMIn/orgNIn/orgKIn/orgKaIn/orgKbIn in the Tiling-side API [SetOrgShape](../Matmul_Tiling/SetOrgShape.md), it is not necessary to call this API.
>-   For a Matmul object created using the [MDL template](MatmulConfig.md) on Ascend 950PR&950DT products, when the shape of the data on L1 is inconsistent with orgMIn/orgNIn/orgKIn/orgKaIn/orgKbIn in the Tiling-side API [SetOrgShape](../Matmul_Tiling/SetOrgShape.md), you must call this API to specify orgM/orgN/orgK/orgKa/orgKb on GM/L1.
>    For example, when using the [MDL template](MatmulConfig.md), in the scenario where input cube A is on L1 and input cube B is on GM, if the shape size of A on L1 is inconsistent with the original orgMIn/orgKIn/orgKaIn on the Tiling side, call SetOrgShape\(orgM, orgN, orgK\)/SetOrgShape\(orgM, orgN, orgKa, orgKb\) to specify the parameters orgM/orgK/orgKa related to cube A on L1.
<!-- end id14 -->

## Return Value

None

## Constraints

This API must be called before the **SetTensorA**, **SetTensorB**, **SetBias**, and **SetSingleShape** APIs.

## Examples

-   Set the original complete shape of the cube.

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gm_a);
    mm.SetTensorB(gm_b);
    mm.SetBias(gm_bias);
    mm.IterateAll(gm_c);
    // Reuse the mm object.
    mm.SetOrgShape(orgM, orgN, orgK);
    mm.SetTensorA(gm_a1);
    mm.SetTensorB(gm_b1);
    mm.SetBias(gm_bias1);
    mm.IterateAll(gm_c1);
    ```

<!-- npu="950" id9 -->
-   For a Matmul object using the [MDL template](MatmulConfig.md) on Ascend 950PR&950DT products, set the complete shape on GM or L1.

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    for (int m = 0; m < mIter_; m++) {
         for (int n = 0; n < nIter_; n++) {
              for (int k = 0; k < kIter_; k++) {
                   // Reuse mm and specify the shapes of A on L1 and B on GM.
                   mm.SetOrgShape(alignedSingleM, tiling.N, alignedSingleK, tiling.Kb, tiling.N);
                   mm.SetSingleShape(curBaseM, curBaseN, curBaseK);
                   mm.SetTensorA(tscm_a[offset_a]); // Set aMatrix tscm input
                   mm.SetTensorB(gm_b[offset_b]);
                   mm.SetBias(gm_bias[offset_bias]);
                   mm.Iterate(k != 0);
                }
                matmulObj.GetTensorC(gm_c[offset_c]);
         }
    }
    ```
<!-- end id9 -->
