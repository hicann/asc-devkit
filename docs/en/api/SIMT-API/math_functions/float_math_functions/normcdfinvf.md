# normcdfinvf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T10:14:49.419Z pushedAt=2026-09-03T07:28:07.166Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the inverse of the standard normal cumulative distribution function of the input data x.

![](../../../figures/zh-cn_formulaimage_0000002533422599.png)

## Function Prototype

```
inline float normcdfinvf(float x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Obtains the inverse of the standard normal cumulative distribution function of the input data x.

-   When x is 0, the return value is -inf.
-   When x is nan, the return value is nan.
-   When x is 1, the return value is inf.
-   When x∉\[0,1\], nan is returned.

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
    __global__ __launch_bounds__(256) void compute_normcdfinvf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = normcdfinvf(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_normcdfinvf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = normcdfinvf(x[idx]);
    }

    __global__ __vector__ void run_normcdfinvf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_normcdfinvf_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: -0.75, -0.25, 0.25, 0.75
result: nan nan -0.6744897 0.6744897
```
