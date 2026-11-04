# hrcp

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-16T12:00:05.200Z pushedAt=2026-10-20T03:02:40.299Z -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hrcp_res.md#id1 -->

## Description

The header file path is `"simt_api/asc_bf16.h"`.

Obtains the reciprocal of the input data x.

![](../../../../figures/zh-cn_formulaimage_0000002545900870.png)

## Prototype

```cpp
inline bfloat16_t hrcp(bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand, input data. |

## Return Value

Returns the reciprocal of the input data. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Return Value in Non-Saturation Mode | Return Value in Saturation Mode |
| --- | --- | --- |
| 0 | inf | ASCRT_MAX_NORMAL_BF16 |
| -0 | -inf | -ASCRT_MAX_NORMAL_BF16 |
| inf | 0 | 0 |
| -inf | -0 | -0 |
| nan | nan | 0 |

## Constraints

<!-- npu="950" id7 -->
For the Ascend 950PR&950DT products, this API does not support the Subnormal scenario: the internal implementation of this API uses the division operator. Because the division operator does not support the Subnormal scenario, when the input x is Subnormal data, the final result of this API is ±inf. When the computation result falls within the Subnormal range (for example, when x is a large normal number), the output is flushed to a signed zero.
<!-- end id7 -->

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void kernel_rcp(bfloat16_t* dst, bfloat16_t* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = hrcp(x[idx]);
    }
    ```

- SIMD and SIMT hybrid programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_rcp(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = hrcp(x[idx]);
    }
    ```
