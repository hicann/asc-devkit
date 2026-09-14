# \_\_bfloat162half\_rna\_sat

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T00:36:54.773Z pushedAt=2026-09-04T11:39:11.835Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In saturation mode, converts bfloat16 type data to half type data according to the CAST\_ROUND mode, and returns the converted value.

## Function Prototype

```
inline half __bfloat162half_rna_sat(const bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The half type data converted from the input in saturation mode according to the CAST\_ROUND mode.

## Constraints

In SIMT programming scenarios, the CTRL register cannot be set, so the saturation mode of this API does not take effect.

## Header Files to Include

To use this API, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__bfloat162half_rna_sat(__gm__ half* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __bfloat162half_rna_sat(x[idx]);
    }
    ```

