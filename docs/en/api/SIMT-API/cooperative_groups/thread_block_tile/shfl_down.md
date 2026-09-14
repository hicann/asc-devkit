# shfl_down

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T06:56:41.945Z pushedAt=2026-09-07T09:11:27.127Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the data of the thread that is `delta` positions behind the current thread within the `thread_block_tile` group.

## Function Prototype

```c++
template <typename T>
T shfl_down(T var, unsigned int delta) const
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. Supported data types: half, int32_t, uint32_t, float, half2, int64_t, uint64_t, bfloat16_t, bfloat16x2_t. |
| delta | Input | Backward offset, relative to the current thread within the group, of the thread whose `var` value is to be obtained. |

## Return Value

Returns the `var` value input by the thread that is `delta` positions behind the current thread within the cooperative group. If the offset exceeds the group range, returns the `var` value input by the current thread.

## Constraints

None

## Example

Divides the thread block into groups of four threads and obtains the `var` value input by the thread that is `delta` positions behind the current thread within the cooperative group.

**Figure 1**  Schematic diagram of the return result of the shfl_down interface  
![](../../../figures/thread_block_tile_shfl_down.png "thread_block_tile_shfl_down")

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        uint32_t result = tile4.shfl_down(threadIdx.x + 100, 2);
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
        uint32_t result = tile4.shfl_down(threadIdx.x + 100, 2);
        ...
    }
    ```
