# shfl

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T06:44:47.617Z pushedAt=2026-09-07T09:11:27.087Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

A data exchange interface for threads within a `coalesced_group`, which directly reads the value in the register of a specified thread in the group without using shared memory.

## Function Prototype

```c++
template <typename T>
T shfl(T var, int src_rank) const
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| var | Input | Data used by the thread for exchange. Supported data types: half, int32_t, uint32_t, float, half2, int64_t, uint64_t, bfloat16_t, bfloat16x2_t. |
| src_rank | Input | Rank of the thread, within the group, that holds the desired `var` value. When `src_rank` is greater than or equal to the number of threads in the group, the `var` value of the thread whose rank is `src_rank % num_threads()` is obtained. |

## Return Value

The `var` value input by the specified thread in the `coalesced_group`.

## Constraints

None

## Example

In the sample code, the conditional branch groups all threads with even thread IDs in a warp into a `coalesced_group`. The result returned by the shfl interface for each thread in the group is shown in the following figure.

**Figure 1**  shfl result  
![](../../../figures/coalesced_group_shfl.png)

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            uint32_t result = active.shfl(threadIdx.x + 100, 3);
        }
        ...
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```c++
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            uint32_t result = active.shfl(threadIdx.x + 100, 3);
        }
        ...
    }
    ```
