# sinpif

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:32:05.417Z pushedAt=2026-10-26T11:03:37.420Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/sinpif_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the sine of the input data multiplied by π.

![](../../../figures/formulaimage_0000002484776394.png)

## Prototype

```cpp
inline float sinpif(float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Sine of the input data multiplied by π.

- When input x is a finite value and Abs\(x\)\>=2^24, the return value is 0.0.
- When input x is an integer, the return value is 0.0.
- When input x = n + 0.5, where n is an even number, the return value is 1.0.
- When input x = n + 0.5, where n is an odd number, the return value is -1.0.
- When x is inf, the return value is nan.
- When input is -inf, the return value is nan.
- When x is nan, the return value is nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_sinpif(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = sinpif(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_sinpif_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = sinpif(x[idx]);
    }

    __global__ __vector__ void run_sinpif(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_sinpif_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.7071068, 0.7071068, -0.7071069, -0.7071065
```
