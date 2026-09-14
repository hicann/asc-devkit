# h2rsqrt

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T10:47:56.368Z pushedAt=2026-09-01T09:20:53.011Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the reciprocal of the square root of each element in the input data **x**.

![](../../../../figures/zh-cn_formulaimage_0000002576421069.png)

## Function Prototype

```
inline bfloat16x2_t h2rsqrt(bfloat16x2_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, that is, the input data. |

## Return Value

Returns the reciprocal of the square root of each element in the input data.

-   When the input element is 0, the return value is inf.
-   When the input element is inf, the return value is 0.
-   When the input element is -inf, the return value is nan.
-   When the input element is nan, the return value is nan.

## Constraints

The input data range supported by this API is x greater than or equal to 0; otherwise, the return value is nan.

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: the internal implementation of this API uses sqrtf, and because sqrtf does not support the Subnormal scenario, when the input x is a Subnormal value, the final result of this API is +inf.
<!-- end id7 -->

## Header Files to Include

To use the bfloat16x2\_t interface, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelRsqrt(bfloat16x2_t* dst, bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rsqrt(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelRsqrt(__gm__ bfloat16x2_t* dst, __gm__ bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rsqrt(x[idx]);
    }
    ```

