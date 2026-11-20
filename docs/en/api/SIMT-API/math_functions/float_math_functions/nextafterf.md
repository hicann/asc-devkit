# nextafterf

<!-- md-trans-meta sourceCommit=e80e2531aba21d60ee54c52d2ae137dd3bc302e2 translatedAt=2026-10-26T08:13:35.139Z pushedAt=2026-10-26T11:03:37.375Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/nextafterf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

For two data values x and y:

If y is greater than x, the next representable floating-point value greater than x is returned.

If y is less than x, the next representable floating-point value less than x is returned.

If y is equal to x, x is returned.

## Prototype

```cpp
inline float nextafterf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

- When x is not equal to y, the next representable floating-point value after x in the direction of y is returned.
- When x is equal to y, x is returned.
- If either x or y is nan, nan is returned.
- If x = +inf and y is not equal to +inf and is not nan, 3.4028235e+38 is returned.
- If x = -inf and y is not equal to -inf and is not nan, -3.4028235e+38 is returned.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_nextafterf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = nextafterf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_nextafterf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = nextafterf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_nextafterf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_nextafterf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.5, 2.5, 3.5, 4.5
result: 0.25, 0.7500001, 1.25, 1.75
```
