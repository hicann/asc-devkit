# \_\_bfloat162short\_rz

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T13:55:50.076Z pushedAt=2026-10-26T06:23:22.831Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162short_rz_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Converts **bfloat16_t** type data to **short int** type data following the CAST\_TRUNC mode, and returns the converted value.

## Prototype

```cpp
inline short int __bfloat162short_rz(const bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the **short int** data converted from the input following the CAST\_TRUNC mode. The special values are as follows:

| x Value | Return Value |
|---|---|
| 0 | 0 |
| nan | 0 |
| inf | 32767(SHRT_MAX) |
| -inf | -32768(SHRT_MIN) |
| Exceeds the upper bound of short | 32767(SHRT_MAX) |
| Exceeds the lower bound of short | -32768(SHRT_MIN) |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel___bfloat162short_rz(short int* dst, bfloat16_t* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __bfloat162short_rz(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___bfloat162short_rz(__gm__ short int* dst, __gm__ bfloat16_t* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __bfloat162short_rz(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, 1.5, 2.5, -1.5, -2.5, 127.5, -127.5, inf
result: 0, 1, 2, -1, -2, 127, -127, 32767
```
