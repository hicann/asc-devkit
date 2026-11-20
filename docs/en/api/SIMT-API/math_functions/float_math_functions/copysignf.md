# copysignf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T07:37:22.293Z pushedAt=2026-10-26T11:03:37.290Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/copysignf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains a floating-point number formed by concatenating the numeric part of the first input x and the sign part of the second input y.

![](../../../figures/formulaimage_0000002518941925.png)

## Prototype

```cpp
inline float copysignf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns a value whose magnitude is the absolute value of x and whose sign bit is the same as that of y.

- When the sign bit of y is 0, Abs\(x\) is returned.
- When the sign bit of y is 1, returns -Abs\(x\).
- When y = nan (0x7fc00000), returns Abs\(x\).
- When y = nan (0xffc00000), returns -Abs\(x\).
- When y = -inf, returns -Abs\(x\).
- When y=inf, returns Abs\(x\).

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_copysignf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = copysignf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_copysignf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = copysignf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_copysignf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_copysignf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, -0.75, 1.25, -1.75
y: 1.5, 2.5, -3.5, -4.5
result: 0.25, 0.75, -1.25, -1.75
```
