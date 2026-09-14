# max

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-29T03:11:47.630Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the maximum value of the two input data.

## Function Prototype

```
long long max(long long x, long long y)
```

```
long max(long x, long y)
```

```
int max(int x, int y)
```

```
short max(short x, short y)
```

```
char max(char x, char y)
```

```
unsigned long long max(unsigned long long x, unsigned long long y)
```

```
unsigned long max(unsigned long x, unsigned long y)
```

```
unsigned int max(unsigned int x, unsigned int y)
```

```
unsigned short max(unsigned short x, unsigned short y)
```

```
unsigned char max(unsigned char x, unsigned char y)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the maximum value of the input data.

## Constraints

None

## Header Files to Include

To use this API, include the **simt\_api/math\_functions.h** header file.

```
#include "simt_api/math_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void KernelMax(long long* dst, long long* x, long long* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = max(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelMax(__gm__ long long* dst, __gm__ long long* x, __gm__ long long* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = max(x[idx], y[idx]);
    }
    ```

