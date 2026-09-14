# frexpf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-01T09:55:34.071Z pushedAt=2026-09-03T07:28:07.126Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts x into a normalized signed number in the range \[1/2, 1\) multiplied by an integral power of 2. Returns the normalized signed number, with the exponent stored in exp.

![](../../../figures/zh-cn_formulaimage_0000002516816359.png)

## Function Prototype

```
inline float frexpf(float x, int *exp)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| exp | Output | Address in Unified Buffer, Global Memory, or stack space for storing the base-2 exponent. |

## Return Value

Returns the signed number obtained when x is converted into a normalized signed number in the range \[1/2, 1\) multiplied by an integral power of 2.

-   When x is inf, the return value is inf and exp is 0.
-   When x is -inf, the return value is -inf and exp is 0.
-   When x is nan, the return value is nan and exp is 0.

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
    __global__ __launch_bounds__(256) void compute_frexpf(float *result, int *exp_out, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = frexpf(x[idx], exp_out + idx);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(256) inline void compute_frexpf_vf(__gm__ float *result, __gm__ int *exp_out, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = frexpf(x[idx], exp_out + idx);
    }

    __global__ __vector__ void run_frexpf(__gm__ float *result, __gm__ int *exp_out, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_frexpf_vf>(dim3(256), result, exp_out, x, count);
    }
    ```

The following shows an example of the input and output:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.5 0.75 0.625 0.875
exp_out: -1 0 1 1
```
