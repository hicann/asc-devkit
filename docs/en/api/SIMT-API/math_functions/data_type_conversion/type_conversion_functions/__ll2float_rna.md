# \_\_ll2float\_rna

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T07:08:54.431Z pushedAt=2026-10-26T11:03:37.243Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rna_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Follows the CAST\_ROUND mode to convert **int64** data to a floating-point number and returns the converted value.

## Prototype

```cpp
inline float __ll2float_rna(const long long int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number converted from the input following the CAST\_ROUND mode. Special values are as follows:

| x Value | Return Value |
|---|---|
| 9223372036854775807(INT64_MAX) | 9.22337204e+18 |
| -9223372036854775808(INT64_MIN) | -9.22337204e+18 |
| ±(2^24+1) | ±(2^24+2) |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__ll2float_rna(float* dst, int64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ll2float_rna(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__ll2float_rna(__gm__ float* dst, __gm__ int64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ll2float_rna(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -9007199254740991, -16777219, -3, 0, 3, 16777219, 9007199254740991, 9223372036854775805
result: -9007199254740992.0, -16777220.0, -3.0, 0.0, 3.0, 16777220.0, 9007199254740992.0, 9.22337204e+18
```
