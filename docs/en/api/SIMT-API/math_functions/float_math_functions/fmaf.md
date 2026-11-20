# fmaf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T07:52:59.483Z pushedAt=2026-10-26T11:03:37.326Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/fmaf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

For input data x, y, and z, computes the result of x multiplied by y plus z.

![](../../../figures/formulaimage_0000002531284496.png)

## Prototype

```cpp
inline float fmaf(float x, float y, float z)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| z | Input | Source operand. |

## Return Value

Returns the value of x \* y + z.

-   When x is ±inf and y is ±0, nan is returned.
-   When x is ±0 and y is ±inf, nan is returned.
-   When x\*y is inf and z is -inf, nan is returned.
-   When x\*y is -inf and z is inf, nan is returned.
-   When x\*y+z exceeds the maximum value of the corresponding type range, inf is returned.
-   When x\*y+z is less than the minimum value of the corresponding type range, -inf is returned.
-   If any one of x, y, and z is nan, nan is returned.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_fmaf(float *result, const float *x, const float *y, const float *z, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = fmaf(x[idx], y[idx], z[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

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

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
z: -0.5, 0.5, 1.5, 2.5
result: -0.125, 2.375, 5.875, 10.375
```
