# llabs

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:45:02.292Z pushedAt=2026-10-27T02:14:54.316Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/llabs_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the absolute value of the input data.

## Prototype

```cpp
inline long long int llabs(long long int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the absolute value of the input data.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(1024) void KernelLlabs(long long int* dst, long long int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = llabs(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelLlabs(__gm__ long long int* dst, __gm__ long long int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = llabs(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -12, 0, 23, -7
result: 12, 0, 23, 7
```
