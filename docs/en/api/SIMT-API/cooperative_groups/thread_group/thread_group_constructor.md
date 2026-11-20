# thread_group Constructor

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T13:00:12.445Z pushedAt=2026-10-26T06:23:22.711Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_group/thread_group_constructor_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

`thread_group` does not provide a default constructor. You can use the non-template version of `tiled_partition` to divide an existing cooperative group into subgroups of a runtime-specified size and obtain a type-erased `thread_group` object.

## Prototype

- The parent group to be divided is of type `thread_block`.

    ```cpp
    thread_group tiled_partition(const thread_block& parent, unsigned int tilesz)
    ```

- The parent group to be divided is of type `thread_group`.

    ```cpp
    thread_group tiled_partition(const thread_group& parent, unsigned int tilesz)
    ```

## Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| parent | Input | Parent cooperative group to be divided. Its type can only be `thread_block` or `thread_group`. |
| tilesz | Input | Size of the subgroup after division. It must be 1, 2, 4, 8, 16, or 32. |

## Return Value

Returns the `thread_group` subgroup to which the current thread belongs.

## Constraints

- `tilesz` must be a power of 2 and less than or equal to 32 (that is, its value range is 1, 2, 4, 8, 16, or 32).
- When the number of threads in the parent group is not divisible by `tilesz`, the last subgroup contains the remaining threads, and its group size is smaller than `tilesz`.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(unsigned long long* result)
    {
        thread_block block = this_thread_block();
        thread_group tile = tiled_partition(block, 16);
        unsigned long long rank = tile.thread_rank();
        result[threadIdx.x] = rank;
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ unsigned long long* result)
    {
        thread_block block = this_thread_block();
        thread_group tile = tiled_partition(block, 16);
        unsigned long long rank = tile.thread_rank();
        result[threadIdx.x] = rank;
    }
    ```

The input and output examples are as follows:

```text
result: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
```
