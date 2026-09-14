# group_index

<!-- md-trans-meta sourceCommit=e290f6324f3caf31e5b53e9c14d1dc9a5b1efebb translatedAt=2026-09-07T06:48:43.109Z pushedAt=2026-09-07T09:11:27.095Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the three-dimensional index of the thread block to which the current thread belongs within the grid. The return value is the same as that of the built-in variable `blockIdx`.

## Function Prototype

```c++
static dim3 group_index()
```

## Parameters

None

## Return Value

Returns the three-dimensional index of the thread block to which the current thread belongs within the grid.

## Constraints

None

## Example

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block g = this_thread_block();
        dim3 block_idx = g.group_index();
        ...
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```c++
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        thread_block g = this_thread_block();
        dim3 block_idx = g.group_index();
        ...
    }
    ```