# lanemask\_gt

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T01:01:40.392Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains a 32-bit mask for the current thread. In the warp to which the current thread belongs, the bits corresponding to threads whose Lane ID is strictly greater than that of the current thread are set to 1, and the remaining bits are set to 0.

For example, a thread with Lane ID 0 obtains the following 32-bit mask by calling this API: 1111 1111 1111 1111 1111 1111 1111 1110.

For example, a thread with Lane ID 31 obtains the following 32-bit mask by calling this API: 0000 0000 0000 0000 0000 0000 0000 0000.

**Figure 1**  lanemask\_gt Schematic diagram

![](../../../figures/lanemask_gt.png "lanemask_gt diagram")

## Function Prototype

```
int32_t lanemask_gt()
```

## Parameters

None

## Return Value

Returns a 32-bit integer bit mask.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/device\_functions.h** header file. 

``` 
#include "simt_api/device_functions.h" 
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernel_lanemask_gt(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lanemask = lanemask_gt();
        dst[idx] = lanemask;
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) void kernel_lanemask_gt(__gm__ int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lanemask = lanemask_gt();
        dst[idx] = lanemask;
    }
    ```
