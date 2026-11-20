---
okftype: reference.api
description: "Pairs with asc_lock. Releases the corresponding Mutex by mutex_id after preceding instructions of the specified pipeline are complete, unblocking the lock wait of other pipelines."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md
    kind: requires
    condition: "Must be used in strict pairing with asc_lock"
version: ">=9.1.0"
version_changes: []
---
# asc_unlock

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.570896+00:00 pushedAt=2026-11-16T02:10:57.835Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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

The header file path is `"c_api/sync/sync.h"`.

Used in pairs with [asc_lock](asc_lock.md) to implement synchronization between asynchronous pipeline instructions within the core. The functions of `asc_lock` and `asc_unlock` are as follows:

- `asc_lock`: Acquires the Mutex according to `mutex_id`. If the Mutex is already locked, it blocks the execution of subsequent instructions on the specified pipeline until the Mutex with the same `mutex_id` in all pipelines has been released by [asc_unlock](asc_unlock.md).
- `asc_unlock`: After the preceding instructions of the specified pipeline are complete, this API releases the corresponding Mutex according to `mutex_id`.

> [!NOTE]
> The `asc_unlock` API must be used together with the [asc_lock](asc_lock.md) API. For the same group of `pipe`, `mutex_id`, and `mode`, `asc_lock` and `asc_unlock` must appear strictly in pairs. For detailed constraints, see [asc_lock Constraints](asc_lock.md#constraints).

## Prototype

```c
__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id)
__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Execution pipeline of the instruction.<br>The parameter type is the `pipe_t` enumeration. For the meaning of each enumeration value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type).|
| mutex_id | Input | Mutex ID, with a value range of [0, 31].|
| mode | Input | Optional parameter that specifies whether to block the pipeline. The default value is `ASC_LOCK_BLOCK`.<br>&bull; `ASC_LOCK_BLOCK`: This instruction waits until all preceding instructions in the pipeline corresponding to `pipe` are complete before execution.<br>&bull; `ASC_LOCK_NON_BLOCK`: This instruction waits until all preceding instructions in the pipeline corresponding to `pipe` are complete and all `asc_unlock` instructions with the same `mutex_id` are complete before execution. |

The `asc_mutex_execute_mode` enum is defined as follows:

```c
enum asc_mutex_execute_mode {
    ASC_LOCK_BLOCK = 0,
    ASC_LOCK_NON_BLOCK = 1
};
```

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- The `pipe` values supported by the AIC and AIV are shown in the following table.

  | Core Type | Supported pipe Values |
  | :--- | :--- |
  | AIC | `PIPE_S`, `PIPE_M`, `PIPE_MTE1`, `PIPE_MTE2`, `PIPE_FIX` |
  | AIV | `PIPE_S`, `PIPE_MTE2`, `PIPE_MTE3`, `PIPE_V` |

- `mutex_id` is managed by the developer. The developer needs to plan `mutex_id` according to the synchronization dependency. The same group of synchronization dependencies must use the same `mutex_id`, while different `mutex_id` values are recommended for different synchronization dependencies; otherwise, the actual synchronization behavior may not match expectations. For example, in the [double buffer sample](../../../../../../../examples/02_simd_c_api/02_features/01_reg_vector_compute/00_add_double_buffer/README.md), the ping buffer and pong buffer correspond to two independent sets of pipelines for data move-in, computation, and data move-out, and different `mutex_id` values are required to keep the synchronization relationships of the two pipeline groups independent and parallel.

- `asc_lock` and `asc_unlock` must be used strictly in pairs and with the same `pipe`, `mutex_id`, and `mode`. In addition, the corresponding `asc_unlock` must always be written after `asc_lock`; otherwise, the behavior is undefined.

    ```cpp
    // Counterexample: asc_unlock is called before asc_lock, reversing the order.
    asc_unlock(PIPE_MTE2, 0);
    asc_lock(PIPE_MTE2, 0);
    ```

- For a pair of `asc_lock` and `asc_unlock` with the same `mutex_id`, they must not be nested in the code regardless of whether `pipe` and `mode` are the same; otherwise, the behavior is undefined.

    ```cpp
    // Counterexample: asc_lock and asc_unlock are nested with the same mutex_id.
    asc_lock(PIPE_MTE2, 0);
    asc_lock(PIPE_MTE3, 0);
    asc_unlock(PIPE_MTE3, 0);
    asc_unlock(PIPE_MTE2, 0);
    ```

- When two pairs of `asc_lock` and `asc_unlock` with the same `mutex_id` and `pipe` are called consecutively, after the first `asc_lock` blocks the pipeline specified by the `pipe` parameter, the second `asc_lock` cannot block that pipeline again. In other words, two consecutively called pairs of `asc_lock` and `asc_unlock` with the same `mutex_id` and `pipe` cannot implement synchronization between different instructions within a single pipeline (specified by the `pipe` parameter). For synchronization between multiple instructions within a single pipeline, use the [asc_sync_pipe](asc_sync_pipe.md) API.

    When the destination addresses of two data movements overlap in the Unified Buffer (UB), the developer must control the execution order of the two instructions on `PIPE_MTE2`. In this scenario, the following pattern (two consecutive pairs of `asc_lock` and `asc_unlock` calls with the same `mutex_id` and `pipe`) only guarantees that instructions on `PIPE_V` execute after the instructions on `PIPE_MTE2` complete; it does not control the execution order of the two instructions on `PIPE_MTE2`. The correct approach is to insert `asc_sync_pipe(PIPE_MTE2)` between `CopyInY` and `CopyInX` (see the multi-line comment).

    ```cpp
    // The addresses of x and y overlap in UB (the overlapping part of x overwrites y). The expected order is to move in y first and then x, so the move-in order must be guaranteed.
    void CopyInX(__ubuf__ float* x, __gm__ float* xGm, uint32_t len, uint8_t mutexId)
    {
        asc_lock(PIPE_MTE2, mutexId);
        asc_copy_gm2ub(x, xGm, len);
        asc_unlock(PIPE_MTE2, mutexId);
    }

    void CopyInY(__ubuf__ float* y, __gm__ float* yGm, uint32_t len, uint8_t mutexId)
    {
        asc_lock(PIPE_MTE2, mutexId);
        asc_copy_gm2ub(y, yGm, len);
        asc_unlock(PIPE_MTE2, mutexId);
    }

    void Process(__ubuf__ float* z, __ubuf__ float* x, __ubuf__ float* y, uint32_t len, uint8_t mutexId)
    {
        // Move in y first, then x (the overlapping part of x overwrites y). An incorrect order causes precision exceptions.
        CopyInY(y, yGm, len, mutexId);

        /*
        * // asc_sync_pipe must be inserted between CopyInY and CopyInX to ensure that the next move-in starts only after the previous move-in completes.
        * asc_sync_pipe(PIPE_MTE2);
        */

        CopyInX(x, xGm, len, mutexId);
        asc_lock(PIPE_V, mutexId);
        asc_add(z, x, y, len);
        asc_unlock(PIPE_V, mutexId);
    }
    ```

## Examples

`asc_unlock` and `asc_lock` must be used in pairs. For a call example, see [asc_lock Example](asc_lock.md#examples).
