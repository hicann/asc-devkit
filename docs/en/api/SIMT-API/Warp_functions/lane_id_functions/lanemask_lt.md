# lanemask\_lt

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:01:54.085Z pushedAt=2026-10-27T09:21:55.766Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/lane_id_functions/lanemask_lt_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is strictly less than that of the current thread are set to 1, and the remaining bits are 0.

For example, a thread with Lane ID 0 obtains the following 32-bit mask by calling this API: 0000 0000 0000 0000 0000 0000 0000 0000.

For example, a thread with Lane ID 31 obtains the following 32-bit mask by calling this API: 0111 1111 1111 1111 1111 1111 1111 1111.

**Figure 1**  lanemask\_lt Schematic diagram

![](../../../figures/lanemask_lt.png "lanemask_lt diagram")

## Prototype

```cpp
int32_t lanemask_lt()
```

## Parameters

None

## Return Value

Returns a 32-bit integer bitmask.

## Constraints

None

## Examples

The following example uses `lanemask_lt()` to count the number of threads before the current Lane, determines whether the current thread is in the upper or lower half of the Warp, and then uses `asc_shfl()` to broadcast the data of the specified Lane in the upper and lower halves of the Warp, respectively. The example uses [asc_shfl](../Warp_shfl_functions/asc_shfl.md), which requires additionally including the `simt_api/device_warp_functions.h` header file, where the value range of `srcLane` is [0, 15].

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void kernel_lanemask_lt(int32_t* src, int32_t* dst, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lanes_before = __popc(static_cast<uint32_t>(lanemask_lt()));
        int32_t group_leader_lane = (lanes_before < 16) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) void kernel_lanemask_lt(__gm__ int32_t* src, __gm__ int32_t* dst, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lanes_before = __popc(static_cast<uint32_t>(lanemask_lt()));
        int32_t group_leader_lane = (lanes_before < 16) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```

The input and output examples are as follows:

```text
src: 45, 12, 90, 27, 63, 8, 76, 34, 51, 18, 95, 3, 69, 41, 22, 84, 57, 15, 72, 36, 11, 66, 29, 93, 47, 6, 80, 23, 58, 31, 87, 20
srcLane: 3
dst: 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36
```
