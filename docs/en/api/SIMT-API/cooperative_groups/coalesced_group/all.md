# all

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:17:13.512Z pushedAt=2026-10-26T06:23:22.602Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/coalesced_group/all_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Determines whether the inputs of all threads in the `coalesced_group` are non-zero.

## Prototype

```cpp
int all(int predicate) const
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| predicate | Input | Operand. |

## Return Value

Returns 1 if the inputs of all threads in the `coalesced_group` are non-zero; otherwise, returns 0.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(int32_t* result)
    {
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            result[threadIdx.x] = active.all(1); // Returns 1.
        } else {
            result[threadIdx.x] = 0;
        }
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ int32_t* result)
    {
        if (threadIdx.x % 2 == 0) {
            coalesced_group active = coalesced_threads();
            result[threadIdx.x] = active.all(1); // Returns 1.
        } else {
            result[threadIdx.x] = 0;
        }
    }
    ```

The input and output examples are as follows:

```text
result: 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0
```
