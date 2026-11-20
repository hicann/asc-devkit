# \_\_asc\_cvt\_float2\_to\_fp8x2

<!-- md-trans-meta sourceCommit=59c3a526dd351fa43a267dc7a2d150dfa30a63e6 translatedAt=2026-10-26T08:44:32.444Z pushedAt=2026-10-26T11:03:37.443Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__asc_cvt_float2_to_fp8x2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp8.h"`.

Converts the two components of float2 data into 8-bit floating-point numbers of the specified type (float8\_e4m3x2\_t and float8\_e5m2x2\_t) in CAST\_RINT mode, and handles overflow according to the specified saturation mode (saturation or non-saturation). The conversion result is stored in bit-packed form as the \_\_asc\_fp8x2\_storage\_t type, which is a 16-bit unsigned integer (unsigned short int) used to store data of the float8\_e4m3x2\_t or float8\_e5m2x2\_t type.

## Prototype

```cpp
inline __asc_fp8x2_storage_t __asc_cvt_float2_to_fp8x2(const float2 x, const __asc_saturation_t saturate, const __asc_fp8_interpretation_t fp8_interpretation)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| saturate | Input | Controls the saturation behavior. Supported values: __ASC_NOSAT, __ASC_SATFINITE.<br><br>__ASC_NOSAT indicates non-saturation mode, and __ASC_SATFINITE indicates saturation mode. |
| fp8_interpretation | Input | Specifies the conversion type. Supported values: __ASC_E4M3, __ASC_E5M2.<br><br>__ASC_E4M3 indicates conversion to float8_e4m3x2_t format, and __ASC_E5M2 indicates conversion to float8_e5m2x2_t format. |

## Return Value

The two input components follow the CAST\_RINT mode and are converted into \_\_asc\_fp8x2\_storage\_t type data according to the specified 8-bit floating-point type and the specified saturation mode. This API is affected by the global saturation mode. The special values are as follows:

-   When fp8_interpretation is __ASC_E4M3:

    | per-component input | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
    |---|---|---|
    | ±0 | ±0 | ±0 |
    | nan | nan | +0 |
    | ±inf | nan | ±448 |
    | Positive value beyond the float8_e4m3_t range | nan | 448 |
    | Negative value beyond the float8_e4m3_t range | nan | -448 |

-   When fp8_interpretation is __ASC_E5M2:

    | per-component input | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
    |---|---|---|
    | ±0 | ±0 | ±0 |
    | nan | nan | +0 |
    | ±inf | ±inf | ±57344 |
    | Positive value beyond the float8_e5m2_t range | inf | 57344 |
    | Negative value beyond the float8_e5m2_t range | -inf | -57344 |

## Constraints

Before using this API, set the CTRL\[60\] register to 0; otherwise, the saturation mode does not take effect. For the setting method, see [Controlling Saturation Behavior](../../data_type_conversion/overview.md#section1194916101549).

This API is currently not supported in SIMT programming scenarios.

## Examples

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp8.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_asc_cvt_float2_to_fp8x2(__gm__ float2* input, __gm__ uint16_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one float2 value, that is, two float values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __asc_cvt_float2_to_fp8x2(input[idx], __ASC_NOSAT, __ASC_E4M3);
    }
    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ uint16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_asc_cvt_float2_to_fp8x2>(dim3(1024), (__gm__ float2*)input, output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x(float2): (-1.5, 0.5), (17.25, -17.25), (600.0, -600.0), (nan, inf)
result(float8_e4m3x2_t): (-1.5, 0.5), (18, -18), (nan, nan), (nan, nan)
```
