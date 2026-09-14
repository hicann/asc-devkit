# \_\_ushort\_as\_bfloat16

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T08:27:47.074Z pushedAt=2026-08-29T09:42:03.905Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Reinterprets the bits of an **unsigned short int** value as **bfloat16**, that is, reads the bits stored in the **unsigned short int** value according to the **bfloat16** format.

## Function Prototype

```
inline bfloat16_t __ushort_as_bfloat16(const unsigned short int x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The value obtained by reinterpreting the bits of the **unsigned short int** value as **bfloat16**.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel__ushort_as_bfloat16(bfloat16_t* dst, unsigned short int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ushort_as_bfloat16(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__ushort_as_bfloat16(__gm__ bfloat16_t* dst, __gm__ unsigned short int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ushort_as_bfloat16(x[idx]);
    }
    ```

