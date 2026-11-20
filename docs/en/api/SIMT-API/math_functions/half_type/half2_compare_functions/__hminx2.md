# \_\_hminx2

<!-- md-trans-meta sourceCommit=9c514a52c4547a5e12361f00d03a37609c0df739 translatedAt=2026-11-10T10:02:40.474Z pushedAt=2026-11-13T08:46:49.182Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_compare_functions/__hminx2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Obtains the minimum value of each component of two half2 values.

## Prototype

```cpp
half2 __hminx2(const half2 x, const half2 y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the minimum value of each component of the input data. This API is affected by the global saturation register. The special values are as follows:

| x component | y component | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
| --- | --- | --- | --- |
| 0 | -0 | -0 | -0 |
| -0 | 0 | -0 | -0 |
| -0 | -0 | -0 | -0 |
| 0 | 0 | 0 | 0 |
| nan | Finite value | y value | y value |
| Finite value | nan | x value | x value |
| nan | ±inf | ±inf | ±ASCRT\_MAX\_NORMAL\_FP16 |
| ±inf | nan | ±inf | ±ASCRT\_MAX\_NORMAL\_FP16 |
| nan | nan | nan | 0 |
| any non-nan value | -inf | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| -inf | any non-nan value | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| inf | inf | inf | ASCRT\_MAX\_NORMAL\_FP16 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hminx2(half* x, half* y, half* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 data, that is, two half data. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        half2* out = (half2*)dst;
        out[idx] = __hminx2(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hminx2(__gm__ half2* x, __gm__ half2* y, __gm__ half2* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 data, that is, two half data. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hminx2(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ half* x, __gm__ half* y, __gm__ half* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hminx2>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, (__gm__ half2*)dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.25, 0.5), (2.5, -3.75), (nan, 1.25), (0.75, nan)
y: (0.5, -2), (1.25, 3), (-2, nan), (nan, -0.25)
result: (-1.25, -2), (1.25, -3.75), (-2, 1.25), (0.75, -0.25)
```
