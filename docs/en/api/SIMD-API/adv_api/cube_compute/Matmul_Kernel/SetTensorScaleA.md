# SetTensorScaleA

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:57:54.550Z pushedAt=2026-10-27T11:32:51.486Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleA_res.md#id1 -->

## Description

In the MxMatmul scenario, sets the quantization coefficient cube scaleA of the left cube in cube multiplication.

## Prototype

```
__aicore__ inline void SetTensorScaleA(const GlobalTensor<ScaleT>& gm, bool isTransposeScaleA = false);
```

```
__aicore__ inline void SetTensorScaleA(const LocalTensor<ScaleT>& leftMatrix, bool isTransposeScaleA = false);
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Input | Quantization coefficient scaleA cube. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The supported data type is fp8_e8m0_t. |
| leftMatrix | Input | Quantization coefficient scaleA cube. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is TSCM/VECOUT. The supported data type is fp8_e8m0_t. |
| isTransposeScaleA | Input | Whether the scaleA cube needs to be transposed.<br><br>The supported values of this parameter are as follows:<br>false: Default value. The scaleA cube is not transposed. true: The scaleA cube is transposed.<br><br>Note:<br>When the scaleA cube is in NZ format, this parameter supports only the value false.<br>If SCALE_ISTRANS in the MatmulTypeWithScale parameter of the scaleA cube is set to true, except for the scenario where scaleA is in NZ format, this parameter supports the values true and false, meaning that at runtime the scaleA cube can be used alternately in transposed and non-transposed forms.<br>If SCALE_ISTRANS in the MatmulTypeWithScale parameter of the scaleA cube is set to false, this parameter supports only the value false. If it is forcibly set to true, the precision will be abnormal.<br><br>For scenarios with Bias input, to ensure that the L1 Buffer space size calculated on the Tiling side and the Kernel side remains consistent and the result precision is correct, the value of this parameter must be consistent with the SCALE_ISTRANS parameter of MatmulTypeWithScale that defines cube A on the Kernel side and the [isScaleTrans](../Matmul_Tiling/SetScaleAType.md#p2934103115919) parameter of the SetScaleAType() API on the Tiling side. That is, in scenarios with Bias input, the above three parameters must all be set to true or all be set to false. |

## Return Value

None

## Constraints

-   The address space size of the passed-in **scaleA** must be no smaller than [singleCoreM](../Matmul_Tiling/TCubeTiling_struct.md)\*[singleCoreK](../Matmul_Tiling/TCubeTiling_struct.md)/32.
-   When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetTensorScaleA(gm_scaleA);    // Set the quantization coefficient cube scaleA of the left cube.
mm.SetTensorScaleB(gm_scaleB);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
```
