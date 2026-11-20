# sync

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:59:39.536Z pushedAt=2026-10-26T06:23:22.708Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_group/sync_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Synchronizes all threads in the current `thread_group`.

## Prototype

```cpp
void sync() const
```

## Parameters

None

## Return Value

None

## Constraints

Ensure that all threads in the group can reach the same `sync()` call; otherwise, the behavior is undefined.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel()
    {
        thread_group tile = tiled_partition(this_thread_block(), 16);
        tile.sync();
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel()
    {
        thread_group tile = tiled_partition(this_thread_block(), 16);
        tile.sync();
    }
    ```
