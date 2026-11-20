# hlog2

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T12:03:33.719Z pushedAt=2026-10-27T02:14:53.888Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_math_functions/hlog2_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Obtains the base-2 logarithm of the input data.

![](../../../../figures/formulaimage_0000002533480321.png)

## Prototype

```cpp
inline half hlog2(half x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The base-2 logarithm of x. This API is affected by the global saturation register. The special values are as follows:

| x Value | Return Value in Non-saturation Mode | Return Value in Saturation Mode |
| --- | --- | --- |
| ±0 | -inf | -ASCRT\_MAX\_NORMAL\_FP16 |
| nan | nan | 0 |
| inf | inf | ASCRT\_MAX\_NORMAL\_FP16 |
| x<0 | nan | 0 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void KernelLog2(half* dst, half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hlog2(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelLog2(__gm__ half* dst, __gm__ half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hlog2(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -1.25, 0.5, 2.5, -3.75
result: nan, -1, 1.32227, nan
```
