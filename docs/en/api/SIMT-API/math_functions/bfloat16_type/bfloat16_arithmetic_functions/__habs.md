# \_\_habs

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-23T13:02:55.825Z pushedAt=2026-10-26T06:23:22.717Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__habs_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Obtains the absolute value of the input data.

![](../../../../figures/formulaimage_0000002576421055.png)

## Prototype

```cpp
inline bfloat16_t __habs(bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the absolute value of the input data. This API is not affected by the global saturation mode. The special values are as follows:

| x Value | Return Value |
| --- | --- |
| ±0 | 0 |
| inf | inf |
| -inf | inf |
| nan | nan |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel_abs(bfloat16_t* dst, bfloat16_t* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __habs(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_abs(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __habs(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, -0.5, -0.0, 0.0, inf, -inf, nan, 1.5
result: 0.5, 0.5, 0.0, 0.0, inf, inf, nan, 1.5
```
