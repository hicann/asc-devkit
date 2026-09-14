# \_\_hadd

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T02:11:22.499Z pushedAt=2026-09-04T11:39:12.104Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the sum of two half type values and rounds the result following the CAST\_RINT mode.

## Function Prototype

```
half __hadd(const half x, const half y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the sum of the input data.

-   \_\_hadd\(x, y\) is equivalent to \_\_hadd\(y, x\).
-   This API is affected by the global saturation mode. The special values are as follows:

    | x Value | y Value | Non-saturation Mode Return Value | Saturation Mode Return Value |
    | --- | --- | --- | --- |
    | Finite value | ±inf | ±inf | ±ASCRT_MAX_NORMAL_FP16 |
    | ±inf | ±inf | ±inf | ±ASCRT_MAX_NORMAL_FP16 |
    | ±inf | ∓inf | nan | 0 |
    | ±0 | ±0 | ±0 | ±0 |
    | Finite value (including ±0) | -x | +0 | +0 |
    | Either x or y is nan | Either x or y is nan | nan | 0 |

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
    __global__ __launch_bounds__(1024) void KernelHadd(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hadd(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHadd(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hadd(x[idx], y[idx]);
    }
    ```
