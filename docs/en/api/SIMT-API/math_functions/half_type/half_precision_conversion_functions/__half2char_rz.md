# \_\_half2char\_rz

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T12:42:57.611Z pushedAt=2026-10-27T02:14:53.986Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2char_rz_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Converts half type data to signed char type data following the CAST\_TRUNC mode, and returns the converted value.

## Prototype

```cpp
inline signed char __half2char_rz(const half x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns signed char data converted in the CAST\_TRUNC mode. The special values are as follows:

| x Value | Return Value |
| --- | --- |
| 0 | 0 |
| nan | 0 |
| inf | 127 (SCHAR_MAX) |
| -inf | -128 (SCHAR_MIN) |
| Exceeds the upper bound of signed char | 127 (SCHAR_MAX) |
| Exceeds the lower bound of signed char | -128 (SCHAR_MIN) |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void kernel___half2char_rz(signed char* dst, half* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __half2char_rz(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___half2char_rz(__gm__ signed char* dst, __gm__ half* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __half2char_rz(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -1.5, -0.5, 0.5, 1.5, 127.5, inf, nan
result: -1, 0, 0, 1, 127, 127, 0
```
