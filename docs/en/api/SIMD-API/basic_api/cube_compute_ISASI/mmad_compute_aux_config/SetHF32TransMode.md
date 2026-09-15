# SetHF32TransMode

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:58:56.256Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Sets the specific rounding method of HF32 mode. You need to first call [SetHF32Mode](SetHF32Mode.md) to enable the HF32 rounding mode.

## Function Prototype

```cpp
__aicore__ inline void SetHF32TransMode(HF32TransMode mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mode | Input | Input parameter for controlling the Mmad HF32 rounding mode, of the HF32TransMode type. The following two values are supported:<br>&nbsp;&nbsp;&bull; NEAREST_ZERO: FP32 is rounded to HF32 toward zero.<br>&nbsp;&nbsp;&bull; NEAREST_EVEN: FP32 is rounded to HF32 toward the nearest even number. |

## Return Value

None

## Constraints

None

## Example

To set the specific rounding method for HF32 mode, first call [SetHF32Mode](SetHF32Mode.md) to enable the HF32 rounding mode.

```cpp
AscendC::SetHF32TransMode(HF32TransMode::NEAREST_ZERO);
```
