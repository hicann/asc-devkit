# \_\_brev

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T02:58:06.806Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Reverses the bit order of the input data and returns the reversed value.

## Function Prototype

```
unsigned int __brev(unsigned int x)
```

```
unsigned long long __brev(unsigned long long x)
```

```
unsigned long __brev(unsigned long x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the bit-reversed value of the input data. When the input type is uint32\_t, the n-th bit of the return value corresponds to the (31-n)-th bit of the input data; when the input type is uint64\_t, the n-th bit of the return value corresponds to the (63-n)-th bit of the input data.

-   When x is 0 and the type is uint32\_t, the return value is 0.
-   When x is 0 and the type is uint64\_t, the return value is 0.
-   When x is 1 and the type is uint32\_t, the return value is 2147483648.
-   When x is 1 and the type is uint64\_t, the return value is 9223372036854775808.

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
    __global__ __launch_bounds__(1024) void KernelBrev(unsigned int* dst, unsigned int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __brev(x[idx]);
    }
    ```

    ```
    __global__ __launch_bounds__(1024) void KernelBrev(unsigned long long* dst, unsigned long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __brev(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelBrev(__gm__ unsigned int* dst, __gm__ unsigned int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __brev(x[idx]);
    }
    ```

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelBrev(__gm__ unsigned long long* dst, __gm__ unsigned long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __brev(x[idx]);
    }
    ```

