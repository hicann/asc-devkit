# asc\_shfl

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:10:16.331Z pushedAt=2026-10-27T09:21:55.786Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_res.md#id1 -->

## Description

The header file paths are: `"simt_api/device_warp_functions.h"` (for APIs other than the half, half2, bfloat16_t, and bfloat16x2_t types), `"simt_api/asc_fp16.h"` (for the half and half2 type APIs), and `"simt_api/asc_bf16.h"` (for the bfloat16_t and bfloat16x2_t type APIs).

The Warp Shfl APIs implement warp-level data exchange, allowing direct reading of data from a specific thread without going through shared memory. These APIs implement data exchange between threads in the group primarily through warp grouping.

- **Warp grouping**

    Threads in a warp can be divided into multiple groups. You configure the group width (the number of threads in a group) through the `width` parameter. Threads within a group can exchange data, and each thread in the group is identified by an index relative to the starting thread position of the group, which is called the logical Lane ID.

- **Data exchange**

    This API obtains the `var` value held by a specified thread in the group. You specify the thread through the `src_lane` parameter. If `src_lane` is greater than or equal to `width`, the logical Lane ID of the specified thread is `src_lane % width`.

**Main use cases**

- Data distribution: broadcast data from a thread at a fixed position to other threads.
- Dynamic data exchange: each thread reads data from a different source thread.

For example, when 32 active threads in a warp call the `asc_shfl(LaneId, 5, 16)` API, the return value of each thread is the var value of the thread numbered 5 in the group to which the current thread belongs.

**Figure 1**  asc\_shfl Result Diagram  
![](../../../figures/asc_shfl_result.png "asc_shfl result")

## Prototype

```cpp
inline int32_t asc_shfl(int32_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline uint32_t asc_shfl(uint32_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline float asc_shfl(float var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline int64_t asc_shfl(int64_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline uint64_t asc_shfl(uint64_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline half asc_shfl(half var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline half2 asc_shfl(half2 var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline bfloat16_t asc_shfl(bfloat16_t var, int32_t src_lane, int32_t width = warpSize)
```

```cpp
inline bfloat16x2_t asc_shfl(bfloat16x2_t var, int32_t src_lane, int32_t width = warpSize)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. |
| src_lane | Input | Lane ID of the thread whose var value is to be obtained. |
| width | Input | Group width of the threads participating in the exchange within the warp. The default value is 32. The value range of width is (0, 32], and width must be a multiple of 2. |

## Return Value

Returns the `var` value of the specified thread in the warp.

## Constraints

-   If the target thread is inactive, an uninitialized value in the register is obtained.
-   If the input parameter `width` is not a multiple of 2 or exceeds 32, the return value is abnormal.

## Examples

For the complete sample, see [InsertHashTable operator sample](../../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table/README.md).

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void kernel_shfl(int32_t* dst, int32_t total_num)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_num) {
            return;
        }
        int32_t laneId = idx % 32;
        // Threads 0-15 return 1, and threads 16-31 return 17.
        int32_t result = asc_shfl(laneId, 1, 16);
        dst[idx] = result;
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_shfl(__gm__ int32_t* dst, int32_t total_num)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}.
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_num) {
            return;
        }
        int32_t laneId = idx % 32;
        // Threads 0-15 return 1, and threads 16-31 return 17.
        int32_t result = asc_shfl(laneId, 1, 16);
        dst[idx] = result;
    }
    ```

The input and output examples are as follows:

```text
src: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
dst: 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17
```
