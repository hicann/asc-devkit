# \_\_hfma\_relu

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T11:38:20.543Z pushedAt=2026-10-27T02:14:53.823Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_arithmetic_functions/__hfma_relu_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

For the input half data x, y, and z, this API computes the result of x multiplied by y plus z, and rounds the result following the CAST\_RINT mode. Negative results are set to 0.

## Prototype

```cpp
half __hfma_relu(const half x, const half y, const half z)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| z | Input | Source operand. |

## Return Value

The value of x \* y + z. If the result is negative, it is set to 0. This API is affected by the global saturation mode. The special values are as follows:

<table>
  <tr>
    <th>x value</th>
    <th>y value</th>
    <th>z value</th>
    <th>Return Value in Non-saturation Mode</th>
    <th>Return Value in Saturation Mode</th>
  </tr>
  <tr>
    <td>±inf</td>
    <td>±0</td>
    <td>—</td>
    <td>nan</td>
    <td>0</td>
  </tr>
  <tr>
    <td>±0</td>
    <td>±inf</td>
    <td>—</td>
    <td>nan</td>
    <td>0</td>
  </tr>
  <tr>
    <td colspan="2">x*y = inf</td>
    <td>-inf</td>
    <td>nan</td>
    <td>0</td>
  </tr>
  <tr>
    <td colspan="2">x*y = -inf</td>
    <td>inf</td>
    <td>nan</td>
    <td>0</td>
  </tr>
  <tr>
    <td colspan="3">x*y+z exceeds ASCRT_MAX_NORMAL_FP16</td>
    <td>inf</td>
    <td>ASCRT_MAX_NORMAL_FP16</td>
  </tr>
  <tr>
    <td colspan="3">x*y+z is less than ASCRT_MIN_NORMAL_FP16</td>
    <td>0</td>
    <td>0</td>
  </tr>
  <tr>
    <td colspan="3">Any of x, y, and z is nan</td>
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

    __global__ __launch_bounds__(1024) void KernelHfma_relu(half* dst, half* x, half* y, half* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma_relu(x[idx], y[idx], z[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelHfma_relu(__gm__ half* dst, __gm__ half* x, __gm__ half* y, __gm__ half* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hfma_relu(x[idx], y[idx], z[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -1.25, 0.5, 2.5, -3.75
y: -0.5, 0.5, 1.25, 2.0
z: 1.0, -1.0, 0.5, 2.5
result: 1.625, 0, 3.625, 0
```
