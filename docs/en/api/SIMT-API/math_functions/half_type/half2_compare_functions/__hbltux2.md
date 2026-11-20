# \_\_hbltux2

<!-- md-trans-meta sourceCommit=ccc1820c8c764c94b6e9a6131f4c550662f52b46 translatedAt=2026-10-26T13:53:40.427Z pushedAt=2026-10-27T02:14:54.174Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_compare_functions/__hbltux2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Compares the two components of two half2 values and returns true when both components satisfy the condition that the first number is less than the second number. If any component of either input is nan, the comparison result of that component is true.

## Prototype

```cpp
bool __hbltux2(half2 x, half2 y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Compares whether each component of the inputs satisfies the condition that the first number is less than the second number. The special values are as follows:

<table>
  <tr>
    <th>x value</th>
    <th>y value</th>
    <th>Return value</th>
  </tr>
  <tr>
    <td colspan="2">Both components satisfy x&lt;y</td>
    <td>true</td>
  </tr>
  <tr>
    <td colspan="2">Any component satisfies x≥y</td>
    <td>false</td>
  </tr>
  <tr>
    <td colspan="2">Any component is nan</td>
    <td>The comparison result of this component is true, and the overall result depends on the remaining components.</td>
  </tr>
  <tr>
    <td>nan</td>
    <td>nan</td>
    <td>true</td>
  </tr>
  <tr>
    <td>±0</td>
    <td>±0</td>
    <td>false</td>
  </tr>
  <tr>
    <td>-inf</td>
    <td>1</td>
    <td>true</td>
  </tr>
  <tr>
    <td>inf</td>
    <td>inf</td>
    <td>false</td>
  </tr>
  <tr>
    <td>-inf</td>
    <td>-inf</td>
    <td>false</td>
  </tr>
</table>

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hbltux2(half* x, half* y, bool* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        dst[idx] = __hbltux2(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hbltux2(__gm__ half2* x, __gm__ half2* y, __gm__ bool* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hbltux2(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ half* x, __gm__ half* y, __gm__ bool* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hbltux2>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1, 0), (1, -1), (inf, -inf), (nan, 0.5)
y: (1, 0), (1, -2), (inf, inf), (0.5, nan)
result: false, false, false, true
```
