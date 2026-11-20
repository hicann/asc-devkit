# Reg Vector Computation API Precision Standard Summary<a name="reg-vector-precision-summary"></a>

<!-- md-trans-meta sourceCommit=a3aa0757c3651219388a144a90057970e743089e translatedAt=2026-11-17T03:39:41.509Z pushedAt=2026-11-17T09:52:41.768Z nl=2 -->

This section summarizes the precision standards for the Reg vector compute APIs of Basic API and C API. The content in this section applies only to the following models:

Ascend 950PR & 950DT products

**Table 1**  Basic API Reg vector compute API precision standards<a id="table-precision-standard"></a>

| API Category | API | Hardware Instruction Precision Standard | Soft Simulation Precision Standard |
| --- | --- | --- | --- |
| Basic Arithmetic | `Exp` | 1ulp, not support denormalized numbers | 1ulp, support denormalized numbers |
| Basic Arithmetic | `Sqrt` | 1ulp, not support denormalized numbers | &bull; float: 0ulp <br>&bull; half/float: 1ulp, support denormalized numbers |
| Basic Arithmetic | `Add` | &bull; half/float: 0ulp, support denormalized numbers<br>&bull; bfloat16_t: Add(x, y) = rint_to_bf16(Add(cast_to_f32(x), cast_to_f32(y))) | 0ulp, support denormalized numbers |
| Basic Arithmetic | `Sub` | Sub(x, y) = Add(x, Neg(y)) | 0ulp, support denormalized numbers |
| Basic Arithmetic | `Adds` | &bull; half/float: 0ulp, support denormalized numbers<br>&bull; bfloat16_t: Add(x, y) = rint_to_bf16(Add(cast_to_f32(x), cast_to_f32(y))) | 0ulp, support denormalized numbers |
| Basic Arithmetic | `Div` | 1ulp, not support denormalized numbers | &bull; complex64: 0ulp, not support denormalized numbers <br>&bull; float: 0ulp, support denormalized numbers <br>&bull; half/float: 1ulp, support denormalized numbers |
| Basic Arithmetic | `Neg` | Sign bit is inverted | - |
| Basic Arithmetic | `Ln/Log` | 1ulp, not support denormalized numbers | 1ulp, support denormalized numbers |
| Basic Arithmetic | `Log2/Log10` | - | &bull; 1ulp, not support denormalized numbers<br>&bull; 1ulp, support denormalized numbers |
| Basic Arithmetic | `Mul` | &bull; half/float: 0ulp, support denormalized numbers<br>&bull; bfloat16_t: Mul(x, y) = rint_to_bf16(Mul(cast_to_f32(x), cast_to_f32(y))) | - |
| Basic Arithmetic | `Muls` | half/float: 0ulp, support denormalized numbers | - |
| compound instruction | `AbsSub` | precision equivalent to subtracting first and then taking the absolute value, AbsSub(x) = Abs(Sub(x)) | - |
| compound instruction | `ExpSub` | - half input, float output:<br>ExpSub(x, y) = Exp(Sub(cast_to_f32(x), cast_to_f32(y)))<br>- float input, float output:<br>ExpSub(x, y) = Exp(Sub(x, y)) | - |
| compound instruction | `Axpy` | Result is equivalent to: multiply-accumulate uses high-precision computation, equivalent to Fma(x, y, z) | - |
| compound instruction | `MulDstAdd` | Result is equivalent to: multiply-accumulate uses high-precision computation, equivalent to Fma(x, y, z) | - |
| compound instruction | `MulAddDst` | Result is equivalent to: multiply-accumulate uses high-precision computation, equivalent to Fma(x, y, z) | - |
| compound instruction | `MulsCast` | MulsCast(x, y) = cast_to_f16&lt;roundingmode&gt;(x * y) | - |
| Reduction Computation | `Reduce` | half/float: 0ulp, uses a pairwise accumulation strategy based on a binary tree structure | - |
| Reduction Computation | `ReduceDataBlock` | half/float: 0ulp, uses a pairwise accumulation strategy based on a binary tree structure | - |
| Reduction Computation | `PairReduceElem` | half/float: 0ulp | - |

**Table 2**  Basic API low-precision instruction support<a id="table-low-precision"></a>

| Data Type | API |
| --- | --- |
| fp4x2_e2m1_t | `Cast`, `Duplicate` (software simulation) |
| fp4x2_e1m2_t | `Cast`, `Duplicate` (software simulation) |
| fp8_e5m2_t | `Cast`, `Duplicate` (software simulation) |
| fp8_e4m3fn_t | `Cast`, `Duplicate` (software simulation) |
| fp8_e8m0_t | `Cast`, `Duplicate` (software simulation) |
| hifloat8_t | `Cast`, `Duplicate` (software simulation) |
| int4x2_t | `Cast` |
| int8_t | `Cast`, `Add`, `Sub`, `Max`, `Min`, `Compare`, `Abs`, `Neg`, `Adds`, `Maxs`, `Mins`, `Compares`, `Duplicate` |
| uint8_t | `Cast`, `Add`, `Sub`, `Max`, `Min`, `Compare`, `Adds`, `Maxs`, `Mins`, `Compares`, `Duplicate` |

