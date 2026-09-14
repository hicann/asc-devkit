# asc\_reduce\_add

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T01:04:23.424Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sums the **val** input from all active threads in a warp. All active threads in the warp return the same result.

## Function Prototype

```
inline int32_t asc_reduce_add(int32_t val)
```

```
inline uint32_t asc_reduce_add(uint32_t val)
```

```
inline float asc_reduce_add(float val)
```

```
inline half asc_reduce_add(half val)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| val | Input | Source operand. |

## Return Value

Returns the sum of the **val** inputs from all threads in the warp.

## Constraints

-   When the summation result overflows, this API does not guarantee calculation precision.
-   The underlying implementation of this API uses a binary reduction algorithm. In certain scenarios, the calculation result may differ from that of sequential computation. In simple terms, the calculation order of \(\(\(a + b) + c\) + d\) differs from that of \(\(a + b\) +\(c + d\)), which may lead to different final results. This is because, during floating-point computation, each addition operation involves a finite-precision numerical representation, and the rounding operations in this process cause precision loss. Therefore, different addition orders may produce different intermediate results, which in turn affects the precision of the final result.

## Header Files to Include

To use the APIs for types other than half, include the "simt\_api/device\_warp\_functions.h" header file. To use the half-type API, include the "simt\_api/asc\_fp16.h" header file.

```
#include "simt_api/device_warp_functions.h"
```

```
#include "simt_api/asc_fp16.h"
```

## Example

For the complete sample, see [MemoryFence Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/02_features/01_api_features/01_sync_instruction/memory_fence/README.md).

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelReduceAdd(int32_t* dst)
    {
         int idx = threadIdx.x + blockIdx.x * blockDim.x;
         int32_t laneId = idx % 32;
         int32_t result = asc_reduce_add(laneId); // The return value is 0+1+2+...+31=496.
         dst[idx] = result;
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelReduceAdd(__gm__ int32_t* dst)
    {
         // asc_vf_call parameter: dim3{1024, 1, 1}.
         int idx = threadIdx.x + blockIdx.x * blockDim.x;
         int32_t laneId = idx % 32;
         int32_t result = asc_reduce_add(laneId); // The return value is 0+1+2+...+31=496.
         dst[idx] = result;
    }
    ```
