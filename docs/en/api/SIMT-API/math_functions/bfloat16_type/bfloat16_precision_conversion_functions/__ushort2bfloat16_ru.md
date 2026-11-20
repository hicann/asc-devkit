# \_\_ushort2bfloat16\_ru

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T15:12:28.367Z pushedAt=2026-10-26T06:23:22.967Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort2bfloat16_ru_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Following the CAST\_CEIL mode, converts unsigned short int type data to bfloat16_t type data and returns the converted value.

## Prototype

```cpp
inline bfloat16_t __ushort2bfloat16_ru(const unsigned short int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns bfloat16_t data converted from the input following the CAST\_CEIL mode. The special values are as follows:

| x Value | Return Value |
|---|---|
| 0 | 0 |
| 2049 | 2064 |
| 4097 | 4128 |
| 65505 | 65536 |
| 65535 | 65536 |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel___ushort2bfloat16_ru(bfloat16_t* dst, unsigned short int* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __ushort2bfloat16_ru(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___ushort2bfloat16_ru(__gm__ bfloat16_t* dst, __gm__ unsigned short int* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __ushort2bfloat16_ru(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 129, 257, 258, 259, 10000, 65535, 0, 1
result: 129.0, 258.0, 258.0, 260.0, 10048.0, 65536.0, 0.0, 1.0
```
