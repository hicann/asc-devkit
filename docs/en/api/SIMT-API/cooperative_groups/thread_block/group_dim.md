# group_dim

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:40:36.867Z pushedAt=2026-10-26T06:23:22.663Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block/group_dim_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the thread configuration of the thread block to which the current thread belongs. This API provides the same functionality as [dim_threads](dim_threads.md).

## Prototype

```cpp
static dim3 group_dim()
```

## Parameters

None

## Return Value

Returns the thread configuration of the thread block to which the current thread belongs.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(dim3* result)
    {
        thread_block g = this_thread_block();
        dim3 block_dim = g.group_dim();
        if (threadIdx.x == 0) {
            result[blockIdx.x].x = block_dim.x;
            result[blockIdx.x].y = block_dim.y;
            result[blockIdx.x].z = block_dim.z;
        }
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ dim3* result)
    {
        thread_block g = this_thread_block();
        dim3 block_dim = g.group_dim();
        if (threadIdx.x == 0) {
            result[blockIdx.x].x = block_dim.x;
            result[blockIdx.x].y = block_dim.y;
            result[blockIdx.x].z = block_dim.z;
        }
    }
    ```

The input and output examples are as follows:

```text
Use launch parameters: blocks = 4, threads = 1024.
result: dim3(1024, 1, 1), dim3(1024, 1, 1), dim3(1024, 1, 1), dim3(1024, 1, 1)
```
