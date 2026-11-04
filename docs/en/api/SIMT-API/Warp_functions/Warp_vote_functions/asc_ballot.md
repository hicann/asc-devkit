# asc\_ballot

<!-- md-trans-meta sourceCommit=343a6e89d2704a866eb5b4d939aacaa8ebe1820b translatedAt=2026-10-27T10:04:56.658Z pushedAt=2026-10-27T11:36:23.285Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/Warp_functions/Warp_vote_functions/asc_ballot_res.md#id1 -->

## Description

The header file path is `"simt_api/device_warp_functions.h"`.

Determines whether the input of each active thread in the Warp is non-zero.

After all active threads in the Warp execute this API, the input operand predicate of each active thread is evaluated, and a 32-bit unsigned integer is returned. If the predicate input by an active thread in the Warp is non-zero, the bit corresponding to the thread's Lane ID in the return value is set to 1; otherwise, it is set to 0. All active threads in the Warp return the same result.

## Prototype

```cpp
inline uint32_t asc_ballot(int32_t predicate)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| predicate | Input | Operand. |

## Return Value

Returns a 32-bit unsigned integer. If the predicate input by an active thread in the Warp is non-zero, the bit corresponding to the thread's Lane ID in the return value is set to 1; otherwise, it is set to 0.

## Constraints

None

## Examples

The following example uses a mask to record the threads in the current Warp whose input data exceeds the threshold, and Lane 0 writes the mask to GM.

For the complete example, see [Sobel edge detection example](../../../../../../examples/03_simt_api/02_features/01_api_features/03_warp_instruction/sobel_warp_shfl/README.md).

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __global__ __launch_bounds__(1024) void KernelBallot(
        const float* input, uint32_t* selected_mask, uint64_t total_length, float threshold)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        uint32_t mask = asc_ballot(input[idx] > threshold);
        if (lane_id == 0) {
            selected_mask[warp_id] = mask; // Each bit indicates whether the data of the corresponding Lane exceeds the threshold.
        }
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```cpp
    #include "simt_api/device_warp_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelBallot(
        __gm__ const float* input, __gm__ uint32_t* selected_mask, uint64_t total_length, float threshold)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }

        uint32_t lane_id = threadIdx.x % warpSize;
        uint32_t warp_id = idx / warpSize;
        uint32_t mask = asc_ballot(input[idx] > threshold);
        if (lane_id == 0) {
            selected_mask[warp_id] = mask; // Each bit indicates whether the data of the corresponding lane exceeds the threshold.
        }
    }
    ```
