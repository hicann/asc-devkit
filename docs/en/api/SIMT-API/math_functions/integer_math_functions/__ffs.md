# \_\_ffs

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T02:58:02.082Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Starting from the least significant bit of the binary input data, finds the position of the first bit whose value is 1 and returns the index of that position. The index starts counting from 1. If there is no 1 in the binary data, 0 is returned.

## Function Prototype

```
int __ffs(int x)
```

```
int __ffs(long long x)
```

```
int __ffs(long x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the position of the first bit whose value is 1, counting from the least significant bit of the binary data.

-   When x = 0, 0 is returned.
-   When x = 1, 1 is returned.

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
    __global__ __launch_bounds__(1024) void KernelFfs(int* dst, int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ffs(x[idx]);
    }
    ```

    ```
    __global__ __launch_bounds__(1024) void KernelFfs(int* dst, long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ffs(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelFfs(__gm__ int* dst, __gm__ int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ffs(x[idx]);
    }
    ```

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelFfs(__gm__ int* dst, __gm__ long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ffs(x[idx]);
    }
    ```

