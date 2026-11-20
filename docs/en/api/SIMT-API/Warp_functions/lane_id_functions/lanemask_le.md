# lanemask\_le

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:01:44.758Z pushedAt=2026-10-27T09:21:55.765Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/lane_id_functions/lanemask_le_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Provides a 32-bit mask for the current thread. In the Warp to which the current thread belongs, the corresponding bits of the threads whose Lane ID is less than or equal to that of the current thread are set to 1, and the remaining bits are 0.

For example, a thread with Lane ID 0 calls this API to obtain a 32-bit mask: `0000 0000 0000 0000 0000 0000 0000 0001`.

For example, a thread with Lane ID 31 calls this API to obtain a 32-bit mask: `1111 1111 1111 1111 1111 1111 1111 1111`.

**Figure 1**  lanemask\_le Schematic diagram

![](../../../figures/lanemask_le.png "lanemask_le diagram")

## Prototype

```cpp
int32_t lanemask_le()
```

## Parameters

None

## Return Value

Returns a 32-bit integer bitmask.

## Constraints

None

## Examples

The following example uses `lanemask_le()` to count the current lane and the lanes preceding it, determines whether the current thread is in the upper or lower half of the Warp, and then uses `asc_shfl()` to broadcast the data of the specified Lane in the upper and lower halves of the Warp respectively. The example uses [asc_shfl](../Warp_shfl_functions/asc_shfl.md), which requires additionally including the `simt_api/device_warp_functions.h` header file, where the value range of `srcLane` is [0, 15].

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void kernel_lanemask_le(int32_t* src, int32_t* dst, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_rank = __popc(static_cast<uint32_t>(lanemask_le())) - 1;
        int32_t group_leader_lane = (lane_rank < 16) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) void kernel_lanemask_le(__gm__ int32_t* src, __gm__ int32_t* dst, int32_t srcLane)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_rank = __popc(static_cast<uint32_t>(lanemask_le())) - 1;
        int32_t group_leader_lane = (lane_rank < 16) ? srcLane : (srcLane + 16);
        int32_t value = src[idx];
        value = asc_shfl(value, group_leader_lane, 32);
        dst[idx] = value;
    }
    ```

The input and output examples are as follows:

```text
src: 28, 73, 10, 65, 39, 94, 17, 52, 81, 4, 77, 33, 68, 21, 88, 46, 7, 59, 96, 25, 71, 14, 83, 37, 62, 9, 78, 30, 91, 48, 19, 55
srcLane: 3
dst: 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25
```
