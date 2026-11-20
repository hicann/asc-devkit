# \_\_hmin\_nan

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T11:52:25.376Z pushedAt=2026-10-27T02:14:53.863Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_compare_functions/__hmin_nan_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Obtains the minimum value of the two inputs. If either input is nan, the result is nan.

![](../../../../figures/formulaimage_0000002574049835.png)

## Prototype

```cpp
half __hmin_nan(const half x, const half y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

The minimum value of the two inputs. Special values are as follows:

| x Value | y Value | Return Value |
| --- | --- | --- |
| -0 | +0 | x |
| +0 | -0 | y |
| nan | Any value | nan |
| Any value | nan | nan |
| -inf | Any value | -inf |
| Any value | -inf | -inf |
| inf | y | y |
| x | inf | x |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void KernelHmin_nan(half* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmin_nan(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelHmin_nan(__gm__ half* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hmin_nan(x[idx], y[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: nan, 0.5, 2.5, -3.75
y: -1.25, -0.25, 3.25, nan
result: nan, -0.25, 2.5, nan
```
