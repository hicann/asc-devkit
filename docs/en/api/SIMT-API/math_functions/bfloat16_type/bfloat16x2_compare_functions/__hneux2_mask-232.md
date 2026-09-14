# \_\_hneux2\_mask

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T10:34:00.057Z pushedAt=2026-09-01T09:20:52.987Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Compares the two components of two **bfloat16x2_t** values and returns the result as an **unsigned int**, where the lower 16 bits hold the mask result of the first component and the upper 16 bits hold the mask result of the second component. If the components are not equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0. If any input component is nan, the corresponding 16-bit mask is 0xFFFF.

## Function Prototype

```
unsigned int __hneux2_mask(bfloat16x2_t x, bfloat16x2_t y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

-   Result of comparing whether the components of the input data are not equal: when the condition is met, the corresponding 16-bit mask result is 0xFFFF; otherwise, the corresponding 16-bit mask result is 0x0.
-   When any input component is nan, the 16-bit mask result of that component is 0xFFFF.

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
    __global__ __launch_bounds__(1024) void simt_hneux2_mask(bfloat16_t* x, bfloat16_t* y, unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input1 = (bfloat16x2_t*)x;
        bfloat16x2_t* input2 = (bfloat16x2_t*)y;
        dst[idx] = __hneux2_mask(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hneux2_mask(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hneux2_mask(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hneux2_mask>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)y, dst, input_total_length);
    }
    ```

