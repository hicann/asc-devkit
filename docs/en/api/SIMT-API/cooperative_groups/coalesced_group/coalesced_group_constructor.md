# coalesced_group Constructor

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:19:54.258Z pushedAt=2026-10-26T06:23:22.610Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/coalesced_group/coalesced_group_constructor_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

`coalesced_group` does not provide a default constructor. Users create a `coalesced_group` object by using the `coalesced_threads` function.

## Prototype

```cpp
coalesced_group coalesced_threads()
```

## Parameters

None

## Return Value

Returns a `coalesced_group` object that contains all active threads in the current warp.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel()
    {
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads(); // This coalesced_group contains all threads with even thread IDs in the current warp.
        }
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel()
    {
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads(); // This coalesced_group contains all threads with even thread IDs in the current warp.
        }
    }
    ```
