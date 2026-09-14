# \_\_float2float\_rz

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T08:57:12.195Z pushedAt=2026-08-29T02:54:47.356Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Rounds a floating-point number to the nearest integer, following the **CAST\_TRUNC** mode.

## Prototype

```
inline float __float2float_rz(const float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Returns the floating-point number obtained by rounding the input in **CAST\_TRUNC** mode.

## Constraints

None

## Required Header Files

To use this interface, include the `simt\_api/device\_functions.h` header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   SIMT programming scenario:

```
__global__ __launch_bounds__(1024) void kernel__float2float_rz(float* dst, float* x)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = __float2float_rz(x[idx]);
}
```

-   SIMD and SIMT hybrid programming scenario:

```
__simt_vf__ __launch_bounds__(1024) inline void kernel__float2float_rz(__gm__ float* dst, __gm__ float* x)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    dst[idx] = __float2float_rz(x[idx]);
}
```
