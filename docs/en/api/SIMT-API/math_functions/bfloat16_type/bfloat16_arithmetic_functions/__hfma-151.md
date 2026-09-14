# \_\_hfma

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T09:59:29.140Z pushedAt=2026-09-01T09:20:52.901Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

For input data x, y, and z, computes the result of x multiplied by y plus z, and rounds the result following the CAST\_RINT mode.

![](../../../../figures/zh-cn_formulaimage_0000002545900872.png)

## Function Prototype

```
inline bfloat16_t __hfma(bfloat16_t x, bfloat16_t y, bfloat16_t z)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| z | Input | Source operand. |

## Return Value

The value of x \* y + z. This API is not affected by the global saturation mode. The special values are as follows:

| x Value | y Value | z Value | Return Value |
| --- | --- | --- | --- |
| ±inf | ±0 | — | nan |
| ±0 | ±inf | — | nan |
| x*y = inf | -inf | nan |  |
| x*y = -inf | inf | nan |  |
| x*y+z exceeds ASCRT_MAX_NORMAL_BF16 | inf |  |  |
| x*y+z is less than -ASCRT_MAX_NORMAL_BF16 | -inf |  |  |
| Any of x, y, or z is nan | nan |  |  |

## Constraints

None

## Header Files to Include

To use the bfloat16\_t interface, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelFma(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y, bfloat16_t* z){
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma(x[idx], y[idx], z[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelFma(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bfloat16_t* z){
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma(x[idx], y[idx], z[idx]);
    }
    ```

