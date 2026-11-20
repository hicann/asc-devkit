# modff

<!-- md-trans-meta sourceCommit=e80e2531aba21d60ee54c52d2ae137dd3bc302e2 translatedAt=2026-10-26T08:11:46.285Z pushedAt=2026-10-26T11:03:37.370Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/modff_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Decomposes the input parameter into a fractional part and an integer part.

## Prototype

```cpp
inline float modff(float x, float *n)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| n | Output | Address in UB, Global Memory, or stack space, used to store the integer part of the input data. |

## Return Value

Returns the fractional part of the input parameter.

-   When x is nan, the return value is nan.
-   When x is inf, the return value is 0.
-   When x is -inf, the return value is 0.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_modff(float *result, float *integer_part, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = modff(x[idx], integer_part + idx);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_modff_vf(__gm__ float *result, __gm__ float *integer_part, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = modff(x[idx], integer_part + idx);
    }

    __global__ __vector__ void run_modff(__gm__ float *result, __gm__ float *integer_part, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_modff_vf>(dim3(256), result, integer_part, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.25, 0.75, 0.25, 0.75
integer_part: 0 0 1 1
```
