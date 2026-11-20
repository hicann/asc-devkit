# expf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T07:50:17.592Z pushedAt=2026-10-26T11:03:37.319Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/expf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Computes e raised to the power of x based on the input x.

![](../../../figures/formulaimage_0000002516816333.png)

## Prototype

```cpp
inline float expf(float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, with the data type float. |

## Return Value

Returns e raised to the power of x.

-   When x is inf, the return value is inf.
-   When x is -inf, the return value is 0.
-   When x is nan, the return value is nan.

## Constraints

This API is affected by the compilation option [--cce-use-fast-math](../../../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#common-compilation-options).
-   --cce-use-fast-math=true: Enables fast computation mode, which does not support subnormal scenarios. Input and output values within the subnormal range are flushed to zero with the sign preserved.
-   --cce-use-fast-math=false: Supports subnormal scenarios.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_expf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = expf(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_expf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = expf(x[idx]);
    }

    __global__ __vector__ void run_expf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_expf_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 1.284025, 2.117, 3.490343, 5.754602
```
