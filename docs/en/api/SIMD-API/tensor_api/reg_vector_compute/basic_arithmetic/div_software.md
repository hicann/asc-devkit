# div

<!-- md-trans-meta sourceCommit=87e50283487785a7cbfce86e2c6c5d46a6bd9987 translatedAt=2026-11-19T02:44:55.434Z pushedAt=2026-11-19T05:54:06.659Z nl=2 -->

## Supported Products

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

Header file path: `"tensor_api/experimental/arch/vector/basic_arithmetic.h"`.

This API performs element-wise division on the two source operands according to the mask carried by src0, and selects the division precision algorithm through compile-time configuration. The calculation formula is as follows:

$$
dst_i = \frac{src0_i}{src1_i}
$$

`operator/` directly uses the hardware division instruction and does not provide precision mode or algorithm configuration; `div` supports configuring the division precision and algorithm through template parameters, and handles Subnormal according to the selected configuration.

## Prototype

```cpp
enum class div_algo {
    INTRINSIC,
    PRECISION_0ULP_FTZ_TRUE,
    DIFF_COMPENSATION,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_0ULP_FTZ_FALSE,
    PRECISION_1ULP_FTZ_FALSE
};

template <bool precisionMode = false, div_algo algo = div_algo::INTRINSIC, typename T>
__simd_callee__ inline reg_tensor<T> div(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| precisionMode | Whether to enable the high-precision division path. When `true`, the difference compensation algorithm is used, and only `float` is supported. For precision details, see [Key Features](#key-features). |
| algo | Division algorithm, supporting the enum values listed above. For the relationship between the algorithm and the Subnormal mode, see [Key Features](#key-features). |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0 | Input | Dividend, of type `reg_tensor<T>`. src0.reg stores the vector data, and src0.mask controls the valid elements. |
| src1 | Input | Divisor, of type `reg_tensor<T>`. src1.mask does not participate in this calculation. |

## Data Type

| algo | Supported data types when precisionMode=false |
| --- | --- |
| `INTRINSIC`, `PRECISION_1ULP_FTZ_TRUE` | int16_t, uint16_t, half, int32_t, uint32_t, float |
| `PRECISION_1ULP_FTZ_FALSE` | half, float |
| `DIFF_COMPENSATION`, `PRECISION_0ULP_FTZ_TRUE`, `PRECISION_0ULP_FTZ_FALSE` | float |

When `precisionMode=true`, all algorithms support only float. An invalid data type or algorithm combination causes a compilation error.

## Return Value

Returns the element-wise division result, of type `reg_tensor<T>`. The return value mask is the same as src0.mask; when the corresponding position of src0.mask is 0, the corresponding element of the return value is set to zero.

## Constraints

- `src0.mask` specifies the valid elements that participate in the calculation. Before calling the API, set it in advance via `with_mask`; elements with a mask of 0 do not participate in the calculation, and the corresponding output elements are set to zero.
- Whether `INTRINSIC` and `DIFF_COMPENSATION` retain Subnormal is determined by `--cce-ftz`; high precision mode does not represent automatically disabling FTZ. For details, see [Configuring Subnormal Mode](#configuring-subnormal-mode).
- When using integer types, the divisor cannot be 0, and the result is the quotient obtained by element-wise integer division.
- When using floating-point types, if the divisor is `0.0` or `-0.0` and the dividend is a non-zero finite value, the result is `inf` with the corresponding sign; if both the dividend and the divisor are zero, the result is `nan`.
- When using floating-point types, if the dividend or the divisor is `nan`, the result is `nan`; if a finite value is divided by `inf` or `-inf`, the result is zero with the corresponding sign; if `inf` or `-inf` is divided by a finite non-zero value, the result is `inf` with the corresponding sign; if `inf` or `-inf` is divided by `inf` or `-inf`, the result is `nan`.
- When a floating-point operation result overflows, `inf` with the corresponding sign is returned; when the result underflows to Subnormal, it is handled according to the selected algorithm and the `--cce-ftz` configuration.

## Key Features

### Maximum Precision Error

For floating-point types, the supported configurations and maximum precision errors are as follows. Integer types do not involve floating-point precision errors or Subnormal handling.

| precisionMode | algo | Supported floating-point types | Maximum precision error |
| --- | --- | --- | --- |
| `false` (default value) | `div_algo::INTRINSIC` (default value) | half, float | 1 ULP |
| `false` | `div_algo::PRECISION_1ULP_FTZ_TRUE`, `div_algo::PRECISION_1ULP_FTZ_FALSE` | half, float | 1 ULP |
| `false` | `div_algo::DIFF_COMPENSATION`, `div_algo::PRECISION_0ULP_FTZ_TRUE`, `div_algo::PRECISION_0ULP_FTZ_FALSE` | float | 0 ULP |
| `true` | All supported algorithms | float | 0 ULP |

When `precisionMode=true`, division precision is improved via difference compensation, but this does not represent automatically retaining Subnormal. The Subnormal handling is determined jointly by `algo` and the compilation option.

### Configuring Subnormal Mode

FTZ (Flush-To-Zero) is a floating-point operation mode in which, when the result is [Subnormal](../../../basic_api/data_structures/builtin_data_types.md#p7381131713310), it is directly cleared to zero (approximated as 0) instead of retaining its exact tiny value. The precision errors above should be understood in combination with the selected Subnormal mode; 0 ULP does not represent that Subnormal results are still retained in FTZ mode.

#### Default Algorithm

`div_algo::INTRINSIC` is the default algorithm, and the Subnormal handling for floating-point types is controlled by the compilation option `--cce-ftz`:

- `--cce-ftz=true` (default value): Adopts FTZ mode.
- `--cce-ftz=false`: Supports Subnormal data calculation and retains Subnormal results.

The rules above apply to both the default precision and high precision configurations. The difference between the two lies in whether difference compensation is enabled, not in whether FTZ is enabled.

#### Explicitly Specified Algorithm

The following table lists the floating-point configurations under different combinations of `precisionMode`, `algo`, and the `--cce-ftz` compilation option. In the table, the `div_algo::` prefix is omitted from the enum values, and ULP represents the maximum precision error under the corresponding Subnormal mode.

| precisionMode | algo | Supported floating-point types | `--cce-ftz=true` (default value): Subnormal handling and maximum precision error | `--cce-ftz=false`: Subnormal handling and maximum precision error |
| --- | --- | --- | --- | --- |
| `false` | `INTRINSIC` (default value) | half, float | FTZ, 1 ULP | Retain Subnormal, 1 ULP |
| `false` | `DIFF_COMPENSATION` | float | FTZ, 0 ULP | Retain Subnormal, 0 ULP |
| `false` | `PRECISION_0ULP_FTZ_TRUE` | float | FTZ, 0 ULP | FTZ, 0 ULP |
| `false` | `PRECISION_0ULP_FTZ_FALSE` | float | Retain Subnormal, 0 ULP | Retain Subnormal, 0 ULP |
| `false` | `PRECISION_1ULP_FTZ_TRUE` | half, float | FTZ, 1 ULP | FTZ, 1 ULP |
| `false` | `PRECISION_1ULP_FTZ_FALSE` | half, float | Retain Subnormal, 1 ULP | Retain Subnormal, 1 ULP |
| `true` | `INTRINSIC` (default value) | float | FTZ, 0 ULP | Retain Subnormal, 0 ULP |
| `true` | `DIFF_COMPENSATION` | float | FTZ, 0 ULP | Retain Subnormal, 0 ULP |
| `true` | `PRECISION_0ULP_FTZ_TRUE` | float | FTZ, 0 ULP | FTZ, 0 ULP |
| `true` | `PRECISION_0ULP_FTZ_FALSE` | float | Retain Subnormal, 0 ULP | Retain Subnormal, 0 ULP |
| `true` | `PRECISION_1ULP_FTZ_TRUE` | float | FTZ, 0 ULP | Retain Subnormal, 0 ULP |
| `true` | `PRECISION_1ULP_FTZ_FALSE` | float | FTZ, 0 ULP | Retain Subnormal, 0 ULP |

When `precisionMode=true`, the 0 ULP algorithm is always used. `PRECISION_0ULP_FTZ_TRUE` always adopts the FTZ path, and `PRECISION_0ULP_FTZ_FALSE` always retains Subnormal; the Subnormal handling of the remaining algorithms follows `--cce-ftz`.

`precisionMode` is not an FTZ switch, and setting it to `false` does not represent that only 1 ULP precision can be achieved. When explicit control of FTZ behavior is required, it is recommended to set `precisionMode=false` and select the precision and FTZ strategy simultaneously via `algo`, to avoid the high precision mode overriding the FTZ semantics of the explicit 1 ULP algorithm. The precision and FTZ strategy of the explicit 0 ULP algorithm are not affected by `precisionMode`.

#### Usage Recommendations

- In general scenarios, div(src0, src1) can be used, which adopts the default 1 ULP precision algorithm, with Subnormal handling following the --cce-ftz compilation option.
- When 0 ULP precision is required and Subnormal handling follows the compilation option, use `div<true>(src0, src1)`.
- When 0 ULP precision is required and FTZ mode is always adopted, use `div<true, div_algo::PRECISION_0ULP_FTZ_TRUE>(src0, src1)`.
- When Subnormal results need to be retained and do not depend on the compilation option, use `div<false, div_algo::PRECISION_1ULP_FTZ_FALSE>(src0, src1)`; if 0 ULP precision is also required, use `div<false, div_algo::PRECISION_0ULP_FTZ_FALSE>(src0, src1)`.

## Example

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"

using asc::te::experimental::div;
using asc::te::experimental::div_algo;

auto default_result = div(src0, src1);
auto precise_result = div<true>(src0, src1);
auto ftz_result = div<true, div_algo::PRECISION_0ULP_FTZ_TRUE>(src0, src1);
auto compensation_result = div<false, div_algo::DIFF_COMPENSATION>(src0, src1);
auto explicit_ftz_result = div<false, div_algo::PRECISION_1ULP_FTZ_TRUE>(src0, src1);
auto subnormal_result = div<false, div_algo::PRECISION_1ULP_FTZ_FALSE>(src0, src1);
auto precise_subnormal_result = div<false, div_algo::PRECISION_0ULP_FTZ_FALSE>(src0, src1);
```
