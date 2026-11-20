# \_\_isnanx2

<!-- md-trans-meta sourceCommit=63d31ee22164094353a3097c524e8859d00acee7 translatedAt=2026-10-23T16:02:22.532Z pushedAt=2026-10-26T06:23:23.063Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__isnanx2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Determines whether the two components of a bfloat16x2_t value are NaN.

## Prototype

```cpp
bfloat16x2_t __isnanx2(bfloat16x2_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the result of checking whether each component of the input data is nan. If a component is nan, the corresponding result is 1.0; otherwise, it is 0.0. The special values are as follows:

| x Component Value | Return Value |
|---|---|
| nan | 1.0 |
| ±0 | 0 |
| ±inf | 0 |
| ±ASCRT_MAX_NORMAL_BF16 | 0 |
| ASCRT_MIN_DENORM_BF16 | 0 |
| Normal value | 0 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_isnanx2(bfloat16_t* x, bfloat16_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        bfloat16x2_t* input = (bfloat16x2_t*)x;
        bfloat16x2_t* out = (bfloat16x2_t*)dst;
        out[idx] = __isnanx2(input[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_isnanx2(__gm__ bfloat16x2_t* x, __gm__ bfloat16x2_t* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one bfloat16x2_t value, that is, two bfloat16_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __isnanx2(x[idx]);
    }

    __global__ __vector__ void compute_kernel(__gm__ bfloat16_t* x, __gm__ bfloat16_t* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_isnanx2>(dim3(1024), (__gm__ bfloat16x2_t*)x, (__gm__ bfloat16x2_t*)dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.25, 0.5), (2.5, -3.75), (nan, 1.25), (0.75, nan)
result: (0, 0), (0, 0), (1, 0), (0, 1)
```
