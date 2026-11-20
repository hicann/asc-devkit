# \_\_hbleux2

<!-- md-trans-meta sourceCommit=63d31ee22164094353a3097c524e8859d00acee7 translatedAt=2026-10-23T15:29:39.631Z pushedAt=2026-10-26T06:23:23.000Z nl=2 -->

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
- Atlas inference product AI Core: Not supported
- Atlas inference product Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbleux2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Compares the two components of two bfloat16x2\_t values and returns true when both components satisfy that the first value is less than or equal to the second value. If any input component is nan, the comparison result for that component is true.

## Prototype

```cpp
bool __hbleux2(bfloat16x2_t x, bfloat16x2_t y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the result of comparing whether all components of the inputs satisfy the condition that the first number is less than or equal to the second number.

-   **true**: All components satisfy that the first number is less than or equal to the second number.
-   **false**: Any component does not satisfy that the first number is less than or equal to the second number.

The special values are as follows:

<table>
  <thead>
    <tr>
      <th>x Component Value</th>
      <th>y Component Value</th>
      <th>Return value</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td colspan="2">Any component contains nan</td>
      <td>true</td>
    </tr>
    <tr>
      <td>±0</td>
      <td>±0</td>
      <td>true</td>
    </tr>
    <tr>
      <td>±inf</td>
      <td>±inf</td>
      <td>Conforms to the floating-point ordering relation.</td>
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
    __global__ __launch_bounds__(1024) void simt_hbleux2(bfloat16_t* x, bfloat16_t* y, bool* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input1 = (bfloat16x2_t*)x;
        bfloat16x2_t* input2 = (bfloat16x2_t*)y;
        dst[idx] = __hbleux2(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hbleux2(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* y, __gm__ bool* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hbleux2(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* y, __gm__ bool* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hbleux2>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)y, dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.0, 0.0), (1.0, -1.0), (inf, -inf), (nan, 0.5)
y: (1.0, 0.0), (1.0, -2.0), (inf, inf), (0.5, nan)
result: true, false, true, true
```
