# hlog2

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-23T13:29:39.442Z pushedAt=2026-10-26T06:23:22.779Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Obtains the base-2 logarithm of the input data.

![](../../../../figures/formulaimage_0000002545900864.png)

## Prototype

```cpp
inline bfloat16_t hlog2(bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The base-2 logarithm of x. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
| --- | --- | --- |
| inf | inf | ASCRT_MAX_NORMAL_BF16 (saturated to the maximum finite value of bfloat16_t) |
| -inf, nan | nan | 0 (nan is saturated to 0) |
| 0 | -inf | -ASCRT_MAX_NORMAL_BF16 |
| x less than 0 | nan | 0 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void KernelLog2(bfloat16_t* dst, bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hlog2(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelLog2(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hlog2(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, 1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0
result: -1.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0
```
