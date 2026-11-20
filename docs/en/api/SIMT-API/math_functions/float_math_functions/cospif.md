# cospif

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T07:39:43.640Z pushedAt=2026-10-26T11:03:37.294Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/cospif_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the cosine of the input data multiplied by π.

![](../../../figures/formulaimage_0000002516816349.png)

## Prototype

```cpp
inline float cospif(float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns cosine of the input data multiplied by π.
- When the input x is a finite value and Abs\(x\)\>=2^24, 1.0 is returned directly.
- When the input x is an integer and |Abs\(x\)\<2^24, 1.0 is returned if x is even, and -1.0 is returned if x is odd.
- When the input x = n + 0.5, the return value is 0.0.
- When x is inf, the return value is nan.
- When x is -inf, the return value is nan.
- When x is nan, the return value is nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_cospif(float *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = cospif(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_cospif_vf(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = cospif(x[idx]);
    }

    __global__ __vector__ void run_cospif(__gm__ float *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_cospif_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.7071068, -0.7071068, -0.7071066, 0.707107
```
