# \_\_float22bfloat162\_rn\_sat

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-29T07:44:08.480Z pushedAt=2026-08-29T09:42:03.805Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In saturation mode, converts the two components of **float2** type data to **bfloat16** precision following the **CAST_RINT** mode, and returns the converted **bfloat16x2_t** type data.

## Function Prototype

```
inline bfloat16x2_t __float22bfloat162_rn_sat(const float2 x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the **bfloat16x2_t** type data obtained by converting the two input components to **bfloat16** precision following the **CAST_RINT** mode in saturation mode.

## Constraints

Before using this API, set the **CTRL[60]** register to 0; otherwise, the saturation mode does not take effect. For the setting method, see [Methods for Controlling Saturation Behavior](../../data_type_conversion/overview_258.md#methods-for-controlling-saturation-behavior).

In SIMT programming scenarios, the **CTRL** register cannot be set, so the saturation mode of this API does not take effect.

## Header Files to Include

To use this API, include the "simt\_api/asc\_bf16.h" header file.

```
#include "simt_api/asc_bf16.h"
```

## Example

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_float22bfloat162_rn_sat(__gm__ float2* input, __gm__ bfloat16x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one **float2** type data item, that is, two **float** type data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        output[idx] = __float22bfloat162_rn_sat(input[idx]);
    }

    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ bfloat16_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_float22bfloat162_rn_sat>(dim3(1024), (__gm__ float2*)input, (__gm__ bfloat16x2_t*)output, input_total_length);
    }
    ```

