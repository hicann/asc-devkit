# llabs

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T03:09:57.176Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the absolute value of the input data.

## Function Prototype

```
inline long long int llabs(long long int x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |

## Return Value

Returns the absolute value of the input data.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelLlabs(long long int* dst, long long int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = llabs(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelLlabs(__gm__ long long int* dst, __gm__ long long int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = llabs(x[idx]);
    }
    ```

