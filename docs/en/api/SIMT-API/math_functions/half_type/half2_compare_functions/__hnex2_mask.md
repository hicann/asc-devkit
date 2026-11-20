# \_\_hnex2\_mask

<!-- md-trans-meta sourceCommit=ccc1820c8c764c94b6e9a6131f4c550662f52b46 translatedAt=2026-10-26T14:16:56.471Z pushedAt=2026-10-27T02:14:54.231Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half2_compare_functions/__hnex2_mask_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Compares the two components of two half2 values and returns the result as an unsigned int, where the lower 16 bits are the mask result of the first component and the upper 16 bits are the mask result of the second component. If the components are not equal, the corresponding 16-bit mask is 0xFFFF; otherwise, it is 0x0.

## Prototype

```cpp
unsigned int __hnex2_mask(half2 x, half2 y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the result of comparing whether the corresponding components of the input data are unequal: when the condition is met, the corresponding 16-bit mask result is 0xFFFF; otherwise, the corresponding 16-bit mask result is 0x0. The mask result of each component is as follows:

<table>
  <tr>
    <th>x Component</th>
    <th>y Component</th>
    <th>Return Value (Corresponding Component)</th>
  </tr>
  <tr>
    <td>±0</td>
    <td>±0</td>
    <td>0x0</td>
  </tr>
  <tr>
    <td colspan="2">Any component is nan</td>
    <td>0x0</td>
  </tr>
  <tr>
    <td>±inf</td>
    <td>±inf</td>
    <td>0x0</td>
  </tr>
</table>


## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __global__ __launch_bounds__(1024) void simt_hnex2_mask(half* x, half* y, unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        half2* input1 = (half2*)x;
        half2* input2 = (half2*)y;
        dst[idx] = __hnex2_mask(input1[idx], input2[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    // Use short vectors to improve data transfer efficiency.
    __simt_vf__ __launch_bounds__(1024) inline void simt_hnex2_mask(__gm__ half2* x, __gm__ half2* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Each thread processes one half2 value, that is, two half values. Therefore, threads with idx >= input_total_length / 2 do not process data.
        if (idx >= input_total_length / 2) {
            return;
        }
        dst[idx] = __hnex2_mask(x[idx], y[idx]);
    }

    __global__ __vector__ void compare_kernel(__gm__ half* x, __gm__ half* y, __gm__ unsigned int* dst, uint32_t input_total_length)
    {
        asc_vf_call<simt_hnex2_mask>(dim3(1024), (__gm__ half2*)x, (__gm__ half2*)y, dst, input_total_length);
    }
    ```

The input and output examples are as follows:

```text
x: (-1, 0), (1, -1), (inf, -inf), (nan, 0.5)
y: (1, 0), (1, -2), (inf, inf), (0.5, nan)
result: 65535, 4294901760, 4294901760, 0
```
