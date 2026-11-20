# asc_get_uintdiv_magic_and_shift

<!-- md-trans-meta sourceCommit=5a612c9c4dbf0f1c9dfd3ccb3b7c6edb3aaab4e0 translatedAt=2026-10-26T14:42:01.988Z pushedAt=2026-10-27T02:14:54.310Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift_res.md#id1 -->

## Description

The header file path is `"simt_api/math_functions.h"`.

Integer fast division is used for fast computation in scenarios with the same divisor. The algorithm computes the quotient in two steps. This API is used for the first step, which obtains the multiplication factor `magic` and the shift amount `shift`. The generated parameters are used by the SIMT VF fast division API [asc_uintdiv](asc_uintdiv.md). The computation process of this API is as follows:

$$
\begin{aligned}
shift &= \left\lceil \log_2 divisor \right\rceil \\
magic &= \left\lfloor 2^N \times \frac{2^{shift} - divisor}{divisor} \right\rfloor + 1
\end{aligned}
$$

Depending on the divisor data type, the value of $N$ in the formula differs: when the divisor is of the `uint32_t` data type, $N$ is 32; when the divisor is of the `uint64_t` data type, $N$ is 64.

The integer fast division algorithm can be used for performance optimization and is suitable for scenarios where one divisor is used in multiple division computations.

> [!NOTE] Note
>
> Usage of the integer fast division algorithm:
> 1. In the scalar computation logic, call the `asc_get_uintdiv_magic_and_shift` API to compute the `magic` and `shift` values of the divisor.
> 2. In the SIMT VF computation logic, call the [asc_uintdiv](asc_uintdiv.md) API to perform the division operation for the specified dividend.

> [!CAUTION] Caution
>
> This API cannot be called in SIMT VF. It can only be called in the scalar computation logic.

## Prototype

- The divisor is of the `uint32_t` type

    ```cpp
    inline void asc_get_uintdiv_magic_and_shift(uint32_t* magic, uint32_t* shift, uint32_t divisor)
    ```

- The divisor is of the `uint64_t` type

    ```cpp
    inline void asc_get_uintdiv_magic_and_shift(uint64_t* magic, uint64_t* shift, uint64_t divisor)
    ```

## Parameters

**Table 1**  Parameters

| parameter name | Input/Output | Description |
| --- | --- | --- |
| magic | Output | Multiplication factor in fast division computed from the specified divisor. |
| shift | Output | Shift amount in fast division computed from the specified divisor. |
| divisor | Input | Divisor. |

## Return Value

None

## Constraints

- When the divisor is of the `uint32_t` data type, the maximum supported divisor is `INT32_MAX`, that is, 2^31-1. When the divisor is of the `uint64_t` data type, the maximum supported divisor is `INT64_MAX`, that is, 2^63-1.
- A divisor of 0 is not supported; otherwise, the computation result is undefined.
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
        // In the scalar computation logic, precompute magic and shift based on the divisor.
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
