# sincospif

<!-- md-trans-meta sourceCommit=1e3d48cde951e4cd58f9cded9fce17c1f24fa02d translatedAt=2026-09-01T10:29:41.953Z pushedAt=2026-09-03T07:28:07.202Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the sine and cosine values of the input data multiplied by π.

![](../../../figures/zh-cn_formulaimage_0000002484776398.png)

## Function Prototype

```
inline void sincospif(float x, float *s, float *c)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| s | Output | Address in Unified Buffer, Global Memory, or stack space for storing the sine value of the input data multiplied by π. |
| c | Output | Address in Unified Buffer, Global Memory, or stack space for storing the cosine value of the input data multiplied by π. |

## Return Value

-   When x\*π exceeds the maximum range of float or falls below the minimum range of float, the output value is nan.
-   When the input x is inf, -inf, or nan, the output value is nan.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_sincospif(float *sin_result, float *cos_result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        sincospif(x[idx], sin_result + idx, cos_result + idx);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_sincospif_vf(__gm__ float *sin_result, __gm__ float *cos_result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        sincospif(x[idx], sin_result + idx, cos_result + idx);
    }

    __global__ __vector__ void run_sincospif(__gm__ float *sin_result, __gm__ float *cos_result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_sincospif_vf>(dim3(256), sin_result, cos_result, x, count);
    }
    ```

The following shows an example of the input and output:

```
x: 0.25, 0.75, 1.25, 1.75
sin_result: 0.7071068 0.7071068 -0.7071069 -0.7071065
cos_result: 0.7071068 -0.7071068 -0.7071066 0.707107
```
