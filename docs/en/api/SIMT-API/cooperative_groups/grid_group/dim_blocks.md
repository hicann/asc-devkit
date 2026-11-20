# dim_blocks

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:33:34.776Z pushedAt=2026-10-26T06:23:22.640Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/grid_group/dim_blocks_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the thread block dimension configuration of the current **Grid**.

## Prototype

```cpp
static dim3 dim_blocks()
```

## Parameters

None

## Return Value

Thread block dimension configuration of the current Grid.

## Constraints

Only the SIMT programming scenario is supported. The SIMD and SIMT hybrid programming scenario is not supported.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(dim3* result)
    {
        grid_group g = this_grid();
        dim3 dim_blocks = g.dim_blocks();
        if (threadIdx.x == 0) {
            result[blockIdx.x].x = dim_blocks.x;
            result[blockIdx.x].y = dim_blocks.y;
            result[blockIdx.x].z = dim_blocks.z;
        }
    }
    ```

The input and output examples are as follows:

```text
Use launch parameters: blocks = 4, threads = 1024
result: dim3(4, 1, 1), dim3(4, 1, 1), dim3(4, 1, 1), dim3(4, 1, 1)
```
