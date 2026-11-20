# \_\_ushort\_as\_half

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T13:37:02.257Z pushedAt=2026-10-27T02:14:54.122Z nl=2 -->

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

The header file path is: `"simt_api/asc_fp16.h"`.

Reinterprets the bits of an **unsigned short int** value as a **half** value, that is, reads the bits stored in the **unsigned short int** value according to the **half** format.

## Prototype

```cpp
inline half __ushort_as_half(const unsigned short int x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns the half value obtained by reinterpreting the bits of the unsigned short int data. The special values are as follows:

| x Value                             | Return Value  |
|--------------------------------|------|
| 0                              | 0    |
| 31744(0x7C00)                  | inf  |
| 64512(0xFC00)                  | -inf |
| 31745(0x7C01) ~ 32767(0x7FFFF) | nan  |
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

-   SIMD and SIMT mixed programming scenario:

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

The input and output examples are as follows:

```text
x: 0, 32768, 1, 15360, 48128, 31744, 64512, 32256
result: 0.0, -0.0, 5.96046448e-08, 1.0, -1.0, inf, -inf, nan
```
