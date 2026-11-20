# \_\_half2uint\_ru

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T13:01:11.591Z pushedAt=2026-10-27T02:14:54.027Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_ru_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Converts half data to unsigned int data following the CAST\_CEIL mode and returns the converted value.

## Prototype

```cpp
inline unsigned int __half2uint_ru(const half x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the unsigned integer converted from the input following the CAST\_CEIL mode. Special scenarios are described as follows:

| x Value | Return Value |
| --- | --- |
| ±0 | 0 |
| nan | 0 |
| inf | 4294967295 (maximum value of uint32_t) |
| -inf | 0 |
| ASCRT\_MAX\_NORMAL\_FP16 | 65504 |
| -ASCRT\_MAX\_NORMAL\_FP16 | 0 |
| ASCRT\_MIN\_DENORM\_FP16 | 1 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void kernel__half2uint_ru(uint32_t* dst, half* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __half2uint_ru(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__half2uint_ru(__gm__ uint32_t* dst, __gm__ half* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __half2uint_ru(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -2.5, -1.5, 0.5, 1.5, 2.5, inf, -inf, nan
result: 0, 0, 1, 2, 3, 4294967295, 0, 0
```
