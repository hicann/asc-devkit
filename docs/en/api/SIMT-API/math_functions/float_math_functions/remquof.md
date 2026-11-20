# remquof

<!-- md-trans-meta sourceCommit=e80e2531aba21d60ee54c52d2ae137dd3bc302e2 translatedAt=2026-10-26T08:20:38.149Z pushedAt=2026-10-26T11:03:37.397Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/remquof_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the remainder r of the input data x divided by y. r = x - ny, where n is the quotient of x divided by y. n is the integer closest to the floating-point result of x divided by y. When the floating-point result of x divided by y is equidistant from the two nearest integers, the quotient is the even integer. The quotient is also assigned to the pointer variable quo.

## Prototype

```cpp
inline float remquof(float x, float y, int *quo)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| quo | Output | Address in UB, Global Memory, or stack space, used to store the quotient of the division operation. |

## Return Value

Returns the remainder of the input data x divided by y.

-   When either **x** or **y** is **nan**, returns **nan**.
-   When y = 0, the return value is nan.
-   When x = inf or -inf, the return value is nan.
-   When x is a finite value and y = inf or -inf, returns x.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_remquof(float *result, int *quo, const float *x, const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = remquof(x[idx], y[idx], quo + idx);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_remquof_vf(__gm__ float *result, __gm__ int *quo, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = remquof(x[idx], y[idx], quo + idx);
    }

    __global__ __vector__ void run_remquof(__gm__ float *result, __gm__ int *quo, __gm__ const float *x, __gm__ const float *y, uint32_t count)
    {
        asc_vf_call<compute_remquof_vf>(dim3(256), result, quo, x, y, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
y: 1.25, 2.25, 3.25, 4.25
result: 0.25, 0.75, 1.25, 1.75
quo: 0, 0, 0, 0
```
