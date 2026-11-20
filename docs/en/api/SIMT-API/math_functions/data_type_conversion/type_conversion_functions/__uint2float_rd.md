# \_\_uint2float\_rd

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T07:12:45.659Z pushedAt=2026-10-26T11:03:37.249Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_rd_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Follows the CAST\_FLOOR mode to convert **uint32** data to a floating-point number and returns the converted value.

## Prototype

```cpp
inline float __uint2float_rd(const unsigned int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number converted from the input following the CAST\_FLOOR mode. Special values are as follows:

| x Value | Return Value |
|---|---|
| 0 | 0 |
| 16777217 (2^24+1) | 16777216 |
| 16777219 (2^24+3) | 16777218 |
| 33554434 (2^25+2) | 33554432 |
| 4294967295 (UINT32_MAX) | 4294967040 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__uint2float_rd(float* dst, uint32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __uint2float_rd(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__uint2float_rd(__gm__ float* dst, __gm__ uint32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __uint2float_rd(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0, 1, 16777216, 16777217, 16777219, 33554434, 2147483648, 4294967295
result: 0.0, 1.0, 16777216.0, 16777216.0, 16777218.0, 33554432.0, 2147483648.0, 4294967040.0
```
