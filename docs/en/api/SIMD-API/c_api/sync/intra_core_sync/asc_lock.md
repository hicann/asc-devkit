---
okftype: reference.api
description: "Acquires the intra-core Mutex by mutex_id. If it is already locked, blocks subsequent instructions on the specified pipeline until all Mutexes with the same mutex_id are released by asc_unlock."
tag:
  - roles: [control]
  - operations: [synchronization]
  - traits: [inter_core_synchronization]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md
    kind: requires
    condition: "The same group of pipe/mutex_id/mode must be strictly paired with asc_unlock"
version: ">=9.1.0"
version_changes: []
---
# asc_lock

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.533844+00:00 pushedAt=2026-11-16T02:10:57.752Z nl=2 -->

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

Used in pairs with [asc_unlock](asc_unlock.md) to implement synchronization between asynchronous pipeline instructions within a core. The functions of `asc_lock` and `asc_unlock` are as follows:

- `asc_lock`: Acquires the Mutex according to `mutex_id`. If the Mutex is already locked, it blocks the execution of subsequent instructions on the specified pipeline until the Mutex with the same `mutex_id` in all pipelines has been released by [asc_unlock](asc_unlock.md).
- `asc_unlock`: After the preceding instructions of the specified pipeline are complete, it releases the corresponding Mutex according to `mutex_id`.

> [!NOTE]
> The `asc_lock` API must be used together with the [asc_unlock](asc_unlock.md) API. For the same group of `pipe`, `mutex_id`, and `mode`, `asc_lock` and `asc_unlock` must appear strictly in pairs; otherwise, the behavior is undefined. For details, see [Constraints](#constraints).

## Prototype

```c
__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id)
__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| :---  | :--- | :--- |
| pipe | Input | Identifies which pipeline's subsequent instructions are blocked until the Mutex with the same `mutex_id` in all pipelines has been released by [asc_unlock](asc_unlock.md).<br>The parameter type is the `pipe_t` enum. For the meaning of each enum value, see [Hardware pipeline type](../overview.md#hardware-pipeline-type).|
| mutex_id | Input | Mutex ID, with a value range of [0, 31].|
| mode | Input | Optional parameter that specifies whether to block the pipeline. The default value is `ASC_LOCK_BLOCK`.<br>&bull; `ASC_LOCK_BLOCK`: Blocks the execution of the pipeline corresponding to `pipe` until all `asc_unlock` calls that appear before the current `asc_lock` in the code and have the same `mutex_id` have completed execution.<br>&bull; `ASC_LOCK_NON_BLOCK`: Does not block the execution of the pipeline corresponding to `pipe`.|

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

This example uses `asc_lock` to block the corresponding pipeline, ensuring that data move-in, vector computation, and data move-out execute in order.

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id8 -->
Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {

constexpr uint32_t ELEMENTS = 64;
constexpr uint32_t BYTES = ELEMENTS * sizeof(float);
constexpr uint8_t MUTEX_ID = 1;

__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    uint32_t count = ELEMENTS;
    vector_bool mask = asc_update_mask_b32(count);
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

__global__ __vector__ void asc_lock_kernel(__gm__ float* output, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float x[ELEMENTS], y[ELEMENTS], z[ELEMENTS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(x, src0, BYTES);
    asc_copy_gm2ub_align(y, src1, BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    asc_lock(PIPE_V, MUTEX_ID);
    add_vf(z, x, y);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, z, BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}

} // namespace

int main()
{
    std::vector<float> src0(ELEMENTS), src1(ELEMENTS), output(ELEMENTS, 0.0f), golden(ELEMENTS);
    for (uint32_t i = 0; i < ELEMENTS; ++i) {
        src0[i] = static_cast<float>(i) * 0.25f;
        src1[i] = static_cast<float>(ELEMENTS - i) * 0.5f;
        golden[i] = src0[i] + src1[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *src0_device = nullptr, *src1_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, BYTES, src0.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, BYTES, src1.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_lock_kernel<<<1, 0>>>(output_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_lock passed." : "[Failed] asc_lock failed.") << std::endl;
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
