# shfl

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T06:58:47.199Z pushedAt=2026-09-07T09:11:27.131Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

A data exchange interface for threads within a `thread_block_tile`, which directly reads the value in the register of a specified thread in the group without using shared memory.

## Function Prototype

```c++
template <typename T>
T shfl(T var, int src_rank) const
```

## Parameters


**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. Supported data types: half, int32_t, uint32_t, float, half2, int64_t, uint64_t, bfloat16_t, bfloat16x2_t. |
| src_rank | Input | Rank of the thread, within the group, whose `var` value is to be obtained. When `src_rank` is greater than or equal to the number of threads in the group, the `var` value of the thread whose rank is `src_rank % Size` is obtained. |

## Return Value

The `var` value input by the specified thread in the `thread_block_tile` group.

## Constraints

None

## Example

In the example code, the thread block is divided into groups of four threads, and the `var` value input by the thread with rank 2 in the group is obtained.

**Figure 1**   Schematic diagram of the return result of the shfl interface  
![](../../../figures/thread_block_tile_shfl.png "thread_block_tile_shfl")

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        uint32_t result = tile4.shfl(threadIdx.x + 100, 2);
        ...
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```c++
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        uint32_t result = tile4.shfl(threadIdx.x + 100, 2);
        ...
    }
    ```
