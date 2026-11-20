# \_\_float\_as\_uint

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T06:42:05.424Z pushedAt=2026-10-26T11:03:37.183Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float_as_uint_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Reinterprets the bits of a floating-point number as an unsigned integer, that is, reads the bits stored in the floating-point number in the format of an unsigned integer.

## Prototype

```cpp
inline unsigned int __float_as_uint(const float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the unsigned integer obtained by reinterpreting the bits of the input floating-point number. The special values are as follows:

| x Value | Return Value |
|---|---|
| 0 | 0 |
| -0 | 2147483648 |
| nan | 2143289344 |
| inf | ASCRT_INF_U |
| -inf | 4286578688 |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__float_as_uint(uint32_t* dst, float* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __float_as_uint(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__float_as_uint(__gm__ uint32_t* dst, __gm__ float* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __float_as_uint(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.0, -0.0, 1.0, -1.0, inf, -inf, nan
result: 0, 2147483648, 1065353216, 3212836864, 2139095040, 4286578688, 2143289344
```
