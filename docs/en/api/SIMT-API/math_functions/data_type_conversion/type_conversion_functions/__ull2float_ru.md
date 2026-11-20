# \_\_ull2float\_ru

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T07:19:43.053Z pushedAt=2026-10-26T11:03:37.261Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_ru_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts **uint64** data to a floating-point number following the CAST\_CEIL mode, and returns the converted value.

## Prototype

```cpp
inline float __ull2float_ru(const unsigned long long int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number converted from the input following the CAST\_CEIL mode. Special values are as follows:

| Input | Return Value |
| --- | --- |
| 0 | 0 |
| 16777217(2^24+1) | 16777218 |
| 18446744073709551615(ULLONG_MAX) | 1.84467441e+19 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__ull2float_ru(float* dst, uint64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ull2float_ru(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__ull2float_ru(__gm__ float* dst, __gm__ uint64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ull2float_ru(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 6, 7, 16777222, 16777224, 4294967292, 9007199254740990, 9223372036854775804, 18446744073709551612
result: 6.0, 7.0, 16777222.0, 16777224.0, 4294967296.0, 9007199254740992.0, 9.22337204e+18, 1.84467441e+19
```
