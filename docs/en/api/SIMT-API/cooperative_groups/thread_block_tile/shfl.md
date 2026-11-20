# shfl

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:52:46.320Z pushedAt=2026-10-26T06:23:22.693Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block_tile/shfl_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Data exchange API for intra-group threads in a `thread_block_tile`, used to obtain the `var` value input by the thread with the specified rank in the group.

## Prototype

```cpp
template <typename T>
T shfl(T var, int src_rank) const
```

## Parameters


**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. Supported data types: half, int32_t, uint32_t, float, half2, int64_t, uint64_t, bfloat16_t, bfloat16x2_t. |
| src_rank | Input | Rank of the thread, within the group, that holds the desired `var` value. When `src_rank` is greater than or equal to the number of threads in the group, the `var` value of the thread whose rank is `src_rank % Size` is obtained. |

## Return Value

The `var` value input by the specified thread in the `thread_block_tile` group.

## Constraints

- In the cross-Warp `thread_block_tile` scenario (Size>32)
    - All threads in the `thread_block_tile` group must specify the same `src_rank`; otherwise, the behavior is undefined.
    - All threads in the `thread_block_tile` group must be active; otherwise, the behavior is undefined.

## Examples

In the example code, the thread block is divided into groups of four threads, and the `var` value input by the thread with rank 2 in the group is obtained.

**Figure 1**   Schematic diagram of the return result of the shfl API  
![](../../../figures/thread_block_tile_shfl.png "thread_block_tile_shfl")

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(uint32_t* result)
    {
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        result[threadIdx.x] = tile4.shfl(threadIdx.x + 100, 2);
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
        result[threadIdx.x] = tile4.shfl(threadIdx.x + 100, 2);
    }
    ```

The input and output examples are as follows:

```text
result: 102, 102, 102, 102, 106, 106, 106, 106, 110, 110, 110, 110, 114, 114, 114, 114
```
