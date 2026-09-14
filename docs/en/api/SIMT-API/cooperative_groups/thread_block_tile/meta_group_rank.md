# meta_group_rank

<!-- md-trans-meta sourceCommit=e290f6324f3caf31e5b53e9c14d1dc9a5b1efebb translatedAt=2026-09-07T06:54:31.887Z pushedAt=2026-09-07T09:11:27.123Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the rank of the group to which the current thread belongs within the set of child groups partitioned from the parent group. The rank starts from 0.

## Function Prototype

```c++
unsigned long long meta_group_rank() const
```

## Parameters

None

## Return Value

Returns the rank of the group to which the current thread belongs within the set of child groups partitioned from the parent group.

## Constraints

None

## Example

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        unsigned long long group_rank = tile4.meta_group_rank();
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
        unsigned long long group_rank = tile4.meta_group_rank();
        ...
    }
    ```