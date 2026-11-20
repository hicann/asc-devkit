# \_\_int2half\_rz\_sat

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T13:16:22.049Z pushedAt=2026-10-27T02:14:54.064Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rz_sat_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Converts int32 type data to half type data following the CAST_TRUNC mode and returns the converted value.

## Prototype

```cpp
inline half __int2half_rz_sat(const int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Follows the CAST\_TRUNC mode to convert the input int32 data to half data. The special values are as follows:

| x Value | Return Value |
| --- | --- |
| 0 | 0 |
| 2049 | 2048 |
| -2049 | -2048 |
| 2147483647 (maximum int32_t value) | ASCRT\_MAX\_NORMAL\_FP16 |
| -2147483648 (minimum int32_t value) | -ASCRT\_MAX\_NORMAL\_FP16 |

## Constraints

In SIMT programming scenarios, the saturation mode of this API does not take effect because the CTRL register cannot be set.

## Examples

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__int2half_rz_sat(__gm__ half* dst, __gm__ int32_t* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __int2half_rz_sat(x[idx]);
    }
    ```

    The input and output examples are as follows:

    ```text
    x: 0, 1, -1, 65504, 65535, 2147483647, -2147483648
    result: 0, 1, -1, 65504, 65504, 65504, -65504
    ```
