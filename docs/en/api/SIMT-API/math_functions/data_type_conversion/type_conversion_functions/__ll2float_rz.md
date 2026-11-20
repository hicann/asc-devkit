# \_\_ll2float\_rz

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T07:10:19.651Z pushedAt=2026-10-26T11:03:37.246Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rz_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts **int64** data to a floating-point number following the CAST\_TRUNC mode and returns the converted value.

## Prototype

```cpp
inline float __ll2float_rz(const long long int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number converted following the CAST\_TRUNC mode. The special values are as follows:

| x Value | Return Value |
|---|---|
| 0 | 0 |
| 16777217(2^24+1 ) | ASCRT_TWO_TO_24_F |
| -16777217(-2^24-1) | -ASCRT_TWO_TO_24_F |
| 9223372036854775807(INT64_MAX) | 9.22337149e+18 |
| -9223372036854775808(INT64_MIN) | -9.22337204e+18 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__ll2float_rz(float* dst, int64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ll2float_rz(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__ll2float_rz(__gm__ float* dst, __gm__ int64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ll2float_rz(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -9007199254740989, -16777221, -5, 0, 5, 16777221, 9007199254740989, 9223372036854775803
result: -9007198717870080.0, -16777220.0, -5.0, 0.0, 5.0, 16777220.0, 9007198717870080.0, 9.22337149e+18
```
