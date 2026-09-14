# asc\_shfl\_xor

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T01:11:12.700Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the **var** value provided by the thread corresponding to **dstLaneId**, which is obtained by performing an XOR operation (Lane ID ^ **lane_mask**) between the current thread's Lane ID in the warp and the input **lane_mask**; if the target thread is inactive, an uninitialized value in the register is obtained. The **width** parameter is used to divide the threads in the warp into groups. The **width** parameter sets the grouping width of the 32 threads participating in the exchange, with a default value of 32, meaning all threads are divided into one group.

In a multi-group scenario (**width** less than 32), each thread obtains the **var** value of the thread corresponding to **dstLaneId** that is located in the current group or in a group with a smaller thread number. That is, if **dstLaneId** is less than the starting Lane ID of the group where the current thread resides, the thread corresponding to **dstLaneId** is located in a group with a smaller thread number, and the **var** value of that **dstLaneId** thread can be obtained. If **dstLaneId** is greater than the maximum Lane ID of the group where the current thread resides, the **var** value of the current thread is returned.

For example, when 32 active threads in a warp call the **asc_shfl_xor** interface with **lane_mask** set to 1, the return value of each thread is the **var** value of the thread corresponding to the current thread's Lane ID ^ 1.

**Figure 1** Result illustration of **asc_shfl_xor**

![](../../../figures/asc_shfl_xor_result.png "Result illustration of asc_shfl_xor")

## Function Prototype

```
inline int32_t asc_shfl_xor(int32_t var, int32_t lane_mask, int32_t width = warpSize)
```

```
inline uint32_t asc_shfl_xor(uint32_t var, int32_t lane_mask, int32_t width = warpSize)
```

```
inline float asc_shfl_xor(float var, int32_t lane_mask, int32_t width = warpSize)
```

```
inline int64_t asc_shfl_xor(int64_t var, int32_t lane_mask, int32_t width = warpSize)
```

```
inline uint64_t asc_shfl_xor(uint64_t var, int32_t lane_mask, int32_t width = warpSize)
```

```
inline half asc_shfl_xor(half var, int32_t lane_mask, int32_t width = warpSize)
```

```
inline half2 asc_shfl_xor(half2 var, int32_t lane_mask, int32_t width = warpSize)
```

```
inline bfloat16_t asc_shfl_xor(bfloat16_t var, int32_t lane_mask, int32_t width = warpSize)
```

```
inline bfloat16x2_t asc_shfl_xor(bfloat16x2_t var, int32_t lane_mask, int32_t width = warpSize)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **var** | Input | Input operand used by the thread for exchange. |
| **lane_mask** | Input | Operand used for the XOR operation with the current thread's Lane ID. The value range is [0, 32), and it must be less than **width**. |
| **width** | Input | Grouping width of the threads participating in the exchange within the warp, with a default value of 32. The value range of **width** is (0, 32], and **width** must be a multiple of 2. |

## Return Value

Returns the **var** value of the specified thread in the warp.

## Constraints

If the target thread is inactive, an uninitialized value in the register is obtained.

## Header Files to Include

To use interfaces of types other than half, half2, bfloat16\_t, and bfloat16x2\_t, include the **simt\_api/device\_warp\_functions.h** header file. To use interfaces of the half and half2 types, include the **simt\_api/asc\_fp16.h** header file. To use interfaces of the bfloat16\_t and bfloat16x2\_t types, include the **simt\_api/asc\_bf16.h** header file.

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

The following examples cover two usage scenarios: Example 1 uses asc\_shfl\_xor to obtain the input value of the corresponding thread after XORing the current thread's Lane ID within the Warp group with lane\_mask; Example 2 uses asc\_shfl\_xor to perform reduction summation within each Warp.

-   Example 1:

    SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelShflXor(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        // The return values of threads 0-15 are {1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14}.
        // The return values of threads 16-31 are {17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30}.
        int32_t result = asc_shfl_xor(laneId, 1, 16);
        dst[idx] = result;
    }
    ```

    SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelShflXor(__gm__ int32_t* dst)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}.
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        // The return values of threads 0-15 are {1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14}.
        // The return values of threads 16-31 are {17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30}.
        int32_t result = asc_shfl_xor(laneId, 1, 16);
        dst[idx] = result;
    }
    ```

-   Example 2:

    SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelShflXorReduceSum(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t value = laneId;

        value += asc_shfl_xor(value, 1, 32);
        value += asc_shfl_xor(value, 2, 32);
        value += asc_shfl_xor(value, 4, 32);
        value += asc_shfl_xor(value, 8, 32);
        value += asc_shfl_xor(value, 16, 32);

        dst[idx] = value; // The reduction summation result is located in all threads within each Warp.
    }
    ```

    SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelShflXorReduceSum(__gm__ int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t value = laneId;

        value += asc_shfl_xor(value, 1, 32);
        value += asc_shfl_xor(value, 2, 32);
        value += asc_shfl_xor(value, 4, 32);
        value += asc_shfl_xor(value, 8, 32);
        value += asc_shfl_xor(value, 16, 32);

        dst[idx] = value; // The reduction summation result is located in all threads within each Warp.
    }
    ```
