# \_\_hbgeux2

<!-- md-trans-meta sourceCommit=ccc1820c8c764c94b6e9a6131f4c550662f52b46 translatedAt=2026-10-26T13:48:38.625Z pushedAt=2026-10-27T02:14:54.162Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_compare_functions/__hbgeux2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Compares the two components of two half2 values and returns true when both components satisfy that the first value is greater than or equal to the second value. If any component of an input is nan, the comparison result of that component is true.

## Prototype

```cpp
bool __hbgeux2(half2 x, half2 y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the result of comparing whether each component of the inputs satisfies that the first number is greater than or equal to the second number. Special values are as follows:

<table>
  <tr>
    <th>x component</th>
    <th>y component</th>
    <th>Return value (this component)</th>
  </tr>
  <tr>
    <td colspan="2">x or y is nan</td>
    <td>true</td>
  </tr>
  <tr>
    <td colspan="2">Both x and y are nan</td>
    <td>true</td>
  </tr>
  <tr>
    <td>inf</td>
    <td>finite value</td>
    <td>true</td>
  </tr>
  <tr>
    <td>finite value</td>
    <td>inf</td>
    <td>false</td>
  </tr>
  <tr>
    <td>-inf</td>
    <td>positive finite value</td>
    <td>false</td>
  </tr>
  <tr>
    <td>positive finite value</td>
    <td>-inf</td>
    <td>true</td>
  </tr>
  <tr>
    <td>inf</td>
    <td>-inf</td>
    <td>true</td>
  </tr>
  <tr>
    <td>-inf</td>
    <td>inf</td>
    <td>false</td>
  </tr>
  <tr>
    <td>0</td>
    <td>-0</td>
    <td>true</td>
  </tr>
  <tr>
    <td>-0</td>
    <td>0</td>
    <td>true</td>
  </tr>
  <tr>
    <td>ASCRT_MAX_NORMAL_FP16</td>
    <td>finite value</td>
    <td>true</td>
  </tr>
  <tr>
    <td>smaller finite value</td>
    <td>ASCRT_MAX_NORMAL_FP16</td>
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
    __global__ __launch_bounds__(1024) void simt_hbgeux2(half* x, half* y, bool* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        dst[idx] = __hbgeux2(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hbgeux2(__gm__ half2* x, __gm__ half2* y, __gm__ bool* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hbgeux2(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ half* x, __gm__ half* y, __gm__ bool* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hbgeux2>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1, 0), (1, -1), (inf, -inf), (nan, 0.5)
y: (1, 0), (1, -2), (inf, inf), (0.5, nan)
result: false, true, false, true
```
