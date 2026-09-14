# asc\_syncthreads

<!-- md-trans-meta sourceCommit=d1fe9e831a69eaae67fc1f463ea1c4ea17a8f417 translatedAt=2026-09-04T00:28:52.764Z pushedAt=2026-09-07T06:34:23.531Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In the SIMT (Single Instruction Multiple Threads) programming paradigm, multiple threads within the same thread block execute in parallel, but their execution progress may differ. When threads need to coordinate work, share data, or ensure that certain operations execute in order, a synchronization mechanism must be used.

asc\_syncthreads is used to **block all threads within the thread block** until all threads reach the synchronization point.

**Key features:**

-   This interface blocks thread execution until all threads within the block reach the synchronization point.
-   It ensures that all memory operations before the synchronization point are visible to all threads within the block.
-   It is commonly used in scenarios such as data sharing within a thread block, staged computation, and parallel reduction.

The following figure illustrates the problems that may arise when multiple threads within the same thread block share data:

![](../../../figures/syncthreads_1.png)

The execution flow of the asc\_syncthreads interface is illustrated as follows:

![](../../../figures/syncthreads_API_function.png)

Note that all threads within thread block must call the asc\_syncthreads\(\) interface before the program can continue. If any thread has not reached the synchronization point, the other threads are blocked and wait, resulting in a deadlock. For example, if the call to this interface is placed inside a branch, some threads may fail to reach the synchronization interface call site, causing the program to hang.

## Prototype

```
inline void asc_syncthreads()
```

## Description

None

## Return Value

None

## Constraints

-   All threads within the thread block must reach the synchronization point; otherwise, a deadlock occurs.

-   Avoid calling this API in a branch unless you can ensure that all threads within thread block enter that branch.
-   Avoid calling this API when the loop iteration counts are inconsistent.

## Required Header File

To use this API, include the "simt\_api/device\_sync\_functions.h" header file.

```
#include "simt_api/device_sync_functions.h"
```

## Example

For the complete sample, see [MemoryFence sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/02_features/01_api_features/01_sync_instruction/memory_fence/README.md).

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelSyncThreads(float* dst, int count)
    {
         int idx = threadIdx.x;
         if (idx > 0 && idx < count) {
             dst[idx] = 1;
         }

         // Wait for all threads within the thread block to reach the current code.
         asc_syncthreads();

         if (idx == 0) {
             dst[0] = 0;
             for(int i = 1023; i > 0; i--) {
                 dst[0] += dst[i];
             }
         }
    }
    ```

    ```
    Output:
    [1023, 1, 1, 1 …]
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelSyncThreads(__gm__ float* dst, int count)
    {
         int idx = threadIdx.x;
         if (idx > 0 && idx < count) {
             dst[idx] = 1;
         }

         // Wait for all threads within the thread block to reach the current code.
         asc_syncthreads();

         if (idx == 0) {
             dst[0] = 0;
             for(int i = 1023; i > 0; i--) {
                 dst[0] += dst[i];
             }
         }
    }
    ```

    ```
    Output:
    [1023, 1, 1, 1 …]
    ```
