# Implementing Multi-Pipeline Synchronization Based on Mutex

<!-- md-trans-meta sourceCommit=8945ab8ed6d1d6230b1aa7819ed166e4be5d8dc7 translatedAt=2026-11-09T10:50:48.154Z pushedAt=2026-11-19T06:32:19.887Z nl=2 -->

## Overview

When data dependencies exist between different pipelines of a vector operator, such as data loading, vector computation, and data storing, use `Mutex` to establish a synchronization relationship between the pipelines so that operations with data dependencies in different pipelines are executed serially. This prevents data errors caused by concurrent access to the same resource by different pipelines.

This document uses [Mutex::Lock](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md) and [Mutex::Unlock](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md) in the basic API as examples to describe the principle of [implementing double-buffer multi-pipeline synchronization with Mutex](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/mutex/README.md). The same synchronization principle also applies to [asc_lock](../../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md) and [asc_unlock](../../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md) in the C API. For a C API double-buffer example, see the [add_double_buffer example](../../../../../../../examples/02_simd_c_api/02_features/01_reg_vector_compute/00_add_double_buffer/README.md).

This document first introduces the forward data dependency and reverse data dependency in double-buffer operators, then describes how to plan `MutexID` and establish a pipeline synchronization relationship, then explains how to use `Mutex` by comparing `Lock`/`Unlock` with `SetFlag`/`WaitFlag`, and finally introduces the usage constraints of the `Mutex` API.

## Multi-Pipeline Data Dependency of Double-Buffer Operators

A typical vector operator consists of three stages: data loading, vector computation, and data storing, which use the `PIPE_MTE2`, `PIPE_V`, and `PIPE_MTE3` pipelines, respectively. When the data volume exceeds the Unified Buffer (UB) capacity, the operator usually splits the data into multiple slices and processes these slices in a loop.

After [double buffering](../../../../operator_practice/simd_operator_impl/vector_programming/double_buffer_scenario.md) is adopted, two groups of buffer resources are used alternately. While the current slice uses one group of resources for computation, other pipelines can use the other group of resources to process adjacent slices.

Pipelines within the same slice have a forward data dependency, which must be resolved through pipeline synchronization to ensure the processing order:

- `PIPE_MTE2 -> PIPE_V`: Only after data loading is complete can `PIPE_V` read the data of this slice and perform computation.
- `PIPE_V -> PIPE_MTE3`: Only after computation is complete can `PIPE_MTE3` store the computation result.

Buffer reuse also introduces a reverse data dependency. When round `i + 2` reuses the same group of Buffer resources as round `i`, the following constraints must be satisfied through pipeline synchronization:

- `PIPE_V -> PIPE_MTE2`: Only after the computation of round `i` is complete can round `i + 2` overwrite the input Buffer.
- `PIPE_MTE3 -> PIPE_V`: Only after the result of round `i` has been stored can round `i + 2` overwrite the output Buffer.

The pipeline synchronization relationship established for the forward data dependency ensures the processing order within the same slice; the pipeline synchronization relationship established for the reverse data dependency ensures that data still being accessed by preceding pipelines is not overwritten during Buffer reuse. For a more detailed description of how pipeline synchronization resolves the preceding two types of data dependency, see the following documents:

