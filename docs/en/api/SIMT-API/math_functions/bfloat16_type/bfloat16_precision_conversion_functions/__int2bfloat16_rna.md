# \_\_int2bfloat16\_rna

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T14:45:26.752Z pushedAt=2026-10-26T06:23:22.916Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rna_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Converts int32 type data to bfloat16 type data following the CAST\_ROUND mode and returns the converted value.

## Prototype

```cpp
inline bfloat16_t __int2bfloat16_rna(const int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns bfloat16 data converted from the input following the CAST\_ROUND mode. The special values are as follows:

| Input | Return Value |
|---|---|
| 2147483647(INT32_MAX) | 2147483648 |
| -2147483648(INT32_MIN) | -2147483648(INT32_MIN) |
| 257 | 258 |
| -257 | -258 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel__int2bfloat16_rna(bfloat16_t* dst, int32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __int2bfloat16_rna(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__int2bfloat16_rna(__gm__ bfloat16_t* dst, __gm__ int32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __int2bfloat16_rna(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 129, 257, 258, 259, -129, -258, 1000000, -1000000
result: 129.0, 258.0, 258.0, 260.0, -129.0, -258.0, 999424.0, -999424.0
```
