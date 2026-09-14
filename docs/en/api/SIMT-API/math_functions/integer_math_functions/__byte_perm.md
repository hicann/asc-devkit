# \_\_byte\_perm

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-08-29T02:58:06.453Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Combines two input 4-byte **uint32_t** values into an 8-byte 64-bit integer, uses the selector **s** to specify four bytes to extract, and concatenates these four bytes from low to high into a **uint32_t** integer. The implementation logic is as follows:

```
// The following shows the computation logic of BytePerm(x, y, s) in C++.
uint64_t tmp64 = ((uint64_t)y << 32) | x; // Concatenate x and y into a uint64 integer.

uint8_t selector0 = (s >> 0) & 0x7; // The value range of selector0 is [0, 7].
uint8_t selector1 = (s >> 4) & 0x7;
uint8_t selector2 = (s >> 8) & 0x7;
uint8_t selector3 = (s >> 12) & 0x7;

uint8_t byte0 = (tmp64 >> (selector0 * 8)) & 0xFF; // Select the selector0-th byte in tmp64.
uint8_t byte1 = (tmp64 >> (selector1 * 8)) & 0xFF;
uint8_t byte2 = (tmp64 >> (selector2 * 8)) & 0xFF;
uint8_t byte3 = (tmp64 >> (selector3 * 8)) & 0xFF;

// result is the return value of BytePerm, which is formed by concatenating the corresponding bytes in order.
uint32_t result = byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
```

## Function Prototype

```
unsigned int __byte_perm(unsigned int x, unsigned int y, unsigned int s)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand of type **uint32_t**, concatenated with **y** into a 64-bit integer whose bits [0:31] are **x**. |
| y | Input | Source operand of type **uint32_t**, concatenated with **x** into a 64-bit integer whose bits [32:63] are **y**. |
| s | Input | Selector of type **uint32_t**, used to specify how to extract 4 bytes of data from the 8-byte 64-bit integer composed of **x** and **y**. Specifically, the values represented by s[0:3], s[4:7], s[8:11], and s[12:15] specify the indices 0 to 7 of the selected bytes in the 8-byte integer. |

## Return Value

Returns the **uint32_t** integer selected by the selector **s**.

-   When x is 0, y is 0, and s is 0, the return value is 0.
-   When **x** is 1, **y** is 1, and **s** is 1, the return value is 16843008.

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
    __global__ __launch_bounds__(1024) void KernelByte_perm(unsigned int* dst, unsigned int* x, unsigned int* y, unsigned int* s)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __byte_perm(x[idx], y[idx], s[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void KernelByte_perm(__gm__ unsigned int* dst, __gm__ unsigned int* x, __gm__ unsigned int* y, __gm__ unsigned int* s)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __byte_perm(x[idx], y[idx], s[idx]);
    }
    ```

