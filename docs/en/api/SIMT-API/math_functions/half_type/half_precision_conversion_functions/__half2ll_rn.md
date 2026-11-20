# \_\_half2ll\_rn

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T12:52:37.464Z pushedAt=2026-10-27T02:14:54.008Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_rn_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Converts half type data to long long type data following the CAST\_RINT mode, and returns the converted value.

## Prototype

```cpp
inline long long int __half2ll_rn(const half x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the long long type data converted from the input following the CAST\_RINT mode. The special values are as follows:

| x Value | Return Value |
| --- | --- |
| ±0 | 0 |
| nan | 0 |
| inf | 9223372036854775807 (maximum long long value) |
| -inf | -9223372036854775808 (minimum long long value) |
| ASCRT\_MAX\_NORMAL\_FP16 | 65504 |
| -ASCRT\_MAX\_NORMAL\_FP16 | -65504 |
| ±0.5 | 0 |
| 2.5 | 2 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void kernel__half2ll_rn(int64_t* dst, half* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __half2ll_rn(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__half2ll_rn(__gm__ int64_t* dst, __gm__ half* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __half2ll_rn(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -2.5, -1.5, 0.5, 1.5, 2.5, inf, -inf, nan
result: -2, -2, 0, 2, 2, 9223372036854775807, -9223372036854775808, 0
```
