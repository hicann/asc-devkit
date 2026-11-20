# \_\_fdividef

<!-- md-trans-meta sourceCommit=e80e2531aba21d60ee54c52d2ae137dd3bc302e2 translatedAt=2026-10-26T07:23:57.087Z pushedAt=2026-10-26T11:03:37.269Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/__fdividef_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Obtains the result of dividing two input data values.

## Prototype

```cpp
float __fdividef(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Result of dividing two input data values. Special values are as follows:

-   When x is a positive number and y is 0.0, returns +inf.
-   When x is a negative number and y is 0.0, returns -inf.
-   When x is 0.0 and y is 0.0, returns nan.
-   When either x or y is nan, returns nan.

## Constraints

-   This API does not support the Subnormal scenario: input and output values within the Subnormal range are flushed to zero with the sign preserved.
-   This API is not affected by the compilation option [--cce-use-fast-math](../../../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#common-compilation-options).

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(256) void compute_fdivide_intrinsic(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = __fdividef(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_fdivide_intrinsic_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = __fdividef(x[idx], y[idx]);
    }

    __global__ __vector__ void run_fdivide_intrinsic(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_fdivide_intrinsic_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
result: 0.1666667, 0.3, 0.3571429, 0.3888889
```
