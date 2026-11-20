# llmax

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-26T14:45:04.599Z pushedAt=2026-10-27T02:14:54.317Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/llmax_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Obtains the maximum value of the two input data.

![](../../../figures/formulaimage_0000002521590972.png)

## Prototype

```cpp
inline long long int llmax(const long long int x, const long long int y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the maximum value of the two input data.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(1024) void kernel_llmax(long long int* dst, long long int* x, long long int* y, uint64_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = llmax(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_llmax(__gm__ long long int* dst, __gm__ long long int* x, __gm__ long long int* y, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = llmax(x[idx], y[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -12, 0, 23, -7
y: -20, 0, 15, 7
result: -12, 0, 23, 7
```
