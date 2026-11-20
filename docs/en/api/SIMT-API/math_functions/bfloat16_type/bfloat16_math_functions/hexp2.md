# hexp2

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-23T13:26:54.516Z pushedAt=2026-10-26T06:23:22.770Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Given the input x, obtains 2 to the power of x.

![](../../../../figures/formulaimage_0000002576421045.png)

## Prototype

```cpp
inline bfloat16_t hexp2(bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns 2 to the power of x. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
| --- | --- | --- |
| inf | inf | ASCRT_MAX_NORMAL_BF16 (saturated to the maximum finite value of bfloat16_t) |
| -inf | 0 | 0 |
| nan | nan | 0 (nan is saturated to 0) |
| Others | 2 to the power of x. When the result exceeds the maximum finite value of bfloat16_t, the result is inf. | 2 to the power of x. When the result exceeds the maximum finite value of bfloat16_t, the result is ASCRT_MAX_NORMAL_BF16. |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void KernelExp2(bfloat16_t* dst, bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hexp2(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelExp2(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hexp2(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -3.0, -2.0, -1.0, 0.0, 0.5, 1.0, 2.0, 3.0
result: 0.125, 0.25, 0.5, 1.0, 1.41406, 2.0, 4.0, 8.0
```
