# num_threads

<!-- md-trans-meta sourceCommit=e290f6324f3caf31e5b53e9c14d1dc9a5b1efebb translatedAt=2026-09-07T06:42:52.900Z pushedAt=2026-09-07T09:11:27.080Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the total number of threads in the `coalesced_group`.

## Function Prototype

```c++
unsigned long long num_threads() const
```

## Parameters

None

## Return Value

Returns the total number of threads in the `coalesced_group`.

## Constraints

None

## Example

In the example code, threads with even IDs cooperate through `coalesced_group`, while threads with odd IDs execute their tasks independently. A Warp contains 16 threads with even IDs.

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            unsigned long long thread_num = active.num_threads(); // Returns 16.
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
            unsigned long long thread_num = active.num_threads(); // Returns 16.
        }
        ...
    }
    ```
