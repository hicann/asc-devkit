# asc\_reduce\_max

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:05:51.120Z pushedAt=2026-10-27T09:21:55.774Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_max_res.md#id1 -->

## Description

The header file path is `"simt_api/device_warp_functions.h"` (for APIs other than the half type) and `"simt_api/asc_fp16.h"` (for the half type API).

Computes the maximum of the val inputs of all active threads in the Warp. All active threads in the Warp return the same result.

## Prototype

```cpp
inline int32_t asc_reduce_max(int32_t val)
```

```cpp
inline uint32_t asc_reduce_max(uint32_t val)
```

```cpp
inline float asc_reduce_max(float val)
```

```cpp
inline half asc_reduce_max(half val)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| val | Input | Source operand. |

## Return Value

Returns the maximum of the val inputs of all active threads in the Warp.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelReduceMax(int32_t* dst)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t result = asc_reduce_max(laneId); // The return value is 31.
        dst[idx] = result;
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelReduceMax(__gm__ int32_t* dst)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        int32_t laneId = idx % 32;
        int32_t result = asc_reduce_max(laneId); // The return value is 31.
        dst[idx] = result;
    }
    ```

The input and output examples are as follows:

```text
dst: 31
```

