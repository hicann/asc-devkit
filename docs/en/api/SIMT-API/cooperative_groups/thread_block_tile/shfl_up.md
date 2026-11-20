# shfl_up

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:51:23.520Z pushedAt=2026-10-26T06:23:22.690Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block_tile/shfl_up_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the data of the thread that is `delta` positions ahead of the current thread within the `thread_block_tile` group.

## Prototype

```cpp
template <typename T>
T shfl_up(T var, unsigned int delta) const
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. Supported data types: half, int32_t, uint32_t, float, half2, int64_t, uint64_t, bfloat16_t, bfloat16x2_t. |
| delta | Input | Forward offset of the thread holding the desired `var` value relative to the current thread within the group. |

## Return Value

Returns the `var` value input by the thread that is `delta` positions ahead of the current thread within the cooperative group. If the offset exceeds the group range, the `var` value input by the current thread is returned.

## Constraints

Only the scenario where the template parameter `Size` of `thread_block_tile` is less than or equal to 32 and is a power of 2 (1, 2, 4, 8, 16, 32) is supported, that is, only thread subgroups within a single warp are supported.

## Examples

Divides the thread block into groups of four threads and obtains the `var` value input by the thread that is `delta` positions ahead of the current thread within the cooperative group.

**Figure 1**  Return result of the shfl_up API  
![](../../../figures/thread_block_tile_shfl_up.png "thread_block_tile_shfl_up")

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(uint32_t* result)
    {
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        result[threadIdx.x] = tile4.shfl_up(threadIdx.x + 100, 2);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ uint32_t* result)
    {
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        result[threadIdx.x] = tile4.shfl_up(threadIdx.x + 100, 2);
    }
    ```

The input and output examples are as follows:

```text
result: 100, 101, 100, 101, 104, 105, 104, 105, 108, 109, 108, 109, 112, 113, 112, 113
```
