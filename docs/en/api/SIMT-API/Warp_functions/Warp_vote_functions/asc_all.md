# asc\_all

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T01:13:04.408Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines whether the inputs of all active threads are non-zero.

After all active threads in a warp execute this API, the input operand **predicate** of each active thread is evaluated. If the **predicate** of every active thread is non-zero, 1 is returned; otherwise, 0 is returned. All active threads in the warp return the same result.

## Function Prototype

```
inline int32_t asc_all(int32_t predicate)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| predicate | Input | Operand. |

## Return Value

Returns 1 if the inputs of all active threads in the warp are non-zero; otherwise, returns 0.

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
    __global__ __launch_bounds__(1024) void kernel_asc_all(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id= idx % 32;
        dst[idx] = asc_all(lane_id); // Returns 0.
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernel_asc_all(__gm__ int32_t* dst)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}.
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id = idx % 32;
        dst[idx] = asc_all(lane_id); // Returns 0.
    }
    ```

