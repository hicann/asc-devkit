# h2rint

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:50:37.110Z pushedAt=2026-09-04T08:03:26.203Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the nearest integer to each input element. If two integers are equally close, the even one is returned.

## Prototype

```
inline half2 h2rint(half2 x)
```

## Description

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the nearest integer value for each input element. Special cases are described as follows:

-   When the input element is 0, the return value is 0.
-   When the input element is 0.5, the return value is 0.
-   When the input element is 1.5, the return value is 2.
-   When the input element is nan, the return value is nan.
-   When the input element is inf, the return value is inf.
-   When the input element is -inf, the return value is -inf.

## Constraints

None

## Required Header File

To use the half2 type APIs, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelRint(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rint(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelRint(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2rint(x[idx]);
    }
    ```

