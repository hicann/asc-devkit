# asc\_ballot

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T01:14:44.344Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Determines whether the input of each active thread in a warp is non-zero.

After all active threads in a warp execute this API, the input operand **predicate** of each active thread is evaluated, and a 32-bit unsigned integer is returned. If the **predicate** input by an active thread in the warp is non-zero, the bit corresponding to the thread's Lane ID in the return value is set to 1; otherwise, it is set to 0. All active threads in the warp return the same result.

## Function Prototype

```
inline uint32_t asc_ballot(int32_t predicate)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| predicate | Input | Operand. |

## Return Value

Returns a 32-bit unsigned integer. If the **predicate** input by an active thread in the warp is non-zero, the bit corresponding to the thread's Lane ID in the return value is set to 1; otherwise, it is set to 0.

## Constraints

None

## Header Files to Include

To use this API, include the "simt\_api/device\_warp\_functions.h" header file.

```
#include "simt_api/device_warp_functions.h"
```

## Example

For the complete sample, see [Sobel edge detection sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/02_features/01_api_features/03_warp_instruction/sobel_warp_shfl/README.md).

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelBallot(uint32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id = idx % 32;
        dst[idx] = asc_ballot(lane_id); // The return value is 0xfffffffe.
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelBallot(__gm__ uint32_t* dst)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}.
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id = idx % 32;
        dst[idx] = asc_ballot(lane_id); // The return value is 0xfffffffe.
    }
    ```
