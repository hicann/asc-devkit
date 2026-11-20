# sincospif

<!-- md-trans-meta sourceCommit=6e11244b08ff98f28bdfed28700f33faca049b68 translatedAt=2026-10-27T03:20:55.991Z pushedAt=2026-10-27T09:21:55.656Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/sincospif_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the sine and cosine values of the input data multiplied by π.

![](../../../figures/formulaimage_0000002484776398.png)

## Prototype

```cpp
inline void sincospif(float x, float *s, float *c)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| s | Output | Address in UB, Global Memory, or stack space for storing the sine value of the input data multiplied by π. |
| c | Output | Address in UB, Global Memory, or stack space for storing the cosine value of the input data multiplied by π. |

## Return Value

- When the input x is a finite value and Abs\(x\)\>=2^24, the output parameter s is 0.0 and the output parameter c is 1.0.
- When the input x is an integer and Abs\(x\)\<2^24, the output parameter s is 0.0.
- When the input x is an integer and Abs\(x\)\<2^24, if x is even, the output parameter c returns 1.0; if x is odd, the output parameter c returns -1.0.
- When the input x = n + 0.5, the output parameter s returns 1.0 if n is even, and returns -1.0 if n is odd.
- When the input x = n + 0.5, the output parameter c returns 0.0.
- When the input x is inf, -inf, or nan, both output parameters s and c return nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_sincospif(float *sin_result, float *cos_result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        sincospif(x[idx], sin_result + idx, cos_result + idx);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

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

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
sin_result: 0.7071068, 0.7071068, -0.7071069, -0.7071065
cos_result: 0.7071068, -0.7071068, -0.7071066, 0.707107
```
