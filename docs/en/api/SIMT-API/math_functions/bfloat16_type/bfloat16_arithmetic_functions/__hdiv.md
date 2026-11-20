# \_\_hdiv

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T13:03:52.387Z pushedAt=2026-10-26T06:23:22.720Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hdiv_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Computes the result of dividing two bfloat16 values and rounds the result following the CAST\_RINT mode.

## Prototype

```cpp
bfloat16_t __hdiv(const bfloat16_t x, const bfloat16_t y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

The result of dividing the input data. This API is affected by the global saturation mode. The special values are as follows:

| x Value | y Value | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
| --- | --- | --- | --- |
| ±0 | ±0 | nan | 0 |
| ±inf | ±inf | nan | 0 |
| Finite value | ±inf | The sign is determined by the XOR of the signs of x and y, and the value is 0. | The sign is determined by the XOR of the signs of x and y, and the value is 0. |
| inf | positive finite value | inf | ASCRT_MAX_NORMAL_BF16 |
| inf | negative finite value | -inf | -ASCRT_MAX_NORMAL_BF16 |
| -inf | positive finite value | -inf | -ASCRT_MAX_NORMAL_BF16 |
| -inf | negative finite value | inf | ASCRT_MAX_NORMAL_BF16 |
| >0 | 0 | inf | ASCRT_MAX_NORMAL_BF16 |
| <0 | 0 | -inf | -ASCRT_MAX_NORMAL_BF16 |
| >0 | -0 | -inf | -ASCRT_MAX_NORMAL_BF16 |
| <0 | -0 | inf | ASCRT_MAX_NORMAL_BF16 |
| ±0 | non-zero | The sign is determined by the XOR of the signs of x and y, and the value is 0. | The sign is determined by the XOR of the signs of x and y, and the value is 0. |
| either x or y is nan | — | nan | 0 |

## Constraints

<!-- npu="950" id7 -->
For the Ascend 950PR&950DT products, this API does not support the Subnormal scenario: input and output values within the Subnormal range are flushed to zero with the sign preserved.
<!-- end id7 -->

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel_hdiv(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hdiv(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_hdiv(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hdiv(x[idx], y[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, -0.5, 1.0, -1.0, 2.0, -2.0, 0.0, 4.0
y: 2.0, 0.5, -1.0, -1.0, 0.5, 2.0, 4.0, 0.5
result: 0.25, -1.0, -1.0, 1.0, 4.0, -1.0, 0.0, 8.0
```
