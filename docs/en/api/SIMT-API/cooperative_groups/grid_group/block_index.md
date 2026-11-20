# block_index

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:31:34.881Z pushedAt=2026-10-26T06:23:22.634Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/grid_group/block_index_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the 3D index of the thread block to which the current thread belongs in the Grid.

## Prototype

```cpp
static dim3 block_index()
```

## Parameters

None

## Return Value

Returns the 3D index of the thread block to which the current thread belongs in the Grid.

## Constraints

Only the SIMT programming scenario is supported. The hybrid SIMD and SIMT programming scenario is not supported.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(dim3* result)
    {
        grid_group g = this_grid();
        dim3 block_index = g.block_index();
        if (threadIdx.x == 0) {
            result[blockIdx.x].x = block_index.x;
            result[blockIdx.x].y = block_index.y;
            result[blockIdx.x].z = block_index.z;
        }
    }
    ```

The input and output examples are as follows:

```text
Launch parameters: blocks = 4, threads = 1024
result: dim3(0, 0, 0), dim3(1, 0, 0), dim3(2, 0, 0), dim3(3, 0, 0)
```
