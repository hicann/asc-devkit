# \_\_float\_as\_uint

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T08:55:17.140Z pushedAt=2026-08-29T02:54:47.353Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Reinterprets the bits of a floating-point number as an unsigned integer, that is, reads the bits stored in the floating-point number in the format of an unsigned integer.

## Prototype

```
inline unsigned int __float_as_uint(const float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Returns the integer value closest to the input floating-point number. Special cases are described as follows:

-   When **x** is nan, the return value is 2143289344.
-   When **x** is inf, the return value is 2139095040.
-   When **x** is -inf, the return value is 4286578688.

## Constraints

None

## Required Header File

To use this API, include the **simt\_api/device\_functions.h"** header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel__float_as_uint(uint32_t* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float_as_uint(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__float_as_uint(__gm__ uint32_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float_as_uint(x[idx]);
    }
    ```