**Table 3**  C API Reg vector compute API precision standards<a id="table-c-api-precision-standard"></a>

| API Category | API | Hardware Instruction Precision Standard | Soft Simulation Precision Standard |
| --- | --- | --- | --- |
| Basic Arithmetic | `asc_exp` | 1ulp, not support denormalized numbers | - |
| Basic Arithmetic | `asc_sqrt` | 1ulp, not support denormalized numbers | - |
| Basic Arithmetic | `asc_add` | &bull; half/float: 0ulp, support denormalized numbers<br>&bull; bfloat16_t: Add(x, y) = rint_to_bf16(Add(cast_to_f32(x), cast_to_f32(y))) | - |
| Basic Arithmetic | `asc_sub` | Sub(x, y) = Add(x, Neg(y)) | - |
| Basic Arithmetic | `asc_add_scalar` | &bull; half/float: 0ulp, support denormalized numbers<br>&bull; bfloat16_t: Add(x, y) = rint_to_bf16(Add(cast_to_f32(x), cast_to_f32(y))) | - |
| Basic Arithmetic | `asc_div` | 1ulp, not support denormalized numbers | - |
| Basic Arithmetic | `asc_neg` | half/float: invert the sign bit | - |
| Basic Arithmetic | `asc_ln`/`asc_log` | 1ulp, not support denormalized numbers | - |
| Basic Arithmetic | `asc_mul` | &bull; half/float: 0ulp, support denormalized numbers<br>&bull; bfloat16_t: Mul(x, y) = rint_to_bf16(Mul(cast_to_f32(x), cast_to_f32(y))) | - |
| Basic Arithmetic | `asc_mul_scalar` | half/float: 0ulp, support denormalized numbers | - |
| compound instruction | `asc_abs_sub` | precision equivalent to subtracting first and then taking the absolute value, AbsSub(x) = Abs(Sub(x)) | - |
| compound instruction | `asc_exp_sub`/`asc_exp_sub_half2float` | - half input, float output:<br>ExpSub(x, y) = Exp(Sub(cast_to_f32(x), cast_to_f32(y)))<br>- float input, float output:<br>ExpSub(x, y) = Exp(Sub(x, y)) | - |
| compound instruction | `asc_axpy` | Result is equivalent to: multiply-accumulate uses high-precision computation, equivalent to Fma(x, y, z) | - |
| compound instruction | `asc_madd` | Result is equivalent to: multiply-accumulate uses high-precision computation, equivalent to Fma(x, y, z) | - |
| compound instruction | `asc_mula` | Result is equivalent to: multiply-accumulate uses high-precision computation, equivalent to Fma(x, y, z) | - |
| compound instruction | `asc_mul_scalar_float2half_rn` | float×float to half, fixed RN rounding; the result is `cast_to_f16_rn(x * y)` | - |
| Reduction Computation | `asc_reduce_sum` | half/float: 0ulp, uses a pairwise accumulation strategy based on a binary tree structure | - |
| Reduction Computation | `asc_reduce_sum_datablock` | half/float: 0ulp, uses a pairwise accumulation strategy based on a binary tree structure | - |
| Reduction Computation | `asc_pair_reduce_sum` | half/float: 0ulp | - |

