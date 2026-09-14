# sync

<!-- md-trans-meta sourceCommit=dbde57aaba265583fc2f8f20806d114b1ff09dc0 translatedAt=2026-09-07T06:46:12.572Z pushedAt=2026-09-07T09:11:27.086Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Synchronizes all threads in the `coalesced_group`.

## Function Prototype

```c++
void sync() const
```

## Parameters

None

## Return Value

None

## Constraints

All threads in the `coalesced_group` must be able to reach the same `sync()` call; otherwise, the behavior is undefined.

## Example

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            active.sync();
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
            active.sync();
        }
        ...
    }
    ```
