# \_\_float2int\_rd

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T08:58:37.332Z pushedAt=2026-08-29T02:54:47.358Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Follows the CAST\_FLOOR mode to convert a floating-point number to a signed integer and returns the converted value.

## Prototype

```
inline int __float2int_rd(const float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the signed integer converted from the input following the `CAST\_FLOOR` mode. Special scenarios are described as follows:

-   When x is nan, the return value is 0.
-   When x is inf, the return value is 2147483647.
-   When x is -inf, the return value is -2147483648.

## Constraints

None

## Required Header File

To use this API, include the **simt\_api/device\_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel__float2int_rd(int32_t* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2int_rd(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2int_rd(__gm__ int32_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2int_rd(x[idx]);
    }
    ```

