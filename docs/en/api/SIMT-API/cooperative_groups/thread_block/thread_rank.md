# thread_rank

<!-- md-trans-meta sourceCommit=e290f6324f3caf31e5b53e9c14d1dc9a5b1efebb translatedAt=2026-09-07T06:52:45.615Z pushedAt=2026-09-07T09:11:27.117Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the rank of the current thread among all threads in the thread block, starting from 0.

## Function Prototype

```c++
static unsigned int thread_rank()
```

## Parameters

None

## Return Value

Rank of the current thread among all threads in the thread block (equivalent to threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * blockDim.x * blockDim.y).

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
        unsigned int rank = g.thread_rank();
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
        unsigned int rank = g.thread_rank();
        ...
    }
    ```