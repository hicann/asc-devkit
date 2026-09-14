# SetHF32

<!-- md-trans-meta sourceCommit=4f4939e215c7dcbcebb0b665d1de1aeb4c46f587 translatedAt=2026-09-09T06:55:57.461Z pushedAt=2026-09-12T09:55:18.130Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

In pure Cube mode (matrix computation only), sets whether to enable the HF32 mode (a data type that can be used in matrix multiplication). When enabled, the float32 data type is converted to the hf32 data type during matrix multiplication, which improves computation performance but also introduces precision loss.

## Prototype

```
__aicore__ inline void SetHF32(bool enableHF32 = false, int32_t transMode = 0)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **enableHF32** | Input | Whether to enable HF32 mode, defaulting to **false** (disabled). |
| **transMode** | Input | Rounding mode used when converting float to hf32 with HF32 mode enabled, defaulting to **0**.<br><br>**0**: Round to the nearest value, with ties rounding to even.<br><br>**1**: Round to the nearest value, with ties rounding away from zero. |

## Return Value

None

## Constraints

This API can be called only in pure Cube mode.

## Examples

```
//Cube-only mode
#define ASCENDC_CUBE_ONLY
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);    //  The A/B/C/BIAS type is float
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
