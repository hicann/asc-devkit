# asc\_shfl\_xor

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:10:12.116Z pushedAt=2026-10-27T09:21:55.782Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_xor_res.md#id1 -->

## Description

The header file path is `"simt_api/device_warp_functions.h"` (for APIs other than the half, half2, bfloat16\_t, and bfloat16x2\_t types), `"simt_api/asc_fp16.h"` (for the half and half2 type APIs), and `"simt_api/asc_bf16.h"` (for the bfloat16\_t and bfloat16x2\_t type APIs).

Obtains the var value that the thread corresponding to dstLaneId inputs for exchange, where dstLaneId is obtained by performing an XOR operation on the current thread's Lane ID within the Warp and the input lane\_mask (Lane ID ^ lane\_mask). If the target thread is inactive, an uninitialized value in the register is obtained. The width parameter is used to divide the threads within the Warp into groups. The width parameter sets the group width of the 32 threads participating in the exchange, with a default value of 32, meaning that all threads form one group.

In a multi-group scenario (width less than 32), each thread obtains the var value of the thread corresponding to dstLaneId that is located within the same group or in a group with a smaller thread number. That is, if dstLaneId is less than the starting Lane ID of the group to which the current thread belongs, the thread corresponding to dstLaneId is located in a group with a smaller thread number, and the var value of that dstLaneId thread can be obtained. If dstLaneId is greater than the maximum Lane ID of the group to which the current thread belongs, the var value of the current thread is returned.

For example, when 32 active threads within a Warp call the asc\_shfl\_xor\(LaneId, 1, 16\) API, the return value of each thread is the var value of the thread corresponding to the current thread's Lane ID ^ 1.

**Figure 1**  asc\_shfl\_xor result diagram

![](../../../figures/asc_shfl_xor_result.png "asc_shfl_xor result diagram")

## Prototype

```cpp
inline int32_t asc_shfl_xor(int32_t var, int32_t lane_mask, int32_t width = warpSize)
```

```cpp
inline uint32_t asc_shfl_xor(uint32_t var, int32_t lane_mask, int32_t width = warpSize)
```

```cpp
inline float asc_shfl_xor(float var, int32_t lane_mask, int32_t width = warpSize)
```

```cpp
inline int64_t asc_shfl_xor(int64_t var, int32_t lane_mask, int32_t width = warpSize)
```

```cpp
inline uint64_t asc_shfl_xor(uint64_t var, int32_t lane_mask, int32_t width = warpSize)
```

```cpp
inline half asc_shfl_xor(half var, int32_t lane_mask, int32_t width = warpSize)
```

```cpp
inline half2 asc_shfl_xor(half2 var, int32_t lane_mask, int32_t width = warpSize)
```

```cpp
inline bfloat16_t asc_shfl_xor(bfloat16_t var, int32_t lane_mask, int32_t width = warpSize)
```

```cpp
inline bfloat16x2_t asc_shfl_xor(bfloat16x2_t var, int32_t lane_mask, int32_t width = warpSize)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. |
| lane_mask | Input | Operand for the XOR operation with the current thread's Lane ID. The value range is [0, 32) and must be less than width. |
| width | Input | Group width of the threads participating in the exchange within the warp. The default value is 32. The value range of width is (0, 32], and width must be a multiple of 2. |

## Return Value

Returns the var value of the specified thread within the Warp.

## Constraints

If the target thread is inactive, an uninitialized value in the register is obtained.

## Examples

The following examples cover two usage scenarios: Example 1 uses asc\_shfl\_xor to obtain the input value of the corresponding thread after XORing the current thread's Lane ID within the Warp group with lane\_mask; Example 2 uses asc\_shfl\_xor to perform a reduction sum within each Warp.

-   Example 1:

    SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelShflXor(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        // Return values of threads 0-15 are {1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14}.
        // Return values of threads 16-31 are {17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30}.
        int32_t result = asc_shfl_xor(laneId, 1, 16);
        dst[idx] = result;
    }
    ```

    SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelShflXor(__gm__ int32_t* dst)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}.
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        // Return values of threads 0-15 are {1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14}.
        // Return values of threads 16-31 are {17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30}.
        int32_t result = asc_shfl_xor(laneId, 1, 16);
        dst[idx] = result;
    }
    ```

-   Example 2:

    SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

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

        dst[idx] = value; // The reduction sum result is available in all threads within each warp.
    }
    ```

    SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

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

        dst[idx] = value; // The reduction sum result is available in all threads within each warp.
    }
    ```

The input and output examples are as follows:

- Example 1:

    ```text
    x: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    dst: 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14, 17, 16, 19, 18, 21, 20, 23, 22, 25, 24, 27, 26, 29, 28, 31, 30
    ```

- Example 2:

    ```text
    dst: 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496, 496
    ```
