# \_\_byte\_perm

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:32:05.148Z pushedAt=2026-10-27T02:14:54.279Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__byte_perm_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Two 4-byte uint32\_t type inputs are combined into an 8-byte 64-bit integer. The selector s specifies 4 bytes to be selected from it, and these 4 bytes are concatenated from low to high into a uint32\_t type integer. The implementation logic is as follows:

```cpp
// The following shows the BytePerm(x, y, s) computation logic in C++.
uint64_t tmp64 = ((uint64_t)y << 32) | x; // Concatenate x and y into a uint64 integer.

uint8_t selector0 = (s >> 0) & 0x7; // selector0 ranges from 0 to 7.
uint8_t selector1 = (s >> 4) & 0x7;
uint8_t selector2 = (s >> 8) & 0x7;
uint8_t selector3 = (s >> 12) & 0x7;

uint8_t byte0 = (tmp64 >> (selector0 * 8)) & 0xFF; // Select the selector0-th byte from tmp64.
uint8_t byte1 = (tmp64 >> (selector1 * 8)) & 0xFF;
uint8_t byte2 = (tmp64 >> (selector2 * 8)) & 0xFF;
uint8_t byte3 = (tmp64 >> (selector3 * 8)) & 0xFF;

// result is the return value of BytePerm, formed by concatenating the corresponding bytes in order.
uint32_t result = byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
```

## Prototype

```cpp
unsigned int __byte_perm(unsigned int x, unsigned int y, unsigned int s)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand of type uint32_t. It is concatenated with y into a 64-bit integer, where bits [0:31] of the integer are x. |
| y | Input | Source operand of type uint32_t. It is concatenated with x into a 64-bit integer, where bits [32:63] of the integer are y. |
| s | Input | Selector of type uint32_t. It specifies how to extract 4 bytes from the 8-byte 64-bit integer formed by x and y. s[0:3], s[4:7], s[8:11], and s[12:15] correspond to the 4 bytes to be selected, and each value indicates the index of the selected byte in the 8-byte integer, ranging from 0 to 7. |

## Return Value

Returns the uint32\_t type integer selected by the selector s.

-   When x is 0, y is 0, and s is 0, the return value is 0.
-   When x is 1, y is 1, and s is 1, the return value is 16843008.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void KernelByte_perm(unsigned int* dst, unsigned int* x, unsigned int* y, unsigned int* s)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __byte_perm(x[idx], y[idx], s[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelByte_perm(__gm__ unsigned int* dst, __gm__ unsigned int* x, __gm__ unsigned int* y, __gm__ unsigned int* s)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __byte_perm(x[idx], y[idx], s[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0, 1, 258, 66051
y: 0, 1, 772, 67305985
s: 0, 1, 2731, 2105376
result: 0, 16843008, 33554432, 16974083
```
