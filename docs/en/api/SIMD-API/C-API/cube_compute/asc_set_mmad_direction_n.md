# asc_set_mmad_direction_n

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:36:56.845Z -->

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

Sets the **Mmad** computation to preferentially generate results along the N direction of M/N, and then generate results along the M direction, where M is the row of the matrix and N is the column of the matrix.

## Prototype

```cpp
__aicore__ inline void asc_set_mmad_direction_n()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
asc_set_mmad_direction_n();
```
