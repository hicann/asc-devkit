/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC_H
#endif

#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_BASIC_ARITHMETIC_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_BASIC_ARITHMETIC_H

#include "tensor_api/tensor/tensor_interface.h"
#include "tensor_api/experimental/arch/vector/reg_tensor.h"
#include "tensor_api/experimental/utils/reg_enum.h"

namespace asc {
namespace te {
namespace experimental {

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise natural logarithms of src under src.mask.
 *
 * @override_id te_experimental_log__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> log(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise natural logarithms of src under src.mask.
 *
 * @override_id te_experimental_ln__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> ln(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise sum of the operands under the source register mask.
 *
 * @override_id te_experimental_operator_add__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator+(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise sum of the operands under the source register mask.
 *
 * @override_id te_experimental_operator_add__const_reg_tensor_T_R_const_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @param [in] scalar Scalar operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator+(const reg_tensor<T>& src, const T& scalar);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise sum of the operands under the source register mask.
 *
 * @override_id te_experimental_operator_add__const_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] scalar Scalar operand.
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator+(const T& scalar, const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise negated values of src under src.mask.
 *
 * @override_id te_experimental_neg__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> neg(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise difference of the operands under the source register mask.
 *
 * @override_id te_experimental_operator_sub__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise difference of the operands under the source register mask.
 *
 * @override_id te_experimental_operator_sub__const_reg_tensor_T_R_const_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @param [in] scalar Scalar operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const reg_tensor<T>& src, const T& scalar);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise difference of the operands under the source register mask.
 *
 * @override_id te_experimental_operator_sub__const_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] scalar Scalar operand.
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const T& scalar, const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise product of the operands under the source register mask.
 *
 * @override_id te_experimental_operator_mul__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator*(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise product of the operands under the source register mask.
 *
 * @override_id te_experimental_operator_mul__const_reg_tensor_T_R_const_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @param [in] scalar Scalar operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator*(const reg_tensor<T>& src, const T& scalar);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise product of the operands under the source register mask.
 *
 * @override_id te_experimental_operator_mul__const_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] scalar Scalar operand.
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator*(const T& scalar, const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise maximum of the operands under the source register mask.
 *
 * @override_id te_experimental_max__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> max(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise maximum of the operands under the source register mask.
 *
 * @override_id te_experimental_max__const_reg_tensor_T_R_const_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @param [in] scalar Scalar operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> max(const reg_tensor<T>& src, const T& scalar);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise maximum of the operands under the source register mask.
 *
 * @override_id te_experimental_max__const_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] scalar Scalar operand.
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> max(const T& scalar, const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise absolute values of src under src.mask.
 *
 * @override_id te_experimental_abs__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> abs(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise exponentials of src under src.mask.
 *
 * @override_id te_experimental_exp__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> exp(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise square roots of src under src.mask.
 *
 * @override_id te_experimental_sqrt__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> sqrt(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise base-2 logarithms of src under src.mask.
 *
 * @override_id te_experimental_log2__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> log2(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise base-10 logarithms of src under src.mask.
 *
 * @override_id te_experimental_log10__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> log10(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise rectified linear values of src under src.mask.
 *
 * @override_id te_experimental_relu__const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> relu(const reg_tensor<T>& src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise parametric rectified linear values using slope under src.mask.
 *
 * @override_id te_experimental_prelu__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @param [in] slope Slope operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> prelu(const reg_tensor<T>& src, const reg_tensor<T>& slope);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise leaky rectified linear values using slope under src.mask.
 *
 * @override_id te_experimental_leaky_relu__const_reg_tensor_T_R_const_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @param [in] slope Slope operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> leaky_relu(const reg_tensor<T>& src, const T& slope);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise sum and carry registers under src0.mask.
 *
 * @override_id te_experimental_add_carry__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it.
 */
template <typename T>
__simd_callee__ inline reg_pair<T, bool> add_carry(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise sum and carry registers under src0.mask.
 *
 * @override_id te_experimental_add_carry__const_reg_tensor_T_R_const_reg_tensor_T_R_const_reg_tensor_bool_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @param [in] carry_src Input carry register tensor.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it.
 */
template <typename T>
__simd_callee__ inline reg_pair<T, bool> add_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<bool>& carry_src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise difference and borrow registers under src0.mask.
 *
 * @override_id te_experimental_sub_carry__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it.
 */
template <typename T>
__simd_callee__ inline reg_pair<T, bool> sub_carry(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise difference and borrow registers under src0.mask.
 *
 * @override_id te_experimental_sub_carry__const_reg_tensor_T_R_const_reg_tensor_T_R_const_reg_tensor_bool_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @param [in] carry_src Input carry register tensor. A value of 1 means no incoming borrow; 0 means an incoming borrow.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it.
 */
template <typename T>
__simd_callee__ inline reg_pair<T, bool> sub_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<bool>& carry_src);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the low and high halves of element-wise products under src0.mask.
 *
 * @override_id te_experimental_mull__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it.
 */
template <typename T>
__simd_callee__ inline reg_pair<T> mull(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise quotients of src0 and src1 under src0.mask.
 *
 * @override_id te_experimental_operator_div__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> operator/(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns element-wise quotients with the selected precision mode.
 *
 * @override_id te_experimental_div__const_reg_tensor_T_R_const_reg_tensor_T_R__template_precisionMode_algo_T
 * @archs 3510
 * @param [in] src0 Dividend.
 * @param [in] src1 Divisor.
 * @note Supports the basic Div algorithms. The 0-ULP algorithms and precisionMode=true support float only;
 * PRECISION_1ULP_FTZ_FALSE supports half/float. INTRINSIC and DIFF_COMPENSATION follow --cce-ftz.
 * With precisionMode=false, explicit FTZ algorithms select their paths independently of --cce-ftz.
 * With precisionMode=true, explicit 0-ULP algorithms select FTZ; other algorithms follow --cce-ftz, as in Reg::Div.
 * Only src0.mask selects active lanes. Unselected elements are zeroed and the result preserves src0.mask.
 */
template <bool precisionMode = false, div_algo algo = div_algo::INTRINSIC, typename T>
__simd_callee__ inline reg_tensor<T> div(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise minimum of the operands under the source register mask.
 *
 * @override_id te_experimental_min__const_reg_tensor_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] src0 First source operand.
 * @param [in] src1 Second source operand.
 * @note The active-lane mask is taken from src0.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> min(const reg_tensor<T>& src0, const reg_tensor<T>& src1);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise minimum of the operands under the source register mask.
 *
 * @override_id te_experimental_min__const_reg_tensor_T_R_const_T_R__template_T
 * @archs 3510
 * @param [in] src Source operand.
 * @param [in] scalar Scalar operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> min(const reg_tensor<T>& src, const T& scalar);

/**
 * @ingroup simd_api.tensor_api.reg_vector_compute.basic_arithmetic
 * @brief Returns the element-wise minimum of the operands under the source register mask.
 *
 * @override_id te_experimental_min__const_T_R_const_reg_tensor_T_R__template_T
 * @archs 3510
 * @param [in] scalar Scalar operand.
 * @param [in] src Source operand.
 * @note The active-lane mask is taken from src.mask. A value of 1 selects the corresponding element; a value of 0
 * excludes it. Unselected elements are set to zero. The result preserves the active-lane mask.
 */
template <typename T>
__simd_callee__ inline reg_tensor<T> min(const T& scalar, const reg_tensor<T>& src);

} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/basic_arithmetic_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_BASIC_ARITHMETIC_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC_H
#endif
