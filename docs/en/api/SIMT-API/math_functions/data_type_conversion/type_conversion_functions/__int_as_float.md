# \_\_int\_as\_float

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T07:02:09.292Z pushedAt=2026-10-26T11:03:37.230Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int_as_float_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Reinterprets the bits in an integer as a floating-point number, that is, reads the bits stored in the integer in the float format.

## Prototype

```cpp
inline float __int_as_float(const int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number obtained by reinterpreting the bits in the input integer. The special values are as follows:

| x Value | Return Value |
|---|---|
| 0x00000000 | 0 |
| 0x80000000 | -0 |
| 0x7F800000 | inf |
| 0xFF800000 | -inf |
| 0x7FFFFFFF | nan |
| 0x7F7FFFFF | ASCRT_MAX_NORMAL_F |
| 0xFF7FFFFF | -ASCRT_MAX_NORMAL_F |
| 0x00000001 | ASCRT_MIN_DENORM_F |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__int_as_float(float* dst, int32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __int_as_float(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__int_as_float(__gm__ float* dst, __gm__ int32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __int_as_float(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0, -2147483648, 1, 1065353216, -1082130432, 2139095040, -8388608, 2143289344
result: 0.0, -0.0, 1e-45, 1.0, -1.0, inf, -inf, nan
```
