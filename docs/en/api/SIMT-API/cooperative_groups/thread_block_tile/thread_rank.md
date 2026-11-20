# thread_rank

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:57:14.628Z pushedAt=2026-10-26T06:23:22.703Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block_tile/thread_rank_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the rank of the current thread within the `thread_block_tile` group it belongs to. The ranking starts from 0.

## Prototype

```cpp
unsigned int thread_rank() const
```

## Parameters

None

## Return Value

Rank of the current thread within the `thread_block_tile` group it belongs to.

## Constraints

None

## Examples

In the sample code, the thread block is divided into groups of four threads. The rank of each thread within the `thread_block_tile` group it belongs to is shown in the following figure.

**Figure 1**   Schematic diagram of the return value of the thread_rank API  
![](../../../figures/thread_block_tile_rank.png "thread_block_tile_rank")

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(unsigned int* result)
    {
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        unsigned int rank = tile4.thread_rank();
        result[threadIdx.x] = rank;
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ unsigned int* result)
    {
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        unsigned int rank = tile4.thread_rank();
        result[threadIdx.x] = rank;
    }
    ```

The input and output examples are as follows:

```text
result: 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3
```
