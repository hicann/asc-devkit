# nanf

<!-- md-trans-meta sourceCommit=e80e2531aba21d60ee54c52d2ae137dd3bc302e2 translatedAt=2026-10-26T08:12:26.374Z pushedAt=2026-10-26T11:03:37.372Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/float_math_functions/nanf_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Generates a quiet NaN value of the float type based on the input string (with the most significant bit of the mantissa set to 1), and writes the low 22 bits of the parsed value of `tagp` into the low 22 bits of the NaN value.

![](../../../figures/nanf.png)

## Prototype

```cpp
inline float nanf(const char* tagp)
```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| tagp | Input | Starting address of the source operand string. Supports decimal, octal, and hexadecimal formats. If the string cannot be parsed in the supported base, it is treated as 0. |

## Return Value

Returns a quiet NaN value of the float type. The low 22 bits of the return value are the low 22 bits of the parsed value of `tagp`. If the `tagp` string cannot be parsed in the supported base, the low 22 bits are 0. If the parsed value of `tagp` overflows, the low 22 bits are all 1s.

## Constraints

None

## Examples

- SIMT programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(1024) void kernel_nanf(float* dst)
    {
        dst[0] = nanf("0x1A");
        dst[1] = nanf("123");
        dst[2] = nanf("abc");
        dst[3] = nanf("0");
    }
    ```

- SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_nanf(__gm__ float* dst)
    {
        dst[0] = nanf("0x1A");
        dst[1] = nanf("123");
        dst[2] = nanf("abc");
        dst[3] = nanf("0");
    }
    ```

The input and output examples are as follows:

```text
tagp: "0x1A", "123", "abc", "0"
result: nan(0x7fc0001a), nan(0x7fc0007b), nan(0x7fc00000), nan(0x7fc00000)
```
