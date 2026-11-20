# SetHF32TransMode

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T04:40:44.948Z pushedAt=2026-11-18T01:25:06.813Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetHF32TransMode_res.md#id1 -->

## Description

Sets the specific rounding mode for HF32. You must first use [SetHF32Mode](SetHF32Mode.md) to enable the HF32 rounding mode.

## Function Prototype

```cpp
__aicore__ inline void SetHF32TransMode(HF32TransMode mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mode | Input | Mmad HF32 rounding mode control input parameter of the HF32TransMode type. The following two values are supported:<br>&nbsp;&nbsp;&bull; **NEAREST_AWAY**: FP32 is rounded to HF32 by rounding to the nearest value, with ties rounded away from zero.<br>&nbsp;&nbsp;&bull; **NEAREST_EVEN**: FP32 is rounded to HF32 by rounding to the nearest value, with ties rounded to even. |

## Return Value

None

## Constraints

None

## Example

- Sets the specific rounding mode for HF32. You must first use [SetHF32Mode](SetHF32Mode.md) to enable the HF32 rounding mode.
- This API is compatible with the enumeration value `NEAREST_ZERO`, but its use is not recommended; the actual behavior is equivalent to `NEAREST_AWAY`.

```cpp
AscendC::SetHF32TransMode(HF32TransMode::NEAREST_AWAY);
```
