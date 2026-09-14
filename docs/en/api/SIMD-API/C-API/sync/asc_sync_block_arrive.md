# asc_sync_block_arrive

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:49:30.665Z pushedAt=2026-09-08T03:45:05.385Z -->

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

Used together with [asc_sync_block_wait](asc_sync_block_wait.md) (associated through **flag_id**) to synchronize signals between blocks and subblocks within a group. By sending a synchronization signal to the inter-core synchronization register, it indicates that the current pipeline has reached the synchronization point corresponding to **flag_id**. Each **flag_id** corresponds to a counter with an initial value of 0. After **asc_sync_block_arrive** is executed, the counter corresponding to **flag_id** is incremented by 1. When **asc_sync_block_wait** is executed, if the corresponding counter value is 0, execution is blocked; if the corresponding counter is greater than 0, the counter is decremented by 1 and subsequent instructions begin to execute.

## Prototype

```cpp
__aicore__ inline void asc_sync_block_arrive(pipe_t pipe, int64_t flag_id)
```

## Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Sets the pipeline type in which this instruction resides. |
| flag_id | Input | Event identifier used to identify the same group of synchronization signals. The value range is [0, 15]. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- When using this synchronization API, set the Kernel type according to the following rules:
  - In pure Vector/Cube scenarios, set the Kernel type to **KERNEL_TYPE_MIX_AIV_1_0** or **KERNEL_TYPE_MIX_AIC_1_0**.
  - For mixed Vector and Cube scenarios, configure the Kernel type flexibly based on the actual situation.
- It is not recommended to use this API together with the Matmul high-level API, as this may cause **flag_id** conflicts.
- The counter of the same **flag_id** can be set at most 15 times.

## Example

```cpp
// Call the API aclrtGetHardwareSyncAddr on the host side to obtain the inter-core synchronization base address ffts_addr.
uint64_t config = *(__gm__ uint64_t*)ffts_addr;
asc_set_ffts_base_addr(config);
int64_t flag_id = 1;
asc_sync_block_arrive(PIPE_S, flag_id);
asc_sync_block_wait(PIPE_S, flag_id);  
```

<!-- npu="950" id8 -->

```cpp
// On Ascend 950PR/Ascend 950DT, there is no need to set the inter-core synchronization base address ffts_addr.
int64_t flag_id = 1;
asc_sync_block_arrive(PIPE_S, flag_id);
asc_sync_block_wait(PIPE_S, flag_id);  
```
<!-- end id8 -->