# asc\_activemask

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:13:55.209Z pushedAt=2026-10-27T09:21:55.788Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_vote_functions/asc_activemask_res.md#id1 -->

## Description

The header file path is `"simt_api/device_warp_functions.h"`.

Checks whether all threads in a warp are active.

Returns a 32-bit unsigned integer. If a thread in the warp is active (a terminated thread is inactive), the bit corresponding to the thread's Lane ID is set to 1; otherwise, it is set to 0. All active threads in the warp return the same result.

## Prototype

```cpp
inline uint32_t asc_activemask()
```

## Parameters

None

## Return Value

A 32-bit unsigned integer: if a thread in the warp is active, the bit corresponding to the thread's Lane ID is set to 1; otherwise, it is set to 0.

## Constraints

None

## Examples

The following example obtains the active status of each thread in the current warp, and Lane 0 writes the result to GM.

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelActiveMask(uint32_t* active_masks, uint64_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        uint32_t active_mask = asc_activemask(); // Obtain the mask of threads that are still active in the current warp.
        if (lane_id == 0) {
            active_masks[warp_id] = active_mask; // Lane 0 writes the active status of each thread in the current warp.
        }
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelActiveMask(
        __gm__ uint32_t* active_masks, uint64_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        // asc_vf_call parameter: dim3{1024, 1, 1}
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        uint32_t active_mask = asc_activemask(); // Obtain the mask of threads that are still active in the current warp.
        if (lane_id == 0) {
            active_masks[warp_id] = active_mask; // Lane 0 writes the active status of each thread in the current warp.
        }
    }
    ```

The input and output examples are as follows:

```text
active_masks: 0xFFFFFFFF
```
