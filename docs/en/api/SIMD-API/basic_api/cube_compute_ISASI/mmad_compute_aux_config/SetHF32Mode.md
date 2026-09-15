# SetHF32Mode

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:56:03.001Z -->

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

Sets whether to enable HF32 mode for Mmad computation. When this mode is enabled, FP32 data in the L0A Buffer/L0B Buffer is rounded to HF32 before participating in Mmad computation.

## Prototype

```cpp
__aicore__ inline void SetHF32Mode(HF32Mode mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mode | Input | Mmad HF32 mode control input parameter, of the **HF32Mode** enumeration type. The following two values are supported:<br>&nbsp;&nbsp;&bull; **ENABLE**: The FP32 data in the L0A Buffer/L0B Buffer is rounded to HF32 before cube multiplication.<br>&nbsp;&nbsp;&bull; **DISABLE**: Regular FP32 cube multiplication is performed. |

## Return Value

None

## Constraints

None

## Example

```cpp
AscendC::SetHF32Mode(HF32Mode::ENABLE); // Control whether HF32 precision is used for mmad computation.
```
