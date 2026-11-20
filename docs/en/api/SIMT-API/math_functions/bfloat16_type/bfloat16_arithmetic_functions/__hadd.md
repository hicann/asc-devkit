# \_\_hadd

<!-- md-trans-meta sourceCommit=7b786840f05dedef3280f2d5a265e24854520432 translatedAt=2026-10-23T13:03:10.001Z pushedAt=2026-10-26T06:23:22.719Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hadd_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Computes the sum of two bfloat16 values and rounds the result according to the CAST\_RINT mode.

## Prototype

```cpp
bfloat16_t __hadd(const bfloat16_t x, const bfloat16_t y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

The result of adding the input data. This API is not affected by the global saturation mode. Special values are as follows:

-   \_\_hadd\(x, y\) is equivalent to \_\_hadd\(y, x\).
-   When x is a finite value and y is ±infinity, the return value is ±infinity.
-   When x is ±infinity and y is ±infinity, the return value is ±infinity.
-   When x is ±infinity and y is ∓infinity, the return value is NaN.
-   When x is ±0 and y is ±0, the return value is ±0.
-   For a finite value x (including ±0), when x = -y, the return value is +0.
-   When either **x** or **y** is **nan**, returns **nan**.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void KernelHadd(bfloat16_t* dst, bfloat16_t* x, bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hadd(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelHadd(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, __gm__ bfloat16_t* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hadd(x[idx], y[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0.5, -0.5, 1.0, -1.0, 2.0, -2.0, 0.0, 4.0
y: 2.0, 0.5, -1.0, -1.0, 0.5, 2.0, 4.0, 0.5
result: 2.5, 0.0, 0.0, -2.0, 2.5, 0.0, 4.0, 4.5
```
