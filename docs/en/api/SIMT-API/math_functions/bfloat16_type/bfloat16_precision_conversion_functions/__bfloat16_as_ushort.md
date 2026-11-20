# \_\_bfloat16\_as\_ushort

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T13:36:40.484Z pushedAt=2026-10-26T06:23:22.793Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat16_as_ushort_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Reinterprets the bits of a bfloat16_t value as an unsigned short int value.

## Prototype

```cpp
inline unsigned short int __bfloat16_as_ushort(const bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the unsigned short int type data obtained by reinterpreting the bits of the input bfloat16_t type data. The special values are as follows:

| x Value | Return Value |
|---|---|
| 0 | 0 |
| inf | 32640 |
| -inf | 65408 |
| nan | 32704 |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel___bfloat16_as_ushort(unsigned short int* dst, bfloat16_t* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __bfloat16_as_ushort(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___bfloat16_as_ushort(__gm__ unsigned short int* dst, __gm__ bfloat16_t* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __bfloat16_as_ushort(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, -0.5, -0.0, 0.0, inf, -inf, nan, 1.5
result: 16128, 48896, 32768, 0, 32640, 65408, 32704, 16320
```
