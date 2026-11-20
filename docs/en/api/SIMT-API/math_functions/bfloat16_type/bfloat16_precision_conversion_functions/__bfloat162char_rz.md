# \_\_bfloat162char\_rz

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T13:42:10.442Z pushedAt=2026-10-26T06:23:22.805Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162char_rz_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Converts bfloat16_t type data to signed char type data following the CAST\_TRUNC mode, and returns the converted value.

## Prototype

```cpp
inline signed char __bfloat162char_rz(const bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns signed char data converted in the CAST\_TRUNC mode. The special values are as follows:

| x Value       | Return Value |
|----------|---|
| 0        | 0 |
| nan      | 0 |
| inf      | 127(SCHAR_MAX) |
| -inf     | -128(SCHAR_MIN) |
| Exceeds the upper bound of char | 127(SCHAR_MAX) |
| Exceeds the lower bound of char | -128(SCHAR_MIN) |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel___bfloat162char_rz(signed char* dst, bfloat16_t* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __bfloat162char_rz(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___bfloat162char_rz(__gm__ signed char* dst, __gm__ bfloat16_t* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __bfloat162char_rz(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, 1.5, 2.5, -1.5, -2.5, 127.5, -127.5, inf
result: 0, 1, 2, -1, -2, 127, -127, 127
```
