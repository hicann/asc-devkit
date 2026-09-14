# SetOrgShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:01:17.098Z pushedAt=2026-09-12T09:55:18.142Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Sets the original complete shape **M**, **N**, and **K** of the **Matmul** computation, in units of elements. It is used to modify the shape at runtime, for example, reusing the same **Matmul** object to fetch data from different matrix blocks for computation.

## Prototype

```
__aicore__ inline void SetOrgShape(int orgM, int orgN, int orgK)
```

```
__aicore__ inline void SetOrgShape(int orgM, int orgN, int orgKa, int orgKb, int orgKc = 0)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| orgM | Input | Set the original complete shape M size, in elements.<br><br>For Ascend 950PR/Ascend 950DT, when this interface is called on a **Matmul** object created using the [MDL template](MatmulConfig.md), this parameter is used for setting the complete shape M size on GM or L1, in elements. |
| orgN | Input | Set the original complete shape N size, in elements.<br><br>For Ascend 950PR/Ascend 950DT, when this interface is called on a **Matmul** object created using the [MDL template](MatmulConfig.md), this parameter is used for setting the complete shape N size on GM or L1, in elements. |
| orgK | Input | Set the original complete shape K size, in elements. It can be set when the original complete shape Ka=Kb.<br><br>For Ascend 950PR/Ascend 950DT, when this interface is called on a **Matmul** object created using the [MDL template](MatmulConfig.md), this parameter is used for setting the complete shape K size on GM or L1, in elements. It can be set when the original complete shape Ka=Kb. |
| orgKa | Input | Set the original complete shape Ka size of matrix A, in elements.<br><br>For Ascend 950PR/Ascend 950DT, when this interface is called on a **Matmul** object created using the [MDL template](MatmulConfig.md), this parameter is used for setting the complete shape Ka size on GM or L1, in elements. |
| orgKb | Input | Set the original complete shape Kb size of matrix B, in elements.<br><br>For Ascend 950PR/Ascend 950DT, when this interface is called on a **Matmul** object created using the [MDL template](MatmulConfig.md), this parameter is used for setting the complete shape Kb size on GM or L1, in elements. |
| orgKc | Input | Set the N of output matrix C, in elements. It can be set when the N of input matrix B and the N of output matrix C are different. Defaults to 0 (that is, the N of matrix B is used without modification). |

> [!CAUTION] Note
> - For a **Matmul** object created using the [MDL template](MatmulConfig.md) on Ascend 950PR/Ascend 950DT, when the shape of the data on L1 is consistent with **orgMIn**/**orgNIn**/**orgKIn**/**orgKaIn**/**orgKbIn** in the Tiling-side API [SetOrgShape](../Matmul-Tiling/SetOrgShape-87.md), it is not necessary to call this interface.
> - For a **Matmul** object created using the [MDL template](MatmulConfig.md) on Ascend 950PR/Ascend 950DT, when the shape of the data on L1 is inconsistent with **orgMIn**/**orgNIn**/**orgKIn**/**orgKaIn**/**orgKbIn** in the Tiling-side API [SetOrgShape](../Matmul-Tiling/SetOrgShape-87.md), you must call this interface to specify **orgM**/**orgN**/**orgK**/**orgKa**/**orgKb** on GM/L1.
>   For example, when using the [MDL template](MatmulConfig.md), in the scenario where input matrix A is on L1 and input matrix B is on GM, when the shape size of A on L1 is inconsistent with the original **orgMIn**/**orgKIn**/**orgKaIn** on the Tiling side, call the SetOrgShape\(orgM, orgN, orgK\)/SetOrgShape\(orgM, orgN, orgKa, orgKb\) API to specify the parameters **orgM**/**orgK**/**orgKa** related to matrix A on L1.

## Return Value

None

## Constraints

This API must be called before the **SetTensorA**, **SetTensorB**, **SetBias**, and **SetSingleShape** APIs.

## Examples

-   Set the original complete shape of the matrix.

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
    mm.SetTensorA(gm_a);
    mm.SetTensorB(gm_b);
    mm.SetBias(gm_bias);
    mm.IterateAll(gm_c);
    //  Reuse the mm object.
    mm.SetOrgShape(orgM, orgN, orgK);
    mm.SetTensorA(gm_a1);
    mm.SetTensorB(gm_b1);
    mm.SetBias(gm_bias1);
    mm.IterateAll(gm_c1);
    ```

-   For a Matmul object that uses the [MDL template](MatmulConfig.md) on Ascend 950PR/Ascend 950DT, set the complete shape on GM or L1.

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
