# thread_rank

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:46:09.699Z pushedAt=2026-10-26T06:23:22.677Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block/thread_rank_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the rank of the current thread among all threads in the thread block, starting from 0.

## Prototype

```cpp
static unsigned int thread_rank()
```

## Parameters

None

## Return Value

Rank of the current thread among all threads in the thread block (equivalent to `threadIdx.x + threadIdx.y * blockDim.x + threadIdx.z * blockDim.x * blockDim.y`).

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(unsigned int* result)
    {
        thread_block g = this_thread_block();
        unsigned int rank = g.thread_rank();
        result[threadIdx.x] = rank;
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ unsigned int* result)
    {
        thread_block g = this_thread_block();
        unsigned int rank = g.thread_rank();
        result[threadIdx.x] = rank;
    }
    ```

The input and output examples are as follows:

```text
result: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
```
