# lanemask\_eq

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T03:59:07.111Z pushedAt=2026-10-27T09:21:55.756Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/lane_id_functions/lanemask_eq_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Obtains a 32-bit mask for the current thread. In the Warp to which the current thread belongs, only the lane bit corresponding to the current thread is set to 1, and the remaining bits are 0.

For example, a thread with Lane ID 0 obtains the following 32-bit mask by calling this API: 0000 0000 0000 0000 0000 0000 0000 0001.

For example, a thread with Lane ID 31 calls this API to obtain a 32-bit mask: 1000 0000 0000 0000 0000 0000 0000 0000.

**Figure 1**  lanemask\_eq Schematic diagram

![](../../../figures/lanemask_eq.png "lanemask_eq diagram")

## Prototype

```cpp
int32_t lanemask_eq()
```

## Parameters

None

## Return Value

Returns a 32-bit integer bitmask.

## Constraints

None

## Examples

The following example uses `lanemask_eq()` to obtain the bitmask corresponding to the current Lane, determines whether the current thread is in the upper or lower half of the Warp, and then uses `asc_shfl()` to broadcast the data of the specified Lane in the upper and lower halves of the Warp respectively. The example uses [asc_shfl](../Warp_shfl_functions/asc_shfl.md), which requires additionally including the `simt_api/device_warp_functions.h` header file. The value range of `srcLane` is [0, 15].

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void kernel_lanemask_eq(int32_t* dst, int32_t* src, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t self_mask = lanemask_eq();
        int32_t group_leader_lane = ((self_mask & 0x0000FFFFU) != 0) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) void kernel_lanemask_eq(__gm__ int32_t* dst, __gm__ int32_t* src, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t self_mask = lanemask_eq();
        int32_t group_leader_lane = ((self_mask & 0x0000FFFFU) != 0) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```

The input and output examples are as follows:

```text
src: 12, 45, 8, 39, 27, 55, 3, 68, 16, 72, 24, 6, 81, 33, 48, 19, 58, 2, 75, 31, 9, 64, 37, 51, 14, 88, 22, 5, 70, 42, 29, 66
srcLane: 3
dst: 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31
```
