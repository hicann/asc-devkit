# \_\_asc\_cvt\_float\_to\_fp8

<!-- md-trans-meta sourceCommit=59c3a526dd351fa43a267dc7a2d150dfa30a63e6 translatedAt=2026-10-26T08:43:23.128Z pushedAt=2026-10-26T11:03:37.438Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__asc_cvt_float_to_fp8_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp8.h"`.

Converts float data to an 8-bit floating-point number of the specified type (`float8_e4m3_t` and `float8_e5m2_t`) in CAST\_RINT mode, and performs overflow handling according to the specified saturation mode (saturation or non-saturation). The conversion result is stored at the bit level as the `__asc_fp8_storage_t` type, which is an unsigned char used to store data of the `float8_e4m3_t` or `float8_e5m2_t` type.

## Prototype

```cpp
inline __asc_fp8_storage_t __asc_cvt_float_to_fp8(const float x, const __asc_saturation_t saturation, const __asc_fp8_interpretation_t fp8_interpretation)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description                                                                                                                 |
| --- | --- |--------------------------------------------------------------------------------------------------------------------|
| x | Input | Source operand.                                                                                                              |
| saturation | Input | Controls the saturation behavior. Supported values: __ASC_NOSAT, __ASC_SATFINITE.<br><br>__ASC_NOSAT indicates the non-saturation mode, and __ASC_SATFINITE indicates the saturation mode.                    |
| fp8_interpretation | Input | Specifies the conversion type. Supported values: __ASC_E4M3, __ASC_E5M2.<br><br>__ASC_E4M3 indicates conversion to a floating-point number in the `float8_e4m3_t` format, and __ASC_E5M2 indicates conversion to a floating-point number in the `float8_e5m2_t` format. |

## Return Value

The input follows the CAST\_RINT mode. According to the specified 8-bit floating-point type and the specified saturation mode, the input is converted to data of the `__asc_fp8_storage_t` type. This API is affected by the global saturation mode. The special values are as follows:

- When the fp8_interpretation parameter is __ASC_E4M3:

    | Input | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
    |---|---|---|
    | 0 | 0 | 0 |
    | nan | nan | +0 |
    | ±inf | nan | ±448 |
    | Positive value exceeding the float8_e4m3_t range | nan | 448 |
    | Negative value exceeding the float8_e4m3_t range | nan | -448 |

- When the fp8_interpretation parameter is __ASC_E5M2:

    | Input | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
    |---|---|---|
    | ±0 | ±0 | ±0 |
    | nan | nan | +0 |
    | ±inf | ±inf | ±57344 |
    | Positive value exceeding the float8_e5m2_t range | inf | 57344 |
    | Negative value exceeding the float8_e5m2_t range | -inf | -57344 |

## Constraints

Before using this API, set the CTRL\[60\] register to 0; otherwise, the saturation mode does not take effect. For the setting method, see [Controlling Saturation Behavior](../../data_type_conversion/overview.md#section1194916101549).

This API is not supported in SIMT programming scenarios.

## Examples

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp8.h"

    __simt_vf__ __launch_bounds__(1024) inline void simt_asc_cvt_float_to_fp8(__gm__ float* input, __gm__ uint8_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= input_total_length) {
            return;
        }
        output[idx] = __asc_cvt_float_to_fp8(input[idx], __ASC_NOSAT, __ASC_E4M3);
    }
    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ uint8_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_asc_cvt_float_to_fp8>(dim3(1024), input, output, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x(float): -1.5, 0.5, 600.0, inf
result(float8_e4m3_t): -1.5, 0.5, nan, nan
```
