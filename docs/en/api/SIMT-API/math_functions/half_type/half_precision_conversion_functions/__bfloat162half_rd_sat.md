# \_\_bfloat162half\_rd\_sat

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T12:09:31.708Z pushedAt=2026-10-27T02:14:53.900Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd_sat_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Converts bfloat16 type data to half type data following the CAST\_FLOOR mode and returns the converted value.

## Prototype

```cpp
inline half __bfloat162half_rd_sat(const bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the half data converted from the input following the CAST_FLOOR mode. This API is affected by the global saturation register. The special values are as follows:

| x Value | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
| --- | --- | --- |
| nan | nan | 0 |
| inf | inf | ASCRT\_MAX\_NORMAL\_FP16 |
| -inf | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| x>ASCRT\_MAX\_NORMAL\_FP16 | inf | ASCRT\_MAX\_NORMAL\_FP16 |
| x<-ASCRT\_MAX\_NORMAL\_FP16 | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| ±0 | ±0 | ±0 |

## Constraints

Before using this API, set the CTRL\[60\] register to 0; otherwise, the saturation mode does not take effect. For the setting method, see [Controlling Saturation Behavior](../../data_type_conversion/overview.md#section1194916101549).

In SIMT programming scenarios, the saturation mode of this API does not take effect because the CTRL register cannot be set.

## Examples

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__bfloat162half_rd_sat(__gm__ half* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __bfloat162half_rd_sat(x[idx]);
    }
    ```

    The input and output examples are as follows:

    ```text
    x: 0, -0, 65536, 65536, 9.96920997e+37, inf, -inf, nan
    result: 0, -0, 65504, 65504, 65504, 65504, -65504, 0
    ```
