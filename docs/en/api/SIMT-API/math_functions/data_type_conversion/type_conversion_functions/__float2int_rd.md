# \_\_float2int\_rd

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T06:46:37.811Z pushedAt=2026-10-26T11:03:37.192Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rd_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts a floating-point number into a signed integer following the CAST\_FLOOR mode and returns the converted value.

## Prototype

```cpp
inline int __float2int_rd(const float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the signed integer converted following the CAST\_FLOOR mode. Special values are as follows:

| x Value | Return Value |
|---|---|
| ±0 | 0 |
| nan | 0 |
| inf | 2147483647 |
| -inf | -2147483648 |
| Positive number exceeding the int range | 2147483647 |
| Negative number exceeding the int range | -2147483648 |
| Positive decimal | Rounded down |
| Negative decimal | Rounded down |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__float2int_rd(int32_t* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2int_rd(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2int_rd(__gm__ int32_t* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __float2int_rd(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -2.5, -1.5, 0.5, 1.5, inf, -inf, nan
result: -3, -2, 0, 1, 2147483647, -2147483648, 0
```
