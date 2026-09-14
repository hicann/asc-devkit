# \_\_hneg

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T02:14:24.653Z pushedAt=2026-09-04T11:39:12.111Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the negative value of the input half type data.

## Function Prototype

```
half __hneg(const half x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the negative value of the input data. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Non-saturation Mode Return Value | Saturation Mode return value |
| --- | --- | --- |
| ±0 | ∓0 | ∓0 |
| ±inf | ∓inf | ∓ASCRT_MAX_NORMAL_FP16 |
| nan | nan | 0 |

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
    __global__ __launch_bounds__(1024) void KernelHneg(half* dst, half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hneg(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHneg(__gm__ half* dst, __gm__ half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hneg(x[idx]);
    }
    ```

