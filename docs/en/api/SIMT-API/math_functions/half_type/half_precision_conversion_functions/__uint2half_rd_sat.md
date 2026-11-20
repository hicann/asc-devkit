# \_\_uint2half\_rd\_sat

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T13:28:10.238Z pushedAt=2026-10-27T02:14:54.098Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rd_sat_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Converts uint32 data to half data following the CAST\_FLOOR mode and returns the converted value.

## Prototype

```cpp
inline half __uint2half_rd_sat(const unsigned int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Converts the input uint32 data to half data following the CAST\_FLOOR mode. The special values are as follows:

| x Value | Return Value |
| --- | --- |
| 0 | 0 |
| 2049 | 2048 |
| ASCRT\_MAX\_NORMAL\_FP16 | ASCRT\_MAX\_NORMAL\_FP16 |
| x>ASCRT\_MAX\_NORMAL\_FP16 | ASCRT\_MAX\_NORMAL\_FP16 |

## Constraints

In SIMT programming scenarios, the saturation mode of this API does not take effect because the CTRL register cannot be set.

## Examples

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__uint2half_rd_sat(__gm__ half* dst, __gm__ uint32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __uint2half_rd_sat(x[idx]);
    }
    ```

    The input and output examples are as follows:

    ```text
    x: 0, 1, 2049, 4097, 65535
    result: 0, 1, 2048, 4096, 65504
    ```
