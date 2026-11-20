# asc\_any

<!-- md-trans-meta sourceCommit=56b07178c51cb72b1f00ec589e17bd88b63f2540 translatedAt=2026-10-27T04:14:55.374Z pushedAt=2026-10-27T09:21:55.791Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_vote_functions/asc_any_res.md#id1 -->

## Description

The header file path is `"simt_api/device_warp_functions.h"`.

Determines whether any active thread has a non-zero input.

After all active threads in a warp execute this API, the input operand `predicate` of all active threads is evaluated. If the `predicate` of all active threads is 0, 0 is returned; otherwise, 1 is returned. All active threads in the warp return the same result.

## Prototype

```cpp
inline int32_t asc_any(int32_t predicate)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| predicate | Input | Operand. |

## Return Value

Returns 0 if the inputs of all active threads in the warp are 0; otherwise, returns 1.

## Constraints

None

## Examples

The following example checks whether an input matching the target value exists in the current warp, and Lane 0 writes the result to GM.

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelAny(
        const int32_t* input, int32_t* match_flags, uint64_t total_length, int32_t target)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        int32_t has_match = asc_any(input[idx] == target);
        if (lane_id == 0) {
            match_flags[warp_id] = has_match;
        }
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelAny(
        __gm__ const int32_t* input, __gm__ int32_t* match_flags, uint64_t total_length, int32_t target)
    {
        // asc_vf_call parameter: dim3{1024, 1, 1}
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        int32_t has_match = asc_any(input[idx] == target);
        if (lane_id == 0) {
            match_flags[warp_id] = has_match;
        }
    }
    ```

The input and output examples are as follows:

```text
input: 0, 1, 2, 3, 14, 15, 16, 17
target: 3
match_flags: 1
```
