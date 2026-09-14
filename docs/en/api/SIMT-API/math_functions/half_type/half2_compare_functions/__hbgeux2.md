# \_\_hbgeux2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:16:15.185Z pushedAt=2026-09-04T08:03:26.090Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Compares the two components of two **half2** values and returns true when both components satisfy the condition that the first value is greater than or equal to the second value. If any component of an input is nan, the comparison result of that component is true.

## Prototype

```
bool __hbgeux2(half2 x, half2 y)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |
| **y** | Input | Source operand. |

## Return Value

Returns the result of comparing whether each component of the inputs satisfies that the first number is greater than or equal to the second number.

-   true: Each component satisfies that the first number is greater than or equal to the second number. If any component of any input is nan, the comparison result of that component is true.
-   false: Any component does not meet the requirement that the first number is greater than or equal to the second number.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hbgeux2(half* x, half* y, bool* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one data item of the half2 type, that is, two data items of the half type. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        dst[idx] = __hbgeux2(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hbgeux2(__gm__ half2* x, __gm__ half2* y, __gm__ bool* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one data item of the half2 type, that is, two data items of the half type. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hbgeux2(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ half* x, __gm__ half* y, __gm__ bool* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hbgeux2>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, dst, input_total_length);
    }
    ```

