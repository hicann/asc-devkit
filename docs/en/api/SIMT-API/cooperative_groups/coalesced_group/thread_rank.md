# thread_rank

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:28:16.038Z pushedAt=2026-10-26T06:23:22.625Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/coalesced_group/thread_rank_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the rank of the current thread within the `coalesced_group`, starting from 0.

## Prototype

```cpp
unsigned int thread_rank() const
```

## Parameters

None

## Return Value

Rank of the current thread within the `coalesced_group`.

## Constraints

None

## Examples

In the conditional branch of the sample code, threads whose thread IDs are even within a warp form a `coalesced_group`. The result returned by the `thread_rank` API for each thread in the group is shown in the following figure.

**Figure 1** Rank of each thread in the coalesced_group  
![](../../../figures/coalesced_group_rank.png "coalesced_group_rank")

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(uint32_t* result)
    {
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            result[threadIdx.x] = active.thread_rank();
        } else {
            result[threadIdx.x] = 0;
        }
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ uint32_t* result)
    {
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            result[threadIdx.x] = active.thread_rank();
        } else {
            result[threadIdx.x] = 0;
        }
    }
    ```

The input and output examples are as follows:

```text
result: 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15, 0
```
