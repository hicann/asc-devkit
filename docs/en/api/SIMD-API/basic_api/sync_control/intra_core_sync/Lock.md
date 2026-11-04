# Lock<a name="ZH-CN_TOPIC_0000002372961532"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:02:44.479Z pushedAt=2026-10-26T07:41:44.826Z nl=2 -->

## Applicable Products<a name="section73648168211"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/intra_core_sync/Lock_res.md#id1 -->

## Description<a name="section54681522111017"></a>

The header file path is `"basic_api/kernel_common.h"`.

Obtains the mutex based on the MutexID. If the mutex is already locked, the execution of subsequent instructions on the specified pipeline is blocked until all mutexes with the same MutexID in all pipelines have been released by [Unlock](Unlock.md).

> [!NOTE]NOTE
> The Lock API must be used together with the [Unlock](Unlock.md) API. For the same MutexID, Lock and Unlock must appear strictly in pairs; otherwise, the behavior is undefined. For details about the constraints, see [Constraint Description](#constraint-description).

## Prototype<a name="section1568410468104"></a>

```cpp
template <pipe_t pipe>
static __aicore__ inline void Lock(MutexID id)
```

## Parameters<a name="section74061251191017"></a>

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| pipe | Template parameter, indicating the pipe type. Supports PIPE_S, PIPE_M, PIPE_V, PIPE_MTE1, PIPE_MTE2, PIPE_MTE3, and PIPE_FIX. For the meaning of each pipe type, see [Hardware Pipe Type](intra_core_sync_overview.md#hardware-pipeline-type). |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| id | Input | MutexID used for pipe synchronization management. In different programming paradigms, the acquisition and release of this ID differ. For details, see [Constraint Description](#constraint-description). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraint Description<a name="section184751024101111"></a>

- Each lock has a fixed MutexID. In different programming paradigms, the way this ID is obtained and released differs:
    - When using the [TPipe-TQue framework programming paradigm](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md), the MutexID must be allocated and released through [AllocMutexID](AllocMutexID_ISASI.md)/[ReleaseMutexID](ReleaseMutexID_ISASI.md). The framework manages the MutexID allocation records in a unified manner to avoid conflicts with the MutexIDs used by resource management APIs such as TQue.
    - When using the [static Tensor programming paradigm](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md), the MutexID can be allocated and released through [AllocMutexID](AllocMutexID_ISASI.md)/[ReleaseMutexID](ReleaseMutexID_ISASI.md), or it can be managed by the developer. Self-management reduces the overhead of API calls, but the developer must ensure that the MutexID does not conflict with the MutexIDs used by other synchronization or resource management APIs. When managing the MutexID yourself, it is recommended to use 0-27; 28-31 are reserved for internal system planning and are not recommended for use.
<a id="mutexid_resource_management_constraint"></a>
- When calling `Lock`/`Unlock`, if the MutexID is managed by the developer, the following resource management APIs that may apply for, obtain, or operate on a MutexID must not be used at the same time:
    - `TPipe` APIs: [InitBuffer](../../resource_management/TPipe/InitBuffer.md) (only the overloads whose parameter type is `TQue` or `TQueBind`), [InitBufPool](../../resource_management/TPipe/InitBufPool.md).
    - `TBufPool` APIs: [InitBuffer](../../resource_management/TBufPool/InitBuffer.md), [InitBufPool](../../resource_management/TBufPool/InitBufPool.md), [Reset](../../resource_management/TBufPool/Reset.md).
    - `TQue` APIs: [AllocTensor](../../resource_management/TQue/AllocTensor.md), [FreeTensor](../../resource_management/TQue/FreeTensor.md), [EnQue](../../resource_management/TQue/EnQue.md), [DeQue](../../resource_management/TQue/DeQue.md).
    - `TQueBind` APIs: [InitBufHandle](../../resource_management/TQueBind/InitBufHandle.md), [AllocTensor](../../resource_management/TQueBind/AllocTensor.md), [FreeTensor](../../resource_management/TQueBind/FreeTensor.md), [EnQue](../../resource_management/TQueBind/EnQue.md), [DeQue](../../resource_management/TQueBind/DeQue.md).

    Under the corresponding architecture and configuration, the preceding resource management APIs may apply for, allocate, or use MutexIDs. MutexIDs managed by developers themselves are not registered in the MutexID allocation records of the resource management mechanism, and may duplicate the MutexIDs used by the preceding resource management APIs, resulting in undefined synchronization behavior. When the preceding resource management APIs and `Lock`/`Unlock` are used together, the MutexID used by `Lock`/`Unlock` must be applied for through [AllocMutexID](AllocMutexID_ISASI.md) and released through [ReleaseMutexID](ReleaseMutexID_ISASI.md) after use.
- Lock and Unlock must be used strictly in pairs, with the same pipe and id. In addition, the corresponding Unlock must always be written after Lock; otherwise, the behavior is undefined.

    ```cpp
    // Counterexample: Unlock is called before Lock, reversing the order.
    AscendC::Mutex::Unlock<PIPE_MTE2>(0);
    AscendC::Mutex::Lock<PIPE_MTE2>(0);
    ```

- For a Lock and Unlock pair with the same id, regardless of whether the pipe is the same, they must not be nested in the code; otherwise, the behavior is undefined.

    ```cpp
    // Counterexample: Lock and Unlock are nested with the same id.
    AscendC::Mutex::Lock<PIPE_MTE2>(0);
    AscendC::Mutex::Lock<PIPE_MTE3>(0);
    AscendC::Mutex::Unlock<PIPE_MTE3>(0);
    AscendC::Mutex::Unlock<PIPE_MTE2>(0);
    ```

- When two Lock and Unlock pairs with the same id and pipe are called consecutively, after the Lock of the first call blocks the pipe specified by the pipe parameter, the Lock of the second call cannot block that pipe again. In other words, two consecutively called Lock and Unlock pairs with the same id and pipe cannot synchronize different instructions within a single pipe (specified by the pipe parameter). To synchronize multiple instructions within a single pipe, use the [PipeBarrier](PipeBarrier_ISASI.md) API.

    As shown in [Figure 1](#fig_mte2_overlap), when the destination addresses of two copy operations overlap in the Unified Buffer (UB), developers need to control the execution order of the two instructions on PIPE_MTE2. In this scenario, the following approach (two consecutively called Lock and Unlock pairs with the same id and pipe) can only ensure that the instructions on PIPE_V execute after the two instructions on PIPE_MTE2 complete, but cannot control the execution order of the two instructions on PIPE_MTE2. The correct approach is to insert `PipeBarrier<PIPE_MTE2>()` between `CopyInY` and `CopyInX`.

    **Figure 1**  Overlap of the destination addresses (UB) of two copy operations on PIPE_MTE2 <a name="fig_mte2_overlap"></a>    
    ![](../../../../figures/mte2_overlap.png)

    ```cpp
    // The addresses of x and y overlap in UB (the overlapping part of x overwrites y). The expected order is to copy in y first and then x, so the copy-in order must be guaranteed.
    void CopyInX(LocalTensor<float>& x, const GlobalTensor<float>& xGm, uint32_t len, uint8_t mutexId)
    {
        AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
        AscendC::DataCopy(x, xGm, len);
        AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);
    }

    void CopyInY(LocalTensor<float>& y, const GlobalTensor<float>& yGm, uint32_t len, uint8_t mutexId)
    {
        AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
        AscendC::DataCopy(y, yGm, len);
        AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);
    }

    void Process(LocalTensor<float>& z, LocalTensor<float>& x, LocalTensor<float>& y, uint32_t len, uint8_t mutexId)
    {
        // Copy in y first, then x (the overlapping part of x overwrites y). An incorrect order causes precision exceptions.
        CopyInY(y, yGm, len, mutexId);

        /*
        * // Insert PipeBarrier between CopyInY and CopyInX to ensure the next copy-in is executed only after the previous copy-in is complete.
        * AscendC::PipeBarrier<PIPE_MTE2>();
        */

        CopyInX(x, xGm, len, mutexId);
        AscendC::Mutex::Lock<PIPE_V>(mutexId);
        AscendC::Add(z, x, y, len);
        AscendC::Mutex::Unlock<PIPE_V>(mutexId);
    }
    ```

## Example<a name="section123275308128"></a>

```cpp
// Apply for two MutexIDs for alternate reuse in the double-buffered pipeline.
uint8_t mutexId0 = AscendC::AllocMutexID();
uint8_t mutexId1 = AscendC::AllocMutexID();

// Use the two MutexIDs alternately to ensure that the MTE2, V, and MTE3 pipeline stages are connected in sequence.
for (int32_t i = 0; i < loopCount; i++) {
    uint8_t mutexId = (i % 2 == 0) ? mutexId0 : mutexId1;

    // Lock the MTE2 pipeline to ensure that the current tile's copy-in is executed in the order of this MutexID.
    AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
    AscendC::DataCopy(xLocal, src0Global[TILE_LENGTH * progress], TILE_LENGTH);
    AscendC::DataCopy(yLocal, src1Global[TILE_LENGTH * progress], TILE_LENGTH);
    // Unlock the MTE2 pipeline after the copy-in is complete to allow subsequent stages to proceed.
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);

    // Lock the V pipeline to wait for the corresponding tile's copy-in to complete before starting computation.
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

// Release the two MutexIDs applied for in this example.
AscendC::ReleaseMutexID(mutexId0);
AscendC::ReleaseMutexID(mutexId1);
```

For the complete sample, see [Mutex Sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/mutex).
