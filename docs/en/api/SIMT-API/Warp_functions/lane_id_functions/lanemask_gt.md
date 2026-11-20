# lanemask\_gt

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:00:10.453Z pushedAt=2026-10-27T09:21:55.761Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/lane_id_functions/lanemask_gt_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is strictly greater than that of the current thread are set to 1, and the remaining bits are 0.

For example, a thread with Lane ID 0 obtains the following 32-bit mask by calling this API: 1111 1111 1111 1111 1111 1111 1111 1110.

For example, a thread with Lane ID 31 obtains the following 32-bit mask by calling this API: 0000 0000 0000 0000 0000 0000 0000 0000.

**Figure 1**  lanemask\_gt schematic diagram

![](../../../figures/lanemask_gt.png "lanemask_gt diagram")

## Prototype

```cpp
int32_t lanemask_gt()
```

## Parameters

None

## Return Value

Returns a 32-bit integer bitmask.

## Constraints

None

## Examples

The following example uses `lanemask_gt()` to count the number of threads after the current Lane, determines whether the current thread is in the upper or lower half of the Warp based on the result, and then uses `asc_shfl()` to broadcast the data of the specified Lane in the upper and lower halves of the Warp, respectively. The example uses [asc_shfl](../Warp_shfl_functions/asc_shfl.md), which requires additionally including the `simt_api/device_warp_functions.h` header file, where the value range of `srcLane` is [0, 15].

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void kernel_lanemask_gt(int32_t* src, int32_t* dst, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lanes_after = __popc(static_cast<uint32_t>(lanemask_gt()));
        int32_t group_leader_lane = (lanes_after >= 16) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) void kernel_lanemask_gt(__gm__ int32_t* src, __gm__ int32_t* dst, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lanes_after = __popc(static_cast<uint32_t>(lanemask_gt()));
        int32_t group_leader_lane = (lanes_after >= 16) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```

The input and output examples are as follows:

```text
src: 62, 7, 91, 35, 48, 16, 83, 24, 75, 3, 56, 89, 12, 67, 40, 29, 94, 18, 53, 6, 79, 31, 70, 21, 85, 9, 44, 66, 13, 58, 37, 80
srcLane: 3
dst: 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
```
