# \_\_half2ll\_rna

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-04T01:09:54.966Z pushedAt=2026-09-04T11:39:11.953Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts half data to a 64-bit signed integer following the **CAST\_ROUND** mode and returns the converted value.

## Function Prototype

```
inline long long int __half2ll_rna(const half x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the 64-bit signed integer converted from the input following the **CAST\_ROUND** mode. Special scenarios are described as follows:

-   When x is nan, the return value is 0.
-   When **x** is **inf**, the return value is 9223372036854775807.
-   When **x** is **-inf**, the return value is -9223372036854775808.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel__half2ll_rna(int64_t* dst, half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __half2ll_rna(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__half2ll_rna(__gm__ int64_t* dst, __gm__ half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __half2ll_rna(x[idx]);
    }
    ```

