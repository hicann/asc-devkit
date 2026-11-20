# rsqrtf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:28:26.610Z pushedAt=2026-10-26T11:03:37.414Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/rsqrtf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the square root reciprocal of the input data **x**.

![](../../../figures/formulaimage_0000002484776402.png)

## Prototype

```cpp
inline float rsqrtf(float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, input data. |

## Return Value

Returns the square root reciprocal of the input data.

-   When x is 0, the return value is inf.
-   When x is inf, the return value is 0.
-   When x is -inf, the return value is nan.
-   When x is nan, the return value is nan.
-   In fast computation mode ([--cce-use-fast-math](../../../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#common-compilation-options) set to `true`), when x is Subnormal data, the API returns +inf.

## Constraints

-   The input data range supported by this API is x greater than or equal to 0; otherwise, the return value is nan.
-   This API is affected by the `--cce-use-fast-math` compilation option.
    -   `--cce-use-fast-math=true`: Enables the fast computation mode. The internal implementation of this API uses sqrtf. Because sqrtf does not support the Subnormal scenario in fast computation mode, this API does not support the scenario where the input x is Subnormal data.
    -   `--cce-use-fast-math=false`: Supports high-precision reciprocal square root computation in the Subnormal scenario.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_rsqrtf(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rsqrtf(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_rsqrtf_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = rsqrtf(x[idx]);
    }

    __global__ __vector__ void run_rsqrtf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_rsqrtf_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 1.25, 2.25, 3.25
result: 2, 0.8944272, 0.6666667, 0.5547002
```
