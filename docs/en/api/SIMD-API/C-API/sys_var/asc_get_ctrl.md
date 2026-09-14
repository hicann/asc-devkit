# asc_get_ctrl

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:09:59.184Z pushedAt=2026-09-08T03:45:05.418Z -->

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

Reads the value of a specific bit in the CTRL register (control register).

## Prototype

```cpp
__aicore__ inline int64_t asc_get_ctrl()
```

## Parameters

None

## Return Value

The value of the CTRL register. For details, see the description in [asc_set_ctrl](asc_set_ctrl.md).

## Pipeline Type

PIPE_S

## Constraints

None

## Example

None