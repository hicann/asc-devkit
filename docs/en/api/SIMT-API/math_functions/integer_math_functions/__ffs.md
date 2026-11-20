# \_\_ffs

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:32:58.847Z pushedAt=2026-10-27T02:14:54.285Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__ffs_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Starting from the least significant bit of the binary input data, finds the position of the first bit whose value is 1 and returns the index of that position, with the index counted from 1. If there is no 1 in the binary data, returns 0.

## Prototype

```cpp
int __ffs(int x)
```

```cpp
int __ffs(long long x)
```

```cpp
int __ffs(long x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Position of the first bit whose value is 1, counted from the least significant bit of the binary data.

-   When x = 0, returns 0.
-   When x = 1, returns 1.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void KernelFfs(int* dst, int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ffs(x[idx]);
    }
    ```

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void KernelFfs(int* dst, long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ffs(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelFfs(__gm__ int* dst, __gm__ int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ffs(x[idx]);
    }
    ```

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelFfs(__gm__ int* dst, __gm__ long long* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __ffs(x[idx]);
    }
    ```

The input and output examples are as follows:

- When the input data type is `int`:

    ```text
    x: 0, 1, 2, 8, 15
    result: 0, 1, 2, 4, 1
    ```

- When the input data type is `long long`:

    ```text
    x: 0, 1, 2, 8, 15
    result: 0, 1, 2, 4, 1
    ```
