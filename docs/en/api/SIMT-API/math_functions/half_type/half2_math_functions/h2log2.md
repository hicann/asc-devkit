# h2log2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-03T12:48:57.376Z pushedAt=2026-09-04T08:03:26.200Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the base-2 logarithm of each element of the input data.

![](../../../../figures/zh-cn_formulaimage_0000002513174472.png)

## Prototype

```
inline half2 h2log2(half2 x)
```

## Description

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the base-2 logarithm of each element of the input data.

-   When the input element is less than 0 or is nan, the return value is nan.
-   When the input element is 0, the return value is -inf.
-   When the input element is inf, the return value is inf.

## Constraints

None

## Required Header File

To use the **half2** type interface, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelLog2(half2* dst, half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2log2(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelLog2(__gm__ half2* dst, __gm__ half2* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = h2log2(x[idx]);
    }
    ```

