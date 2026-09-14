# \_\_hgtx2\_mask

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:29:46.215Z pushedAt=2026-09-04T08:03:26.156Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If a component satisfies that the first value is greater than the second value, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0.

## Prototype

```
unsigned int __hgtx2_mask(half2 x, half2 y)
```

## Description

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

-   Compares whether each component of the input data satisfies the condition that the first number is greater than the second number: if satisfied, the corresponding 16-bit mask result is 0xFFFF; otherwise, the corresponding 16-bit mask result is 0x0.
-   If any component of either input is nan, the 16-bit mask result for that component is 0x0.

## Constraints

None

## Required Header File

To use this API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hgtx2_mask(half* x, half* y, unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 data item, that is, two half data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        dst[idx] = __hgtx2_mask(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hgtx2_mask(__gm__ half2* x, __gm__ half2* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 data item, that is, two half data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hgtx2_mask(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ half* x, __gm__ half* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hgtx2_mask>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, dst, input_total_length);
    }
    ```

