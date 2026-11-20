# \_\_hsub

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T11:41:42.797Z pushedAt=2026-10-27T02:14:53.834Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_arithmetic_functions/__hsub_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Computes the result of subtracting two half data values and rounds the result following the CAST\_RINT mode.

## Prototype

```cpp
half __hsub(const half x, const half y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Result of subtracting the input data. This API is affected by the global saturation mode. The special values are as follows:

<table>
  <tr>
    <th>x value</th>
    <th>y value</th>
    <th>Return Value in Non-saturation Mode</th>
    <th>Return Value in Saturation Mode</th>
  </tr>
  <tr>
    <td>±inf</td>
    <td>finite value</td>
    <td>±inf</td>
    <td>±ASCRT_MAX_NORMAL_FP16</td>
  </tr>
  <tr>
    <td>finite value</td>
    <td>±inf</td>
    <td>∓inf</td>
    <td>∓ASCRT_MAX_NORMAL_FP16</td>
  </tr>
  <tr>
    <td>±inf</td>
    <td>±inf</td>
    <td>nan</td>
    <td>0</td>
  </tr>
  <tr>
    <td>±inf</td>
    <td>∓inf</td>
    <td>±inf</td>
    <td>±ASCRT_MAX_NORMAL_FP16</td>
  </tr>
  <tr>
    <td>Finite value (including ±0)</td>
    <td>x</td>
    <td>+0</td>
    <td>+0</td>
  </tr>
  <tr>
    <td colspan="2">Either x or y is nan</td>
    <td>nan</td>
    <td>0</td>
  </tr>
</table>

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void kernel_hsub(half* dst, half* x, half* y, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hsub(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_hsub(__gm__ half* dst, __gm__ half* x, __gm__ half* y, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hsub(x[idx], y[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -1.25, 0.5, 2.5, -3.75
y: -0.5, 0.5, 1.25, 2.0
result: -0.75, 0, 1.25, -5.75
```
