# tiled_partition

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:31:15.341Z pushedAt=2026-10-26T06:23:22.633Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/tiled_partition_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

The `tiled_partition` API divides a thread group into multiple smaller, fixed-size subgroups so that threads can collaborate at a finer granularity. It provides both a template version and a non-template version, which are used for scenarios where the partition size is determined at compile time and at runtime, respectively. The template version supports creating a `thread_block_tile` that spans warps.

## Prototype

```cpp
template <unsigned int Size, typename ParentT>
thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g)
```

```cpp
thread_group tiled_partition(const thread_group& parent, unsigned int tilesz)
```

```cpp
thread_group tiled_partition(const thread_block& parent, unsigned int tilesz)
```

```cpp
coalesced_group tiled_partition(const coalesced_group& parent, unsigned int tilesz)
```

## Parameters

**Table 1** Template version parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| g | Input | Parent group to be partitioned. The type can only be `thread_block` or `thread_block_tile`. |
| Size | Input | Template parameter that specifies the size of the partitioned `thread_block_tile` group. |

**Table 2** Non-template version parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| parent | Input | Parent group to be partitioned. The type supports `thread_group`, `thread_block`, and `coalesced_group`. |
| tilesz | Input | Specifies the size of the partitioned subgroup. |

## Return Value

Returns the partitioned subgroup object.

## Constraints

- In the template version, `Size` must be $2^n$. The currently available values are 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, and 2048.
- The non-template version supports creating only subgroups with `tilesz <= 32`, and `tilesz` must be a power of 2 (that is, the value range is 1, 2, 4, 8, 16, and 32).
- To create a `thread_block_tile` that spans warps, create a [block_tile_memory](./thread_block/thread_block_constructor.md#block_tile_memory-description) as temporary storage, and create the parent `thread_block` through `this_thread_block` with the `scratch` parameter. The passed `block_tile_memory` object must reside in Global Memory or Unified Buffer (UB), and cannot be an object created in stack space. An object in UB delivers better performance than one in Global Memory.
- For the template version of the API, the number of threads in the parent group must be divisible by `Size`. In addition, if the parent group is a `thread_block_tile`, `Size` must be smaller than the parent group size.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel()
    {
        // No need to use a thread_block_tile that spans warps.
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);

        // Need to use a thread_block_tile that spans warps.
        __ubuf__ block_tile_memory<1024> scratch;
        thread_block block_with_memory = this_thread_block(scratch);
        auto tile64 = tiled_partition<64>(block_with_memory);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel()
    {
        // No need to use a thread_block_tile that spans warps.
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);

        // Need to use a thread_block_tile that spans warps.
        __ubuf__ block_tile_memory<1024> scratch;
        thread_block block_with_memory = this_thread_block(scratch);
        auto tile64 = tiled_partition<64>(block_with_memory);
    }
    ```
