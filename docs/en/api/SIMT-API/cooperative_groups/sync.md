# sync

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:31:05.051Z pushedAt=2026-10-26T06:23:22.631Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/sync_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Synchronizes all threads in the specified collaboration group. This is functionally equivalent to calling the member function `sync()` of the collaboration group object.

## Prototype

```cpp
template <typename GroupType>
void sync(const GroupType& g)
```

## Parameters

**Table 1**  Template parameters

| parameter name | Description |
| --- | --- |
| GroupType | Type of the collaboration group object. |

**Table 2**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| g | Input | Collaboration group object to be synchronized. |

## Return Value

None

## Constraints

- For `grid_group`, this API supports only the SIMT programming scenario and does not support mixed SIMD and SIMT programming.
- Ensure that all threads in the collaboration group can reach the same `sync(g)` call; otherwise, the behavior is undefined.
- When using `grid_group` for cross-thread-block synchronization, ensure that the total number of thread blocks in the grid does not exceed the number of AIV physical cores on the device to avoid program hang.
- Synchronization of `grid_group` guarantees only the synchronization of thread instruction execution, and does not guarantee cache coherence between the Data Cache and Global Memory of each thread block.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel()
    {
        thread_block block = this_thread_block();
        sync(block); // Equivalent to block.sync().

        grid_group grid = this_grid();
        sync(grid); // Equivalent to grid.sync().
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel()
    {
        thread_block block = this_thread_block();
        sync(block); // Equivalent to block.sync().
    }
    ```
