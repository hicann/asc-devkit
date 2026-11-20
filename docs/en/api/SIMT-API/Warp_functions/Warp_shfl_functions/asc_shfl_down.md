# asc\_shfl\_down

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:08:31.524Z pushedAt=2026-10-27T09:21:55.781Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_down_res.md#id1 -->

## Description

The header file path is `"simt_api/device_warp_functions.h"` (for APIs other than the half, half2, bfloat16\_t, and bfloat16x2\_t types), `"simt_api/asc_fp16.h"` (for the half and half2 type APIs), and `"simt_api/asc_bf16.h"` (for the bfloat16\_t and bfloat16x2\_t type APIs).

Obtains the `var` value input by the thread that is backward offset by delta (current thread Lane ID + delta) from the current thread within the Warp for exchange. If the target thread is inactive, an uninitialized value in the register is obtained. The `width` parameter is used to divide the threads within the Warp into groups. The `width` parameter sets the group width of the 32 threads participating in the exchange, with a default value of 32, meaning all threads form one group.

In the multi-group scenario (width less than 32), the exchange operation of each group is independent, and each thread obtains the `var` value of the thread offset backward by delta from the current thread within its own group. If the thread number of the thread offset backward by delta from the current thread, that is, Lane ID + delta, is greater than the maximum Lane ID of the group, the `var` value of the current thread is returned.

For example, when 32 active threads within a Warp call the `asc_shfl_down(LaneId, 2, 16)` API, the return value of each thread is the `var` value of the thread corresponding to the current thread's Lane ID + 2, or the `var` value of the current thread.

**Figure 1**  asc\_shfl\_down result diagram

![](../../../figures/asc_shfl_down_result.png "asc_shfl_down result diagram")

## Prototype

```cpp
inline int32_t asc_shfl_down(int32_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline uint32_t asc_shfl_down(uint32_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline float asc_shfl_down(float var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline int64_t asc_shfl_down(int64_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline uint64_t asc_shfl_down(uint64_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline half asc_shfl_down(half var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline half2 asc_shfl_down(half2 var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline bfloat16_t asc_shfl_down(bfloat16_t var, uint32_t delta, int32_t width = warpSize)
```

```cpp
inline bfloat16x2_t asc_shfl_down(bfloat16x2_t var, uint32_t delta, int32_t width = warpSize)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| var | Input | Input operand used by the thread for exchange. |
| delta | Input | Backward offset, relative to the current thread, of the thread whose `var` value is to be obtained. |
| width | Input | Group width of the threads participating in the exchange within the warp. The default value is 32. The value range of width is (0, 32], and width must be a multiple of 2. |

## Return Value

Returns the var value of the specified thread within the Warp.

## Constraints

-   If the target thread is inactive, an uninitialized value in the register is obtained.
-   If delta \>=width, all threads return the var value of the current thread.
-   If width is not a multiple of 2 or exceeds 32, the return value is abnormal.

## Examples

For the complete example, see [Sobel edge detection example](../../../../../../examples/03_simt_api/02_features/01_api_features/03_warp_instruction/sobel_warp_shfl/README.md).

The following examples include two usage scenarios: Example 1 uses asc\_shfl\_down to obtain the input value of the thread that is offset backward by delta relative to the current thread within the Warp group; Example 2 uses asc\_shfl\_down to perform a reduction sum within each Warp.

-   Example 1:

    SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelShflDown(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        // Threads 0-15 return values {2,3,4,5,6,7,8,9,10,11,12,13,14,15,14,15} respectively.
        // Threads 16-31 return values {18,19,20,21,22,23,24,25,26,27,28,29,30,31,30,31}.
        int32_t result = asc_shfl_down(laneId, 2, 16);
        dst[idx] = result;
    }
    ```

    SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelShflDown(__gm__ int32_t* dst)
    {
        // asc_vf_call parameters: dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        // Threads 0-15 return values {2,3,4,5,6,7,8,9,10,11,12,13,14,15,14,15} respectively.
        // Threads 16-31 return values {18,19,20,21,22,23,24,25,26,27,28,29,30,31,30,31}.
        int32_t result = asc_shfl_down(laneId, 2, 16);
        dst[idx] = result;
    }
    ```

-   Example 2:

    SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelShflDownReduceSum(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t value = laneId;

        value += asc_shfl_down(value, 16, 32);
        value += asc_shfl_down(value, 8, 32);
        value += asc_shfl_down(value, 4, 32);
        value += asc_shfl_down(value, 2, 32);
        value += asc_shfl_down(value, 1, 32);

        dst[idx] = value; // The reduction sum result is located in the thread with laneId 0 in each Warp.
    }
    ```

    SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelShflDownReduceSum(__gm__ int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t value = laneId;

        value += asc_shfl_down(value, 16, 32);
        value += asc_shfl_down(value, 8, 32);
        value += asc_shfl_down(value, 4, 32);
        value += asc_shfl_down(value, 2, 32);
        value += asc_shfl_down(value, 1, 32);

        dst[idx] = value; // The reduction sum result is located in the thread with laneId 0 in each Warp.
    }
    ```

The input and output examples are as follows:

- Example 1:

    ```text
    x: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    dst: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 15, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 30, 31
    ```

- Example 2:

    ```text
    dst: 496, 512, 528, 544, 560, 576, 592, 608, 624, 640, 656, 672, 688, 704, 720, 736, 752, 768, 784, 800, 816, 832, 848, 864, 880, 896, 912, 928, 944, 960, 976, 992
    ```
