# asc\_threadfence\_block

<!-- md-trans-meta sourceCommit=90ead4546752e22e3ebcd5433909f1c9f5e26e49 translatedAt=2026-10-27T03:55:07.542Z pushedAt=2026-10-27T09:21:55.746Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence_block_res.md#id1 -->

## Description

The header file path is `"simt_api/device_sync_functions.h"`.

Within the same Thread Block, multiple threads share a 256 KB Unified Buffer (UB), which can also lead to data races between threads.

Similar to the [asc\_threadfence](asc_threadfence.md) API, the `asc_threadfence_block()` function ensures the visibility order of the current thread's memory read and write operations within the same Thread Block, guaranteeing that all memory read and write operations performed by a thread before calling `asc_threadfence_block()` are visible to other threads in the same thread block. This function does not block the execution of the current thread; it only establishes a visibility order constraint for memory operations within the Block scope. `asc_threadfence_block()` provides visibility guarantees within the Block scope, and its implementation only needs to ensure cache hierarchy consistency shared within the Block, without requiring a global memory barrier operation.

## Prototype

```cpp
inline void asc_threadfence_block()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_sync_functions.h"

    __global__ __launch_bounds__(1024) void kernel_thread_fence_block(float* dst, float* src)
    {
        if (threadIdx.x == 0) {
            src[0] = src[0] + 1;
            asc_threadfence_block();
        }
        if (threadIdx.x == 1) {
            dst[0] = src[0];
        }
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_sync_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_thread_fence_block(__gm__ float* dst, __gm__ float* src)
    {
        src[0] = src[0] + 1;
        asc_threadfence_block();
        dst[0] = src[0];
    }
    ```

The input and output examples are as follows:

```text
src: 9
dst: 10
```
