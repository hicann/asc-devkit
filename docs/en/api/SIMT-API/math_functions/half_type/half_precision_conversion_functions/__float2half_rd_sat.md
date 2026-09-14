# \_\_float2half\_rd\_sat

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-04T00:49:25.822Z pushedAt=2026-09-04T11:39:11.882Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In saturation mode, converts a floating-point number to a half-precision floating-point number following the CAST\_FLOOR mode and returns the converted value.

## Function Prototype

```
inline half __float2half_rd_sat(const float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the half-precision floating-point number converted from the input following the CAST\_FLOOR mode in saturation mode.

## Constraints

Before using this API, set the CTRL\[60\] register to 0; otherwise, the saturation mode does not take effect. For details about how to set it, see [Methods for Controlling Saturation Behavior](../../data_type_conversion/overview_258.md#methods-for-controlling-saturation-behavior).

In SIMT programming scenarios, the CTRL register cannot be set, so the saturation mode of this API does not take effect.

## Header Files to Include

To use this API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2half_rd_sat(__gm__ half* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2half_rd_sat(x[idx]);
    }
    ```

