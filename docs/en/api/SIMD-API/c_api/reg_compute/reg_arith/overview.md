# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-18T07:40:48.331585+00:00 pushedAt=2026-11-18T07:42:30.265Z nl=2 -->

## API List

**Table 1** Basic arithmetic APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_abs](asc_abs.md) | Computes the absolute value element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_add](asc_add.md) | Performs addition element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_add_scalar](asc_add_scalar.md) | Performs addition of a vector and a scalar element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_addc](asc_addc.md) | Performs addition with carry element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_div](asc_div.md) | Performs division element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_exp](asc_exp.md) | Computes the exponential of the source operand element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_ln](asc_ln.md) | Computes the natural logarithm of the source operand element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_log](asc_log.md) | Computes the natural logarithm of the source operand element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_max](asc_max.md) | Computes the maximum value element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_max_scalar](asc_max_scalar.md) | Computes the maximum of a vector and a scalar element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_min](asc_min.md) | Performs an element-wise minimum operation on the source operands `src0` and `src1` according to `mask`, and writes the result to the destination operand `dst`. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_min_scalar](asc_min_scalar.md) | Compares each element in the source operand vector with a scalar and takes the smaller value. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_mul](asc_mul.md) | Performs multiplication on the source operands `src0` and `src1`, and writes the result to the destination operand `dst`. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_mul_scalar](asc_mul_scalar.md) | Performs element-wise multiplication on the source operands `src` and `value`, and writes the result to the destination operand `dst`. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_mull](asc_mull.md) | Performs element-wise multiplication on the input data `src0` and `src1` based on `mask`, writes the result to `dst0`, and writes the overflow part to `dst1`. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_neg](asc_neg.md) | Performs negation on the source operand `src` based on `mask`, and writes the result to the destination operand `dst`. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_sqrt](asc_sqrt.md) | Computes the square root element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_sub](asc_sub.md) | Performs subtraction element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
| [asc_subc](asc_subc.md) | Performs subtraction with borrow element-wise. | `c_api/reg_compute/compute/reg_arith.h` |
