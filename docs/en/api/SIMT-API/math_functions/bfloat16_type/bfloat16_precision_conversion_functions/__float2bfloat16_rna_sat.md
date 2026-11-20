# \_\_float2bfloat16\_rna\_sat

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T14:25:12.691Z pushedAt=2026-10-26T06:23:22.880Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna_sat_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

In saturation mode, converts a floating-point number to bfloat16 precision following the CAST\_ROUND mode and returns the converted value.

## Prototype

```cpp
inline bfloat16_t __float2bfloat16_rna_sat(const float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns bfloat16 data converted from the input following the CAST\_ROUND mode in saturation mode. The special values are as follows:

| x Value | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
|---|---|---|
| 0 | 0 | 0 |
| -0 | -0 | -0 |
| nan | nan | 0 |
| inf | inf | ASCRT_MAX_NORMAL_BF16 |
| -inf | -inf | -ASCRT_MAX_NORMAL_BF16 |
| ASCRT_MAX_NORMAL_F | inf | ASCRT_MAX_NORMAL_BF16 |
| -ASCRT_MAX_NORMAL_F | -inf | -ASCRT_MAX_NORMAL_BF16 |

## Constraints

Before using this API, set the CTRL\[60\] register to 0; otherwise, the saturation mode does not take effect. For the setting method, see [Controlling Saturation Behavior](../../data_type_conversion/overview.md#section1194916101549).

In SIMT programming scenarios, the saturation mode of this API does not take effect because the CTRL register cannot be set.

## Examples

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2bfloat16_rna_sat(__gm__ bfloat16_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2bfloat16_rna_sat(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 128.25, 128.5, 128.75, -128.5, inf, -inf, 0.0, nan
result: 128.0, 129.0, 129.0, -129.0, 3.38953e+38, -3.38953e+38, 0.0, 0.0
```
