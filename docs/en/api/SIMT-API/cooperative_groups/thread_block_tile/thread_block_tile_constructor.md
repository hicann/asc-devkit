# thread_block_tile Constructor

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:55:54.539Z pushedAt=2026-10-26T06:23:22.701Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block_tile/thread_block_tile_constructor_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

`thread_block_tile` does not provide a default constructor. It can be obtained by partitioning another cooperative group through the `tiled_partition()` API.

## Prototype

```cpp
template <unsigned int Size, typename ParentT>
thread_block_tile<Size, ParentT> tiled_partition(const ParentT& g)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| g | Input | Parent group to be partitioned. The type can only be `thread_block` or `thread_block_tile`. |
| Size | Input | Template parameter that specifies the size of the partitioned `thread_block_tile` group. |

## Return Value

Returns the `thread_block_tile` group to which the current thread belongs after partitioning.

## Constraints

- `Size` must be $2^n$. The currently available values are 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, and 2048.
- When `Size` is greater than 32, that is, in cross-warp cooperative scenarios where `Size` is 64, 128, 256, 512, 1024, or 2048, the parent `thread_block` must be created using [block_tile_memory](../thread_block/thread_block_constructor.md#block_tile_memory-description). In addition, the `block_tile_memory` object used to create the parent `thread_block` must reside in Global Memory or Unified Buffer (UB), and it cannot be an object created in stack space. An object residing in UB delivers better performance than one residing in Global Memory.
- For the template version of the API, the number of threads in the parent group must be divisible by `Size`. In addition, if the parent group is a `thread_block_tile`, `Size` must be smaller than the parent group size.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    constexpr int THREAD_NUM = 1024;
    __global__ void simt_kernel()
    {
        // Create a thread_block_tile with Size <= 32.
        thread_block block = this_thread_block();
        thread_block_tile<32> tile32 = tiled_partition<32>(block);              // Divide the thread_block into groups of 32 threads.
        auto tile32_auto = tiled_partition<32>(block);                          // Use auto to manage the returned object.
        thread_block_tile<4, thread_block> tile4 = tiled_partition<4>(block);   // Divide the thread_block into groups of 4 threads, retaining the parent group information in the object type.

        // Create a thread_block_tile with Size > 32.
        __ubuf__ block_tile_memory<THREAD_NUM> scratch;
        thread_block block_with_memory = this_thread_block(scratch);
        auto tile64 = tiled_partition<64>(block_with_memory);                   // Divide the thread_block into groups of 64 threads.
    }

    int main()
    {
        simt_kernel<<<dim3(1), dim3(THREAD_NUM), 0, stream>>>();             // The number of threads created in the thread block must be less than or equal to the template parameter of `block_tile_memory`.
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    constexpr int THREAD_NUM = 1024;
    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel()
    {
        // Create a thread_block_tile with Size <= 32.
        thread_block block = this_thread_block();
        thread_block_tile<32> tile32 = tiled_partition<32>(block);              // Divide the thread_block into groups of 32 threads.
        auto tile32_auto = tiled_partition<32>(block);                          // Use auto to manage the returned object.
        thread_block_tile<4, thread_block> tile4 = tiled_partition<4>(block);   // Partition thread_block into groups of 4 threads, retaining the parent group information in the object type.

        // Create a thread_block_tile with Size greater than 32.
        __ubuf__ block_tile_memory<THREAD_NUM> scratch = {};                    // In mixed programming scenarios, use this method to create the object in UB.
        thread_block block_with_memory = this_thread_block(scratch);
        auto tile64 = tiled_partition<64>(block_with_memory);                   // Use UB temporary storage to partition thread_block.
    }

    __global__ __vector__ void global_kernel()
    {
        asc_vf_call<simt_kernel>(dim3(THREAD_NUM));                        // The number of threads configured when launching the SIMT VF must be less than or equal to the template parameter of block_tile_memory.
    }
    ```
