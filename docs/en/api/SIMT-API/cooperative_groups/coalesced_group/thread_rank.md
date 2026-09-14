# thread_rank

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T06:46:21.871Z pushedAt=2026-09-07T09:11:27.091Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the rank of the current thread within the `coalesced_group`, starting from 0.

## Function Prototype

```c++
unsigned long long thread_rank() const
```

## Parameters

None

## Return Value

Returns the rank of the current thread within the `coalesced_group`.

## Constraints

None

## Example

In the sample code, the conditional branch groups all threads with even thread IDs in a warp into a `coalesced_group`. The result returned by the `thread_rank` interface for each thread in the group is shown in the following figure.

**Figure 1**  Rank of each thread in the coalesced_group  
![](../../../figures/coalesced_group_rank.png "coalesced_group_rank")

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            unsigned long long rank = active.thread_rank();
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
            unsigned long long rank = active.thread_rank();
        }
        ...
    }
    ```
