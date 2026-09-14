# asc_float2int32

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:32:35.219Z pushedAt=2026-09-08T03:45:05.354Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

Converts float data to int32_t and supports multiple rounding modes.

Rounding mode:
- RINT rounding mode: round to nearest, ties to even.
- ROUND rounding mode: round half up.
- FLOOR rounding mode: round toward negative infinity.
- CEIL rounding mode: round toward positive infinity.

## Prototype

```cpp
// RINT rounding mode.
__aicore__ inline int32_t asc_float2int32_rn(float value)
// ROUND rounding mode.
__aicore__ inline int32_t asc_float2int32_rna(float value)
// FLOOR rounding mode.
__aicore__ inline int32_t asc_float2int32_rd(float value)
// CEIL rounding mode.
__aicore__ inline int32_t asc_float2int32_ru(float value)
```

## Parameters

| Parameter | Input/Output | Description |
| :------ | :----- | :----------------- |
| value | Input | Source operand (scalar). |

## Return Value

Destination operand (scalar), the result of converting the precision of **value** to **int32_t**.

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
float value = 3.0;
int32_t dst = asc_float2int32_rn(value);
```
