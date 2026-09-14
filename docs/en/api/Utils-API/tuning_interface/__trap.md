# \_\_trap

<!-- md-trans-meta sourceCommit=74353d201bbcd306a2be8e7b4400f2f610835041 translatedAt=2026-09-06T08:56:44.689Z pushedAt=2026-09-11T01:45:38.720Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: not supported
- Atlas A2 training products/Atlas A2 inference products: not supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

Calling this API in SIMT code interrupts the operator execution, which is suitable for debugging abnormal scenarios on the Kernel side.

## Prototype

```
inline void __trap()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Header Files to Include

To use this API, include the "utils/debug/asc\_assert.h" header file.

```
#include "utils/debug/asc_assert.h"
```

## Example

-   SIMT programming scenario:

    ```c++
    __global__ __launch_bounds__(1024) inline void SimtKernel(float* dst, float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (isnan(x[idx])) {
            __trap();
        }
        dst[idx] = x[idx];
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```c++
    __simt_vf__ __launch_bounds__(1024) inline void SimtKernel(__gm__ float* dst, __gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (isnan(x[idx])) {
            __trap();
        }
        dst[idx] = x[idx];
    }
    ```
