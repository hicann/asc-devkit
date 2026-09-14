# shfl_xor

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T06:57:53.653Z pushedAt=2026-09-07T09:11:27.130Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the `var` value input by the thread whose rank is the result of a bitwise XOR operation between the current thread's rank and `lane_mask`.

## Function Prototype

```c++
template <typename T>
T shfl_xor(T var, unsigned int lane_mask) const
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. Supported data types: half, int32_t, uint32_t, float, half2, int64_t, uint64_t, bfloat16_t, bfloat16x2_t. |
| lane_mask | Input | Operand used in the XOR operation with the current thread's rank. |

## Return Value

Returns the `var` value of the specified thread in the `thread_block_tile` group.

## Constraints

`lane_mask` must be less than the number of threads in the `thread_block_tile` group.

## Example

Divides the thread block into groups of four threads, and obtains the `var` value input by the thread whose rank is the result of a bitwise XOR operation between the current thread's rank and `lane_mask` within the group.

**Figure 1**  Result of the shfl_xor interface  
![](../../../figures/thread_block_tile_shfl_xor.png "thread_block_tile_shfl_xor")

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        uint32_t result = tile4.shfl_xor(threadIdx.x + 100, 1);
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
        uint32_t result = tile4.shfl_xor(threadIdx.x + 100, 1);
        ...
    }
    ```
