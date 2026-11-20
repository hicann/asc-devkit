# asc_uintdiv

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:42:32.938Z pushedAt=2026-10-27T02:14:54.312Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

The integer fast division algorithm computes the quotient `quotient` in two steps. In the first step, `magic` and `shift` are precomputed based on the fixed divisor, which can be done by the companion precomputation API [asc_get_uintdiv_magic_and_shift](asc_get_uintdiv_magic_and_shift.md). In the second step, this API is called to compute the quotient based on the dividend `dividend`, `magic`, and `shift`. The calculation formula is as follows:

$$
quotient = \left(\left\lfloor\frac{dividend \times magic}{2^N}\right\rfloor + dividend\right) \gg shift
$$

Depending on the data type of the dividend, the value of $N$ in the formula differs: when the dividend is of the `uint32_t` data type, $N$ is 32; when the dividend is of the `uint64_t` data type, $N$ is 64.

The integer fast division algorithm can be used for performance optimization and is suitable for scenarios where one divisor is used for multiple division operations.

> [!NOTE] Note
>
> Usage of the integer fast division algorithm:
> 1. In the scalar computation logic, call the [asc_get_uintdiv_magic_and_shift](asc_get_uintdiv_magic_and_shift.md) API to compute the `magic` and `shift` values of the divisor.
> 2. In the SIMT VF computation logic, call the `asc_uintdiv` API to perform the division operation for the specified dividend.

## Prototype

- The dividend is of the `uint32_t` type

    ```cpp
    inline uint32_t asc_uintdiv(uint32_t dividend, uint32_t magic, uint32_t shift)
    ```

- The dividend is of the `uint64_t` type

    ```cpp
    inline uint64_t asc_uintdiv(uint64_t dividend, uint64_t magic, uint64_t shift)
    ```

## Parameters

**Table 1**  Parameters

| Name | Input/Output | Description |
| --- | --- | --- |
| dividend | Input | Dividend. |
| magic | Input | Multiplication factor in the fast division, precomputed based on the specified divisor. |
| shift | Input | Shift amount in the fast division, precomputed based on the specified divisor. |

## Return Value

Returns the quotient obtained by dividing `dividend` by the divisor corresponding to `magic` and `shift`.

## Constraints

- When the dividend is of the `uint32_t` data type, the maximum supported dividend is `INT32_MAX`, that is, 2^31-1; when the dividend is of the `uint64_t` data type, the maximum supported dividend is `INT64_MAX`, that is, 2^63-1.
- `magic` and `shift` must correspond to the same non-zero divisor; the maximum divisors for `uint32_t` data and `uint64_t` data are `INT32_MAX` and `INT64_MAX`, respectively.
- This API is not supported in SIMT programming scenarios.

## Examples

For the complete sample, see [SIMD and SIMT Hybrid Programming for Fast Division Operator Sample](../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_integer_fast_div/README.md).

-   SIMD and SIMT mixed programming scenario:

    ```cpp
    #include "simt_api/asc_simt.h"
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void simt_fast_div(
        __gm__ uint32_t* output, uint32_t value, uint32_t magic, uint32_t shift, uint32_t total_length)
    {
        uint32_t idx = threadIdx.x;
        if (idx >= total_length) {
            return;
        }
        output[idx] = asc_uintdiv(value, magic, shift);
    }

    __global__ __vector__ void integer_div_kernel(
        __gm__ uint32_t* output, uint32_t value, uint32_t divisor, uint32_t total_length)
    {
        // Precompute magic and shift based on the divisor in the scalar computation logic.
        uint32_t magic = 0;
        uint32_t shift = 0;
        asc_get_uintdiv_magic_and_shift(&magic, &shift, divisor);
        asc_vf_call<simt_fast_div>(dim3(1024), output, value, magic, shift, total_length);
    }
    ```

The input and output examples are as follows:

```text
value: 100, 256, 1024, 2048
divisor: 5, 6, 7, 8
result: 20, 42, 146, 256
```
