# laneid

<!-- md-trans-meta sourceCommit=b666412ab6defb951e2247c9cf853e459f5b4171 translatedAt=2026-09-08T01:01:37.843Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the index of a thread within its warp.

## Function Prototype

```
int32_t laneid()
```

## Parameters

None

## Return Value

Returns the index of the thread within its warp. Since the number of threads in a warp is currently a fixed value of 32, the Lane ID ranges from [0, 31].

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
    __global__ __launch_bounds__(1024) void kernel_laneid(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id = laneid();
        dst[idx] = lane_id;
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) void kernel_laneid(__gm__ int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t lane_id = laneid();
        dst[idx] = lane_id;
    }
    ```
