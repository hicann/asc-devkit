# \_\_popc

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:36:44.879Z pushedAt=2026-10-27T02:14:54.296Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__popc_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Counts the number of bits set to 1 in the input data, from the most significant bit to the least significant bit of the binary representation.

## Prototype

```cpp
int __popc(unsigned int x)
```

```cpp
int __popc(unsigned long long x)
```

```cpp
int __popc(unsigned long x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Number of bits set to 1 in the binary representation of the input data.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void KernelPopc(int* dst, unsigned int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __popc(x[idx]);
    }
    ```

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void KernelPopc(int* dst, unsigned long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __popc(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelPopc(__gm__ int* dst, __gm__ unsigned int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __popc(x[idx]);
    }
    ```

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelPopc(__gm__ int* dst, __gm__ unsigned long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __popc(x[idx]);
    }
    ```

The input and output examples are as follows:

- When the input data type is `unsigned int`:

    ```text
    x: 0, 1, 2, 8, 15
    result: 0, 1, 1, 1, 4
    ```

- When the input data type is `unsigned long long`:

    ```text
    x: 0, 1, 2, 8, 15
    result: 0, 1, 1, 1, 4
    ```
