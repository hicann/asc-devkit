# asc\_activemask

<!-- md-trans-meta sourceCommit=b666412ab6defb951e2247c9cf853e459f5b4171 translatedAt=2026-09-08T01:12:18.473Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Checks whether all threads in a warp are active.

Returns a 32-bit unsigned integer. If a thread in the warp is active (threads that have exited are inactive), the bit corresponding to the thread's Lane ID in the return value is 1; otherwise, it is 0. All active threads in the warp return the same result.

## Function Prototype

```
inline uint32_t asc_activemask()
```

## Parameters

None

## Return Value

A 32-bit unsigned integer: if a thread in the warp is active, the bit corresponding to the thread's Lane ID in the return value is 1; otherwise, it is 0.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/device\_warp\_functions.h" header file.

```
#include "simt_api/device_warp_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelActiveMask(uint32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        uint32_t result = asc_activemask();
        dst[idx] = result;
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelActiveMask(__gm__ uint32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        // asc_vf_call parameter: dim3{1024, 1, 1}
        uint32_t result = asc_activemask(); // The return value is 0xffffffff.
        dst[idx] = result;
    }
    ```
