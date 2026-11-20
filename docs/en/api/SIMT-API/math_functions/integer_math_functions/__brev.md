# \_\_brev

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:31:23.096Z pushedAt=2026-10-27T02:14:54.277Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__brev_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Reverses the bit order of the input data and returns the reversed value.

## Prototype

```cpp
unsigned int __brev(unsigned int x)
```

```cpp
unsigned long long __brev(unsigned long long x)
```

```cpp
unsigned long __brev(unsigned long x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Bit-reversed value of the input data. When the input type is uint32_t, bit n of the return value corresponds to bit 31-n of the input data. When the input type is uint64_t, bit n of the return value corresponds to bit 63-n of the input data.

- When x is 0 and the type is uint32\_t, the return value is 0.
- When x is 0 and the type is uint64\_t, the return value is 0.
- When x is 1 and the type is uint32\_t, the return value is 2147483648.
- When x is 1 and the type is uint64\_t, the return value is 9223372036854775808.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel_brev(unsigned int* dst, unsigned int* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __brev(x[idx]);
    }
    ```

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel_brev(unsigned long long* dst, unsigned long long* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __brev(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_brev(__gm__ unsigned int* dst, __gm__ unsigned int* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __brev(x[idx]);
    }
    ```

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_brev(__gm__ unsigned long long* dst, __gm__ unsigned long long* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __brev(x[idx]);
    }
    ```

The input and output examples are as follows:

- When the input data type is `unsigned int`:

    ```text
    x: 0, 1, 2, 8, 15
    result: 0, 2147483648, 1073741824, 268435456, 4026531840
    ```

- When the input data type is `unsigned long long`:

    ```text
    x: 0, 1, 2, 8, 15
    result: 0, 9223372036854775808, 4611686018427387904, 1152921504606846976, 17293822569102704640
    ```
