# is_valid

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:36:28.480Z pushedAt=2026-10-26T06:23:22.651Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/grid_group/is_valid_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Determines whether the current `grid_group` can use the `sync()` API for cross-thread-block synchronization. `grid_group` supports synchronization by default, so the API always returns `true`.

## Prototype

```cpp
bool is_valid() const
```

## Parameters

None

## Return Value

Always returns `true`.

## Constraints

Only the SIMT programming scenario is supported. Mixed SIMD and SIMT programming is not supported.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel()
    {
        grid_group g = this_grid();
        if (g.is_valid()) {
            g.sync();
        }
    }
    ```
