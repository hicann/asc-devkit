# hsin

<!-- md-trans-meta sourceCommit=0d09b47b5c2b1e70c4f1a697ef5ab035754d5e54 translatedAt=2026-10-23T13:33:16.585Z pushedAt=2026-10-26T06:23:22.785Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hsin_res.md#id1 -->

## Description

The header file path is: `"simt_api/asc_bf16.h"`.

Obtains the sine value of the input data.

![](../../../../figures/formulaimage_0000002576421051.png)

## Prototype

```cpp
inline bfloat16_t hsin(bfloat16_t x)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| x | Input | Source operand. |

## Return Value

Sine value of the input data. This API is affected by the global saturation mode. The special values are as follows:

| x Value | Return Value in Non-saturation Mode  | Return Value in Saturation Mode |
| --- | --- | --- |
| inf,-inf,nan | nan | 0 |

## Constraints

None

## Examples

-   SIMT programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __global__ __launch_bounds__(1024) void KernelSin(bfloat16_t* dst, bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hsin(x[idx]); // Compute the sine of the idx-th element at the src source address.
    }
    ```

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_bf16.h"

    __simt_vf__ __launch_bounds__(1024) inline void KernelSin(__gm__ bfloat16_t* dst, __gm__ bfloat16_t* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = hsin(x[idx]); // Compute the sine of the idx-th element at the src source address.
    }
    ```

The input and output examples are as follows:

```text
x: 0.0, 0.5, -0.5, 1.0, -1.0, 2.0, -2.0, 3.0
result: 0.0, 0.478516, -0.478516, 0.839844, -0.839844, 0.910156, -0.910156, 0.141602
```
