# hrcp

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T09:51:46.916Z pushedAt=2026-09-01T09:20:52.885Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the reciprocal of the input data x.

![](../../../../figures/zh-cn_formulaimage_0000002545900870.png)

## Function Prototype

```
inline bfloat16_t hrcp(bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, that is, the input data. |

## Return Value

Returns the reciprocal of the input data. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Non-saturation Mode Return Value | Saturation Mode Return Value |
| --- | --- | --- |
| 0 | inf | ASCRT_MAX_NORMAL_BF16 |
| inf | 0 | 0 |
| -inf | -0 | -0 |
| nan | nan | 0 |

## Constraints

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: the internal implementation of this API uses the division operator. Since the division operator does not support the Subnormal scenario, when the input x is Subnormal data, the final result of this API is ±inf.
<!-- end id7 -->

## Header Files to Include

To use the bfloat16\_t interface, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelRcp(bfloat16_t* dst, bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hrcp(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelRcp(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hrcp(x[idx]);
    }
    ```

