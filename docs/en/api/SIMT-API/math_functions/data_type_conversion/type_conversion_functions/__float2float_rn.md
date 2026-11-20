# \_\_float2float\_rn

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T06:44:10.335Z pushedAt=2026-10-26T11:03:37.186Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rn_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Rounds the floating-point number according to the CAST\_RINT mode, rounding to the nearest even number.

## Prototype

```cpp
inline float __float2float_rn(const float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number obtained by rounding the input according to the CAST_RINT mode. Special values are as follows:

| x Value | Return Value |
| --- | --- |
| ±0 | 0 |
| nan | nan |
| inf | inf |
| -inf | -inf |
| ASCRT_MAX_NORMAL_F | ASCRT_MAX_NORMAL_F |
| ASCRT_MIN_DENORM_F | 0 |
| 0.5 | 0 |
| 1.5 | 2.0 |
| -0.5 | 0 |
| -1.5 | -2.0 |

## Constraints

float-to-float conversion supports only non-saturating behavior.

## Examples

SIMT programming scenario:

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__float2float_rn(float* dst, float* x, uint32_t total_length)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx >= total_length) {
        return;
    }
    dst[idx] = __float2float_rn(x[idx]);
}
```

SIMD and SIMT mixed programming scenario:

```cpp
#include "simt_api/device_functions.h"

__simt_vf__ __launch_bounds__(1024) inline void kernel__float2float_rn(__gm__ float* dst, __gm__ float* x, uint32_t total_length)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx >= total_length) {
        return;
    }
    dst[idx] = __float2float_rn(x[idx]);
}
```

The input and output examples are as follows:

```text
x: -3.5, -0.5, 2.5, 3.5, inf, -inf, nan
result: -4.0, 0.0, 2.0, 4.0, inf, -inf, nan
```
