# erfcf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:43:45.129Z pushedAt=2026-09-03T07:28:07.087Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the complementary error function value of the input data.

![](../../../figures/zh-cn_formulaimage_0000002516816375.png)

## Function Prototype

```
inline float erfcf(float x)
```

## Parameters

**Table 1** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

-   When x is nan, the return value is nan.
-   When x is inf, the return value is 0.
-   When x is -inf, the return value is 2.

## Constraints

<!-- npu="950" id7 -->
For Ascend 950PR/Ascend 950DT, this API does not support the Subnormal scenario: the internal implementation of this API uses exp2f. Since exp2f does not support the Subnormal scenario, in rare cases the internal exponent calculation result is Subnormal data, causing the final result of this API to be 0.
<!-- end id7 -->

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

- SIMT programming scenario:

    ```
    __global__ __launch_bounds__(256) void compute_erfcf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = erfcf(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_erfcf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = erfcf(x[idx]);
    }

    __global__ __vector__ void run_erfcf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_erfcf_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.7236736 0.2888444 0.07709987 0.01332833
```
