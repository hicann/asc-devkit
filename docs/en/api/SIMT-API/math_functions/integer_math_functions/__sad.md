# \_\_sad

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T03:04:17.337Z -->

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
unsigned int __sad(int x, int y, unsigned int z)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, of type **int32_t**. |
| y | Input | Source operand, of type **int32_t**. |
| z | Input | Source operand, of type **uint32_t**. |

## Return Value

| Returns the result of |x - y| + z. |

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
    __global__ __launch_bounds__(1024) void KernelSad(unsigned int* dst, int* x, int* y, unsigned int* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __sad(x[idx], y[idx], z[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelSad(__gm__ unsigned int* dst, __gm__ int* x, __gm__ int* y, __gm__ unsigned int* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __sad(x[idx], y[idx], z[idx]);
    }
    ```

