# binary_partition

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:28:24.208Z pushedAt=2026-10-26T06:23:22.627Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/binary_partition_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

The `binary_partition` API divides a parent group into two subgroups based on a label (0 or 1). Threads with the same label are assigned to the same subgroup.

## Prototype

```cpp
coalesced_group binary_partition(const coalesced_group& g, bool pred)
```

```cpp
template <unsigned int Size, typename ParentT>
coalesced_group binary_partition(const thread_block_tile<Size, ParentT>& g, bool pred)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| g | Input | Parent group to be divided. The type can be `coalesced_group` or `thread_block_tile`. |
| pred | Input | Label used to divide the subgroups. |

## Return Value

Returns the subgroup that contains the current calling thread. The current thread and all threads with the same `pred` label are assigned to the same subgroup. The return type is `coalesced_group`.

## Constraints

- All threads in the group must call this API collectively; otherwise, the behavior is undefined.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(int *inputArr)
    {
        auto block = this_thread_block();
        auto tile32 = tiled_partition<32>(block);

        // inputArr contains random integers.
        int elem = inputArr[block.thread_rank()];
        // Divide tile32 into two subgroups based on whether elem&1 is true.
        auto subtile = binary_partition(tile32, (elem & 1));
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ int *inputArr)
    {
        auto block = this_thread_block();
        auto tile32 = tiled_partition<32>(block);

        // inputArr contains random integers.
        int elem = inputArr[block.thread_rank()];
        // Divide tile32 into two subgroups based on whether elem&1 is true.
        auto subtile = binary_partition(tile32, (elem & 1));
    }
    ```
