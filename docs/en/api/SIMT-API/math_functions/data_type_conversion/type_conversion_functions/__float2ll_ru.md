# \_\_float2ll\_ru

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T06:52:45.842Z pushedAt=2026-10-26T11:03:37.204Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_ru_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts a floating-point number into a 64-bit signed integer following the CAST\_CEIL mode and returns the converted value.

## Prototype

```cpp
inline long long int __float2ll_ru(const float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the 64-bit signed integer converted from the input following the CAST\_CEIL mode. Special values are as follows:

| x Value | Return Value |
|---|---|
| ±0 | 0 |
| nan | 0 |
| inf | 9223372036854775807(INT64_MAX) |
| -inf | -9223372036854775808(INT64_MIN) |
| Positive value exceeding the int64 range | 9223372036854775807(INT64_MAX) |
| Negative value exceeding the int64 range | -9223372036854775808(INT64_MIN) |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__float2ll_ru(int64_t* dst, float* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __float2ll_ru(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2ll_ru(__gm__ int64_t* dst, __gm__ float* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __float2ll_ru(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -5.5, -0.25, 2.25, 5.5, inf, -inf, nan
result: -5, 0, 3, 6, 9223372036854775807, -9223372036854775808, 0
```
