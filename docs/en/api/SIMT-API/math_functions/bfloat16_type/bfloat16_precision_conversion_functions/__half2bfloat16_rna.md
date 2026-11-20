# \_\_half2bfloat16\_rna

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T14:35:40.179Z pushedAt=2026-10-26T06:23:22.898Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rna_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Converts **half** type data to **bfloat16** type following the CAST\_ROUND mode, and returns the converted value.

## Prototype

```cpp
inline bfloat16_t __half2bfloat16_rna(const half x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns bfloat16 data converted from the input following the CAST\_ROUND mode. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
|---|---|---|
| ±0 | ±0 | ±0 |
| nan | nan | 0 |
| inf | inf | ASCRT_MAX_NORMAL_BF16 |
| -inf | -inf | -ASCRT_MAX_NORMAL_BF16 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel__half2bfloat16_rna(bfloat16_t* dst, half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __half2bfloat16_rna(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__half2bfloat16_rna(__gm__ bfloat16_t* dst, __gm__ half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __half2bfloat16_rna(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 128.5, 129.5, 0.5, -0.5, inf, -inf, nan, 0.0
result: 129.0, 130.0, 0.5, -0.5, inf, -inf, nan, 0.0
```
