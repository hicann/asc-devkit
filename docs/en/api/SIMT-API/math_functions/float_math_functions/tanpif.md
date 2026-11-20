# tanpif

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:37:04.538Z pushedAt=2026-10-26T11:03:37.426Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/tanpif_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the tangent of the input data multiplied by π.

![](../../../figures/formulaimage_0000002516816327.png)

## Prototype

```cpp
inline float tanpif(float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Tangent of the input data multiplied by π.

- When input x is a finite value and Abs\(x\)\>=2^24, the return value is 0.0.
- When input x is an integer, the return value is 0.0.
- When input x = n + 0.5, where n is an even number, the return value is inf.
- When input x = n + 0.5, where n is an odd number, the return value is -inf.
- When x is inf, the return value is nan.
- When x is -inf, the return value is nan.
- When x is nan, the return value is nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_tanpif(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = tanpif(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_tanpif_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = tanpif(x[idx]);
    }

    __global__ __vector__ void run_tanpif(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_tanpif_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 1, -0.9999999, 1, -0.9999993
```
