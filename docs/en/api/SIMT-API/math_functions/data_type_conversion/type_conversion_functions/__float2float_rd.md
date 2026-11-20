# \_\_float2float\_rd

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T06:43:04.118Z pushedAt=2026-10-26T11:03:37.185Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rd_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Rounds a floating-point number down (toward negative infinity) and follows the CAST\_FLOOR mode.

## Prototype

```cpp
inline float __float2float_rd(const float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number obtained after rounding the input according to the CAST\_FLOOR mode. Special values are as follows:

| Input | Return Value |
|---|---|
| ±0 | 0 |
| nan | nan |
| inf | inf |
| -inf | -inf |

## Constraints

None

## Examples

SIMT programming scenario:

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__float2float_rd(float* dst, float* x, uint32_t total_length)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx >= total_length) {
        return;
    }
    dst[idx] = __float2float_rd(x[idx]);
}
```

SIMD and SIMT mixed programming scenario:

```cpp
#include "simt_api/device_functions.h"

__simt_vf__ __launch_bounds__(1024) inline void kernel__float2float_rd(__gm__ float* dst, __gm__ float* x, uint32_t total_length)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx >= total_length) {
        return;
    }
    dst[idx] = __float2float_rd(x[idx]);
}
```

The input and output examples are as follows:

```text
x: -2.5, -1.5, 0.5, 1.5, inf, -inf, nan
result: -3.0, -2.0, 0.0, 1.0, inf, -inf, nan
```
