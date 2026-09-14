# asc_sync_subblock_arrive

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:59:00.610Z pushedAt=2026-09-08T03:45:05.401Z -->

## Applicable Products

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
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

Used together with [asc_sync_subblock_wait](asc_sync_subblock_wait.md) (associated through **flag_id**) to synchronize signals between subblocks within a group. It sends a synchronization signal to the inter-core synchronization register to indicate that the current pipeline has reached the synchronization point corresponding to **flag_id**. Each **flag_id** corresponds to a counter with an initial value of 0. After **asc_sync_subblock_arrive** is executed, the counter corresponding to **flag_id** increases by 1. When **asc_sync_subblock_wait** is executed, if the corresponding counter value is 0, execution is blocked. If the corresponding counter is greater than 0, the counter decreases by 1 and subsequent instructions begin to execute.

## Prototype

```cpp
   __aicore__ inline void asc_sync_subblock_arrive(pipe_t pipe, int64_t flag_id)
```

## Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Sets the pipeline type in which this instruction resides. |
| flag_id | Input | Event ID used to identify the same group of synchronization signals. The value range is [0, 15]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Developers are not advised to use this API together with the Matmul high-level API, as doing so may cause **flag_id** conflicts.
- The counter of the same **flag_id** can be set at most 15 times.

## Example

<!-- npu="A3,910b" id5 -->
- The following example supports the following product models:
  <!-- npu="A3" id6 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id6 -->
  <!-- npu="910b" id7 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id7 -->
  ```cpp
  // Obtain the inter-core synchronization base address ffts_addr by calling aclrtGetHardwareSyncAddr on the host side.
  uint64_t config = *(__gm__ uint64_t*)ffts_addr;
  asc_set_ffts_base_addr(config);
  int64_t flag_id = 1;
  asc_sync_subblock_arrive(PIPE_S, flag_id);
  asc_sync_subblock_wait(PIPE_S, flag_id);  
  ```
<!-- end id5 -->

<!-- npu="950" id4 -->
- The following example supports the following product models:
  <!-- npu="950" id8 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id8 -->
  ```cpp
  // On Ascend 950PR/Ascend 950DT, there is no need to set the inter-core synchronization base address ffts_addr.
  int64_t flag_id = 1;
  asc_sync_subblock_arrive(PIPE_S, flag_id);
  asc_sync_subblock_wait(PIPE_S, flag_id);  
  ```
<!-- end id4 -->
