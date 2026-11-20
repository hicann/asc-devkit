# \_\_float2int\_rz

<!-- md-trans-meta sourceCommit=71583672fcec81fb360e34fa8e07523eec804851 translatedAt=2026-10-26T06:49:25.887Z pushedAt=2026-10-26T11:03:37.198Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rz_res.md#id1 -->

## Description

The header file path is `"simt_api/device_functions.h"`.

Converts a floating-point number to a signed integer following the CAST\_TRUNC mode and returns the converted value.

## Prototype

```cpp
inline int __float2int_rz(const float x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

The input is converted to a signed integer following the CAST_TRUNC mode. Special cases are as follows:

| x Value | Return Value |
|---|---|
| ±0 | 0 |
| nan | 0 |
| inf | 2147483647 |
| -inf | -2147483648 |
| Positive value exceeding the INT_MAX range | 2147483647 |
| Negative value below the INT_MIN range | -2147483648 |

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel__float2int_rz(int32_t* dst, float* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __float2int_rz(x[idx]);
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__float2int_rz(__gm__ int32_t* dst, __gm__ float* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __float2int_rz(x[idx]);
    }

    __global__ __vector__ void run_kernel__float2int_rz(__gm__ int32_t* dst, __gm__ float* x, uint32_t total_length)
    {
        asc_vf_call<kernel__float2int_rz>(dim3(256), dst, x, total_length);
    }
    ```

The input and output examples are as follows:

```text
x: -6.5, -1.75, 1.75, 6.5, inf, -inf, nan
result: -6, -1, 1, 6, 2147483647, -2147483648, 0
```
