# SetHF32

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:48:03.343Z pushedAt=2026-10-27T11:29:28.286Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetHF32_res.md#id1 -->

## Description

In pure Cube mode (cube computation only), sets whether to enable the HF32 mode (a data type that can be used in cube multiplication). When enabled, the float32 data type is converted to the HF32 data type during cube multiplication, which improves computation performance but also introduces precision loss.

## Prototype

```
__aicore__ inline void SetHF32(bool enableHF32 = false, int32_t transMode = 0)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| enableHF32 | Input | Configures whether to enable the HF32 mode. The default value is **false** (disabled). |
| transMode | Input | Configures the ROUND mode used when converting float to hf32 with the HF32 mode enabled. The default value is **0**.<br><br>**0**: Round to the nearest value, and round to an even number when the distances are equal.<br><br>**1**: Round to the nearest value, and round away from 0 when the distances are equal. |

## Return Value

None

## Constraints

This API can be called only in pure Cube mode.

## Examples

```
// Pure Cube mode.
#define ASCENDC_CUBE_ONLY
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);    // A/B/C/BIAS types are float.
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.SetHF32(true);
mm.IterateAll(gm_c);
mm.SetHF32(false);
mm.End();
```
