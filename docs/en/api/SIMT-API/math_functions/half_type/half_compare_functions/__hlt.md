# \_\_hlt

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T11:49:42.845Z pushedAt=2026-10-27T02:14:53.856Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_compare_functions/__hlt_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Compares two half values and returns true only when the first value is less than the second value.

## Prototype

```cpp
bool __hlt(half x, half y)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |
| y | Input | Source operand. |

## Return Value

Returns the result of comparing whether the first input value is less than the second input value. Returns false when either input is nan.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void kernel_hlt(bool* dst, half* x, half* y, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hlt(x[idx], y[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_hlt(__gm__ bool* dst, __gm__ half* x, __gm__ half* y, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __hlt(x[idx], y[idx]);
    }

    __global__ __vector__ void run_kernel_hlt(__gm__ bool* dst, __gm__ half* x, __gm__ half* y, uint32_t total_length)
    {
        asc_vf_call<kernel_hlt>(dim3(1024), dst, x, y, total_length);
    }
    ```

The input and output examples are as follows:

```text
x: -1.25, 0.5, 2.5, nan
y: -0.5, 0.5, 1.25, nan
result: true, false, false, false
```
