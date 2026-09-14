# h2trunc

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T10:50:55.737Z pushedAt=2026-09-01T09:20:53.017Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the integer after truncating the floating-point value of each input element.

## Function Prototype

```
inline bfloat16x2_t h2trunc(bfloat16x2_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The integer obtained by truncating the floating-point value of each input element. Special scenarios are described as follows:

-   When the input element is nan, the return value is nan.
-   When the input element is inf, the return value is inf.
-   When the input element is -inf, the return value is -inf.

## Constraints

None

## Header Files to Include

To use the bfloat16x2\_t interface, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelTrunc(bfloat16x2_t* dst, bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2trunc(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelTrunc(__gm__ bfloat16x2_t* dst, __gm__ bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2trunc(x[idx]);
    }
    ```

