# \_\_float2uint\_ru

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T09:07:06.772Z pushedAt=2026-08-29T02:54:47.377Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts a floating-point number to an unsigned integer following the **CAST\_CEIL** mode and returns the converted value.

## Function Prototype

```
inline unsigned int __float2uint_ru(const float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the unsigned integer converted from the input following the CAST\_CEIL mode. Special scenarios are described as follows:

-   When x is nan, the return value is 0.
-   When x is inf, the return value is 4294967295.
-   When x is -inf, the return value is 0.

## Constraints

None

## Header File to Include

To use this API, include the **simt\_api/device\_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel__float2uint_ru(uint32_t* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2uint_ru(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2uint_ru(__gm__ uint32_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2uint_ru(x[idx]);
    }
    ```

