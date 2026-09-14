# sync

<!-- md-trans-meta sourceCommit=dbde57aaba265583fc2f8f20806d114b1ff09dc0 translatedAt=2026-09-07T07:00:02.905Z pushedAt=2026-09-07T09:11:27.133Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Synchronizes all threads within the `thread_block_tile` group to which the current thread belongs.

## Function Prototype

```c++
void sync() const
```

## Parameters

None

## Return Value

None

## Constraints

All threads within the `thread_block_tile` group must be able to reach the same `sync()` call; otherwise, the behavior is undefined.

## Example

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        tile4.sync();
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
        tile4.sync();
        ...
    }
    ```