# h2ceil

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T10:40:07.717Z pushedAt=2026-09-01T09:20:52.996Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the smallest integer value greater than or equal to each element of the input data.

## Function Prototype

```
inline bfloat16x2_t h2ceil(bfloat16x2_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Returns the smallest integer value greater than or equal to each element of the input data. Special scenarios are described as follows:

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
    __global__ __launch_bounds__(1024) void KernelCeil(bfloat16x2_t* dst, bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2ceil(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelCeil(__gm__ bfloat16x2_t* dst, __gm__ bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2ceil(x[idx]);
    }
    ```

