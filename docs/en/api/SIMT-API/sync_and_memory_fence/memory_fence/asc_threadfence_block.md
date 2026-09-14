# asc\_threadfence\_block

<!-- md-trans-meta sourceCommit=e290f6324f3caf31e5b53e9c14d1dc9a5b1efebb translatedAt=2026-09-04T00:28:39.882Z pushedAt=2026-09-07T06:34:23.526Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Within the same Thread Block, multiple threads share the Unified Buffer (256 KB shared memory), and data races may also occur between threads.

Similar to the [asc\_threadfence](asc_threadfence.md) interface, the asc\_threadfence\_block function is used to guarantee the visibility order of the current thread's memory read and write operations within the same Thread Block, ensuring that all memory read and write operations performed by a thread before calling asc\_threadfence\_block\(\) are visible to other threads in the same thread block. This function does not block the execution of the current thread; it only establishes a visibility order constraint for memory operations within the Block scope. asc\_threadfence\_block provides a visibility guarantee within the Block scope, and its implementation only needs to ensure cache hierarchy consistency shared within the Block, without requiring a global memory barrier operation.

## Prototype

```
inline void asc_threadfence_block()
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

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelThreadFenceBlock(float* dst, float* src)
    {
        src[0] = src[0] + 1;
        asc_threadfence_block();
        dst[0] = src[0];
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelThreadFenceBlock(__gm__ float* dst, __gm__ float* src)
    {
        src[0] = src[0] + 1;
        asc_threadfence_block();
        dst[0] = src[0];
    }
    ```

