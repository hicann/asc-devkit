# \_\_lows2half2

<!-- md-trans-meta sourceCommit=6e11244b08ff98f28bdfed28700f33faca049b68 translatedAt=2026-10-27T03:27:04.451Z pushedAt=2026-10-27T09:21:55.675Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__lows2half2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Extracts the low 16 bits from each of the two half2 inputs, fills them into a half2 value, and returns the filled data.

## Prototype

```cpp
inline half2 __lows2half2(const half2 x, const half2 y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Extracts the low 16 bits from each of the two half2 inputs and fills them into a half2 result. The special values are as follows:

| x (y) value | Return Value |
| --- | --- |
| 0 | 0 |
| -0 | -0 |
| nan | nan |
| inf | inf |
| -inf | -inf |
| ASCRT\_MAX\_NORMAL\_FP16 | ASCRT\_MAX\_NORMAL\_FP16 |
| -ASCRT\_MAX\_NORMAL\_FP16 | -ASCRT\_MAX\_NORMAL\_FP16 |
| ASCRT\_MIN\_DENORM\_FP16 | ASCRT\_MIN\_DENORM\_FP16 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __aicore__ void simt_lows2half2(half2* input1, half2* input2, half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __lows2half2(input1[idx], input2[idx]);
    }
    __global__ __launch_bounds__(1024)void cast_kernel(half* input1, half* input2, half* output, uint32_t input_total_length)
    {
        simt_lows2half2((half2*)input1, (half2*)input2, (half2*)output, input_total_length);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_lows2half2(__gm__ half2* input1, __gm__ half2* input2, __gm__ half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __lows2half2(input1[idx], input2[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ half* input1,  __gm__ half* input2, __gm__ half* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_lows2half2>(dim3(1024), (__gm__ half2*)input1, (__gm__ half2*)input2, (__gm__ half2*)output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.25, 0.5), (2.5, -3.75), (-2, 1.25), (0.75, -0.25)
y: (0.5, -2), (1.25, 3), (1.5, -1), (-0.5, 2.25)
result: (-1.25, 0.5), (2.5, 1.25), (-2, 1.5), (0.75, -0.5)
```
