# \_\_hisinf

<!-- md-trans-meta sourceCommit=1deaafd74cfce7bb2c24fdeae01adde70629d44c translatedAt=2026-10-26T11:47:12.112Z pushedAt=2026-10-27T02:14:53.849Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_compare_functions/__hisinf_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_fp16.h"`.

Determines whether a floating-point number is infinity.

## Prototype

```cpp
inline bool __hisinf(half x)
```

## Parameters

**Table 1**  Parameters

| Parameter name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Returns whether the input is infinity. Returns false for a non-infinity input and true for ±inf.

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void KernelIsInf(bool* dst, half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hisinf(x[idx]);
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelIsInf(__gm__ bool* dst, __gm__ half* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __hisinf(x[idx]);
    }
    ```

The input and output examples are as follows:

```text
x: -1.25, inf, -inf, nan
result: false, true, true, false
```
