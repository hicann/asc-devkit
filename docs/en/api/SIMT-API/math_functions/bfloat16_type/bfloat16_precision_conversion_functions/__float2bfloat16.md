# \_\_float2bfloat16

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T07:58:32.736Z pushedAt=2026-08-29T09:42:03.836Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts a floating-point number to bfloat16 precision following the **CAST_RINT** mode and returns the converted value.

## Function Prototype

```
inline bfloat16_t __float2bfloat16(const float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

The input is converted to bfloat16 type data following the **CAST_RINT** mode. Special scenarios are described as follows:

-   When **x** is nan, the return value is nan.
-   When **x** is inf, the return value is inf.
-   When **x** is -inf, the return value is -inf.

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
    __global__ __launch_bounds__(1024) void kernel__float2bfloat16(bfloat16_t* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2bfloat16(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2bfloat16(__gm__ bfloat16_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2bfloat16(x[idx]);
    }
    ```

