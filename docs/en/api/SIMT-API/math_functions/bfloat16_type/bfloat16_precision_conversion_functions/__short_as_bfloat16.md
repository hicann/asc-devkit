# \_\_short\_as\_bfloat16

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T14:57:18.567Z pushedAt=2026-10-26T06:23:22.937Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short_as_bfloat16_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Reinterprets short int type data bitwise as bfloat16_t type data.

## Prototype

```cpp
inline bfloat16_t __short_as_bfloat16(const short int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the bfloat16 value obtained by reinterpreting the bits of the short int data. The special values are as follows:

| x Value                            | Return Value  |
|-------------------------------|------|
| 0                             | 0    |
| 32640(0x7F81)                 | inf  |
| -128(0xFF80)                  | -inf |
| 32641(0x7F81) ~ 32767(0x7FFF) | nan  |
| -127(0xFF81) ~ -1(0xFFFF)     | nan  |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel___short_as_bfloat16(bfloat16_t* dst, short int* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __short_as_bfloat16(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___short_as_bfloat16(__gm__ bfloat16_t* dst, __gm__ short int* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __short_as_bfloat16(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0, -32768, 16256, -16640, 1, 32640, -1024, 32256
result: 0.0, -0.0, 1.0, -0.5, 0.0, inf, -2.65846e+36, 4.25353e+37
```
