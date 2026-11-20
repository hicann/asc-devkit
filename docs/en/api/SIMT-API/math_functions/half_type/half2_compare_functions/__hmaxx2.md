# \_\_hmaxx2

<!-- md-trans-meta sourceCommit=9c514a52c4547a5e12361f00d03a37609c0df739 translatedAt=2026-11-10T10:02:40.942Z pushedAt=2026-11-13T08:46:49.186Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_compare_functions/__hmaxx2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Obtains the maximum value of each corresponding component of the two half2 values.

## Prototype

```cpp
half2 __hmaxx2(const half2 x, const half2 y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the maximum value among all components of the input data. This API is affected by the global saturation register. The special values are as follows:

<table>
  <tr>
    <th>x component</th>
    <th>y component</th>
    <th>Return value in non-saturation mode</th>
    <th>Return value in saturation mode</th>
  </tr>
  <tr>
    <td>0</td>
    <td>-0</td>
    <td>0</td>
    <td>0</td>
  </tr>
  <tr>
    <td>-0</td>
    <td>0</td>
    <td>0</td>
    <td>0</td>
  </tr>
  <tr>
    <td>-0</td>
    <td>-0</td>
    <td>-0</td>
    <td>-0</td>
  </tr>
  <tr>
    <td colspan="2">Any component is inf</td>
    <td>inf</td>
    <td>ASCRT_MAX_NORMAL_FP16</td>
  </tr>
  <tr>
    <td>-inf</td>
    <td>-inf</td>
    <td>-inf</td>
    <td>-ASCRT_MAX_NORMAL_FP16</td>
  </tr>
  <tr>
    <td>finite value</td>
    <td>nan</td>
    <td>x value</td>
    <td>x value</td>
  </tr>
    <tr>
    <td>nan</td>
    <td>finite value</td>
    <td>y value</td>
    <td>y value</td>
  </tr>
    <tr>
    <td>nan</td>
    <td>nan</td>
    <td>nan</td>
    <td>0</td>
  </tr>
</table>

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hmaxx2(half* x, half* y, half* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        half2* out = (half2*)dst;
        out[idx] = __hmaxx2(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hmaxx2(__gm__ half2* x, __gm__ half2* y, __gm__ half2* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hmaxx2(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ half* x, __gm__ half* y, __gm__ half* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hmaxx2>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, (__gm__ half2*)dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.25, 0.5), (2.5, -3.75), (nan, 1.25), (0.75, nan)
y: (0.5, -2), (1.25, 3), (-2, nan), (nan, -0.25)
result: (0.5, 0.5), (2.5, 3), (-2, 1.25), (0.75, -0.25)
```
