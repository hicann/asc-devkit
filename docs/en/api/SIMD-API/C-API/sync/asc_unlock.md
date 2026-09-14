# asc_unlock

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:04:27.848Z pushedAt=2026-09-08T03:45:05.410Z -->

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

An instruction used for asynchronous channel synchronization within the AI Core, which releases the buffer requested by the **asc_lock** instruction.

## Prototype

```cpp
enum ascMutexExecuteMode {
    ASC_LOCK_BLOCK = 0,
    ASC_LOCK_NON_BLOCK = 1
};

__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id)
__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id, const ascMutexExecuteMode mode)
```

## Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Execution pipeline of this instruction. |
| mutex_id | Input | Buffer ID. Value range: [0, 31]. |
| mode | Input | Optional parameter that specifies whether to block the pipeline. The default value is **ASC_LOCK_BLOCK**.<br>&bull; **ASC_LOCK_BLOCK**: This instruction waits until all preceding instructions in the pipeline corresponding to **pipe** are complete before execution.<br>&bull; **ASC_LOCK_NON_BLOCK**: This instruction waits until all preceding instructions in the pipeline corresponding to **pipe** are complete and all **asc_unlock** instructions with the same **mutex_id** are complete before execution. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Must be used together with **asc_lock**, and the same **mutex_id** and **mode** must be used. In addition, **asc_unlock** must always be inserted after the corresponding **asc_lock** instruction; otherwise, undefined hardware behavior occurs.
- For a combination of **asc_lock** and **asc_unlock** with the same **mutex_id**, regardless of whether **pipe** and **mode** are the same, they must not be nested in program order; otherwise, the hardware behavior is unpredictable.
- For instruction pairs that appear consecutively in program order with the same **pipe** and **mutex_id**, the subsequent **asc_lock** no longer blocks pipeline execution. To implement a dependency on the same pipeline, the **asc_sync_pipe** instruction must be used.

## Example

```cpp
//Buffer ID is 1.
uint8_t mutex_id = 1;
//Wait until all preceding instructions in PIPE_S are complete, and then release the buffer with ID 1.
asc_lock(PIPE_S, mutex_id);
asc_unlock(PIPE_S, mutex_id); 
```
