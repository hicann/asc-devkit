# scalblnf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:26:23.863Z pushedAt=2026-09-03T07:28:07.190Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the product of the input data **x** and 2 raised to the power of **n**.

![](../../../figures/zh-cn_formulaimage_0000002486788810.png)

## Function Prototype

```
inline float scalblnf(float x, int64_t n)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| n | Input | Source operand. |

## Return Value

Returns the result of multiplying the input data **x** by 2 raised to the power of **n**.

-   When x is nan, the return value is nan.
-   When x is inf, the return value is inf.
-   When x is -inf, the return value is -inf.

## Constraints

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: the internal implementation of this API uses the division operator. Since the division operator does not support the Subnormal scenario, in extremely rare cases the internal division result is Subnormal data, causing the final result of this API to be 0.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_scalblnf(float *result, const int64_t *n, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = scalblnf(x[idx], n[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_scalblnf_vf(__gm__ float *result, __gm__ const int64_t *n, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = scalblnf(x[idx], n[idx]);
    }

    __global__ __vector__ void run_scalblnf(__gm__ float *result, __gm__ const int64_t *n, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_scalblnf_vf>(dim3(256), result, n, x, count);
    }
    ```

The input and output examples are as follows:

```
n: 1, 2, 3, 1
x: 0.25, 0.75, 1.25, 1.75
result: 0.5 3 10 3.5
```
