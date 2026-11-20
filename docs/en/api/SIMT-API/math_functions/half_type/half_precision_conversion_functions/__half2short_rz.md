# \_\_half2short\_rz

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T12:56:14.634Z pushedAt=2026-10-27T02:14:54.019Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2short_rz_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Converts half type data to short int type data following the CAST\_TRUNC mode, and returns the converted value.

## Prototype

```cpp
inline short int __half2short_rz(const half x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the **short int** data converted from the input following the CAST\_TRUNC mode. The special values are as follows:

| x Value | Return Value |
| --- | --- |
| 0 | 0 |
| nan | 0 |
| inf | 32767(SHRT_MAX) |
| -inf | -32768(SHRT_MIN) |
| Exceeds the upper bound of short | 32767(SHRT_MAX) |
| Exceeds the lower bound of short | -32768(SHRT_MIN) |

## Constraints

None.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void kernel___half2short_rz(short int* dst, half* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __half2short_rz(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___half2short_rz(__gm__ short int* dst, __gm__ half* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __half2short_rz(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -32768, -2048, -1.5, 0.5, 1.5, 2048, inf, nan
result: -32768, -2048, -1, 0, 1, 2048, 32767, 0
```
