# \_\_hcmadd

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T10:52:57.591Z pushedAt=2026-09-01T09:20:53.021Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Treats the three **bfloat16x2_t** inputs as complex numbers, with the first component as the real part and the second component as the imaginary part, and performs the complex multiply-add operation x\*y+z.

## Function Prototype

```
bfloat16x2_t __hcmadd(const bfloat16x2_t x, const bfloat16x2_t y, const bfloat16x2_t z)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| z | Input | Source operand. |

## Return Value

The input data is treated as complex numbers, and the result of the complex multiply-add operation is returned. For inputs a, b, and c:

-   The real part of the result is: \_\_hfma\(-a.y, b.y, \_\_hfma\(a.x, b.x, c.x\)\).
-   The imaginary part of the result is: \_\_hfma\( a.y, b.x, \_\_hfma\(a.x, b.y, c.y\)\).

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
    __global__ __launch_bounds__(1024) void simt_hcmadd(bfloat16_t* x, bfloat16_t* y, bfloat16_t* z, bfloat16_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input1 = (bfloat16x2_t*)x;
        bfloat16x2_t* input2 = (bfloat16x2_t*)y;
        bfloat16x2_t* input3 = (bfloat16x2_t*)z;
        bfloat16x2_t* out = (bfloat16x2_t*)dst;
        out[idx] = __hcmadd(input1[idx], input2[idx], input3[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hcmadd(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* y, __gm__ bfloat16x2_t* z, __gm__ bfloat16x2_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hcmadd(x[idx], y[idx], z[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bfloat16_t* z, __gm__ bfloat16_t* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hcmadd>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)y, (__gm__ bfloat16x2_t*)z, (__gm__ bfloat16x2_t*)dst, input_total_length);
    }
    ```

