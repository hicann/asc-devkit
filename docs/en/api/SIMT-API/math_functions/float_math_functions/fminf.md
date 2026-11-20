# fminf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T07:54:31.739Z pushedAt=2026-10-26T11:03:37.330Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/fminf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the minimum value of the two input data items.

![](../../../figures/formulaimage_0000002518941923.png)

## Prototype

```cpp
inline float fminf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the minimum value of the two input data items.

-   When **x** is **-0** and **y** is **+0**, returns **x**.
-   When **x** is **+0** and **y** is **-0**, returns **y**.
-   When x is nan, returns y.
-   When y is nan, returns x.
-   When both x and y are nan, returns nan.
-   When either **x** or **y** is **-inf**, the return value is **-inf**.
-   When **x** is **inf**, the return value is **y**.
-   When **y** is **inf**, the return value is **x**.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_fminf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fminf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_fminf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fminf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_fminf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_fminf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
result: 0.25, 0.75, 1.25, 1.75
```
