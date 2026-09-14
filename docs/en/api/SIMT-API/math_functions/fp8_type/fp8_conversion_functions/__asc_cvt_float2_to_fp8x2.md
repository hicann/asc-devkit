# \_\_asc\_cvt\_float2\_to\_fp8x2

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-01T10:39:30.323Z pushedAt=2026-09-03T07:28:07.233Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts the two components of a **float2** value into 8-bit floating-point numbers of the specified type (**float8\_e4m3x2\_t** or **float8\_e5m2x2\_t**) following the **CAST\_RINT** mode, and handles overflow according to the specified saturation mode (saturation or non-saturation). The conversion result is stored in bit-packed form as the **\_\_asc\_fp8x2\_storage\_t** type, which is a 16-bit unsigned integer (**unsigned short int**) used to store data of the **float8\_e4m3x2\_t** or **float8\_e5m2x2\_t** type.

## Function Prototype

```
inline __asc_fp8x2_storage_t __asc_cvt_float2_to_fp8x2(const float2 x, const __asc_saturation_t saturate, const __asc_fp8_interpretation_t fp8_interpretation)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| saturate | Input | Controls the saturation behavior. Supported values: **__ASC_NOSAT**, **__ASC_SATFINITE**.<br><br>**__ASC_NOSAT** indicates the non-saturation mode, and **__ASC_SATFINITE** indicates the saturation mode. |
| fp8_interpretation | Input | Specifies the conversion type. Supported values: **__ASC_E4M3**, **__ASC_E5M2**.<br><br>**__ASC_E4M3** indicates conversion to floating-point numbers in the **float8_e4m3x2_t** format, and **__ASC_E5M2** indicates conversion to floating-point numbers in the **float8_e5m2x2_t** format. |

## Return Value

The two input components follow the **CAST\_RINT** mode and are converted into **\_\_asc\_fp8x2\_storage\_t** data according to the specified 8-bit floating-point type and the specified saturation mode.

-   The **float8\_e4m3\_t** data type has no inf value. In non-saturation mode, when the input exceeds the representable range of this type, the overflow result is nan; in saturation mode, the overflow result is the maximum or minimum value representable by this type.
-   The **float8\_e5m2\_t** type has an inf value. In non-saturation mode, when the input exceeds the representable range of this type, the conversion result is the inf value with the corresponding sign; in saturation mode, the overflow result is the maximum or minimum value representable by this type.
-   In saturation mode, for the **float8\_e5m2\_t** and **float8\_e4m3\_t** data types, when register CTRL\[50\]=0, a nan value is converted to 0; when register CTRL\[50\]=1, a nan value is converted to the nan value of the fp8 type. The CTRL register value can be obtained through the [GetCtrlSpr\(ISASI\)](../../../../SIMD-API/basic_api/special_register_access/GetCtrlSpr_ISASI.md) API.

## Constraints

Before using this API, set the CTRL\[60\] register to 0; otherwise, the saturation mode does not take effect. For details about how to set it, see [Methods for Controlling Saturation Behavior](../../data_type_conversion/overview_258.md#methods-for-controlling-saturation-behavior).

This API is not supported in SIMT programming scenarios.

## Header Files to Include

To use this API, include the "simt\_api/asc\_fp8.h" header file.

```
#include "simt_api/asc_fp8.h"
```

## Example

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_asc_cvt_float2_to_fp8x2(__gm__ float2* input, __gm__ uint16_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one float2 data item, that is, two float data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
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
