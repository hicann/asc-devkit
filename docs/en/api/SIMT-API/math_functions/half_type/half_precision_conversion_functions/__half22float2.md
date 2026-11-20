# \_\_half22float2

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T12:41:47.970Z pushedAt=2026-10-27T02:14:53.978Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half22float2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Converts the two components of half2 to float respectively, fills them into float2, and returns the result.

## Prototype

```cpp
float2 __half22float2(const half2 x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Converts the two components of half2 to float respectively and fills them into float2. The special values are as follows:

| x value (each half component) | Return Value |
| --- | --- |
| 0 | 0 |
| -0 | -0 |
| nan | nan |
| inf | inf |
| -inf | -inf |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __aicore__ void simt_half22float2(half2* input, float2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __half22float2(input[idx]);
    }

    __global__ __launch_bounds__(1024) void cast_kernel(half* input, float* output, uint32_t input_total_length)
    {
        simt_half22float2((half2*)input, (float2*)output, input_total_length);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_half22float2(__gm__ half2* input, __gm__ float2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __half22float2(input[idx]);
    }

    __global__ __vector__ void cast_kernel(__gm__ half* input, __gm__ float* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_half22float2>(dim3(1024), (__gm__ half2*)input, (__gm__ float2*)output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-2.5, -1.5), (-0.5, 0.5), (1.5, 2.5), (inf, nan)
result: (-2.5, -1.5), (-0.5, 0.5), (1.5, 2.5), (inf, nan)
```
