# thread_rank

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T07:02:52.114Z pushedAt=2026-09-07T09:11:27.139Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the rank of the current thread within the `thread_block_tile` group it belongs to. The rank starts from 0.

## Function Prototype

```c++
unsigned long long thread_rank() const
```

## Parameters

None

## Return Value

The rank of the current thread within the `thread_block_tile` group it belongs to.

## Constraints

None

## Example

In the sample code, the thread block is divided into groups of four threads. The rank of each thread within its `thread_block_tile` group is shown in the following figure.

**Figure 1**   Schematic diagram of the return value of the thread_rank API  
![](../../../figures/thread_block_tile_rank.png "thread_block_tile_rank")

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        unsigned long long rank = tile4.thread_rank();
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
        unsigned long long rank = tile4.thread_rank();
        ...
    }
    ```
