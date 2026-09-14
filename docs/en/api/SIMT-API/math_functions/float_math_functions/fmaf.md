# fmaf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:53:26.825Z pushedAt=2026-09-03T07:28:07.120Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

For the input data **x**, **y**, and **z**, computes the result of multiplying **x** by **y** and adding **z**.

![](../../../figures/zh-cn_formulaimage_0000002531284496.png)

## Function Prototype

```
inline float fmaf(float x, float y, float z)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| z | Input | Source operand. |

## Return Value

Returns the value of x \* y + z.

-   If x is ±inf and y is ±0, nan is returned.
-   If x is ±0 and y is ±inf, nan is returned.
-   If x\*y is inf and z is -inf, nan is returned.
-   If x\*y is -inf and z is inf, nan is returned.
-   If x\*y+z exceeds the maximum value of the corresponding type range, inf is returned.
-   If x\*y+z is less than the minimum value of the corresponding type range, -inf is returned.
-   If any of x, y, or z is nan, nan is returned.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_fmaf(float *result, const float *x, const float *y, const float *z, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fmaf(x[idx], y[idx], z[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_fmaf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, __gm__ const float *z, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fmaf(x[idx], y[idx], z[idx]);
    }

    __global__ __vector__ void run_fmaf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, __gm__ const float *z, uint32_t count)
    {
        asc_vf_call<compute_fmaf_vf>(dim3(256), result, x, y, z, count);
    }
    ```

The following shows an example of the input and output:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
z: -0.5, 0.5, 1.5, 2.5
result: -0.125 2.375 5.875 10.375
```
