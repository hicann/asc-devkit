# \_\_bfloat162ull\_rna

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T14:04:10.929Z pushedAt=2026-10-26T06:23:22.845Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rna_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

It follows the CAST\_ROUND mode to convert bfloat16 data to a 64-bit unsigned integer and returns the converted value.

## Prototype

```cpp
inline unsigned long long int __bfloat162ull_rna(const bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the 64-bit unsigned integer converted from the input following the CAST\_ROUND mode. Special scenarios are described as follows:

| x Value | Return Value |
|---|---|
| ±0 | 0 |
| nan | 0 |
| inf | 18446744073709551615(ULLONG_MAX) |
| -inf | 0 |
| ASCRT_MAX_NORMAL_BF16 | 18446744073709551615(ULLONG_MAX) |
| -ASCRT_MAX_NORMAL_BF16 | 0 |
| 0.5 | 1 |
| 1.5 | 2 |
| 2.5 | 3 |
| Negative number | 0 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel__bfloat162ull_rna(uint64_t* dst, bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __bfloat162ull_rna(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__bfloat162ull_rna(__gm__ uint64_t* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __bfloat162ull_rna(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, 1.5, 2.5, 63.5, 64.5, 16777216.0, 4294967296.0, inf
result: 1, 2, 3, 64, 65, 16777216, 4294967296, 18446744073709551615
```
