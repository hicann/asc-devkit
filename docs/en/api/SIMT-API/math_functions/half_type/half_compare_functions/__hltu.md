# \_\_hltu

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T13:11:29.000Z pushedAt=2026-09-04T08:03:26.259Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Compares two **half** values and returns **true** when the first value is less than the second. If either input is **nan**, returns **true**.

## Prototype

```
bool __hltu(half x, half y)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |
| **y** | Input | Source operand. |

## Return Value

Returns the result of comparing whether the first input number is less than the second.

-   true: The first number is less than the second. If either input is nan, true is returned.
-   false: The first number is greater than or equal to the second.

## Constraints

None

## Required Header File

To use this API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   **SIMT** programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelHltu(bool* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hltu(x[idx], y[idx]);
    }
    ```

-   **SIMD** and **SIMT** hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHltu(__gm__ bool* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hltu(x[idx], y[idx]);
    }
    ```

