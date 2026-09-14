# \_\_mul\_i32toi64

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T02:59:54.767Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the product of the input 32-bit integers **x** and **y** and returns a 64-bit result.

## Function Prototype

```
long long __mul_i32toi64(int x, int y)
```

```
unsigned long long __mul_i32toi64(unsigned int x, unsigned int y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, multiplier. |
| y | Input | Source operand, multiplier. |

## Return Value

Returns the 64-bit result of the product of the input data **x** and **y**.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/device\_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelMul_i32toi64(long long* dst, int* x, int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __mul_i32toi64(x[idx], y[idx]);
    }
    ```

    ```
    __global__ __launch_bounds__(1024) void KernelMul_i32toi64(unsigned long long* dst, unsigned int* x, unsigned int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __mul_i32toi64(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelMul_i32toi64(__gm__ long long* dst, __gm__ int* x, __gm__ int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __mul_i32toi64(x[idx], y[idx]);
    }
    ```

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelMul_i32toi64(__gm__ unsigned long long* dst, __gm__ unsigned int* x, __gm__ unsigned int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __mul_i32toi64(x[idx], y[idx]);
    }
    ```
