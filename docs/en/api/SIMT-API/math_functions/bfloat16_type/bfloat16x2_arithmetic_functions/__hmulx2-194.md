# \_\_hmulx2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T10:56:16.119Z pushedAt=2026-09-01T09:20:53.026Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the product of the corresponding components of two bfloat16x2_t values, and rounds the result following the CAST_RINT mode.

## Function Prototype

```
bfloat16x2_t __hmulx2(const bfloat16x2_t x, const bfloat16x2_t y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

The product of the corresponding components of the input data. The multiplied components x and y satisfy the following:

-   When neither the inputs nor the result is nan, the sign of x\*y is the XOR of the signs of x and y.
-   When x is a non-zero value and y is ±inf, the sign of the return value is determined by the XOR of the signs of x and y, and the value is inf.
-   When x is ±0 and y is ±inf, the return value is nan.
-   When x is ±0 and y is a finite value, the sign of the return value is determined by the XOR of the signs of x and y, and the value is 0.
-   When either x or y is nan, the return value is nan.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hmulx2(bfloat16_t* x, bfloat16_t* y, bfloat16_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input1 = (bfloat16x2_t*)x;
        bfloat16x2_t* input2 = (bfloat16x2_t*)y;
        bfloat16x2_t* out = (bfloat16x2_t*)dst;
        out[idx] = __hmulx2(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hmulx2(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* y, __gm__ bfloat16x2_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hmulx2(x[idx], y[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bfloat16_t* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hmulx2>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)y, (__gm__ bfloat16x2_t*)dst, input_total_length);
    }
    ```

