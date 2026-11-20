# normf

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T08:17:55.132Z pushedAt=2026-10-26T11:03:37.391Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/normf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the square root of the sum of squares of the first n elements in the input data a: a\[0\]^2 + a\[1\]^2 +...+ a\[n-1\]^2.

![](../../../figures/formulaimage_0000002484776410.png)

## Prototype

```cpp
inline float normf(int n, float* a)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| n | Input | Source operand. Number of consecutive elements to be computed in the input data a. |
| a | Input | Source operand. Address in UB, Global Memory, or stack space. |

## Return Value

Returns the square root of a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2.

-   If the square root of a\[0\]^2 + a\[1\]^2 + ...+ a\[n-1\]^2 exceeds the maximum float range, the return value is inf.
-   If any one or more of a\[0\], a\[1\], ..., a\[n-1\] are ±inf, the return value is inf.
-   If any one or more of a\[0\], a\[1\], ..., a\[n-1\] are NaN and none of them are ±inf, the return value is NaN.
-   If n is less than 1, the absolute value of a[0] is returned.

## Constraints

-   The length of input data a must be greater than or equal to parameter n.
-   If n is too large, API performance cannot be guaranteed.

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(256) void compute_normf(float *result, const int *n, float *vector_data, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = normf(n[idx], vector_data + idx * 4);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(256) inline void compute_normf_vf(__gm__ float *result, __gm__ const int *n, __gm__ float *vector_data, uint32_t count)
    {
        const uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= count) {
            return;
        }
        result[idx] = normf(n[idx], vector_data + idx * 4);
    }

    __global__ __vector__ void run_normf(__gm__ float *result, __gm__ const int *n, __gm__ float *vector_data, uint32_t count)
    {
        asc_vf_call<compute_normf_vf>(dim3(256), result, n, vector_data, count);
    }
    ```

The input and output examples are as follows:

```
n: 1, 2, 3, 1
vector_data: [[1, 2, 3, 4], [2, 3, 4, 5], [3, 4, 5, 6], [4, 5, 6, 7]]
result: 1, 3.605551, 7.071068, 4
```
