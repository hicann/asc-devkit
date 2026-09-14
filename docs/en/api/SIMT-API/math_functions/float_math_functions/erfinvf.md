# erfinvf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:46:32.342Z pushedAt=2026-09-03T07:28:07.093Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the inverse error function value of the input data.

![](../../../figures/zh-cn_formulaimage_0000002484776418.png)

## Function Prototype

```
inline float erfinvf(float x)
```

## Parameters

**Table 1** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

-   When x is 0, the return value is 0.
-   When x is nan, the return value is nan.
-   When x is 1, the return value is inf.
-   When x is -1, the return value is -inf.
-   When x∉\[-1,1\], the return value is nan.

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
    __global__ __launch_bounds__(256) void compute_erfinvf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = erfinvf(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_erfinvf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = erfinvf(x[idx]);
    }

    __global__ __vector__ void run_erfinvf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_erfinvf_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: -0.75, -0.25, 0.25, 0.75
result: -0.8134198 -0.2253121 0.2253121 0.8134198
```
