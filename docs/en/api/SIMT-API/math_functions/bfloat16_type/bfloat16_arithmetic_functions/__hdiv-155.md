# \_\_hdiv

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T09:58:02.174Z pushedAt=2026-09-01T09:20:52.898Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the quotient of two bfloat16 values and rounds the result following the CAST\_RINT mode.

## Function Prototype

```
bfloat16_t __hdiv(const bfloat16_t x, const bfloat16_t y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the quotient of the input data. This API is affected by the global saturation mode. The special values are as follows:

| x Value | y Value | Non-saturation Mode Return Value | Saturation Mode Return Value |
| --- | --- | --- | --- |
| ±0 | ±0 | nan | 0 |
| ±inf | ±inf | nan | 0 |
| Finite value | ±inf | The sign is determined by the XOR of the signs of x and y, and the value is 0. | The sign is determined by the XOR of the signs of x and y, and the value is 0. |
| inf | Positive finite value | inf | ASCRT_MAX_NORMAL_BF16 |
| inf | Negative finite value | -inf | -ASCRT_MAX_NORMAL_BF16 |
| -inf | Positive finite value | -inf | -ASCRT_MAX_NORMAL_BF16 |
| -inf | Negative finite value | inf | ASCRT_MAX_NORMAL_BF16 |
| >0 | 0 | inf | ASCRT_MAX_NORMAL_BF16 |
| <0 | 0 | -inf | -ASCRT_MAX_NORMAL_BF16 |
| >0 | -0 | -inf | -ASCRT_MAX_NORMAL_BF16 |
| <0 | -0 | inf | ASCRT_MAX_NORMAL_BF16 |
| ±0 | Nonzero | The sign is determined by the XOR of the signs of x and y, and the value is 0. | The sign is determined by the XOR of the signs of x and y, and the value is 0. |
| Either x or y is nan | nan | 0 |  |

## Constraints

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: input and output values within the Subnormal range are flushed to zero with the sign preserved.
<!-- end id7 -->

## Header Files to Include

To use this API, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelHdiv(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hdiv(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHdiv(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hdiv(x[idx], y[idx]);
    }
    ```

