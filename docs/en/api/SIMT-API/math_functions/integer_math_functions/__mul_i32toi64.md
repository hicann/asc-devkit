# \_\_mul\_i32toi64

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:34:13.737Z pushedAt=2026-10-27T02:14:54.289Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__mul_i32toi64_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Computes the product of the input 32-bit integers x and y and returns a 64-bit result.

## Prototype

```cpp
long long __mul_i32toi64(int x, int y)
```

```cpp
unsigned long long __mul_i32toi64(unsigned int x, unsigned int y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, multiplier. |
| y | Input | Source operand, multiplier. |

## Return Value

Returns the 64-bit result of the product of the input data x and y.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void KernelMul_i32toi64(long long* dst, int* x, int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __mul_i32toi64(x[idx], y[idx]);
    }
    ```

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void KernelMul_i32toi64(unsigned long long* dst, unsigned int* x, unsigned int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __mul_i32toi64(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelMul_i32toi64(__gm__ long long* dst, __gm__ int* x, __gm__ int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __mul_i32toi64(x[idx], y[idx]);
    }
    ```

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelMul_i32toi64(__gm__ unsigned long long* dst, __gm__ unsigned int* x, __gm__ unsigned int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __mul_i32toi64(x[idx], y[idx]);
    }
    ```

The input and output examples are as follows:

- When the input data type is `int`:

    ```text
    x: -12, 23, -7, 6
    y: 6, 3, 4, 15
    result: -72, 69, -28, 90
    ```

- When the input data type is `unsigned int`:

    ```text
    x: 0, 12, 23, 70
    y: 6, 15, 3, 4
    result: 0, 180, 69, 280
    ```
