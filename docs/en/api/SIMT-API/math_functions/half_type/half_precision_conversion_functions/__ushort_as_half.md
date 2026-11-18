# \_\_ushort\_as\_half

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-21T08:44:52.100Z pushedAt=2026-10-22T00:53:52.502Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ushort_as_half_res.md#id1 -->

## Description

The header file path is `"simt_api/asc_fp16.h"`.

Reinterprets the bits of an **unsigned short int** value as **half**, that is, reads the bits stored in the **unsigned short int** value according to the **half** format.

## Prototype

```cpp
inline half __ushort_as_half(const unsigned short int x)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the value obtained by reinterpreting the bits of the **unsigned short int** value as **half**. Special values are as follows:

| x Value                             | Return Value  |
|--------------------------------|------|
| 0                              | 0    |
| 31744(0x7C00)                  | inf  |
| 64512(0xFC00)                  | -inf |
| 31745(0x7C01) ~ 32767(0x7FFF) | nan  |
| 64513(0xFC01) ~ 65535(0xFFFF)  | nan  |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void kernel__ushort_as_half(half * dst, unsigned short int* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __ushort_as_half(x[idx]);
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__ushort_as_half(__gm__ half * dst, __gm__ unsigned short int* x, uint32_t input_total_length)
    {
        uint32_t idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= input_total_length) {
            return;
        }
        dst[idx] = __ushort_as_half(x[idx]);
    }
    ```
