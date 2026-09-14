# asc_get_block_idx

<!-- md-trans-meta sourceCommit=dda4c6b0ddd773ea533849b7c2bd57f8cdeb9cda translatedAt=2026-09-05T13:07:33.345Z pushedAt=2026-09-08T03:45:05.414Z -->

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

Obtains the index of the currently running core.

## Prototype

```cpp
__aicore__ inline int64_t asc_get_block_idx()
```

## Parameters

None

## Return Value

Returns the index of the currently running core.

## Pipeline Type

PIPE_S

## Constraints

None

## Example

None
