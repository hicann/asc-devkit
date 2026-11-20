# \_\_sad

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:37:54.962Z pushedAt=2026-10-27T02:14:54.299Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__sad_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Computes `|x - y| + z` for the input data `x`, `y`, and `z`, that is, the sum of the absolute value of the difference between the first two inputs and the third input.

## Prototype

```cpp
unsigned int __sad(int x, int y, unsigned int z)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand of the int32_t type. |
| y | Input | Source operand of the int32_t type. |
| z | Input | Source operand of the uint32_t type. |

## Return Value

The result of `|x - y| + z`.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void KernelSad(unsigned int* dst, int* x, int* y, unsigned int* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __sad(x[idx], y[idx], z[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelSad(__gm__ unsigned int* dst, __gm__ int* x, __gm__ int* y, __gm__ unsigned int* z)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __sad(x[idx], y[idx], z[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -12, 0, 23, 7
y: 0, 15, 7, -13
z: 0, 5, 9, 3
result: 12, 20, 25, 23
```
