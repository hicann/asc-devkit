# rnormf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:23:21.374Z pushedAt=2026-09-03T07:28:07.187Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the reciprocal of the square root of the sum of squares of the first n elements in the input data a, that is, a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2.

![](../../../figures/zh-cn_formulaimage_0000002516816371.png)

## Function Prototype

```
inline float rnormf(int n, float* a)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| n | Input | Source operand. Number of consecutive elements in the input data a to be computed. |
| a | Input | Source operand. Address of Unified Buffer, Global Memory, or stack space. |

## Return Value

The reciprocal of the square root of a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2.

-   If the square root of a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2 exceeds the maximum float range, the return value is 0.
-   If the reciprocal of the square root of a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2 exceeds the maximum float range, the return value is inf.
-   If a\[0\], a\[1\], ..., a\[n-1\] are all 0, the return value is inf.
-   If any one or more of a\[0\], a\[1\], ..., a\[n-1\] are ±inf, the return value is 0.
-   If any one or more of a\[0\], a\[1\], ..., a\[n-1\] are nan and none of them are ±inf, the return value is nan.
-   If n is less than 1, the reciprocal of the absolute value of a[0] is returned. If a[0] is 0, the return value is inf.

## Constraints

-   The length of the input data a must be greater than or equal to the parameter n.
-   If n is too large, the API performance cannot be guaranteed.
<!-- npu="950" id7 -->
-   For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: the internal implementation of this API uses the division operator. Since the division operator does not support the Subnormal scenario, when all elements of the input a are Subnormal data, the final result of this API is nan.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_rnormf(float *result, const int *n, float *vector_data, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rnormf(n[idx], vector_data + idx * 4);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_rnormf_vf(__gm__ float *result, __gm__ const int *n, __gm__ float *vector_data, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rnormf(n[idx], vector_data + idx * 4);
    }

    __global__ __vector__ void run_rnormf(__gm__ float *result, __gm__ const int *n, __gm__ float *vector_data, uint32_t count)
    {
        asc_vf_call<compute_rnormf_vf>(dim3(256), result, n, vector_data, count);
    }
    ```

The input and output examples are as follows:

```
n: 1, 2, 3, 1
vector_data: [[1, 2, 3, 4], [2, 3, 4, 5], [3, 4, 5, 6], [4, 5, 6, 7]]
result: 1 0.2773501 0.1414214 0.25
```
