# thread_block_tile Constructor

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-07T07:01:31.763Z pushedAt=2026-09-07T09:11:27.136Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

`thread_block_tile` does not provide a default constructor. It can be obtained by partitioning another cooperative group through the `tiled_partition` interface.

## Function Prototype

```c++
template <unsigned int Size, typename ParentT>
thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| g | Input | The parent group to be partitioned. Its type can only be `thread_block` or `thread_block_tile`. |
| Size | Input | Template parameter that specifies the size of the partitioned `thread_block_tile` group. |

## Return Value

Returns the `thread_block_tile` group to which the current thread belongs after partitioning.

## Constraints

- `Size` must be $2^n$ and must be less than or equal to 32 (warpSize). The currently supported values are 1, 2, 4, 8, 16, and 32.

## Example

- SIMT programming scenario:

    ```c++
    using namespace cooperative_groups;
    __global__ void simt_kernel(...)
    {
        ...
        thread_block block = this_thread_block();
        thread_block_tile<32> tile32 = tiled_partition<32>(block);              // Partition thread_block into groups of 32 threads.
        auto tile32_auto = tiled_partition<32>(block);                          // Use auto to manage the returned object.
        thread_block_tile<4, thread_block> tile4 = tiled_partition<4>(block);   // Partition thread_block into groups of 4 threads, retaining the parent group information in the object type.
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
        thread_block_tile<32> tile32 = tiled_partition<32>(block);              // Partition thread_block into groups of 32 threads.
        auto tile32_auto = tiled_partition<32>(block);                          // Use auto to manage the returned object.
        thread_block_tile<4, thread_block> tile4 = tiled_partition<4>(block);   // Partition thread_block into groups of 4 threads, and retain the parent group information in the object type.
        ...
    }
    ```
