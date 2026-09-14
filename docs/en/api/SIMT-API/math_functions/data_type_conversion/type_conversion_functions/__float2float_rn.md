# \_\_float2float\_rn

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T08:55:15.043Z pushedAt=2026-08-29T02:54:47.337Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Rounds a floating-point number to the nearest integer, following the CAST\_RINT mode.

## Prototype

```
inline float __float2float_rn(const float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number obtained by rounding the input following the **CAST\_RINT** mode.

## Constraints

Float-to-float conversion supports only non-saturating behavior.

## Required Header File

To use this API, include the **simt\_api/device\_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel__float2float_rn(float* dst, float* x)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = __float2float_rn(x[idx]);
}
```

SIMD and SIMT hybrid programming scenario:

```
__simt_vf__ __launch_bounds__(1024) inline void kernel__float2float_rn(__gm__ float* dst, __gm__ float* x)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = __float2float_rn(x[idx]);
}
```

