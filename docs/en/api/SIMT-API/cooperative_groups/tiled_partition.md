# tiled_partition

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T07:04:33.626Z pushedAt=2026-09-07T09:11:27.142Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

The `tiled_partition` API divides a thread group into multiple smaller, fixed-size subgroups so that threads can collaborate at a finer granularity. It provides both template and non-template versions, which are used for scenarios where the partition size is determined at compile time and at runtime, respectively.

## Function Prototype

```c++
template <unsigned int Size, typename ParentT>
thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g)
```

```c++
thread_group tiled_partition(const thread_group& parent, unsigned int tilesz)
```

```c++
thread_group tiled_partition(const thread_block& parent, unsigned int tilesz)
```

```c++
coalesced_group tiled_partition(const coalesced_group& parent, unsigned int tilesz)
```

## Parameters

**Table 1** Template version parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| g | Input | Parent group to be partitioned. Its type can only be `thread_block` or `thread_block_tile`. |
| Size | Input | Template parameter that specifies the size of the partitioned `thread_block_tile` group. |

**Table 2** Non-template version parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| parent | Input | Parent group to be partitioned. Its type can only be `thread_block` or `coalesced_group`. |
| tilesz | Input | Specifies the size of the partitioned subgroup. |

## Return Value

Returns the partitioned subgroup object.

## Constraints

- `Size` must be $2^n$ and must be less than or equal to 32 (warpSize). The currently supported values are 1, 2, 4, 8, 16, and 32.
- For the template version, the number of threads in the parent group must be divisible by `Size`. In addition, if the parent group is `thread_block_tile`, `Size` must be smaller than the parent group size.

## Example

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        ...
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```c++
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        ...
    }
    ```
