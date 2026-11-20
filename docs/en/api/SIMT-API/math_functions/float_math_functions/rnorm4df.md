# rnorm4df

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:24:03.465Z pushedAt=2026-10-26T11:03:37.404Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/rnorm4df_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the reciprocal of the square root of the sum of squares a^2 + b^2 + c^2 + d^2 of the input data a, b, c, and d.

![](../../../figures/formulaimage_0000002516816369.png)

## Prototype

```cpp
inline float rnorm4df(float a, float b, float c, float d)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| a | Input | Source operand. |
| b | Input | Source operand. |
| c | Input | Source operand. |
| d | Input | Source operand. |

## Return Value

Returns the reciprocal of the square root of a^2 + b^2 + c^2 + d^2.

-   If the square root of a^2 + b^2 + c^2 + d^2 exceeds the maximum float range, the return value is 0.
-   If the reciprocal of the square root of a^2 + b^2 + c^2 + d^2 exceeds the maximum float range, the return value is inf.
-   If a, b, c, and d are all 0, the return value is inf.
-   If any one or more of a, b, c, and d are ±inf, the return value is 0.
-   If any one or more of a, b, c, and d are nan and not ±inf, the return value is nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_rnorm4df(float *result, const float *x, const float *y, const float *z, const float *w, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rnorm4df(x[idx], y[idx], z[idx], w[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_rnorm4df_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, __gm__ const float *z, __gm__ const float *w, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rnorm4df(x[idx], y[idx], z[idx], w[idx]);
    }

    __global__ __vector__ void run_rnorm4df(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, __gm__ const float *z, __gm__ const float *w, uint32_t count)
    {
        asc_vf_call<compute_rnorm4df_vf>(dim3(256), result, x, y, z, w, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
z: -0.5, 0.5, 1.5, 2.5
w: 2, 3, 4, 5
result: 0.39036, 0.2495131, 0.1766043, 0.1353795
```
