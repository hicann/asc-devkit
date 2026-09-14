# SetTensorScaleB

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:06:17.621Z pushedAt=2026-09-12T09:55:18.155Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

In the MxMatmul scenario, sets the quantization coefficient matrix scaleB of the right matrix in matrix multiplication.

## Prototype

```
__aicore__ inline void SetTensorScaleB(const GlobalTensor<ScaleT>& gm, bool isTransposeScaleB = true);
```

```
__aicore__ inline void SetTensorScaleB(const LocalTensor<ScaleT>& rightMatrix, bool isTransposeScaleB = true);
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Input | Quantization coefficient scaleB matrix. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). Supported data type: fp8_e8m0_t. |
| rightMatrix | Input | Quantization coefficient scaleB matrix. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), with supported TPosition of TSCM/VECOUT. Supported data type: fp8_e8m0_t. |
| isTransposeScaleB | Input | Whether the scaleB matrix needs to be transposed.<br><br>The supported values of this parameter are as follows:<br>false: The scaleB matrix is not transposed. true: Default value, the scaleB matrix is transposed.<br><br>Note:<br>When the scaleB matrix is in NZ format, this parameter supports only the value true.<br>If SCALE_ISTRANS in the MatmulTypeWithScale parameter of the scaleB matrix is set to true, this parameter supports both true and false, except when scaleB is in NZ format. That is, at runtime the scaleB matrix can be used alternately in transposed and non-transposed forms.<br>If SCALE_ISTRANS in the MatmulTypeWithScale parameter of the scaleB matrix is set to false, this parameter supports only the value false. If it is forcibly set to true, precision will be abnormal.<br><br>For scenarios with Bias input, to ensure that the L1 buffer space sizes computed on the Tiling side and the Kernel side remain consistent and that the result precision is correct, the value of this parameter must be consistent with the SCALE_ISTRANS parameter of MatmulTypeWithScale that defines the B matrix on the Kernel side and with the [isScaleTrans](../Matmul-Tiling/SetScaleBType.md) parameter of the SetScaleBType() API on the Tiling side. That is, in scenarios with Bias input, the above three parameters must all be set to true or all be set to false. |

## Return Value

None

## Constraints

-   The address space size of the passed scaleB must be no smaller than [singleCoreK](../Matmul-Tiling/tcubetiling_structure.md)\*[singleCoreN](../Matmul-Tiling/tcubetiling_structure.md)/32.
-   When MixDualMaster is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetTensorScaleA(gm_scaleA);
mm.SetTensorScaleB(gm_scaleB);    // Set the quantization coefficient matrix scaleB of the right matrix.
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
```