- [Synchronization Management in Static Tensor Programming](../../../programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#synchronization-management)
- [Cross-Pipeline Memory Consistency in Memory Consistency](../../memory_model/memory_consistency.md#cross-pipeline-memory-consistency)

## MutexID Planning

Developers need to plan the `MutexID` used for synchronization based on the data dependency between pipelines: pipelines that process the same group of Buffer resources and have data dependency use the same `MutexID`, while different resource groups use different `MutexID` values. For example, `xLocal`, `yLocal`, and `zLocal` of the Add operator form a resource group.

Double buffering uses two groups of Buffer resources and two `MutexID` values:

- In round `i` of the loop, `bufferIndex = i % 2` is selected, so that the same `MutexID` manages only the pipeline synchronization of its corresponding resource group.
- In round `i + 2`, the same resource group and `MutexID` are selected again. At this point, `Lock` on `PIPE_MTE2` waits until all pipelines that previously used this `MutexID` have called `Unlock`, including the `Unlock` of `PIPE_MTE3` in round `i`. This satisfies the reverse data dependency and ensures that the resources can be safely reused.

| Round | Buffer Resource Group | `MutexID` | Key Constraint |
| --- | --- | --- | --- |
| `i` | `buffer0` | `mutexId0` | `PIPE_MTE2`, `PIPE_V`, and `PIPE_MTE3` process the current slice in order. |
| `i + 1` | `buffer1` | `mutexId1` | Uses the other resource group and can overlap with the asynchronous pipeline of round `i`. |
| `i + 2` | `buffer0` | `mutexId0` | `PIPE_MTE2` waits for the resources of round `i` to be released before moving data in, avoiding overwriting `buffer0`. |

The acquisition and release of `MutexID` depend on the programming paradigm. For complete constraints, see [Lock Constraint Description](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Lock.md#constraints):

- In the TPipe-TQue framework programming paradigm, `MutexID` must be applied for through [AllocMutexID](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/AllocMutexID_ISASI.md) and released through [ReleaseMutexID](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/ReleaseMutexID_ISASI.md) after use.
- In the static tensor programming paradigm, `MutexID` can be applied for and released through `AllocMutexID`/`ReleaseMutexID`, or managed by developers themselves. When managed by developers, ensure that the `MutexID` does not conflict with `MutexID` values used by other synchronization or resource management APIs. It is recommended to use 0-27; 28-31 are reserved for internal system planning and are not recommended for use.

## Mutex and Other Intra-Core Synchronization APIs

### Functions of Lock/Unlock and SetFlag/WaitFlag

`Lock` and `Unlock` must be used together. Their functions are as follows:

- `Lock` obtains the `Mutex` based on `MutexID`. If the `Mutex` is already locked, it blocks the subsequent instructions on the specified pipeline until all `Mutex` objects using the same `MutexID` in all previous pipelines have been released through `Unlock`.
- `Unlock` releases the corresponding `Mutex` based on `MutexID` after the preceding instructions on the specified pipeline are executed.

`SetFlag` and `WaitFlag` must also be used together. Their functions are as follows:

- `SetFlag` sets the corresponding flag after the preceding instructions on the source pipeline are completed, but does not block the subsequent instructions on the source pipeline.
- When `WaitFlag` is executed, if the flag is 0, it blocks the subsequent instructions on the destination pipeline; if the flag is 1, it clears the flag and continues execution.

Figure 1 and Figure 2 use the synchronization between `PIPE_MTE2` and `PIPE_V` as an example to compare the principles by which the two groups of APIs implement synchronization:

- In Figure 1, `SetFlag`/`WaitFlag` enable `PIPE_V` to continue execution after `PIPE_MTE2` completes its preceding instructions by setting and waiting for the event flag.
- In Figure 2, `Lock`/`Unlock` enable `PIPE_V` to obtain the `Mutex` and continue execution after `PIPE_MTE2` completes its preceding instructions and releases the `Mutex`, by releasing and obtaining the same `Mutex`.

By comparing the waiting, state change, and unblocking processes in the figures, you can understand how the two groups of APIs resolve data dependencies between different pipelines through different mechanisms.

**Figure 1**  `SetFlag`/`WaitFlag` synchronization sequence diagram    
![](../../../../figures/set_flag_wait_flag_sequence_diagram.png)

**Figure 2**  `Lock`/`Unlock` synchronization sequence diagram    
![](../../../../figures/mutex_lock_unlock_sequence_diagram.png)

### Differences in Synchronization Between the Two API Groups for Forward and Reverse Data Dependencies

`Mutex` binds a `MutexID` to a group of Buffer resources and records the completion status of each previous stage of the resources. Each stage only needs to lock the current pipeline:

```cpp
for (uint32_t i = 0; i < loopCount; ++i) {
    uint32_t bufferIndex = i % 2;
    uint8_t mutexId = (bufferIndex == 0) ? mutexId0 : mutexId1;

    // Wait for the previous access to the current resource group to complete, and then move in the current slice.
    AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
    // PIPE_MTE2 moves in the data of the current slice.
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);

    // Wait for PIPE_MTE2 to complete, and then compute the current slice.
    AscendC::Mutex::Lock<PIPE_V>(mutexId);
    // PIPE_V computes the current slice.
    AscendC::Mutex::Unlock<PIPE_V>(mutexId);

    // Wait for PIPE_V to complete, and then move out the computation result.
    AscendC::Mutex::Lock<PIPE_MTE3>(mutexId);
    // PIPE_MTE3 moves out the result of the current slice.
    AscendC::Mutex::Unlock<PIPE_MTE3>(mutexId);
}
```

When the same resource group is reused in round `i + 2`, `PIPE_MTE2` and `PIPE_V` `Lock` the same `MutexID` again and uniformly wait for all previous pipelines that use this `MutexID` to complete. Therefore, the code does not need to specify the preceding pipeline in the current call, nor does it need to establish synchronization relationships separately for reverse data dependencies such as `PIPE_V -> PIPE_MTE2` and `PIPE_MTE3 -> PIPE_V`.

If `SetFlag`/`WaitFlag` is used, a synchronization relationship must be established separately for each data dependency from the source pipeline to the destination pipeline. The following code retains only the core logic related to synchronization, with the data move-in, computation, and move-out operations represented by comments:

```cpp
for (uint32_t i = 0; i < loopCount; ++i) {
    uint32_t bufferIndex = i % 2;

    if (i >= BUFFER_NUM) {
        // Before reusing the input resources, wait for the previous round (round i-2) of PIPE_V in the same resource group to complete: PIPE_V -> PIPE_MTE2.
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(eventVToMTE2[bufferIndex]);
    }

    // PIPE_MTE2 moves in the data of the current slice.
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(eventMTE2ToV[bufferIndex]);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(eventMTE2ToV[bufferIndex]);

    if (i >= BUFFER_NUM) {
        // Before reusing the output resources, wait for the previous round (round i-2) of PIPE_MTE3 in the same resource group to complete: PIPE_MTE3 -> PIPE_V.
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(eventMTE3ToV[bufferIndex]);
    }

    // PIPE_V computes the current slice.
    if (i + BUFFER_NUM < loopCount) {
        // Set the PIPE_V -> PIPE_MTE2 event only when the input resources will still be reused later.
        AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(eventVToMTE2[bufferIndex]);
    }

    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(eventVToMTE3[bufferIndex]);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(eventVToMTE3[bufferIndex]);

    // PIPE_MTE3 moves out the result of the current slice.
    if (i + BUFFER_NUM < loopCount) {
        // Set the PIPE_MTE3 -> PIPE_V event only when the output resources will still be reused later.
        AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(eventMTE3ToV[bufferIndex]);
    }
}
```

`SetFlag`/`WaitFlag` requires selecting a `HardEvent` and managing the event ID separately for forward data dependency and reverse data dependency to establish the corresponding pipeline synchronization relationship. It also requires determining whether the reverse event exists based on the start and end positions of the loop. In contrast, `Mutex` does not require passing the source pipeline and destination pipeline combination. It only needs to use the same `MutexID` on the current pipeline to synchronously manage the multi-pipeline lifecycle of the same resource group.

In addition, `SetFlag`/`WaitFlag` represents the synchronization relationship from the source pipeline to the destination pipeline through `HardEvent`. The `HardEvent` combinations supported by different hardware architectures may differ. When using `SetFlag`/`WaitFlag`, you need to confirm whether the corresponding `HardEvent` combination exists based on the target architecture.

## Constraints

The following constraints must be observed when using `Mutex`. For the complete content, see the [lock constraint description](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Lock.md#constraints) and [unlock constraint description](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Unlock.md#constraints):

- For the same `MutexID`, `Lock` and `Unlock` must appear strictly in pairs and use the same pipeline type and `MutexID`; the corresponding `Unlock` must be placed after `Lock`.
- For `Lock`/`Unlock` combinations with the same `MutexID`, nesting is not allowed regardless of whether the pipeline types are the same; otherwise, the behavior is undefined.
- Consecutive `Lock`/`Unlock` operations with the same `MutexID` and the same pipeline type cannot implement synchronization between different instructions within the same pipeline. To ensure the execution order of different instructions within the same pipeline, use [PipeBarrier](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI.md) to establish a synchronization relationship within a single pipeline.
- In the TPipe-TQue framework programming paradigm, a `MutexID` applied through `AllocMutexID` should be released through `ReleaseMutexID` in a timely manner after use to avoid `MutexID` exhaustion.
- All related pipelines of the same group of Buffer resources should use the same `MutexID`, and different resource groups should use different `MutexID`s.
