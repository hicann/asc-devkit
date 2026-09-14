# \_\_hisinf

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T13:07:57.758Z pushedAt=2026-09-04T08:03:26.253Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines whether a floating-point number is infinity.

## Prototype

```
inline bool __hisinf(half x)
```

## Description

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

-   **false**: The input is not infinity.
-   **true**: The input is **inf** or **-inf**.

## Constraints

None

## Header Files to Include

To use the **half** type APIs, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelIsInf(bool* dst, half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hisinf(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelIsInf(__gm__ bool* dst, __gm__ half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hisinf(x[idx]);
    }
    ```

