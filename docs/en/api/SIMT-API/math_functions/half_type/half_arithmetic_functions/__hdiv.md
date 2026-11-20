# \_\_hdiv

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T11:40:12.622Z pushedAt=2026-10-27T02:14:53.829Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_arithmetic_functions/__hdiv_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Computes the result of dividing two half values and rounds the result following the CAST\_RINT mode.

## Prototype

```cpp
half __hdiv(const half x, const half y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the result of dividing the input values.

-   When neither the input nor the result is nan, the sign of x/y is the XOR of the signs of x and y.
-   This API is affected by the global saturation mode. The special values are as follows:

    <table>
      <tr>
        <th>x Value</th>
        <th>y Value</th>
        <th>Return Value in Non-saturation Mode</th>
        <th>Return Value in Saturation Mode</th>
      </tr>
      <tr>
        <td>±0</td>
        <td>±0</td>
        <td>nan</td>
        <td>0</td>
      </tr>
      <tr>
        <td>±inf</td>
        <td>±inf</td>
        <td>nan</td>
        <td>0</td>
      </tr>
      <tr>
        <td>Finite value</td>
        <td>±inf</td>
        <td>The return value sign is determined by the XOR of the signs of x and y, and the value is 0.</td>
        <td>The return value sign is determined by the XOR of the signs of x and y, and the value is 0.</td>
      </tr>
      <tr>
        <td>±inf</td>
        <td>Finite value</td>
        <td>The return value sign is determined by the XOR of the signs of x and y, and the value is inf.</td>
        <td>The return value sign is determined by the XOR of the signs of x and y, and the value is ASCRT_MAX_NORMAL_FP16.</td>
      </tr>
      <tr>
        <td>Nonzero</td>
        <td>±0</td>
        <td>The return value sign is determined by the XOR of the signs of x and y, and the value is inf.</td>
        <td>The return value sign is determined by the XOR of the signs of x and y, and the value is ASCRT_MAX_NORMAL_FP16.</td>
      </tr>
      <tr>
        <td>±0</td>
        <td>Nonzero</td>
        <td>The return value sign is determined by the XOR of the signs of x and y, and the value is 0.</td>
        <td>The return value sign is determined by the XOR of the signs of x and y, and the value is 0.</td>
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

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void KernelHdiv(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hdiv(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelHdiv(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hdiv(x[idx], y[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -1.25, 0.5, 2.5, -3.75
y: 0.5, -2.0, 5.0, -0.25
result: -2.5, -0.25, 0.5, 15
```
