# asc\_threadfence

<!-- md-trans-meta sourceCommit=d1fe9e831a69eaae67fc1f463ea1c4ea17a8f417 translatedAt=2026-09-04T00:28:46.614Z pushedAt=2026-09-07T06:34:23.528Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In the SIMT programming paradigm, read and write operations on the same memory from different threads may cause a **data race** (Data Race). Due to the characteristics of the NPU architecture, the execution order of threads and the memory access order may be inconsistent, causing other threads to observe a data update order different from the actual write order.

Memory fence interfaces can resolve such problems of multi-thread synchronized read/write access to shared memory. Among them, the asc\_threadfence interface is used to ensure the **ordering and visibility of write operations** during access to the same global and shared memory by all threads.

**Key features**:

-   This interface **does not block threads**; it only ensures the visibility order of memory operations.
-   Its scope is global: after the call, it ensures that all global memory and shared memory write operations performed by the calling thread before asc\_threadfence\(\) are visible to other threads.

Typical scenarios where this interface applies include:

-   Producer-consumer synchronization: ensures that data writes are completed before the flag is set.
-   Atomic counter scenario: ensures that related data has been written before the atomic counter is updated.
-   Pointer update scenario: ensures data consistency when operating on data structures such as linked lists and trees.
-   Multithreaded shared data: Ensures that the update order of shared data is visible to other threads.

The following figure shows the memory visibility issue that may occur without a memory fence:

![](../../../figures/threadface_memory_visibility_issue.png)

By using the asc\_threadfence\(\) interface, the memory visibility issue described above can be resolved:

![](../../../figures/threadface_interface_effect.png)

## Prototype

```
inline void asc_threadfence()
```

## Description

None

## Return Value

None

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/device\_sync\_functions.h" header file.

```
#include "simt_api/device_sync_functions.h"
```

## Example

For the complete sample, see [MemoryFence sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/02_features/01_api_features/01_sync_instruction/memory_fence/README.md).

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelThreadFence(float* dst, float* src)
    {
        src[0] = src[0] + 1;
        asc_threadfence(); // asc_threadfence() ensures that the write operations of this thread are globally visible in order.
        dst[0] = src[0];
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelThreadFence(__gm__ float* dst, __gm__ float* src)
    {
        src[0] = src[0] + 1;
        asc_threadfence(); // asc_threadfence() ensures that the write operations of this thread are globally visible in order.
        dst[0] = src[0];
    }
    ```
