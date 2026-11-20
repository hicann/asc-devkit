# \_\_float22half2\_rd\_sat

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T12:17:57.642Z pushedAt=2026-10-27T02:14:53.929Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rd_sat_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Converts the two components of float2 type data to half-precision floating-point numbers following the CAST\_FLOOR mode, and returns the converted half2 type data.

## Prototype

```cpp
inline half2 __float22half2_rd_sat(const float2 x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the half2 type data obtained by converting the two input components following the CAST\_FLOOR mode. This API is affected by the global saturation register. The special values are as follows:

| x Value | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
| --- | --- | --- |
| ±0 | ±0 | ±0 |
| nan | nan | 0 |
| inf | inf | ASCRT\_MAX\_NORMAL\_FP16 |
| -inf | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| x>ASCRT\_MAX\_NORMAL\_FP16 | inf | ASCRT\_MAX\_NORMAL\_FP16 |
| x<-ASCRT\_MAX\_NORMAL\_FP16 | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |

## Constraints

Before using this API, set the CTRL\[60\] register to 0; otherwise, the saturation mode does not take effect. For the setting method, see [Controlling Saturation Behavior](../../data_type_conversion/overview.md#section1194916101549).

In SIMT programming scenarios, the saturation mode of this API does not take effect because the CTRL register cannot be set.

## Examples

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data movement efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_float22half2_rd_sat(__gm__ float2* input, __gm__ half2* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one float2 type data item, that is, two float type data items. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __float22half2_rd_sat(input[idx]);
    }

    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ half* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_float22half2_rd_sat>(dim3(1024), (__gm__ float2*)input, (__gm__ half2*)output, input_total_length);
    }
    ```

    The input and output examples are as follows:

    ```text
    x: (128, 128.0625), (128.1875, -128.1875), (inf, -inf), (nan, 65536)
    result: (128, 128), (128.125, -128.25), (65504, -65504), (0, 65504)
    ```
