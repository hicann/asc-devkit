# \_\_hcmadd

<!-- md-trans-meta sourceCommit=ccc1820c8c764c94b6e9a6131f4c550662f52b46 translatedAt=2026-10-26T13:41:52.161Z pushedAt=2026-10-27T02:14:54.142Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hcmadd_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Treats the three half2 inputs as complex numbers, with the first component as the real part and the second component as the imaginary part, and performs the complex multiply-add operation x\*y+z.

## Prototype

```cpp
half2 __hcmadd(const half2 x, const half2 y, const half2 z)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |
| z | Input | Source operand. |

## Return Value

Returns the result of the complex multiply-add operation, with the input data treated as complex numbers. For inputs x, y, and z, the result is as follows:

| Real Part Result | Imaginary Part Result |
| --- | --- |
| __hfma(-x.y, y.y, __hfma(x.x, y.x, z.x)) | __hfma(x.y, y.x, __hfma(x.x, y.y, z.y)) |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Using short vectors improves data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hcmadd(half* x, half* y, half* z, half* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        half2* input3 = (half2*)z;
        half2* out = (half2*)dst;
        out[idx] = __hcmadd(input1[idx], input2[idx], input3[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hcmadd(__gm__ half2* x, __gm__ half2* y, __gm__ half2* z, __gm__ half2* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hcmadd(x[idx], y[idx], z[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ half* x, __gm__ half* y, __gm__ half* z, __gm__ half* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hcmadd>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, (__gm__ half2*)z, (__gm__ half2*)dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.25, 0.5), (2.5, -3.75), (-2, 1.25), (0.75, -0.25)
y: (0.5, -2), (1.25, 3), (1.5, -1), (-0.5, 2.25)
z: (-0.5, 1), (-2, 0.75), (1, -1.5), (0.25, 2)
result: (-0.125, 3.75), (12.375, 3.5625), (-0.75, 2.375), (0.4375, 3.8125)
```
