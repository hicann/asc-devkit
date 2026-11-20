# asc\_all

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:13:54.422Z pushedAt=2026-10-27T09:21:55.789Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_vote_functions/asc_all_res.md#id1 -->

## Description

The header file path is `"simt_api/device_warp_functions.h"`.

Determines whether the inputs of all active threads are non-zero.

After all active threads in a warp execute this API, the input operand `predicate` of each active thread is checked. If the `predicate` of every active thread is non-zero, 1 is returned; otherwise, 0 is returned. All active threads in the warp return the same result.

## Prototype

```cpp
inline int32_t asc_all(int32_t predicate)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| predicate | Input | Operand. |

## Return Value

Returns 1 if the input of every active thread in the warp is non-zero; otherwise, returns 0.

## Constraints

None

## Examples

The following example compares two sets of input data, and Lane 0 writes out whether the data handled by the current warp are all equal.

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelAll(
        const int32_t* input1, const int32_t* input2, int32_t* equal_flags, uint64_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        int32_t all_equal = asc_all(input1[idx] == input2[idx]);
        if (lane_id == 0) {
            equal_flags[warp_id] = all_equal;
        }
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelAll(
        __gm__ const int32_t* input1, __gm__ const int32_t* input2, __gm__ int32_t* equal_flags,
        uint64_t total_length)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        int32_t all_equal = asc_all(input1[idx] == input2[idx]);
        if (lane_id == 0) {
            equal_flags[warp_id] = all_equal;
        }
    }
    ```

The input and output examples are as follows:

```text
input: 0, 1, 2, 3, 14, 15, 16, 17
input2: 0, 1, 2, 3, 14, 15, 16, 17
equal_flags: 1
```
