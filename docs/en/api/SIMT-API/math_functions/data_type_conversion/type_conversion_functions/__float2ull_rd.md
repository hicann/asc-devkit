# \_\_float2ull\_rd

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T09:08:32.059Z pushedAt=2026-08-29T02:54:47.379Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Follows the **CAST\_FLOOR** mode to convert a floating-point number to a 64-bit unsigned integer and returns the converted value.

## Prototype

```
inline unsigned long long int __float2ull_rd(const float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the 64-bit unsigned integer obtained by converting the input following the **CAST\_FLOOR** mode. Special scenarios are described as follows:

-   When x is **nan**, the return value is 0.
-   When x is **inf**, the return value is 18446744073709551615.
-   When x is **-inf**, the return value is 0.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/device\_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel__float2ull_rd(uint64_t* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2ull_rd(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2ull_rd(__gm__ uint64_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2ull_rd(x[idx]);
    }
    ```

