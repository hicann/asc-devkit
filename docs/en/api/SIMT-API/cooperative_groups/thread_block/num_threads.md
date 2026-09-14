# num_threads

<!-- md-trans-meta sourceCommit=e290f6324f3caf31e5b53e9c14d1dc9a5b1efebb translatedAt=2026-09-07T06:48:59.754Z pushedAt=2026-09-07T09:11:27.096Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the total number of threads in the thread block to which the current thread belongs.

## Function Prototype

```c++
static unsigned int num_threads()
```

## Parameters

None

## Return Value

Total number of threads in the thread block to which the current thread belongs, that is, blockDim.x * blockDim.y * blockDim.z.

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
        unsigned int thread_num = g.num_threads();
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
        unsigned int thread_num = g.num_threads();
        ...
    }
    ```