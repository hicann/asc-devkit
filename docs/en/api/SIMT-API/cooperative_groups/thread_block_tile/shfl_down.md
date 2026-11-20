# shfl_down

<!-- md-trans-meta sourceCommit=9e88a90fcdb188f1c617d6165494ce195fe2a52b translatedAt=2026-10-23T12:50:55.137Z pushedAt=2026-10-26T06:23:22.688Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/cooperative_groups/thread_block_tile/shfl_down_res.md#id1 -->

## Description

The header file path is `"simt_api/cooperative_groups.h"`.

Obtains the data of the thread that is `delta` positions behind the current thread within the `thread_block_tile` group.

## Prototype

```cpp
template <typename T>
T shfl_down(T var, unsigned int delta) const
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. Supported data types: half, int32_t, uint32_t, float, half2, int64_t, uint64_t, bfloat16_t, bfloat16x2_t. |
| delta | Input | Backward offset, relative to the current thread, of the thread whose `var` value is to be obtained within the group. |

## Return Value

The `var` value input by the thread that is `delta` positions behind the current thread within the cooperative group. If the offset exceeds the group range, the `var` value input by the current thread is returned.

## Constraints

Only the scenario where the template parameter `Size` of `thread_block_tile` is a power of 2 (1, 2, 4, 8, 16, 32) and less than or equal to 32 is supported, that is, only thread subgroups within a single warp are supported.

## Examples

Divide the thread block into groups of 4 threads, and obtain the `var` value input by the thread that is `delta` positions behind the current thread within the cooperative group.

**Figure 1**  Diagram of the return result of the shfl_down API  
![](../../../figures/thread_block_tile_shfl_down.png "thread_block_tile_shfl_down")

- SIMT programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __global__ void simt_kernel(uint32_t* result)
    {
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        result[threadIdx.x] = tile4.shfl_down(threadIdx.x + 100, 2);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/cooperative_groups.h"

    using namespace cooperative_groups;
    __simt_vf__ inline void simt_kernel(__gm__ uint32_t* result)
    {
        thread_block block = this_thread_block();
        auto tile4 = tiled_partition<4>(block);
        result[threadIdx.x] = tile4.shfl_down(threadIdx.x + 100, 2);
    }
    ```

The input and output examples are as follows:

```text
result: 102, 103, 102, 103, 106, 107, 106, 107, 110, 111, 110, 111, 114, 115, 114, 115
```
