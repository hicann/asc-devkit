# shfl_up

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T06:43:29.703Z pushedAt=2026-09-07T09:11:27.084Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the data of the thread that is `delta` positions ahead of the current thread within the `coalesced_group`.

## Function Prototype

```c++
template <typename T>
T shfl_up(T var, unsigned int delta) const
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. Supported data types: half, int32_t, uint32_t, float, half2, int64_t, uint64_t, bfloat16_t, bfloat16x2_t. |
| delta | Input | Forward offset of the thread, relative to the current thread within the group, whose `var` value is to be obtained. |

## Return Value

The `var` value input by the thread that is `delta` positions ahead of the current thread within the `coalesced_group`. If the offset exceeds the group range, the `var` value input by the current thread is returned.

## Constraints

None

## Example

In the sample code, the conditional branch groups all threads with even thread IDs in a warp into a `coalesced_group`. The result returned by the `shfl_up` interface for each thread in the group is shown in the following figure.

**Figure 1**  Result of shfl_up
![](../../../figures/coalesced_group_shfl_up.png "Result of shfl_up")

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            uint32_t result = active.shfl_up(threadIdx.x + 100, 2);
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
            uint32_t result = active.shfl_up(threadIdx.x + 100, 2);
        }
        ...
    }
    ```
