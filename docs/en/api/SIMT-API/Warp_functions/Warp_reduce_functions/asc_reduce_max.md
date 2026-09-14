# asc\_reduce\_max

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T01:05:44.682Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the maximum value of **val** across all active threads in the warp. All active threads in the warp return the same result.

## Function Prototype

```
inline int32_t asc_reduce_max(int32_t val)
```

```
inline uint32_t asc_reduce_max(uint32_t val)
```

```
inline float asc_reduce_max(float val)
```

```
inline half asc_reduce_max(half val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| val | Input | Source operand. |

## Return Value

Returns the maximum value of **val** across all active threads in the warp.

## Constraints

None

## Header Files to Include

To use interfaces of types other than **half**, include the "simt\_api/device\_warp\_functions.h" header file. To use the **half** type interface, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/device_warp_functions.h"
```

```
#include "simt_api/asc_fp16.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelReduceMax(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t result = asc_reduce_max(laneId); // Returns 31.
        dst[idx] = result;
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelReduceMax(__gm__ int32_t* dst)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t result = asc_reduce_max(laneId); // Returns 31.
        dst[idx] = result;
    }
    ```

