# \_\_hmulx2

<!-- md-trans-meta sourceCommit=ccc1820c8c764c94b6e9a6131f4c550662f52b46 translatedAt=2026-10-26T13:44:26.796Z pushedAt=2026-10-27T02:14:54.151Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hmulx2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Computes the product of the corresponding components of two half2 values and rounds the result following the CAST\_RINT mode.

## Prototype

```cpp
half2 __hmulx2(const half2 x, const half2 y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

The product of the corresponding components of the input data. This API is affected by the global saturation register. The special values are as follows:

| x component | y component | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
| --- | --- | --- | --- |
| nan | Any value | nan | 0 |
| Any value | nan | nan | 0 |
| ±0 | ±inf | nan | 0 |
| ±inf | ±0 | nan | 0 |
| inf | non-infinity, non-zero | the sign is determined by the XOR of the signs of x and y, with the value inf | the sign is determined by the XOR of the signs of x and y, with the value ASCRT\_MAX\_NORMAL\_FP16 |
| -inf | non-infinity, non-zero | the sign is determined by the XOR of the signs of x and y, with the value inf | the sign is determined by the XOR of the signs of x and y, with the value ASCRT\_MAX\_NORMAL\_FP16 |
| inf | inf | inf | ASCRT\_MAX\_NORMAL\_FP16 |
| -inf | -inf | inf | ASCRT\_MAX\_NORMAL\_FP16 |
| inf | -inf | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| -inf | inf | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| ASCRT\_MAX\_NORMAL\_FP16 | ASCRT\_MAX\_NORMAL\_FP16 | inf | ASCRT\_MAX\_NORMAL\_FP16 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hmulx2(half* x, half* y, half* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        half2* out = (half2*)dst;
        out[idx] = __hmulx2(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hmulx2(__gm__ half2* x, __gm__ half2* y, __gm__ half2* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hmulx2(x[idx], y[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ half* x, __gm__ half* y, __gm__ half* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hmulx2>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, (__gm__ half2*)dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.25, 0.5), (2.5, -3.75), (-2, 1.25), (0.75, -0.25)
y: (0.5, -2), (1.25, 3), (1.5, -1), (-0.5, 2.25)
result: (-0.625, -1), (3.125, -11.25), (-3, -1.25), (-0.375, -0.5625)
```
