# \_\_ull2float\_rna

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T09:23:53.654Z pushedAt=2026-08-29T02:54:47.420Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Converts a uint64 value to a floating-point number following the CAST\_ROUND mode and returns the converted value.

## Prototype

```
inline float __ull2float_rna(const unsigned long long int x)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the floating-point number converted from the input following the **CAST\_ROUND** mode.

## Constraints

None

## Required Header File

To use this API, include the **simt\_api/device\_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   **SIMT** programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel__ull2float_rna(float* dst, uint64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ull2float_rna(x[idx]);
    }
    ```

-   **SIMD** and **SIMT** mixed programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel__ull2float_rna(__gm__ float* dst, __gm__ uint64_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ull2float_rna(x[idx]);
    }
    ```

