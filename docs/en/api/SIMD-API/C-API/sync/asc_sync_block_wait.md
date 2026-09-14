# asc_sync_block_wait

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:50:04.646Z pushedAt=2026-09-08T03:45:05.385Z -->

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

Used together with [asc_sync_block_arrive](asc_sync_block_arrive.md) (associated through **flag_id**) to wait for all synchronization objects to arrive at the synchronization point corresponding to **flag_id**. If the counter value corresponding to **flag_id** is 0, execution is blocked; if the corresponding counter is greater than 0, the counter is decremented by one and subsequent instructions begin execution.

## Prototype

```cpp
__aicore__ inline void asc_sync_block_wait(pipe_t pipe, uint8_t flag_id)
__aicore__ inline void asc_sync_block_wait(pipe_t pipe, int64_t flag_id)
```

## Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Pipeline type on which the synchronization instruction is to be executed. |
| flag_id | Input | Event ID, with a value range of [0, 15]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The value range of **flag_id** is 0 to 15, and the counter range of each **flag_id** is 0 to 15.
- The four-bit counter of each **flag_id** must not overflow; otherwise, an exception is raised.
- The same **flag_id** must be used by only one pipeline at a time.

## Example

```cpp
int64_t flag_id = 1;
asc_sync_block_arrive(PIPE_S, flag_id);
asc_sync_block_wait(PIPE_S, flag_id);  
```
