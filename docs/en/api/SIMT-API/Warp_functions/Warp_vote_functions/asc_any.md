# asc\_any

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T01:13:44.642Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines whether any active thread has a non-zero input.

After all active threads in the warp execute this API, the input operand **predicate** of all active threads is evaluated. If the **predicate** of all active threads is 0, 0 is returned; otherwise, 1 is returned. All active threads in the warp return the same result.

## Function Prototype

```
inline int32_t asc_any(int32_t predicate)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| predicate | Input | Operand. |

## Return Value

Returns 0 if the inputs of all active threads in the warp are 0; otherwise, returns 1.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/device\_warp\_functions.h" header file.

```
#include "simt_api/device_warp_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel_asc_any(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id= idx % 32;
        dst[idx] = asc_any(lane_id); // The return value is 1.
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_any(__gm__ int32_t* dst)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id = idx % 32;
        dst[idx] = asc_any(lane_id); // The return value is 1.
    }
    ```

