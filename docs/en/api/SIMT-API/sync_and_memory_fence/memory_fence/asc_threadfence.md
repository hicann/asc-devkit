# asc\_threadfence

<!-- md-trans-meta sourceCommit=90ead4546752e22e3ebcd5433909f1c9f5e26e49 translatedAt=2026-10-27T03:55:53.242Z pushedAt=2026-10-27T09:21:55.747Z nl=2 -->

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
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence_res.md#id1 -->

## Description

The header file path is `"simt_api/device_sync_functions.h"`.

In the SIMT programming paradigm, read and write operations on the same memory from different threads may cause **data races** (Data Race). Due to the characteristics of the NPU architecture, the thread execution order and the memory access order may be inconsistent, causing other threads to observe a data update order that differs from the actual write order.

Memory fence APIs can solve such problems of multi-thread synchronized read/write access to shared memory. Among them, the `asc_threadfence()` API ensures the **ordering and visibility of write operations** when all threads access the same global and shared memory.

**Key features**:

-   This API **does not block threads**; it only guarantees the visibility order of memory operations.
-   Its scope is global: after the call, all global memory and shared memory write operations performed by the calling thread before asc\_threadfence\(\) are guaranteed to be visible to other threads.

Typical scenarios where this API applies include:

-   Producer-consumer synchronization: ensure that data writes complete before the flag is set.
-   Atomic counter scenario: ensure that related data has been written before the atomic counter is updated.
-   Pointer update scenarios: Ensure data consistency when operating on data structures such as linked lists and trees.
-   Multi-thread shared data: Ensure that the update order of shared data is visible to other threads.

The following figure shows the memory visibility issue that may occur without a memory fence:

![](../../../figures/threadface_memory_visibility.png)

By using the asc\_threadfence\(\) API, you can resolve the memory visibility issue described above:

![](../../../figures/threadface_function.png)

## Prototype

```cpp
inline void asc_threadfence()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Examples

For the complete sample, see [MemoryFence Sample](../../../../../../examples/03_simt_api/02_features/01_api_features/01_sync_instruction/memory_fence/README.md).

In the following example, assume that before execution, `data[0]` holds an old value and `flag[0]` is 0. The producer thread first writes `data[0] = 100`, then calls `asc_threadfence()` to constrain the global visibility order, and finally writes `flag[0] = 1` to publish the data-ready flag. The consumer thread polls until `flag[0]` becomes 1 and then reads `data[0]`. At this point, it should read the value 100 written by the producer rather than the old value.

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_sync_functions.h"

    __global__ __launch_bounds__(1024) void KernelThreadFence(int32_t* data, int32_t* flag, int32_t* result)
    {
        if (blockIdx.x == 0 && threadIdx.x == 0) {
            data[0] = 100;
            asc_threadfence(); // Ensure that the write to data[0] is globally visible before the write to flag[0].
            flag[0] = 1;
        }

        if (blockIdx.x == 1 && threadIdx.x == 0) {
            volatile int32_t* flag_volatile = reinterpret_cast<volatile int32_t*>(flag);
            while (flag_volatile[0] == 0) {  // Wait for the producer to set flag[0] to 1.
            }
            result[0] = data[0];
        }
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_sync_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelThreadFence(
        __gm__ int32_t* data, __gm__ int32_t* flag, __gm__ int32_t* result)
    {
        if (blockIdx.x == 0 && threadIdx.x == 0) {
            data[0] = 100;
            asc_threadfence(); // Ensure that the write to data[0] is globally visible before the write to flag[0].
            flag[0] = 1;
        }

        if (blockIdx.x == 1 && threadIdx.x == 0) {
            volatile __gm__ int32_t* flag_volatile = reinterpret_cast<volatile __gm__ int32_t*>(flag);
            while (flag_volatile[0] == 0) {   // Wait for the producer to set flag[0] to 1.
            }
            result[0] = data[0];
        }
    }
    ```

The input and output examples are as follows:

```text
data: 0
flag: 0
result: 100
```
