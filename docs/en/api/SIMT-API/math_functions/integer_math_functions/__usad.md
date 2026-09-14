# \_\_usad

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T03:08:43.029Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

For input data **x**, **y**, and **z**, computes the result of |x - y| + z, that is, the sum of the absolute value of the difference between the first two inputs and the third input.

## Function Prototype

```
unsigned int __usad(unsigned int x, unsigned int y, unsigned int z)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, of type `uint32_t`. |
| y | Input | Source operand, of type `uint32_t`. |
| z | Input | Source operand, of type `uint32_t`. |

## Return Value

Returns the result of |x - y| + z.

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
    __global__ __launch_bounds__(1024) void KernelUsad(unsigned int* dst, unsigned int* x, unsigned int* y, unsigned int* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __usad(x[idx], y[idx], z[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelUsad(__gm__ unsigned int* dst, __gm__ unsigned int* x, __gm__ unsigned int* y, __gm__ unsigned int* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __usad(x[idx], y[idx], z[idx]);
    }
    ```

