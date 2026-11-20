# \_\_umul24

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:39:06.701Z pushedAt=2026-10-27T02:14:54.302Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__umul24_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Obtains the lower 32 bits of the product of the lower 24 bits of the uint32 data x and y. The upper 8 bits of x and y are ignored.

## Prototype

```cpp
unsigned int __umul24(unsigned int x, unsigned int y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Lower 32 bits of the product of the lower 24 bits of the input data.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void KernelUmul24(unsigned int* dst, unsigned int* x, unsigned int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __umul24(x[idx], y[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelUmul24(__gm__ unsigned int* dst, __gm__ unsigned int* x, __gm__ unsigned int* y)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __umul24(x[idx], y[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: 0, 12, 23, 70
y: 6, 15, 3, 4
result: 0, 180, 69, 280
```
