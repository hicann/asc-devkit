# sincosf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:29:17.568Z pushedAt=2026-10-26T11:03:37.413Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/sincosf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the sine and cosine values of the input data.

![](../../../figures/formulaimage_0000002516816357.png)

## Prototype

```cpp
inline void sincosf(float x, float *s, float *c)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| s | Output | Address in UB, Global Memory, or stack space for storing the sine value of the input data. |
| c | Output | Address in UB, Global Memory, or stack space for storing the cosine value of the input data. |

## Return Value

-   Returns nan when the input x is inf, -inf, or nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_sincosf(float *sin_result, float *cos_result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        sincosf(x[idx], sin_result + idx, cos_result + idx);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_sincosf_vf(__gm__ float *sin_result, __gm__ float *cos_result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        sincosf(x[idx], sin_result + idx, cos_result + idx);
    }

    __global__ __vector__ void run_sincosf(__gm__ float *sin_result, __gm__ float *cos_result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_sincosf_vf>(dim3(256), sin_result, cos_result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
sin_result: 0.247404, 0.6816388, 0.9489846, 0.983986
cos_result: 0.9689124, 0.7316889, 0.3153224, -0.1782461
```
