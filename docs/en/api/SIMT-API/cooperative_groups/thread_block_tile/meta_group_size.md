# meta_group_size

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:49:23.061Z pushedAt=2026-10-26T06:23:22.685Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block_tile/meta_group_size_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the number of subgroups created when the direct parent group of the cooperative group to which the current thread belongs is partitioned.

## Prototype

```cpp
unsigned int meta_group_size() const
```

## Parameters

None

## Return Value

The number of subgroups created when the direct parent group is partitioned.

## Constraints

None

## Examples

Each thread block contains 1024 threads, which are partitioned into groups of 4 threads, yielding a total of 256 subgroups.

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(unsigned int* result)
    {
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        unsigned int group_size = tile4.meta_group_size();    // Returns 256.
        result[threadIdx.x] = group_size;
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
        unsigned int group_size = tile4.meta_group_size();    // Returns 256.
        result[threadIdx.x] = group_size;
    }
    ```

The input and output examples are as follows:

```text
result: 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256
```
