# \_\_hdivx2

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-16T13:24:52.106Z pushedAt=2026-10-20T03:02:40.497Z -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hdivx2_res.md#id1 -->

## Description

The header file path is `"simt_api/asc_bf16.h"`.

Computes the division result of each component of two bfloat16x2_t values, and rounds the result following the CAST_RINT mode.

## Prototype

```cpp
bfloat16x2_t __hdivx2(const bfloat16x2_t x, const bfloat16x2_t y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the division result of each component of the input data. This API is affected by the global saturation mode. The divided components x and y satisfy:

<table>
  <thead>
    <tr>
      <th>x component value</th>
      <th>y component value</th>
      <th>Return value in non-saturation mode</th>
      <th>Return value in saturation mode</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>±0</td>
      <td>±0</td>
      <td>nan</td>
      <td>0</td>
    </tr>
    <tr>
      <td>finite value</td>
      <td>±0</td>
      <td>±inf (the sign is determined by the XOR of the signs of x and y)</td>
      <td>±ASCRT_MAX_NORMAL_BF16</td>
    </tr>
    <tr>
      <td>±inf</td>
      <td>±inf</td>
      <td>nan</td>
      <td>0</td>
    </tr>
    <tr>
      <td colspan="2">Either x or y is nan.</td>
      <td>nan</td>
      <td>0</td>
    </tr>
    <tr>
      <td>±inf</td>
      <td>±0</td>
      <td>±inf</td>
      <td>±ASCRT_MAX_NORMAL_BF16</td>
    </tr>
    <tr>
      <td>±inf</td>
      <td>finite value</td>
      <td>±inf (the sign is determined by the XOR of the signs of x and y)</td>
      <td>±ASCRT_MAX_NORMAL_BF16</td>
    </tr>
    <tr>
      <td>finite value</td>
      <td>±inf</td>
      <td>±0 (sign determined by XOR of the signs of x and y)</td>
      <td>±0</td>
    </tr>
    <tr>
      <td>ASCRT_MAX_NORMAL_BF16</td>
      <td>&lt;1.0</td>
      <td>inf</td>
      <td>ASCRT_MAX_NORMAL_BF16</td>
    </tr>
  </tbody>
</table>

## Constraints

<!-- npu="950" id7 -->
For the Ascend 950PR&950DT products, this API does not support the Subnormal scenario: input and output values within the Subnormal range are flushed to zero with the sign preserved.
<!-- end id7 -->

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hdivx2(bfloat16_t* x, bfloat16_t* y, bfloat16_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input1 = (bfloat16x2_t*)x;
        bfloat16x2_t* input2 = (bfloat16x2_t*)y;
        bfloat16x2_t* out = (bfloat16x2_t*)dst;
        out[idx] = __hdivx2(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hdivx2(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* y, __gm__ bfloat16x2_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hdivx2(x[idx], y[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bfloat16_t* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hdivx2>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)y, (__gm__ bfloat16x2_t*)dst, input_total_length);
    }
    ```
