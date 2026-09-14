# \_\_uint2half\_ru\_sat

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T01:44:38.693Z pushedAt=2026-09-04T11:39:12.045Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In saturation mode, converts **uint32** data to **half** data following the CAST\_CEIL mode, and returns the converted value.

## Function Prototype

```
inline half __uint2half_ru_sat(const unsigned int x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

In saturation mode, converts the input **uint32** data to **half** data following the CAST\_CEIL mode.

## Constraints

In SIMT programming scenarios, the CTRL register cannot be set, so the saturation mode of this API does not take effect.

## Header Files to Include

To use this API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__uint2half_ru_sat(__gm__ half* dst, __gm__ uint32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __uint2half_ru_sat(x[idx]);
    }
    ```

