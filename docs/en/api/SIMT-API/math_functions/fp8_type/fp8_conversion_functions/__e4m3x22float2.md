# \_\_e4m3x22float2

<!-- md-trans-meta sourceCommit=343a6e89d2704a866eb5b4d939aacaa8ebe1820b translatedAt=2026-10-27T09:50:01.491Z pushedAt=2026-10-27T11:36:23.254Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__e4m3x22float2_res.md#id1 -->

## Description

The header file path is `"simt_api/asc_fp8.h"`.

Converts the two components of **float8_e4m3x2_t** type data to floating-point numbers and returns the converted **float2** type data.

## Prototype

```cpp
inline float2 __e4m3x22float2(const float8_e4m3x2_t x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the float2 data converted from the float8\_e4m3x2\_t type input. The special values are as follows:

| x Component Value | Return Value |
|---|---|
| 0 | 0 |
| -0 | -0 |
| nan | nan |
| 448 (maximum normal value) | 448.0 |

## Constraints

This API is not supported in the SIMT programming scenario.

## Examples

-   SIMD and SIMT hybrid programming scenario:

    ```cpp
    #include "simt_api/asc_fp8.h"

    // Using short vectors improves data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_e4m3x22float2(__gm__ float8_e4m3x2_t* input, __gm__ float2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one float8_e4m3x2_t value, that is, two fp8_e4m3fn_t values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __e4m3x22float2(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ uint8_t* input,  __gm__ float* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_e4m3x22float2>(dim3(1024), (__gm__ float8_e4m3x2_t*)input, (__gm__ float2*)output, input_total_length);
    }
    ```
