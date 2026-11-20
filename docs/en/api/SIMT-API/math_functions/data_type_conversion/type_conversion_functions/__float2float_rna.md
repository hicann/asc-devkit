# \_\_float2float\_rna

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T06:45:16.640Z pushedAt=2026-10-26T11:03:37.187Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rna_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Rounds a floating-point number to an integer, following the CAST\_ROUND mode.

## Prototype

```cpp
inline float __float2float_rna(const float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number obtained by rounding the input according to the CAST_ROUND mode. Special values are as follows:

| x Value | Return Value |
|---|---|
| ±0 | 0 |
| nan | nan |
| inf | inf |
| -inf | -inf |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__float2float_rna(float* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2float_rna(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2float_rna(__gm__ float* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2float_rna(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -4.5, -1.25, 1.25, 4.5, inf, -inf, nan
result: -5.0, -1.0, 1.0, 5.0, inf, -inf, nan
```
