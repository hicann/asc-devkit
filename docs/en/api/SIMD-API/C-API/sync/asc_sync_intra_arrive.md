# asc_sync_intra_arrive

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:52:59.591Z pushedAt=2026-09-08T03:45:05.392Z -->

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

An intra-block synchronization instruction used together with [asc_sync_intra_wait](asc_sync_intra_wait.md). It is a pipeline synchronization instruction for different cores within a block, used to send synchronization signals to the synchronization register of each core. Each sync_id corresponds to a counter with an initial value of 0 in the Vector Core and the Cube Core. After the asc_sync_intra_arrive instruction, the corresponding counter of other cores increases by 1. When asc_sync_intra_wait is executed, if the corresponding counter value is 0, execution is blocked. If the corresponding counter is greater than 0, the counter decreases by 1 and subsequent instructions begin to execute.

## Prototype

```cpp
__aicore__ inline void asc_sync_intra_arrive(pipe_t pipe, uint8_t sync_id)
__aicore__ inline void asc_sync_intra_arrive(pipe_t pipe, uint64_t sync_id)
```

## Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | The execution pipeline of this instruction. |
| sync_id | Input | Event ID. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Ensure that the counter of each ID does not overflow; otherwise, an exception occurs.
- Ensure that the same ID is controlled by only one pipeline.
- Each ID corresponds to a counter. The synchronization ID counter of a specific core can be incremented by asc_sync_intra_arrive initiated by other cores, and decremented by asc_sync_intra_wait initiated by the core itself.
- The ID mapping rules of the three cores are as follows:
    - The asc_sync_intra_arrive instruction of VECCORE0 with IDs 0 to 15 is mapped to IDs 0 to 15 of CUBECORE.
    - The asc_sync_intra_arrive instruction of VECCORE1 with IDs 0 to 15 is mapped to IDs 16 to 31 of CUBECORE.
    - The **asc_sync_intra_arrive** instructions of **CUBECORE** with IDs 0 to 15 are mapped to IDs 0 to 15 of **VECCORE0**. 
    - The **asc_sync_intra_arrive** instructions of **CUBECORE** with IDs 16 to 31 are mapped to IDs 0 to 15 of **VECCORE1**. 

## Example

```cpp
//Set the sync instruction event ID to 1.
uint64_t sync_id = 1;
asc_sync_intra_arrive(PIPE_S, sync_id);
asc_sync_intra_wait(PIPE_S, sync_id);  
```
