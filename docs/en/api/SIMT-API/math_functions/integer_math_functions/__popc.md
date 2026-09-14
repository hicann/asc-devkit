# \_\_popc

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T03:02:43.767Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Counts the number of bits set to 1 in the input data, from the most significant bit to the least significant bit.

## Function Prototype

```
int __popc(unsigned int x)
```

```
int __popc(unsigned long long x)
```

```
int __popc(unsigned long x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The number of bits set to 1 in the binary representation of the input data.

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
    __global__ __launch_bounds__(1024) void KernelPopc(int* dst, unsigned int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __popc(x[idx]);
    }
    ```

    ```
    __global__ __launch_bounds__(1024) void KernelPopc(int* dst, unsigned long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __popc(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelPopc(__gm__ int* dst, __gm__ unsigned int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __popc(x[idx]);
    }
    ```

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelPopc(__gm__ int* dst, __gm__ unsigned long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __popc(x[idx]);
    }
    ```

