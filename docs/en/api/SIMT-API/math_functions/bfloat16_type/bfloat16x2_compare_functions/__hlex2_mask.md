# \_\_hlex2\_mask

<!-- md-trans-meta sourceCommit=63d31ee22164094353a3097c524e8859d00acee7 translatedAt=2026-10-23T15:48:26.814Z pushedAt=2026-10-26T06:23:23.036Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2_mask_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Compares the two components of two bfloat16x2_t data values and returns the result as an **unsigned int**, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the first number is less than or equal to the second number, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0.

## Prototype

```cpp
unsigned int __hlex2_mask(bfloat16x2_t x, bfloat16x2_t y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the comparison result of whether each component of the input data satisfies the condition that the first number is less than or equal to the second number. If satisfied, the corresponding 16-bit mask result is 0xFFFF; otherwise, the corresponding 16-bit mask result is 0x0. Special values are as follows:

<table>
  <thead>
    <tr>
      <th>x Component Value</th>
      <th>y Component Value</th>
      <th>Return value of the corresponding component</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td colspan="2">x ≤ y</td>
      <td>0xFFFF</td>
    </tr>
    <tr>
      <td colspan="2">x &gt; y</td>
      <td>0x0000</td>
    </tr>
    <tr>
      <td colspan="2">x == y (including ±0)</td>
      <td>0xFFFF</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>inf</td>
      <td>0xFFFF</td>
    </tr>
    <tr>
      <td>inf</td>
      <td>finite value</td>
      <td>0x0000</td>
    </tr>
    <tr>
      <td>finite value</td>
      <td>inf</td>
      <td>0xFFFF</td>
    </tr>
    <tr>
      <td>-inf</td>
      <td>finite value</td>
      <td>0xFFFF</td>
    </tr>
    <tr>
      <td>finite value</td>
      <td>-inf</td>
      <td>0x0000</td>
    </tr>
    <tr>
      <td colspan="2">Any component contains nan</td>
      <td>0x0000</td>
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
    __global__ __launch_bounds__(1024) void simt_hlex2_mask(bfloat16_t* x, bfloat16_t* y, unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input1 = (bfloat16x2_t*)x;
        bfloat16x2_t* input2 = (bfloat16x2_t*)y;
        dst[idx] = __hlex2_mask(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hlex2_mask(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hlex2_mask(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hlex2_mask>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)y, dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.0, 0.0), (1.0, -1.0), (inf, -inf), (nan, 0.5)
y: (1.0, 0.0), (1.0, -2.0), (inf, inf), (0.5, nan)
result: 4294967295, 65535, 4294967295, 0
```
