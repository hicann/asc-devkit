# \_\_bfloat162bfloat16\_rn

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T13:39:09.784Z pushedAt=2026-10-26T06:23:22.798Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rn_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Rounds the bfloat16\_t data following the CAST\_RINT mode.

## Prototype

```cpp
inline bfloat16_t __bfloat162bfloat16_rn(const bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns bfloat16_t data rounded following the CAST_RINT mode. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
|---|---|---|
| ±0 | 0 | 0 |
| ASCRT_MIN_DENORM_BF16 | 0 | 0 |
| nan | nan | 0 |
| inf | inf | ASCRT_MAX_NORMAL_BF16 |
| -inf | -inf | -ASCRT_MAX_NORMAL_BF16 |
| ASCRT_MAX_NORMAL_BF16 | ASCRT_MAX_NORMAL_BF16 | ASCRT_MAX_NORMAL_BF16 |
| -ASCRT_MAX_NORMAL_BF16 | -ASCRT_MAX_NORMAL_BF16 | -ASCRT_MAX_NORMAL_BF16 |
| 0.5 | 0 | 0 |
| 1.5 | 2.0 | 2.0 |
| 2.5 | 2.0 | 2.0 |
| -1.5 | -2.0 | -2.0 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel__bfloat162bfloat16_rn(bfloat16_t * dst, bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __bfloat162bfloat16_rn(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__bfloat162bfloat16_rn(__gm__ bfloat16_t * dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __bfloat162bfloat16_rn(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, -0.5, -0.0, 0.0, inf, -inf, nan, 1.5
result: 0.0, 0.0, 0.0, 0.0, inf, -inf, nan, 2.0
```
