# logf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:07:56.048Z pushedAt=2026-10-26T11:03:37.363Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/logf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the natural logarithm of the input data.

![](../../../figures/formulaimage_0000002516816339.png)

## Prototype

```cpp
inline float logf(float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the natural logarithm (base e) of x.

-   When x is less than 0 or x is nan, the return value is nan.
-   When x is 0, the return value is -inf.
-   When x is inf, the return value is inf.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_logf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = logf(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_logf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = logf(x[idx]);
    }

    __global__ __vector__ void run_logf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_logf_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 1, 2, 3, 4
result: 0, 0.6931472, 1.098612, 1.386294
```
