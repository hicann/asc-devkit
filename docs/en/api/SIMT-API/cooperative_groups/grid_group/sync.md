# sync

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-16T10:55:40.460Z pushedAt=2026-10-16T11:12:29.246Z -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/grid_group/sync_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Synchronizes all SIMT threads in the current grid. Only after all threads in all thread blocks have reached this synchronization point can the threads in the grid continue execution.

## Prototype

```cpp
void sync() const
```

## Parameters

None

## Return Value

None

## Constraints

- Only the SIMT programming scenario is supported. The SIMD and SIMT hybrid programming scenario is not supported.
- Ensure that all threads in the grid can reach the same `sync()` call; otherwise, the behavior is undefined.
- Ensure that the total number of thread blocks in the grid does not exceed the number of AIV physical cores of the device to avoid program hangs. Developers can query the number of AIV physical cores of the device through the [aclrtGetDeviceInfo](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/04_device_management.md#aclrtGetDeviceInfo) API.
- The `sync()` API only ensures synchronization of thread instruction execution. It does not guarantee [cache coherence](../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md) between the Data Cache and Global Memory of each thread block.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(uint64_t* output)
    {
        grid_group g = this_grid(); // Obtain the grid_group object.
        unsigned int block = static_cast<unsigned int>(g.block_rank());

        if (threadIdx.x == 0) {
            output[block] = block + 1;
        }

        g.sync(); // Synchronize all threads in the grid.

        if (threadIdx.x == 0) {
            uint64_t sum = 0;
            asc_dcci_entire(output); // Flush the cache.
            for (unsigned int i = 0; i < g.num_blocks(); ++i) {
                sum += output[i];
            }
            output[g.num_blocks() + block] = sum;
        }
    }
    ```
