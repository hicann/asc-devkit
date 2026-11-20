# h2rsqrt

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T06:36:57.205Z pushedAt=2026-10-26T11:03:37.175Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2rsqrt_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Obtains the square root reciprocal of each element of the input data **x**.

![](../../../../figures/formulaimage_0000002576421069.png)

## Prototype

```cpp
inline bfloat16x2_t h2rsqrt(bfloat16x2_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, input data. |

## Return Value

Returns the reciprocal of the square root of each element of the input data. This API is affected by the global saturation mode. The special values are as follows:

| Input | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
|---|---|---|
| 0 | inf | ASCRT_MAX_NORMAL_BF16 |
| -0 | inf | ASCRT_MAX_NORMAL_BF16 |
| nan | nan | 0 |
| inf | 0 | 0 |
| -inf | nan | 0 |
| x<0 | nan | 0 |

## Constraints

The input data range supported by this API is x greater than or equal to 0; otherwise, the return value is nan.

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void KernelRsqrt(bfloat16x2_t* dst, bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rsqrt(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelRsqrt(__gm__ bfloat16x2_t* dst, __gm__ bfloat16x2_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rsqrt(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: (-1.25, 0.5), (2.5, -3.75), (-2, 1.25), (0.75, -0.25)
result: (nan, 1.41406), (0.632813, nan), (nan, 0.894531), (1.15625, nan)
```
