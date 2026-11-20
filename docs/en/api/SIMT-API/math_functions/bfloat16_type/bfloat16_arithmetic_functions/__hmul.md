# \_\_hmul

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T13:06:09.803Z pushedAt=2026-10-26T06:23:22.725Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hmul_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Computes the product of two bfloat16 values and rounds the result according to the CAST\_RINT mode.

## Prototype

```cpp
bfloat16_t __hmul(const bfloat16_t x, const bfloat16_t y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the product of the input values. This API is not affected by the global saturation mode. The special values are as follows:

-   When neither the input nor the result is nan, the sign of `x*y` is the XOR of the signs of `x` and `y`.
-   `__hmul(x, y)` is equivalent to `__hmul(y, x)`.
-   When `x` is a non-zero value and `y` is ±inf, the sign of the return value is determined by the XOR of the signs of `x` and `y`, and the value is inf.
-   When `x` is ±0 and `y` is ±inf, the return value is nan.
-   When `x` is ±0 and `y` is a finite value, the sign of the return value is determined by the XOR of the signs of `x` and `y`, and the value is 0.
-   When either `x` or `y` is nan, the return value is nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel_hmul(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hmul(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_hmul(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hmul(x[idx], y[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y, uint32_t total_length)
    {
        asc_vf_call<kernel_hmul>(dim3(1024), dst, x, y, total_length);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, -0.5, 1.0, -1.0, 2.0, -2.0, 0.0, 4.0
y: 2.0, 0.5, -1.0, -1.0, 0.5, 2.0, 4.0, 0.5
result: 1.0, -0.25, -1.0, 1.0, 1.0, -4.0, 0.0, 2.0
```
