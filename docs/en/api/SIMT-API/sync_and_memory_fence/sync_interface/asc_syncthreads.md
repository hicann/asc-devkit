# asc\_syncthreads

<!-- md-trans-meta sourceCommit=90ead4546752e22e3ebcd5433909f1c9f5e26e49 translatedAt=2026-10-27T03:57:11.411Z pushedAt=2026-10-27T09:21:55.753Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads_res.md#id1 -->

## Description

The header file path is `"simt_api/device_sync_functions.h"`.

In the SIMT (Single Instruction Multiple Threads) programming paradigm, multiple threads within a thread block execute in parallel, but their execution progress may differ. When threads need to coordinate work, share data, or ensure that certain operations execute in order, a synchronization mechanism is required.

asc\_syncthreads API is used to **block all threads within a thread block** until all threads reach the synchronization point.

**Key features:**

- This API blocks thread execution until all threads in the block reach the synchronization point.
- It ensures that all memory operations before the synchronization point are visible to all threads in the block.
- It is commonly used for data sharing, staged computation, and parallel reduction within a thread block.

The following figure illustrates the problems that may arise when multiple threads within a thread block share data:

![](../../../figures/syncthreads_1.png)

The execution flow of the asc\_syncthreads API is shown in the following figure:

![](../../../figures/syncthreads_function.png)

Note that all threads within a thread block must call the asc\_syncthreads\(\) API before the program can continue. If any thread does not reach the synchronization point, the other threads are blocked and wait, resulting in a deadlock. For example, placing a call to this API inside a branch may prevent some threads from reaching the API call site, causing the program to hang.

## Prototype

```cpp
inline void asc_syncthreads()
```

## Parameters

None

## Return Value

None

## Constraints

- All threads within a thread block must reach the synchronization point; otherwise, a deadlock occurs.

- Avoid calling this API inside a branch unless you can ensure that all threads within a thread block enter that branch.
- Avoid calling this API when the loop counts are inconsistent.

## Examples

For the complete sample, see [MemoryFence Sample](../../../../../../examples/03_simt_api/02_features/01_api_features/01_sync_instruction/memory_fence/README.md).

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_sync_functions.h"

    __global__ __launch_bounds__(1024) void kernel_sync_threads(float* input, float* output, uint32_t input_total_length)
    {
        extern __ubuf__ float smem[];
        uint32_t tid = threadIdx.x;
        uint32_t lane_id = laneid();
        uint32_t warp_id = tid / warpSize;

        float val = (tid < input_total_length) ? input[tid] : 0.0f;

        // Intra-warp reduction. asc_reduce_add returns the sum of val across all threads in the current warp.
        val = asc_reduce_add(val);
        // Only the first thread writes to shared memory.
        if (lane_id == 0) {
            smem[warp_id] = val;
        }

        asc_syncthreads(); // Synchronize across warps.

        if (tid == 0) {
            for (int i = 1; i < blockDim.x / warpSize; i++) {
                val += smem[i];
            }
            *output = val;
        }
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_sync_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_sync_threads(__gm__ float* input, __gm__ float* output, uint32_t input_total_length)
    {
        extern __ubuf__ float smem[];
        uint32_t tid = threadIdx.x;
        uint32_t lane_id = laneid();
        uint32_t warp_id = tid / warpSize;

        float val = (tid < input_total_length) ? input[tid] : 0.0f;

        // Intra-warp reduction. asc_reduce_add returns the sum of val across all threads in the current warp.
        val = asc_reduce_add(val);
        // Only the first thread writes to shared memory.
        if (lane_id == 0) {
            smem[warp_id] = val;
        }

        asc_syncthreads(); // Synchronize across warps.

        if (tid == 0) {
            for (int i = 1; i < blockDim.x / warpSize; i++) {
                val += smem[i];
            }
            *output = val;
        }
    }
    ```

The input and output examples are as follows:

```text
input: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
dst: 2016
```
