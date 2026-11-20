# \_\_hfmax2\_relu

<!-- md-trans-meta sourceCommit=63d31ee22164094353a3097c524e8859d00acee7 translatedAt=2026-10-23T15:19:13.739Z pushedAt=2026-10-26T06:23:22.981Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2_relu_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Computes the multiply-add result of each component of the input bfloat16x2\_t data (the first two inputs are multiplied and then added to the third input), and rounds the result following the CAST\_RINT mode. Negative results are set to 0.

## Prototype

```cpp
bfloat16x2_t __hfmax2_relu(const bfloat16x2_t x, const bfloat16x2_t y, const bfloat16x2_t z)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| z | Input | Source operand. |

## Return Value

Returns the fused multiply-add result of each component of the input data. The computed components a, b, and c satisfy the following conditions:

<table>
  <thead>
    <tr>
      <th>x Component Value</th>
      <th>y Component Value</th>
      <th>z component value</th>
      <th>Return Value</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td colspan="3">Any of x, y, or z is nan</td>
      <td>nan</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>0</td>
      <td>any value</td>
      <td>nan</td>
    </tr>
    <tr>
      <td>0</td>
      <td>inf</td>
      <td>any value</td>
      <td>nan</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>inf</td>
      <td>-inf</td>
      <td>nan</td>
    </tr>
    <tr>
      <td>-inf</td>
      <td>-inf</td>
      <td>inf</td>
      <td>inf</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>positive number</td>
      <td>non-infinite value</td>
      <td>inf</td>
    </tr>
    <tr>
      <td>-inf</td>
      <td>positive number</td>
      <td>non-infinite value</td>
      <td>0</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>inf</td>
      <td>0</td>
      <td>inf</td>
    </tr>
    <tr>
      <td>-inf</td>
      <td>-inf</td>
      <td>0</td>
      <td>inf</td>
    </tr>
    <tr>
      <td colspan="3">x, y, and z are normal values, and the result is less than 0</td>
      <td>0</td>
    </tr>
  </tbody>
</table>

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hfmax2_relu(bfloat16_t* x, bfloat16_t* y, bfloat16_t* z, bfloat16_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input1 = (bfloat16x2_t*)x;
        bfloat16x2_t* input2 = (bfloat16x2_t*)y;
        bfloat16x2_t* input3 = (bfloat16x2_t*)z;
        bfloat16x2_t* out = (bfloat16x2_t*)dst;
        out[idx] = __hfmax2_relu(input1[idx], input2[idx], input3[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hfmax2_relu(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* y, __gm__ bfloat16x2_t* z, __gm__ bfloat16x2_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hfmax2_relu(x[idx], y[idx], z[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bfloat16_t* z, __gm__ bfloat16_t* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hfmax2_relu>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)y, (__gm__ bfloat16x2_t*)z, (__gm__ bfloat16x2_t*)dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.25, 0.5), (2.5, -3.75), (-2, 1.25), (0.75, -0.25)
y: (0.5, -2), (1.25, 3), (1.5, -1), (-0.5, 2.25)
z: (-0.5, 1), (-2, 0.75), (1, -1.5), (0.25, 2)
result: (0, 0), (1.125, 0), (0, 0), (0, 1.4375)
```
