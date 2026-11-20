# \_\_float2ull\_rn

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T06:59:33.604Z pushedAt=2026-10-26T11:03:37.222Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_rn_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts a floating-point number into a 64-bit unsigned integer following the CAST\_RINT mode and returns the converted value.

## Prototype

```cpp
inline unsigned long long int __float2ull_rn(const float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the 64-bit unsigned integer converted from the input following the CAST\_RINT mode. Special values are as follows:

| x Value | Return Value |
|---|---|
| ±0 | 0 |
| nan | 0 |
| inf | 18446744073709551615(ULLONG_MAX) |
| -inf | 0 |
| Negative number | 0 |
| Positive value exceeding the range of unsigned long long int | 18446744073709551615(ULLONG_MAX) |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__float2ull_rn(uint64_t* dst, float* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __float2ull_rn(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2ull_rn(__gm__ uint64_t* dst, __gm__ float* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __float2ull_rn(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -3.5, -0.5, 2.5, 3.5, 1.84467441e+19, inf, nan
result: 0, 0, 2, 4, 18446744073709551615, 18446744073709551615, 0
```
