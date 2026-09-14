# asc_get_vf_len

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:21:56.874Z pushedAt=2026-09-08T03:45:05.431Z -->

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

Obtains the size of the Tensor bit width VL (Vector Length).

## Prototype

```cpp
__aicore__ inline constexpr int64_t asc_get_vf_len()
```

## Parameters

None

## Return Value

Returns the size of the bit width VL.

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
int64_t len = asc_get_vf_len();
```
