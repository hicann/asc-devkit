# ynf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:41:16.398Z pushedAt=2026-10-26T11:03:37.434Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/ynf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the value of the Bessel function of the second kind yn of order n for the input data x.

![](../../../figures/formulaimage_0000002516816391.png)

## Prototype

```cpp
inline float ynf(int n, float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| n | Input | Source operand. |
| x | Input | Source operand. |

## Return Value

The value of the Bessel function of the second kind yn for the input data.

-   When n<0, returns nan.
-   When x=0, returns -inf.
-   When x<0, returns nan.
-   When x=inf, returns 0.
-   When x=nan, returns nan.
-   In fast math mode (with [--cce-use-fast-math](../../../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#common-compilation-options) set to `true`), if the divisor in the internal computation is a Subnormal value, the API returns ±inf.

## Constraints

-  The maximum value of n is 128.
-  This API is affected by the `--cce-use-fast-math` compilation option.
    -   --cce-use-fast-math=true: Enables fast math mode. The internal implementation of this API uses the division operator. Because the division operator does not support Subnormal scenarios in fast math mode, this API does not support scenarios where the divisor in the internal computation is a Subnormal value.
    -   --cce-use-fast-math=false: Supports high-precision computation for Subnormal scenarios.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_ynf(float *result, const int *n, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = ynf(n[idx], x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_ynf_vf(__gm__ float *result, __gm__ const int *n, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = ynf(n[idx], x[idx]);
    }

    __global__ __vector__ void run_ynf(__gm__ float *result, __gm__ const int *n, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_ynf_vf>(dim3(256), result, n, x, count);
    }
    ```

The input and output examples are as follows:

```
n: 1, 2, 3, 1
x: 1, 2, 3, 4
result: -0.7812128, -0.6174081, -0.5385417, 0.3979257
```
