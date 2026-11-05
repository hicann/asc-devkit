# SetTensorScaleB

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:59:11.823Z pushedAt=2026-10-27T11:33:03.170Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleB_res.md#id1 -->

## Description

In the MxMatmul scenario, sets the quantization coefficient cube scaleB of the right cube in cube multiplication.

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
| gm | Input | Quantization coefficient scaleB cube. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). Supported data type: fp8_e8m0_t. |
| rightMatrix | Input | Quantization coefficient scaleB cube. The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is TSCM/VECOUT. Supported data type: fp8_e8m0_t. |
| isTransposeScaleB | Input | Whether the scaleB cube needs to be transposed.<br><br>The supported values of this parameter are as follows:<br>**false**: The scaleB cube is not transposed. **true**: Default value. The scaleB cube is transposed.<br><br>Note:<br>When the scaleB cube is in NZ format, this parameter supports only the value **true**.<br>If **SCALE_ISTRANS** in the **MatmulTypeWithScale** parameter of the scaleB cube is set to **true**, except in the scenario where scaleB is in NZ format, this parameter supports the values **true** and **false**, meaning that at runtime the scaleB cube can alternate between transposed and non-transposed.<br>If **SCALE_ISTRANS** in the **MatmulTypeWithScale** parameter of the scaleB cube is set to **false**, this parameter supports only the value **false**. If it is forcibly set to **true**, precision will be abnormal.<br><br>For scenarios with Bias input, to ensure that the L1 Buffer space calculation size on the Tiling side and the kernel function (Kernel) side remains consistent and the result precision is correct, the value of this parameter must be consistent with the **SCALE_ISTRANS** parameter of the **MatmulTypeWithScale** of the B cube defined on the kernel function (Kernel) side and the [isScaleTrans](../Matmul_Tiling/SetScaleBType.md#p2934103115919) parameter of the **SetScaleBType()** API on the Tiling side. That is, in scenarios with Bias input, the above three parameters must all be set to **true** or all set to **false**. |

## Return Value

None

## Constraints

-   The size of the address space of the passed-in **scaleB** must be no smaller than [singleCoreK](../Matmul_Tiling/TCubeTiling_struct.md)\*[singleCoreN](../Matmul_Tiling/TCubeTiling_struct.md)/32.
-   When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.

## Examples

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetTensorScaleA(gm_scaleA);
mm.SetTensorScaleB(gm_scaleB);    // Set the quantization coefficient cube scaleB of the right cube.
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
```
