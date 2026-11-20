# num_threads

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:58:13.682Z pushedAt=2026-10-26T06:23:22.705Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_group/num_threads_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the total number of threads in the current `thread_group`, which is functionally the same as [size()](size.md).

## Prototype

```cpp
unsigned long long num_threads() const
```

## Parameters

None

## Return Value

Returns the total number of threads in the current `thread_group`.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(unsigned long long* result)
    {
        thread_group tile = tiled_partition(this_thread_block(), 16);
        unsigned long long thread_num = tile.num_threads(); // Returns 16.
        result[threadIdx.x] = thread_num;
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ unsigned long long* result)
    {
        thread_group tile = tiled_partition(this_thread_block(), 16);
        unsigned long long thread_num = tile.num_threads(); // Returns 16.
        result[threadIdx.x] = thread_num;
    }
    ```

The input and output examples are as follows:

```text
result: 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16
```
