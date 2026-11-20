# \_\_ull2float\_rn

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T07:18:15.602Z pushedAt=2026-10-26T11:03:37.258Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_rn_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts uint64 data to a floating-point number following the CAST\_RINT mode and returns the converted value.

## Prototype

```cpp
inline float __ull2float_rn(const unsigned long long int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number converted from the input following the CAST\_RINT mode. The special values are as follows:

| x Value | Return Value |
|---|---|
| 16777217 (2^24+1) | 16777216.0 |
| 33554433 (2^25+1) | 33554432.0 |
| 4294967295 (UINT32_MAX) | 4294967296.0 |
| 18446744073709551615 (UINT64_MAX) | 1.84467441e+19 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__ull2float_rn(float* dst, uint64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ull2float_rn(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__ull2float_rn(__gm__ float* dst, __gm__ uint64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ull2float_rn(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 2, 3, 16777218, 16777220, 4294967294, 9007199254740992, 9223372036854775806, 18446744073709551614
result: 2.0, 3.0, 16777218.0, 16777220.0, 4294967296.0, 9007199254740992.0, 9.22337204e+18, 1.84467441e+19
```
