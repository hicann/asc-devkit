# \_\_hfma\_relu

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T09:58:52.049Z pushedAt=2026-09-01T09:20:52.900Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

For the input bfloat16 data **x**, **y**, and **z**, computes the result of **x** multiplied by **y** plus **z**, and rounds the result following the CAST\_RINT mode. Negative results are set to 0.

## Function Prototype

```
bfloat16_t __hfma_relu(const bfloat16_t x, const bfloat16_t y, const bfloat16_t z)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| z | Input | Source operand. |

## Return Value

The value of x \* y + z. This API is not affected by the global saturation mode. The special values are as follows:

-   If x is ±inf and y is ±0, nan is returned.
-   If x is ±0 and y is ±inf, nan is returned.
-   If x\*y is +inf and z is -inf, nan is returned.
-   If x\*y is -inf and z is +inf, nan is returned.
-   If x\*y+z exceeds the maximum value of the corresponding type range, +inf is returned.
-   If x\*y+z is less than the minimum value of the corresponding type range, 0 is returned.
-   If any of x, y, or z is nan, nan is returned.

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
    __global__ __launch_bounds__(1024) void KernelHfma_relu(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y, bfloat16_t* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma_relu(x[idx], y[idx], z[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHfma_relu(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bfloat16_t* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma_relu(x[idx], y[idx], z[idx]);
    }
    ```

