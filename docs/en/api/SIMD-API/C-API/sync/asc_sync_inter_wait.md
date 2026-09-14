# asc_sync_inter_wait

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:52:19.192Z pushedAt=2026-09-08T03:45:05.390Z -->

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

This is a synchronization instruction provided by the inter-core synchronization register. Each **flag_id** is assigned a counter. After **asc_sync_inter_arrive** is executed, the counter increments. When **asc_sync_inter_wait** is executed, if the corresponding counter value is 0, the instruction blocks; if the corresponding counter is greater than 0, the counter decrements by one and subsequent instructions begin execution.

## Prototype

```cpp
__aicore__ inline void asc_sync_inter_wait(pipe_t pipe, uint8_t flag_id)
__aicore__ inline void asc_sync_inter_wait(pipe_t pipe, int64_t flag_id)
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
- Ensure that the counter of each **flag_id** does not overflow; otherwise, an exception is raised.
- Ensure that the same ID is waited on by only one pipeline at a time.

## Example

```cpp
int64_t flag_id = 1;
asc_sync_inter_arrive(PIPE_S, flag_id);
asc_sync_inter_wait(PIPE_S, flag_id);  
```
