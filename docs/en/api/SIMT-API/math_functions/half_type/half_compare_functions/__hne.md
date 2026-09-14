# \_\_hne

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T13:15:54.665Z pushedAt=2026-09-04T08:03:26.269Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Compares whether two **half** values are not equal, and returns **true** if they are not equal.

## Prototype

```
bool __hne(half x, half y)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **x** | Input | Source operand. |
| **y** | Input | Source operand. |

## Return Value

Returns the result of comparing whether the input data is not equal.

-   true: The input data is not equal.
-   false: The input data is equal. If either input is nan, false is returned.

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
    __global__ __launch_bounds__(1024) void KernelHne(bool* dst, half* x, half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hne(x[idx], y[idx]);
    }
    ```

-   **SIMD** and **SIMT** hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelHne(__gm__ bool* dst, __gm__ half* x, __gm__ half* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hne(x[idx], y[idx]);
    }
    ```

