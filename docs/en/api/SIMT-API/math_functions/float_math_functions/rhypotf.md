# rhypotf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:21:46.806Z pushedAt=2026-10-26T11:03:37.398Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/rhypotf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the reciprocal of the square root of the sum of squares of the input data x and y, that is, the reciprocal of sqrt(x^2 + y^2).

![](../../../figures/formulaimage_0000002484776404.png)

## Prototype

```cpp
inline float rhypotf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Parameter name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the reciprocal of the square root of x^2 + y^2.

-   If the square root of x^2 + y^2 exceeds the maximum range of float, the function returns 0.
-   If the reciprocal of the square root of x^2 + y^2 exceeds the maximum range of float, the function returns inf.
-   If both x and y are 0, the function returns inf.
-   If either x or y is ±inf, the function returns 0.
-   If either x or y is nan and the other is not ±inf, the function returns nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_rhypotf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rhypotf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_rhypotf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rhypotf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_rhypotf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_rhypotf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
result: 0.6575959, 0.3831305, 0.2690691, 0.2071121
```
