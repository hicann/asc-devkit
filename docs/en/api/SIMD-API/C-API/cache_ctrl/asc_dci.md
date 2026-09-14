# asc_dci

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:26:39.853Z -->

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

Data cache invalidation. This instruction invalidates the entire data cache without writing back cache lines that are in the dirty state. In other words, all cache lines are directly marked as invalid, and any modified data that has not yet been synchronized to main memory is discarded.

## Prototype

```cpp
__aicore__ inline void asc_dci()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

Before calling **asc_dci**, call [asc_sync_data_barrier](../sync/asc_sync_data_barrier.md) to insert the **DSB_ALL** instruction and wait for all memory access instructions to complete.

## Example

```cpp
asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
asc_dci();
```
