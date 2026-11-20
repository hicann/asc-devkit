# \_\_uint2bfloat16\_rd

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T15:01:23.399Z pushedAt=2026-10-26T06:23:22.945Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rd_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Converts uint32 type data to bfloat16 type data following the CAST\_FLOOR mode and returns the converted value.

## Prototype

```cpp
inline bfloat16_t __uint2bfloat16_rd(const unsigned int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns bfloat16 data converted from the input following the CAST\_FLOOR mode. The special values are as follows:

| x Value | Return Value |
|---|---|
| 0 | 0 |
| 257 | 256 |
| 1000000 | 999424 |
| 4294967295(UINT32_MAX) | 4.27819e+09 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel__uint2bfloat16_rd(bfloat16_t* dst, uint32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __uint2bfloat16_rd(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__uint2bfloat16_rd(__gm__ bfloat16_t* dst, __gm__ uint32_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __uint2bfloat16_rd(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 129, 257, 258, 259, 100000, 1000000, 4294967295, 0
result: 129.0, 256.0, 258.0, 258.0, 99840.0, 999424.0, 4278190080.0, 0.0
```
