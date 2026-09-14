# **coalesced_group** Constructor

<!-- md-trans-meta sourceCommit=e290f6324f3caf31e5b53e9c14d1dc9a5b1efebb translatedAt=2026-09-07T06:39:49.944Z pushedAt=2026-09-07T09:11:27.072Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

`coalesced_group` does not provide a default constructor. Users use the `coalesced_threads` function to create a `coalesced_group` object.

## Function Prototype

```c++
coalesced_group coalesced_threads()
```

## Parameters

None

## Return Value

Returns a `coalesced_group` object that contains all active threads in the current warp.

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
            coalesced_group active = coalesced_threads(); // The coalesced_group contains all threads with even thread IDs in the current warp.
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
            coalesced_group active = coalesced_threads(); // The coalesced_group contains all threads with even thread IDs in the current warp.
        }
        ...
    }
    ```
