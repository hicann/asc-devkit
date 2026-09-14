# asc_enable_hf32_trans

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:29:15.480Z -->

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
- Atlas 200I/500 A2 inference products: Not supported
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

Sets the rounding mode of the HF32 mode. You need to first call [asc_enable_hf32](./asc_enable_hf32.md) to enable the HF32 rounding mode.

## Prototype

 ```cpp
   __aicore__ inline void asc_enable_hf32_trans(uint32_t mode)
```

## Parameters

| Parameter | Input/Output | Description |
|:-------|:----------|:------|
| mode | Input | HF32 rounding mode control input parameter, of the **uint32_t** type. The following two values are supported:<br>**0**: FP32 is rounded to HF32 in the round-to-nearest-even manner.<br>**1**: FP32 is rounded to HF32 in the round-toward-zero manner. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

**mode** supports only the following two values:<br>**0**: FP32 is rounded to HF32 in the round-to-nearest-even manner.<br>**1**: FP32 is rounded to HF32 in the round-toward-zero manner.

## Example

```cpp
uint32_t mode = 0;
asc_enable_hf32_trans(mode);
```
