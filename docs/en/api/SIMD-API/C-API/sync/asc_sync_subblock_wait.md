# asc_sync_subblock_wait

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:00:19.000Z pushedAt=2026-09-08T03:45:05.402Z -->

## Applicable Products


<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310b" id9 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id9 -->
<!-- npu="310p" id10 -->
- Atlas inference products AI Core: Not supported
<!-- end id10 -->
<!-- npu="310p" id11 -->
- Atlas inference products Vector Core: Not supported
<!-- end id11 -->
<!-- npu="910" id12 -->
- Atlas training products: Not supported
<!-- end id12 -->

## Description

This is an inter-core synchronization register instruction. Each **flag_id** is assigned a counter. After **asc_sync_subblock_arrive** is executed, the counter increments. When **asc_sync_subblock_wait** is executed, if the corresponding counter value is 0, the instruction blocks; if the corresponding counter is greater than 0, the counter decrements by one and subsequent instructions begin to execute.

## Prototype

```cpp
    __aicore__ inline void asc_sync_subblock_wait(pipe_t pipe, uint8_t flag_id)
    __aicore__ inline void asc_sync_subblock_wait(pipe_t pipe, int64_t flag_id)
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

<!-- npu="A3,910b" id1 -->
- The following example supports these product models:
  <!-- npu="A3" id5 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id5 -->
  <!-- npu="910b" id7 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id7 -->
  ```cpp
  // On the Host side, call the aclrtGetHardwareSyncAddr API to obtain the inter-core synchronization base address ffts_addr.
  uint64_t config = *(__gm__ uint64_t*)ffts_addr;
  asc_set_ffts_base_addr(config);
  int64_t flag_id = 1;
  asc_sync_subblock_arrive(PIPE_S, flag_id);
  asc_sync_subblock_wait(PIPE_S, flag_id);  
  ```
<!-- end id1 -->

<!-- npu="950" id6 -->
- The following example supports these product models:
  <!-- npu="950" id8 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id8 -->
  ```cpp
  // On Ascend 950PR/Ascend 950DT, there is no need to set the inter-core synchronization base address ffts_addr.
  int64_t flag_id = 1;
  asc_sync_subblock_arrive(PIPE_S, flag_id);
  asc_sync_subblock_wait(PIPE_S, flag_id);  
  ```
<!-- end id6 -->