Note: Log2/Log10 in [Table 1](#table-precision-standard) currently has no corresponding C API, so it is not listed in Table 3.

When the C API needs to compute Log2 or Log10, call `asc_ln`, then use the change-of-base formula and multiply by the reciprocal constant through `asc_mul_scalar`:

$$
\log_2(x) = \frac{\ln(x)}{\ln(2)} = \ln(x) \times \frac{1}{\ln(2)}
$$

$$
\log_{10}(x) = \frac{\ln(x)}{\ln(10)} = \ln(x) \times \frac{1}{\ln(10)}
$$

**Table 4**  C API low-precision instruction support<a id="table-c-api-low-precision"></a>

| Data Type | API |
| --- | --- |
| fp4x2_e2m1_t | `asc_bfloat162e2m1x2_<round_mode>`, `asc_e2m1x22bfloat16` |
| fp4x2_e1m2_t | `asc_bfloat162e1m2x2_<round_mode>`, `asc_e1m2x22bfloat16` |
| fp8_e5m2_t | `asc_float2e5m2_rn<sat_mode>`, `asc_e5m22float`, `asc_duplicate`, `asc_duplicate_merge`, `asc_duplicate_highest`, `asc_duplicate_highest_merge`, `asc_duplicate_scalar`, `asc_duplicate_scalar_merge` |
| fp8_e4m3fn_t | `asc_float2e4m3_rn<sat_mode>`, `asc_e4m32float`, `asc_duplicate`, `asc_duplicate_merge`, `asc_duplicate_highest`, `asc_duplicate_highest_merge`, `asc_duplicate_scalar`, `asc_duplicate_scalar_merge` |
| fp8_e8m0_t | `asc_duplicate`, `asc_duplicate_merge`, `asc_duplicate_highest`, `asc_duplicate_highest_merge`, `asc_duplicate_scalar`, `asc_duplicate_scalar_merge` |
| hifloat8_t | `asc_float2hif8_<round_mode><sat_mode>`, `asc_half2hif8_<round_mode><sat_mode>`, `asc_hif82float`, `asc_hif82half` |
| int4x2_t | `asc_half2int4x2_<round_mode><sat_mode>`, `asc_int4x22bfloat16`, `asc_int4x22half`, `asc_int4x22int16` |
| int8_t | conversion: `asc_half2int8_<round_mode><sat_mode>`, `asc_int82half`, `asc_int82int16`, `asc_int82int32`<br>arithmetic: `asc_add`, `asc_sub`, `asc_max`, `asc_min`, `asc_abs`, `asc_neg`, `asc_add_scalar`, `asc_max_scalar`, `asc_min_scalar`<br>comparison: `asc_eq`, `asc_ne`, `asc_lt`, `asc_le`, `asc_gt`, `asc_ge`, `asc_eq_scalar`, `asc_ne_scalar`, `asc_lt_scalar`, `asc_le_scalar`, `asc_gt_scalar`, `asc_ge_scalar`<br>broadcast: `asc_duplicate`, `asc_duplicate_merge`, `asc_duplicate_highest`, `asc_duplicate_highest_merge`, `asc_duplicate_scalar`, `asc_duplicate_scalar_merge` |
| uint8_t | conversion: `asc_half2uint8_<round_mode><sat_mode>`, `asc_int162uint8_<sat_mode>`, `asc_int322uint8_<sat_mode>`, `asc_uint162uint8_<sat_mode>`, `asc_uint322uint8_<sat_mode>`, `asc_uint82half`, `asc_uint82uint16`, `asc_uint82uint32`<br>arithmetic: `asc_add`, `asc_sub`, `asc_max`, `asc_min`, `asc_add_scalar`, `asc_max_scalar`, `asc_min_scalar`<br>comparison: `asc_eq`, `asc_ne`, `asc_lt`, `asc_le`, `asc_gt`, `asc_ge`, `asc_eq_scalar`, `asc_ne_scalar`, `asc_lt_scalar`, `asc_le_scalar`, `asc_gt_scalar`, `asc_ge_scalar`<br>broadcast: `asc_duplicate`, `asc_duplicate_merge`, `asc_duplicate_highest`, `asc_duplicate_highest_merge`, `asc_duplicate_scalar`, `asc_duplicate_scalar_merge` |

Where:

- The source data type of `asc_bfloat162e2m1x2_<round_mode>` and `asc_bfloat162e1m2x2_<round_mode>` is `bfloat16_t`. `<round_mode>` can be: `rd` (FLOOR, round down), `rn` (RINT, round according to RINT rules), `rna` (ROUND, round to nearest integer), `ru` (CEIL, round up), and `rz` (TRUNC, truncate toward zero).
- The source data type of `asc_half2int4x2_<round_mode><sat_mode>`, `asc_half2int8_<round_mode><sat_mode>`, and `asc_half2uint8_<round_mode><sat_mode>` is `half`. `<round_mode>` can be: `rd` (FLOOR, round down), `rn` (RINT, round according to RINT rules), `rna` (ROUND, round to nearest integer), `ru` (CEIL, round up), and `rz` (TRUNC, truncate toward zero).
- `asc_float2e5m2_rn<sat_mode>` and `asc_float2e4m3_rn<sat_mode>` use fixed `rn` rounding.
- For `asc_float2hif8_<round_mode><sat_mode>` and `asc_half2hif8_<round_mode><sat_mode>`, `<round_mode>` can be `rh` (HIFLOAT8-specific rounding mode) and `rna` (ROUND, round to nearest integer).
- `<sat_mode>` can be empty (non-saturating) or `_sat` (saturating). `asc_int162uint8`, `asc_int322uint8`, `asc_uint162uint8`, and `asc_uint322uint8` support only the forms without a suffix and with the `_sat` suffix, and do not support `<round_mode>`.
