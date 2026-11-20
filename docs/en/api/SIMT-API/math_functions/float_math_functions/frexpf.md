# frexpf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T07:56:34.493Z pushedAt=2026-10-26T11:03:37.334Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/frexpf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Converts `x` to a normalized \[1/2, 1\) signed number multiplied by an integral power of 2. Returns the normalized signed number, with the exponent stored in `exp`.

![](../../../figures/formulaimage_0000002516816359.png)

## Prototype

```cpp
inline float frexpf(float x, int *exp)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| exp | Output | Address in UB, Global Memory, or stack space for storing the base-2 exponent. |

## Return Value

Returns the signed number obtained when x is converted to a normalized signed value in [1/2, 1) multiplied by an integer power of 2.

- When x is inf, the return value is inf, and exp is 0.
- When x is -inf, the return value is -inf, and exp is 0.
- When x is nan, the return value is nan, and exp is 0.
- When x is 0, the return value is 0, and exp is 0.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_frexpf(float *result, int *exp_out, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = frexpf(x[idx], exp_out + idx);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_frexpf_vf(__gm__ float *result, __gm__ int *exp_out, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = frexpf(x[idx], exp_out + idx);
    }

    __global__ __vector__ void run_frexpf(__gm__ float *result, __gm__ int *exp_out, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_frexpf_vf>(dim3(256), result, exp_out, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0.5, 0.75, 0.625, 0.875
exp_out: -1 0 1 1
```
