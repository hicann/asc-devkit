<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T13:52:48.216Z -->
# Lock<a name="ZH-CN_TOPIC_0000002372961532"></a>


## Applicable Products<a name="section73648168211"></a>

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

## Description<a name="section54681522111017"></a>

The header file path is `"basic_api/kernel_common.h"`.

Obtains the mutex based on **MutexID**. If the mutex is already locked, the subsequent specified pipeline instruction queue is blocked until the mutex corresponding to **MutexID** in the preceding instructions of the current pipeline is unlocked by [Unlock](Unlock.md).

> [!NOTE]
> The **Lock** API must be used together with the [Unlock](Unlock.md) API. For the same **MutexID**, **Lock** and **Unlock** must appear strictly in pairs; otherwise, the hardware behavior is unpredictable. For details, see [Constraint Description](#constraint-description).

## Prototype<a name="section1568410468104"></a>

```cpp
template <pipe_t pipe>
static __aicore__ inline void Lock(MutexID id)
```

## Parameters<a name="section74061251191017"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| pipe | Template parameter, indicating the pipeline type. The supported pipeline types are **PIPE_S**/**PIPE_M**/**PIPE_V**/**PIPE_MTE1**/**PIPE_MTE2**/**PIPE_MTE3**/**PIPE_FIX**. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| id | Input | **MutexID** used for pipeline synchronization management. In different programming paradigms, this ID is obtained and released in different ways. For details, see [Constraint Description](#constraint-description). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraint Description<a name="section184751024101111"></a>

- Each lock has a fixed **MutexID**. In different programming paradigms, the way this ID is obtained and released differs:
    - When the [TPipe-TQue framework programming paradigm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_framework_programming_paradigm.md) is used, the **MutexID** must be applied for and released through [AllocMutexID](AllocMutexID_ISASI.md)/[ReleaseMutexID](ReleaseMutexID_ISASI.md).
    - When the [static Tensor programming paradigm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md) is used, the **MutexID** is managed by the developer. It is recommended to use 0 to 27, while 28 to 31 are reserved for internal system planning and are not recommended.
- For the same **MutexID**, **Lock** must be executed before **Unlock**, and the specified **pipe** must be the same. If **Lock** and **Unlock** are not arranged in this "paired" order, the hardware behavior will be unpredictable. The following are examples of common incorrect and correct usage:

    ```cpp
    // Incorrect usage 1: Unlock is executed before Lock, reversing the order.
    AscendC::Mutex::Unlock<PIPE_V>(0);
    AscendC::Mutex::Lock<PIPE_V>(0);
    
    // Incorrect usage 2: Two consecutive Locks are followed by two consecutive Unlocks, violating the paired usage principle.
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    AscendC::Mutex::Unlock<PIPE_V>(0);


    
    
    // Correct usage: Lock and Unlock appear strictly in pairs.
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    ```

- When two groups of **Lock**/**UnLock** instructions use the same **MutexID** and the same pipe, the second **Lock** no longer blocks the pipe. In this case, it is recommended to use the [PipeBarrier](PipeBarrier_ISASI.md) API. The following are examples of incorrect and correct usage:

    ```cpp
    // Incorrect usage: The second Lock no longer blocks and fails to achieve the synchronization effect.
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    // Cannot achieve the effect of blocking PIPE_V.
    AscendC::Mutex::Lock<PIPE_V>(0); 
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    
    // Correct usage: Use PipeBarrier for synchronization within a single pipeline.
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    ```

## Example<a name="section123275308128"></a>

```cpp
// Apply for two MutexIDs for alternate reuse by the double-buffered pipeline.
uint8_t mutexId0 = AscendC::AllocMutexID();
uint8_t mutexId1 = AscendC::AllocMutexID();

// Use the two MutexIDs alternately to serialize the MTE2, V, and MTE3 pipeline stages in order.
for (int32_t i = 0; i < loopCount; i++) {
    uint8_t mutexId = (i % 2 == 0) ? mutexId0 : mutexId1;

    // Lock the MTE2 pipeline to ensure that the current tile is copied in according to the order of this MutexID.
    AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
    AscendC::DataCopy(xLocal, src0Global[TILE_LENGTH * progress], TILE_LENGTH);
    AscendC::DataCopy(yLocal, src1Global[TILE_LENGTH * progress], TILE_LENGTH);
    // Unlock the MTE2 pipeline after the copy-in is complete to allow subsequent stages to proceed.
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);

    // Lock the V pipeline and wait until the corresponding tile is copied in before starting computation.
    AscendC::Mutex::Lock<PIPE_V>(mutexId);
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    // Unlock the V pipeline after the computation is complete to allow subsequent computation or copy-out.
    AscendC::Mutex::Unlock<PIPE_V>(mutexId);

    // Lock the MTE3 pipeline to ensure that the computation result is complete before writing back to GM.
    AscendC::Mutex::Lock<PIPE_MTE3>(mutexId);
    AscendC::DataCopy(dstGlobal[TILE_LENGTH * progress], zLocal, TILE_LENGTH);
    // Unlock the MTE3 pipeline after the copy-out is complete to finish processing the current tile.
    AscendC::Mutex::Unlock<PIPE_MTE3>(mutexId);
}

// Release the two MutexIDs applied for in this sample.
AscendC::ReleaseMutexID(mutexId0);
AscendC::ReleaseMutexID(mutexId1);
```

For the complete sample, see [Mutex sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/05_sync_control/mutex).
