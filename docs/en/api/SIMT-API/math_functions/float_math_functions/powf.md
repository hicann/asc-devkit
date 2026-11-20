# powf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:18:56.189Z pushedAt=2026-10-26T11:03:37.394Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/powf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the y-th power of the input data x.

![](../../../figures/formulaimage_0000002516816365.png)

## Prototype

```cpp
inline float powf(float x, float y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, the base of the power operation. |
| y | Input | Source operand, the exponent of the power operation. |

## Return Value

Returns the result of x raised to the power of y.

-   If x^y exceeds the maximum range of float, the return value is inf.
-   If x is ±0, y is less than 0, and y is an odd number, the return value is ±inf.
-   If x is ±0, y is less than 0, and y is not an odd number, the return value is inf.
-   If x is ±0, y is greater than 0, and y is an odd number, the return value is ±0.
-   If x is ±0 and y is greater than 0 and is not an odd integer, the return value is 0.
-   If x is -1 and y is ±inf, the return value is 1.
-   If x is 1 and y is any value (including nan), the return value is 1.
-   If y is ±0 and x is any value (including nan), the return value is 1.
-   If x is less than 0 and y is not an integer, the return value is nan.
-   If |x| < 1 and y is -inf, the return value is inf.
-   If |x|\>1 and y is -inf, the return value is 0.
-   If |x| < 1 and y is inf, the return value is 0.
-   If |x|\>1 and y is inf, the return value is inf.
-   If x is -inf and y is less than 0 and is an odd integer, the return value is -0.
-   If x is -inf, y is less than 0, and y is not an odd number, the return value is 0.
-   If x is -inf, y is greater than 0, and y is an odd number, the return value is -inf.
-   If x is -inf, y is greater than 0, and y is not an odd number, the return value is inf.
-   If x is inf and y is less than 0, the return value is 0.
-   If x is inf and y is greater than 0, the return value is inf.
-   In the following boundary scenarios, the return value is nan.
    -   x is nan and y is not 0.
    -   y is nan and x is not 1.
    -   Both x and y are nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_powf(float *result, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = powf(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_powf_vf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = powf(x[idx], y[idx]);
    }

    __global__ __vector__ void run_powf(__gm__ float *result, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_powf_vf>(dim3(256), result, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.25, 2.25, 3.25, 4.25
result: 0.176777, 0.523465, 2.065178, 10.787274
```
