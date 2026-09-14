# \_\_bfloat162uint\_rd

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T07:34:39.412Z pushedAt=2026-08-29T09:42:03.785Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts bfloat16 data to an unsigned integer following the **CAST_FLOOR** mode and returns the converted value.

## Function Prototype

```
inline unsigned int __bfloat162uint_rd(const bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Returns the unsigned integer converted from the input following the **CAST\_FLOOR** mode. Special scenarios are described as follows:

-   When x is nan, the return value is 0.
-   When x is inf, the return value is 4294967295.
-   When x is -inf, the return value is 0.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel__bfloat162uint_rd(uint32_t* dst, bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __bfloat162uint_rd(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__bfloat162uint_rd(__gm__ uint32_t* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __bfloat162uint_rd(x[idx]);
    }
    ```

