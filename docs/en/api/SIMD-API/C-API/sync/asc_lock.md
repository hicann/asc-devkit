# **asc_lock**

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T12:49:30.176Z pushedAt=2026-09-08T03:45:05.384Z -->

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

This API is used for synchronization between asynchronous pipelines inside the AI Core. It can block the execution of a specified pipeline as needed.

## Prototype

```cpp
enum ascMutexExecuteMode {
    ASC_LOCK_BLOCK = 0,
    ASC_LOCK_NON_BLOCK = 1
};

__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id)
__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id, const ascMutexExecuteMode mode)
```

## Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Specifies the pipeline type to which the current API belongs. |
| mutex_id | Input | Mutex ID, with a value range of [0, 31]. |
| mode | Input | Optional parameter that specifies whether to block the pipeline. The default value is **ASC_LOCK_BLOCK**.<br>&bull; **ASC_LOCK_BLOCK**: Blocks the execution of the pipeline corresponding to **pipe** until all **asc_unlock** calls that appear before the current **asc_lock** in the code and have the same **mutex_id** have completed execution.<br>&bull; **ASC_LOCK_NON_BLOCK**: Does not block the execution of the pipeline corresponding to **pipe**. |

## Return Value

None

## Pipeline Type

**PIPE_S**

## Constraints

- **asc_lock** and **asc_unlock** must be used strictly in pairs and with the same **mutex_id** and **mode**. In addition, the corresponding **asc_unlock** must always be written after **asc_lock**; otherwise, the hardware behavior is undefined.
- For a pair of **asc_lock** and **asc_unlock** with the same **mutex_id**, they must not be nested in the code regardless of whether **pipe** and **mode** are the same; otherwise, the hardware behavior is unpredictable.
- **asc_lock** and **asc_unlock** used in pairs must be configured with the same **mode**; otherwise, the hardware behavior is unpredictable.
- If multiple consecutive calls with the same **pipe** and **mutex_id** appear in the code, the subsequent **asc_lock** no longer blocks the pipeline execution. To express dependencies within the same pipeline, the **asc_sync_pipe** API must be used.

## Example

```cpp
uint8_t mutex_id = 1;
asc_lock(PIPE_S, mutex_id);
asc_unlock(PIPE_S, mutex_id);
```
