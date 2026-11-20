# meta_group_size

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:23:16.374Z pushedAt=2026-10-26T06:23:22.613Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/coalesced_group/meta_group_size_res.md#id1 -->

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

Returns the number of subgroups created when the direct parent group is partitioned.

- If the group is created through `coalesced_threads`, `meta_group_size()` returns 1.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(uint32_t* result)
    {
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            result[threadIdx.x] = active.meta_group_size(); // Returns 1.
        } else {
            result[threadIdx.x] = 0;
        }
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ uint32_t* result)
    {
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            result[threadIdx.x] = active.meta_group_size(); // Returns 1.
        } else {
            result[threadIdx.x] = 0;
        }
    }
    ```

The input and output examples are as follows:

```text
result: 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0
```
