# asc\_shfl

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T01:10:22.963Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

The Warp Shfl APIs mainly implement warp-level data exchange, allowing direct reading of data from a specific thread without going through shared memory. These APIs primarily implement data exchange operations among intra-group threads through warp grouping.

-   **Warp Grouping**

    Threads within a warp can be divided into multiple groups. You configure the group width (the number of threads in a group) through the **width** parameter. Threads within a group can exchange data, and intra-group threads are identified by an index relative to the starting thread position within the group, which is called the logical Lane ID.

-   **Data Exchange**

    This API mainly obtains the **var** value held by a specified thread within the group. You specify the thread through the src\_lane parameter. If src\_lane is greater than or equal to **width**, the logical Lane ID of the specified thread is src\_lane%width.

**Main Use Scenarios**

-   Data distribution: Broadcast data from a thread at a fixed position to other threads.
-   Dynamic data exchange: Each thread reads data from a different source thread.

For example, when 32 active threads within a warp call the asc\_shfl\(LaneId, 5, 16\) API, the return value of each thread is the **var** value of the thread numbered 5 within the group to which the current thread belongs.

**Figure 1**  asc\_shfl result illustration  
![](../../../figures/asc_shfl_result.png "asc_shfl result")

## Function Prototype

```
inline int32_t asc_shfl(int32_t var, int32_t src_lane, int32_t width = warpSize)
```

```
inline uint32_t asc_shfl(uint32_t var, int32_t src_lane, int32_t width = warpSize)
```

```
inline float asc_shfl(float var, int32_t src_lane, int32_t width = warpSize)
```

```
inline int64_t asc_shfl(int64_t var, int32_t src_lane, int32_t width = warpSize)
```

```
inline uint64_t asc_shfl(uint64_t var, int32_t src_lane, int32_t width = warpSize)
```

```
inline half asc_shfl(half var, int32_t src_lane, int32_t width = warpSize)
```

```
inline half2 asc_shfl(half2 var, int32_t src_lane, int32_t width = warpSize)
```

```
inline bfloat16_t asc_shfl(bfloat16_t var, int32_t src_lane, int32_t width = warpSize)
```

```
inline bfloat16x2_t asc_shfl(bfloat16x2_t var, int32_t src_lane, int32_t width = warpSize)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **var** | Input | Input operand used by the thread for exchange. |
| **src_lane** | Input | Lane ID of the thread that holds the desired **var** value. |
| **width** | Input | Group width of the threads participating in the exchange within the warp, defaulting to 32. The value range of **width** is (0, 32], and **width** must be a multiple of 2. |

## Return Value

Returns the **var** value of the specified thread in the warp.

## Constraints

-   If the target thread is inactive, an uninitialized value in the register is obtained.
-   If the input parameter **width** is not a multiple of 2 or exceeds 32, the return value is abnormal.

## Header Files to Include

To use interfaces of types other than half, half2, bfloat16\_t, and bfloat16x2\_t, include the "simt\_api/device\_warp\_functions.h" header file. To use interfaces of the half and half2 types, include the "simt\_api/asc\_fp16.h" header file. To use interfaces of the bfloat16\_t and bfloat16x2\_t types, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/device_warp_functions.h"
```

```
#include "simt_api/asc_fp16.h"
```

```
#include "simt_api/asc_bf16.h"
```

## Example

For the complete sample, see [InsertHashTable operator sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table/README.md).

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelShfl(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        // Threads 0-15 return 1, and threads 16-31 return 17.
        int32_t result = asc_shfl(laneId, 1, 16);
        dst[idx] = result;
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelShfl(__gm__ int32_t* dst)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}.
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        // Threads 0-15 return 1, and threads 16-31 return 17.
        int32_t result = asc_shfl(laneId, 1, 16);
        dst[idx] = result;
    }
    ```
