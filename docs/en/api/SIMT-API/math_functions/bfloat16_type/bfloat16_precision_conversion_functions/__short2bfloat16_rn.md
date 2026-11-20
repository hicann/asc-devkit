# \_\_short2bfloat16\_rn

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T14:59:34.336Z pushedAt=2026-10-26T06:23:22.940Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__short2bfloat16_rn_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Converts short int type data to bfloat16_t type data following the CAST\_RINT mode, and returns the converted value.

## Prototype

```cpp
inline bfloat16_t __short2bfloat16_rn(const short int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns bfloat16_t data converted from the input following the CAST_RINT mode. The special values are as follows:

| Input | Return Value |
|---|---|
| 0 | 0 |
| 2049 | 2048 |
| -2049 | -2048 |
| 32767 (short int maximum value) | 32768 |
| -32768 (short int minimum value) | -32768 |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel___short2bfloat16_rn(bfloat16_t* dst, short int* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __short2bfloat16_rn(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___short2bfloat16_rn(__gm__ bfloat16_t* dst, __gm__ short int* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __short2bfloat16_rn(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -129, -258, 258, 259, -32768, 32767, 0, 1
result: -129.0, -258.0, 258.0, 260.0, -32768.0, 32768.0, 0.0, 1.0
```
