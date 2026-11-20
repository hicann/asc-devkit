# llrintf

<!-- md-trans-meta sourceCommit=e80e2531aba21d60ee54c52d2ae137dd3bc302e2 translatedAt=2026-10-26T08:04:03.112Z pushedAt=2026-10-26T11:03:37.349Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/llrintf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the nearest integer to the input data. If two integers are equally close, the even one is obtained.

## Prototype

```cpp
inline long long int llrintf(float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the integer value closest to the input floating-point number. Special cases are described as follows:

-   When x is 0, the return value is 0.
-   When x is 0.5, the return value is 0.
-   When x is 1.5, the return value is 2.
-   When x is nan, the return value is 0.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_llrintf(long long int *result, const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = llrintf(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_llrintf_vf(__gm__ long long int *result, __gm__ const float *x, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = llrintf(x[idx]);
    }

    __global__ __vector__ void run_llrintf(__gm__ long long int *result, __gm__ const float *x, uint32_t count)
    {
        asc_vf_call<compute_llrintf_vf>(dim3(256), result, x, count);
    }
    ```

The input and output examples are as follows:

```
x: 0.25, 0.75, 1.25, 1.75
result: 0, 1, 1, 2
```
