# meta_group_rank

<!-- md-trans-meta sourceCommit=e290f6324f3caf31e5b53e9c14d1dc9a5b1efebb translatedAt=2026-09-07T06:41:29.345Z pushedAt=2026-09-07T09:11:27.078Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the rank of the group to which the current thread belongs within the set of subgroups divided from its parent group. The rank starts from 0.

## Function Prototype

```c++
unsigned long long meta_group_rank() const
```

## Parameters

None

## Return Value

The rank of the group to which the current thread belongs within the set of subgroups divided from its parent group.

- If the group is created by `coalesced_threads`, the return value of `meta_group_rank()` is 0.

## Constraints

None

## Example

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            unsigned long long thread_num = active.meta_group_rank(); // Returns 0.
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
            unsigned long long thread_num = active.meta_group_rank(); // Returns 0.
        }
        ...
    }
    ```
