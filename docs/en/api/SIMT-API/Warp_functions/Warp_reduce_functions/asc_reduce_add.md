# asc\_reduce\_add

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:03:07.088Z pushedAt=2026-10-27T09:21:55.768Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_add_res.md#id1 -->

## Description

The header file path is `"simt_api/device_warp_functions.h"` (for APIs other than the half type) and `"simt_api/asc_fp16.h"` (for the half type API).

Sums the val values of all active threads in the warp. All active threads in the warp return the same result.

## Prototype

```cpp
inline int32_t asc_reduce_add(int32_t val)
```

```cpp
inline uint32_t asc_reduce_add(uint32_t val)
```

```cpp
inline float asc_reduce_add(float val)
```

```cpp
inline half asc_reduce_add(half val)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| val | Input | Source operand. |

## Return Value

Returns the sum of the val values of all threads in the warp.

## Constraints

-   When the summation result overflows, this API does not guarantee computation precision.
-   The underlying implementation of this API uses a binary algorithm. In some scenarios, the computation result may differ from that of sequential computation. In short, \(\(\(a + b\) + c\) + d\) and \(\(a + b\) + \(c + d\)\) follow different computation orders, which may lead to different final results. This is because, during floating-point computation, each addition involves a finite-precision numerical representation, and the rounding operations in this process cause precision loss. Therefore, different addition orders may produce different intermediate results, which in turn affects the precision of the final computation result.

## Examples

For the complete sample, see [MemoryFence Sample](../../../../../../examples/03_simt_api/02_features/01_api_features/01_sync_instruction/memory_fence/README.md).

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelReduceAdd(int32_t* dst, int32_t* src, int32_t total_num)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_num) {
            return;
        }
        int32_t result = asc_reduce_add(src[idx]);
        if (laneid() == 0) {
            asc_atomic_add(dst, result);
        }
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelReduceAdd(__gm__ int32_t* dst, __gm__ int32_t* src, int32_t total_num)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_num) {
            return;
        }
        int32_t result = asc_reduce_add(src[idx]);
        if (laneid() == 0) {
            asc_atomic_add(dst, result);
        }
    }
    ```

The input and output examples are as follows:

```text
src: 17, -8, 42, 3, -25, 60, 11, -37, 24, 5, -14, 73, 8, -2, 39, 19, -31, 54, 7, 12, -18, 28, 66, -9, 33, 1, -45, 22, 48, -5, 36, 15
dst: 429
```
