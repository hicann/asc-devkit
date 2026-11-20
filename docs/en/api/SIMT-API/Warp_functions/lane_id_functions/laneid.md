# laneid

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T03:58:46.820Z pushedAt=2026-10-27T09:21:55.755Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/lane_id_functions/laneid_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Obtains the index of a thread within its warp.

## Prototype

```cpp
int32_t laneid()
```

## Parameters

None

## Return Value

Returns the index of a thread within its warp. The number of threads in a warp is fixed at 32, so the Lane ID ranges from 0 to 31.

## Constraints

None

## Examples

The following example uses `laneid()` to compute the sum of all input data and assigns the result to position 0 of the output data. The example uses [asc_reduce_add](../Warp_reduce_functions/asc_reduce_add.md), which requires the `simt_api/device_warp_functions.h` header file, and [asc_atomic_add](../../atomic_operations/asc_atomic_add.md), which requires the `simt_api/device_atomic_functions.h` header file.

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"
    #include "simt_api/device_atomic_functions.h"

    __global__ __launch_bounds__(1024) void kernel_laneid(int32_t* dst, int32_t* src, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        int32_t result = asc_reduce_add(src[idx]);
        if (laneid() == 0) {
            asc_atomic_add(dst, result);
        }
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"
    #include "simt_api/device_warp_functions.h"
    #include "simt_api/device_atomic_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_laneid(__gm__ int32_t* dst, __gm__ int32_t* src, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
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
