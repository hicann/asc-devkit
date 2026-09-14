# \_\_float22hif82\_rna\_sat

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-01T10:43:22.001Z pushedAt=2026-09-03T07:28:07.240Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In saturation mode, converts the two components of **float2** type data to **hifloat8\_t** precision following the CAST\_ROUND mode, and returns the converted **hifloat8x2\_t** type data.

## Function Prototype

```
inline hifloat8x2_t __float22hif82_rna_sat(const float2 x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the hifloat8x2\_t data converted from the two input components in saturation mode using the CAST\_ROUND mode.

## Constraints

Before using this API, set the CTRL\[60\] register to 0; otherwise, the saturation mode does not take effect. For details about how to set it, see [Methods for Controlling Saturation Behavior](../../data_type_conversion/overview_258.md#methods-for-controlling-saturation-behavior).

This API is not supported in SIMT programming scenarios.

## Header Files to Include

To use this API, include the "simt\_api/asc\_fp8.h" header file.

```
#include "simt_api/asc_fp8.h"
```

## Example

-   SIMD and SIMT hybrid programming scenario:

    ```
    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_float22hif82_rna_sat(__gm__ float2* input, __gm__ hifloat8x2_t* output, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one **float2** type data item, that is, two **float** type data items. Therefore, threads with idx equal to or greater than half the input_total_length do not process data.
        if (idx > input_total_length /2) {
            return;
        }
        output[idx] = __float22hif82_rna_sat(input[idx]);
    }
    __global__ __vector__ void cast_kernel(__gm__ float* input, __gm__ uint8_t* output, uint32_t input_total_length)
    {
        asc_vf_call<simt_float22hif82_rna_sat>(dim3(1024), (__gm__ float2*)input, (__gm__ hifloat8x2_t*)output, input_total_length);
    }
    ```

