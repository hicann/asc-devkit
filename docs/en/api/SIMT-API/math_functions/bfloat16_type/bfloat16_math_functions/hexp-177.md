# hexp

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T09:47:25.530Z pushedAt=2026-09-01T09:20:52.843Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Given the input **x**, computes e raised to the power of **x**.

![](../../../../figures/zh-cn_formulaimage_0000002545900860.png)

## Function Prototype

```
inline bfloat16_t hexp(bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns e raised to the power of **x**. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Non-saturation Mode Return Value | Saturation Mode Return Value |
| --- | --- | --- |
| inf | inf | ASCRT_MAX_NORMAL_BF16 |
| -inf | 0 | 0 |
| nan | nan | 0 |
| Others | 2 raised to the power of x. When the result exceeds the maximum finite value of bfloat16_t, the result is inf. | 2 raised to the power of x. When the result exceeds the maximum finite value of bfloat16_t, the result is ASCRT_MAX_NORMAL_BF16. |

## Constraints

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: input and output values within the Subnormal range are flushed to zero with the sign preserved.
<!-- end id7 -->

## Header Files to Include

To use the bfloat16\_t interface, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelExp(bfloat16_t* dst, bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hexp(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelExp(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hexp(x[idx]);
    }
    ```

