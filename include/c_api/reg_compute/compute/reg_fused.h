/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "compute/reg_fused.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_FUSED_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_FUSED_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_FUSED_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 9201))
#include "impl/c_api/reg_base_impl/reg_fused_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Writes the elementwise ReLU of the half src register into dst under mask, so negative values and mask-cleared
 * lanes become 0.
 *
 * @override_id asc_relu__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half: it receives src_i where src_i >= 0 and 0 elsewhere; previous contents are
 * overwritten and it is never read as an input.
 * @param [in] src Source vector_half whose elements are rectified; it is read only.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit keeps the corresponding element, a 0 bit excludes it
 * and the dst element becomes 0.
 */
__simd_callee__ inline void asc_relu(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Writes the elementwise ReLU of the int32_t src register into dst under mask, so negative values and
 * mask-cleared lanes become 0.
 *
 * @override_id asc_relu__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector_int32_t: it receives src_i where src_i >= 0 and 0 elsewhere; previous contents
 * are overwritten and it is never read as an input.
 * @param [in] src Source vector_int32_t whose elements are rectified; it is read only.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit keeps the corresponding element, a 0 bit excludes it
 * and the dst element becomes 0.
 */
__simd_callee__ inline void asc_relu(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Writes the elementwise ReLU of the float src register into dst under mask, so negative values and mask-cleared
 * lanes become 0.
 *
 * @override_id asc_relu__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float: it receives src_i where src_i >= 0 and 0 elsewhere; previous contents are
 * overwritten and it is never read as an input.
 * @param [in] src Source vector_float whose elements are rectified; it is read only.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit keeps the corresponding element, a 0 bit excludes it
 * and the dst element becomes 0.
 */
__simd_callee__ inline void asc_relu(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Returns a vector_half register holding the elementwise ReLU of src under mask, so negative values and
 * mask-cleared lanes become 0.
 *
 * @override_id asc_relu__vf16_vb
 * @archs 3510
 * @param [in] src Source vector_half whose elements are rectified; it is read only and left unchanged.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit keeps the corresponding element, a 0 bit excludes it
 * and its result element becomes 0.
 */
__simd_callee__ inline vector_half asc_relu(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Returns a vector_int32_t register holding the elementwise ReLU of src under mask, so negative values and
 * mask-cleared lanes become 0.
 *
 * @override_id asc_relu__vi32_vb
 * @archs 3510
 * @param [in] src Source vector_int32_t whose elements are rectified; it is read only and left unchanged.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit keeps the corresponding element, a 0 bit excludes it
 * and its result element becomes 0.
 */
__simd_callee__ inline vector_int32_t asc_relu(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Returns a vector_float register holding the elementwise ReLU of src under mask, so negative values and
 * mask-cleared lanes become 0.
 *
 * @override_id asc_relu__vf32_vb
 * @archs 3510
 * @param [in] src Source vector_float whose elements are rectified; it is read only and left unchanged.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit keeps the corresponding element, a 0 bit excludes it
 * and its result element becomes 0.
 */
__simd_callee__ inline vector_float asc_relu(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Computes the element-wise absolute difference of the half operands src0 and src1 and writes it into dst,
 * zeroing positions whose mask bit is 0.
 *
 * @override_id asc_abs_sub__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the 128 absolute-difference results; a position whose
 * mask bit is 0 is written as zero.
 * @param [in] src0 First half source vector data register; only read (minuend of the element-wise subtraction).
 * @param [in] src1 Second half source vector data register; only read (subtrahend of the element-wise subtraction).
 * @param [in] mask vector_bool mask register gating each element: 1 computes the absolute difference, 0 forces that
 * result element to zero.
 */
__simd_callee__ inline void asc_abs_sub(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Computes the element-wise absolute difference of the float operands src0 and src1 and writes it into dst,
 * zeroing positions whose mask bit is 0.
 *
 * @override_id asc_abs_sub__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the 64 absolute-difference results; a position whose mask
 * bit is 0 is written as zero.
 * @param [in] src0 First float source vector data register; only read (minuend of the element-wise subtraction).
 * @param [in] src1 Second float source vector data register; only read (subtrahend of the element-wise subtraction).
 * @param [in] mask vector_bool mask register gating each element: 1 computes the absolute difference, 0 forces that
 * result element to zero.
 */
__simd_callee__ inline void asc_abs_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Computes the element-wise absolute difference of the half operands src0 and src1 and returns it as a
 * vector_half, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_abs_sub__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First half source vector data register; only read (minuend of the element-wise subtraction).
 * @param [in] src1 Second half source vector data register; only read (subtrahend of the element-wise subtraction).
 * @param [in] mask vector_bool mask register gating each element: 1 computes the absolute difference, 0 forces that
 * result element to zero.
 */
__simd_callee__ inline vector_half asc_abs_sub(vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Computes the element-wise absolute difference of the float operands src0 and src1 and returns it as a
 * vector_float, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_abs_sub__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First float source vector data register; only read (minuend of the element-wise subtraction).
 * @param [in] src1 Second float source vector data register; only read (subtrahend of the element-wise subtraction).
 * @param [in] mask vector_bool mask register gating each element: 1 computes the absolute difference, 0 forces that
 * result element to zero.
 */
__simd_callee__ inline vector_float asc_abs_sub(vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Computes the fused multiply-add dst = src * value + dst element-wise for half vectors under mask, overwriting
 * dst.
 *
 * @override_id asc_axpy__Rvf16_vf16_f16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector data register used as the addend accumulator and overwritten with src * value
 * + dst.
 * @param [in] src Source vector data register whose elements are multiplied by value.
 * @param [in] value Scalar multiplier of half type applied to each element of src.
 * @param [in] mask Mask register controlling which elements participate; a 0 bit zeroes the corresponding dst element.
 */
__simd_callee__ inline void asc_axpy(vector_half& dst, vector_half src, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Computes the fused multiply-add dst = src * value + dst element-wise for float vectors under mask, overwriting
 * dst.
 *
 * @override_id asc_axpy__Rvf32_vf32_f32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector data register used as the addend accumulator and overwritten with src * value
 * + dst.
 * @param [in] src Source vector data register whose elements are multiplied by value.
 * @param [in] value Scalar multiplier of float type applied to each element of src.
 * @param [in] mask Mask register controlling which elements participate; a 0 bit zeroes the corresponding dst element.
 */
__simd_callee__ inline void asc_axpy(vector_float& dst, vector_float src, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Multiplies the float elements of src by the scalar value, converts them to half and writes them into the even
 * positions of dst, leaving the odd positions zero and dropping elements whose mask bit is 0.
 *
 * @override_id asc_mul_scalar_float2half_rn__Rvf16_vf32_f32_vb_icposeven
 * @archs 3510
 * @param [out] dst Destination vector_half register whose even-index positions receive the 64 converted half results
 * (positions 2*i plus the 0 offset of the tag); the odd-index positions are set to zero.
 * @param [in] src Source vector_float register holding the 32-bit values to scale; read only.
 * @param [in] value Scalar float multiplier applied element-wise before the narrowing conversion; no unit, read only.
 * @param [in] mask vector_bool mask register controlling which elements take part: a bit of 1 converts and stores that
 * element, a bit of 0 writes zero to the corresponding result position.
 * @param [in] dst_pos Compile-time position tag fixed to asc_position_mode::EVEN: results are written to even indices
 * and the odd indices are forced to 0; it only selects the overload.
 */
__simd_callee__ inline void asc_mul_scalar_float2half_rn(
    vector_half& dst, vector_float src, float value, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Multiplies the float elements of src by the scalar value, converts them to half and writes them into the odd
 * positions of dst, leaving the even positions zero and dropping elements whose mask bit is 0.
 *
 * @override_id asc_mul_scalar_float2half_rn__Rvf16_vf32_f32_vb_icposodd
 * @archs 3510
 * @param [out] dst Destination vector_half register whose odd-index positions receive the 64 converted half results
 * (positions 2*i plus the 1 offset of the tag); the even-index positions are set to zero.
 * @param [in] src Source vector_float register holding the 32-bit values to scale; read only.
 * @param [in] value Scalar float multiplier applied element-wise before the narrowing conversion; no unit, read only.
 * @param [in] mask vector_bool mask register controlling which elements take part: a bit of 1 converts and stores that
 * element, a bit of 0 writes zero to the corresponding result position.
 * @param [in] dst_pos Compile-time position tag fixed to asc_position_mode::ODD: results are written to odd indices and
 * the even indices are forced to 0; it only selects the overload.
 */
__simd_callee__ inline void asc_mul_scalar_float2half_rn(
    vector_half& dst, vector_float src, float value, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Multiplies the float elements of src by the scalar value, converts them to half and returns them in the even
 * positions of a vector_half, leaving the odd positions zero and dropping elements whose mask bit is 0.
 *
 * @override_id asc_mul_scalar_float2half_rn__vf32_f32_vb_icposeven
 * @archs 3510
 * @param [in] src Source vector_float register holding the 32-bit values to scale; read only.
 * @param [in] value Scalar float multiplier applied element-wise before the narrowing conversion; no unit, read only.
 * @param [in] mask vector_bool mask register controlling which elements take part: a bit of 1 converts and stores that
 * element, a bit of 0 writes zero to the corresponding result position.
 * @param [in] dst_pos Compile-time position tag fixed to asc_position_mode::EVEN: results are written to even indices
 * and the odd indices are forced to 0; it only selects the overload.
 */
__simd_callee__ inline vector_half asc_mul_scalar_float2half_rn(
    vector_float src, float value, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Multiplies the float elements of src by the scalar value, converts them to half and returns them in the odd
 * positions of a vector_half, leaving the even positions zero and dropping elements whose mask bit is 0.
 *
 * @override_id asc_mul_scalar_float2half_rn__vf32_f32_vb_icposodd
 * @archs 3510
 * @param [in] src Source vector_float register holding the 32-bit values to scale; read only.
 * @param [in] value Scalar float multiplier applied element-wise before the narrowing conversion; no unit, read only.
 * @param [in] mask vector_bool mask register controlling which elements take part: a bit of 1 converts and stores that
 * element, a bit of 0 writes zero to the corresponding result position.
 * @param [in] dst_pos Compile-time position tag fixed to asc_position_mode::ODD: results are written to odd indices and
 * the even indices are forced to 0; it only selects the overload.
 */
__simd_callee__ inline vector_half asc_mul_scalar_float2half_rn(
    vector_float src, float value, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Computes dst * src0 + src1 element-wise in half and writes the result back into dst under mask.
 *
 * @override_id asc_madd__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [in,out] dst Destination half vector data register; it is multiplied as an input and receives the fused
 * multiply-add result.
 * @param [in] src0 half source vector data register multiplied with dst.
 * @param [in] src1 half source vector data register added to the product.
 * @param [in] mask Mask register whose set bits mark participating elements; elements with a 0 bit are set to 0 in dst.
 */
__simd_callee__ inline void asc_madd(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Computes dst * src0 + src1 element-wise in float and writes the result back into dst under mask.
 *
 * @override_id asc_madd__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [in,out] dst Destination float vector data register; it is multiplied as an input and receives the fused
 * multiply-add result.
 * @param [in] src0 float source vector data register multiplied with dst.
 * @param [in] src1 float source vector data register added to the product.
 * @param [in] mask Mask register whose set bits mark participating elements; elements with a 0 bit are set to 0 in dst.
 */
__simd_callee__ inline void asc_madd(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Computes dst * src0 + src1 element-wise in bfloat16_t and writes the result back into dst under mask.
 *
 * @override_id asc_madd__Rvbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [in,out] dst Destination bfloat16_t vector data register; it is multiplied as an input and receives the fused
 * multiply-add result.
 * @param [in] src0 bfloat16_t source vector data register multiplied with dst.
 * @param [in] src1 bfloat16_t source vector data register added to the product.
 * @param [in] mask Mask register whose set bits mark participating elements; elements with a 0 bit are set to 0 in dst.
 */
__simd_callee__ inline void asc_madd(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Applies an element-wise Leaky ReLU with scalar negative slope alpha to the float elements of src and writes
 * the result into dst, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_leakyrelu__Rvf32_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the 64 Leaky ReLU results; a position whose mask bit is 0
 * is written as zero.
 * @param [in] src Source float vector data register holding the values to activate; read only.
 * @param [in] alpha Scalar float immediate used as the negative-slope factor for elements less than or equal to 0; the
 * page requires its type to match src.
 * @param [in] mask vector_bool mask register gating each element: 1 activates the element, 0 forces that result element
 * to zero.
 */
__simd_callee__ inline void asc_leakyrelu(vector_float& dst, vector_float src, float alpha, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Applies an element-wise Leaky ReLU with scalar negative slope alpha to the half elements of src and writes the
 * result into dst, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_leakyrelu__Rvf16_vf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving the 128 Leaky ReLU results; a position whose mask bit is
 * 0 is written as zero.
 * @param [in] src Source half vector data register holding the values to activate; read only.
 * @param [in] alpha Scalar half immediate used as the negative-slope factor for elements less than or equal to 0; the
 * page requires its type to match src.
 * @param [in] mask vector_bool mask register gating each element: 1 activates the element, 0 forces that result element
 * to zero.
 */
__simd_callee__ inline void asc_leakyrelu(vector_half& dst, vector_half src, half alpha, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Applies an element-wise Leaky ReLU with scalar negative slope alpha to the float elements of src and returns
 * the result as a vector_float, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_leakyrelu__vf32_f32_vb
 * @archs 3510
 * @param [in] src Source float vector data register holding the values to activate; read only.
 * @param [in] alpha Scalar float immediate used as the negative-slope factor for elements less than or equal to 0; the
 * page requires its type to match src.
 * @param [in] mask vector_bool mask register gating each element: 1 activates the element, 0 forces that result element
 * to zero.
 */
__simd_callee__ inline vector_float asc_leakyrelu(vector_float src, float alpha, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Applies an element-wise Leaky ReLU with scalar negative slope alpha to the half elements of src and returns
 * the result as a vector_half, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_leakyrelu__vf16_f16_vb
 * @archs 3510
 * @param [in] src Source half vector data register holding the values to activate; read only.
 * @param [in] alpha Scalar half immediate used as the negative-slope factor for elements less than or equal to 0; the
 * page requires its type to match src.
 * @param [in] mask vector_bool mask register gating each element: 1 activates the element, 0 forces that result element
 * to zero.
 */
__simd_callee__ inline vector_half asc_leakyrelu(vector_half src, half alpha, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Writes e raised to the element-wise difference src0 - src1 into float vector dst under mask.
 *
 * @override_id asc_exp_sub__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register receiving e^(src0 - src1); a 0 mask bit zeroes the corresponding
 * element.
 * @param [in] src0 Minuend source vector data register of float elements.
 * @param [in] src1 Subtrahend source vector data register of float elements.
 * @param [in] mask Mask register controlling which output elements participate; a 0 bit zeroes the corresponding result
 * element.
 */
__simd_callee__ inline void asc_exp_sub(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline void asc_exp_sub_half2float(
    vector_float& dst, vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos);

__simd_callee__ inline void asc_exp_sub_half2float(
    vector_float& dst, vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos);

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Returns a float vector holding e raised to the element-wise difference src0 - src1 under mask.
 *
 * @override_id asc_exp_sub__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 Minuend source vector data register of float elements.
 * @param [in] src1 Subtrahend source vector data register of float elements.
 * @param [in] mask Mask register controlling which output elements participate; a 0 bit zeroes the corresponding result
 * element.
 */
__simd_callee__ inline vector_float asc_exp_sub(vector_float src0, vector_float src1, vector_bool mask);

__simd_callee__ inline vector_float asc_exp_sub_half2float(
    vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos);

__simd_callee__ inline vector_float asc_exp_sub_half2float(
    vector_half src0, vector_half src1, vector_bool mask,
    std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos);

#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Evaluates an element-wise PReLU on the float elements of src0 using the per-element negative slopes in alpha
 * and writes the result into dst, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_prelu__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the 64 PReLU results; a position whose mask bit is 0 is
 * written as zero.
 * @param [in] src0 Source float vector data register holding the values to activate; values greater than 0 pass through
 * unchanged.
 * @param [in] alpha Per-element float vector data register of negative-half slopes; its element type must match src0
 * and it is only read.
 * @param [in] mask vector_bool mask register gating each element: 1 evaluates the element, 0 forces that result element
 * to zero.
 */
__simd_callee__ inline void asc_prelu(vector_float& dst, vector_float src0, vector_float alpha, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Evaluates an element-wise PReLU on the half elements of src0 using the per-element negative slopes in alpha
 * and writes the result into dst, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_prelu__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector register receiving the 128 PReLU results; a position whose mask bit is 0 is
 * written as zero.
 * @param [in] src0 Source half vector data register holding the values to activate; values greater than 0 pass through
 * unchanged.
 * @param [in] alpha Per-element half vector data register of negative-half slopes; its element type must match src0 and
 * it is only read.
 * @param [in] mask vector_bool mask register gating each element: 1 evaluates the element, 0 forces that result element
 * to zero.
 */
__simd_callee__ inline void asc_prelu(vector_half& dst, vector_half src0, vector_half alpha, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Evaluates an element-wise PReLU on the float elements of src0 using the per-element negative slopes in alpha
 * and returns the result as a vector_float, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_prelu__vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 Source float vector data register holding the values to activate; values greater than 0 pass through
 * unchanged.
 * @param [in] alpha Per-element float vector data register of negative-half slopes; its element type must match src0
 * and it is only read.
 * @param [in] mask vector_bool mask register gating each element: 1 evaluates the element, 0 forces that result element
 * to zero.
 */
__simd_callee__ inline vector_float asc_prelu(vector_float src0, vector_float alpha, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Evaluates an element-wise PReLU on the half elements of src0 using the per-element negative slopes in alpha
 * and returns the result as a vector_half, zeroing positions whose mask bit is 0.
 *
 * @override_id asc_prelu__vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 Source half vector data register holding the values to activate; values greater than 0 pass through
 * unchanged.
 * @param [in] alpha Per-element half vector data register of negative-half slopes; its element type must match src0 and
 * it is only read.
 * @param [in] mask vector_bool mask register gating each element: 1 evaluates the element, 0 forces that result element
 * to zero.
 */
__simd_callee__ inline vector_half asc_prelu(vector_half src0, vector_half alpha, vector_bool mask);

//=================asc_mula==================
/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Accumulates the elementwise product of the int16_t src0 and src1 registers into the dst register where mask is
 * set (dst = src0 * src1 + dst).
 *
 * @override_id asc_mula__Rvi16_vi16_vi16_vb
 * @archs 3510
 * @param [in,out] dst Accumulator and result register of type vector_int16_t: its current elements are read as the
 * addend and replaced by the fused result, with elements whose mask bit is 0 cleared.
 * @param [in] src0 First int16_t multiplicand register; read only, elementwise.
 * @param [in] src1 Second int16_t multiplicand register; read only, elementwise.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit lets the corresponding element participate, a 0 bit
 * excludes it and zeroes that dst element.
 */
__simd_callee__ inline void asc_mula(vector_int16_t& dst, vector_int16_t src0, vector_int16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Accumulates the elementwise product of the uint16_t src0 and src1 registers into the dst register where mask
 * is set (dst = src0 * src1 + dst).
 *
 * @override_id asc_mula__Rvu16_vu16_vu16_vb
 * @archs 3510
 * @param [in,out] dst Accumulator and result register of type vector_uint16_t: its current elements are read as the
 * addend and replaced by the fused result, with elements whose mask bit is 0 cleared.
 * @param [in] src0 First uint16_t multiplicand register; read only, elementwise.
 * @param [in] src1 Second uint16_t multiplicand register; read only, elementwise.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit lets the corresponding element participate, a 0 bit
 * excludes it and zeroes that dst element.
 */
__simd_callee__ inline void asc_mula(
    vector_uint16_t& dst, vector_uint16_t src0, vector_uint16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Accumulates the elementwise product of the half src0 and src1 registers into the dst register where mask is
 * set (dst = src0 * src1 + dst).
 *
 * @override_id asc_mula__Rvf16_vf16_vf16_vb
 * @archs 3510
 * @param [in,out] dst Accumulator and result register of type vector_half: its current elements are read as the addend
 * and replaced by the fused result, with elements whose mask bit is 0 cleared.
 * @param [in] src0 First half multiplicand register; read only, elementwise.
 * @param [in] src1 Second half multiplicand register; read only, elementwise.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit lets the corresponding element participate, a 0 bit
 * excludes it and zeroes that dst element.
 */
__simd_callee__ inline void asc_mula(vector_half& dst, vector_half src0, vector_half src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Accumulates the elementwise product of the bfloat16_t src0 and src1 registers into the dst register where mask
 * is set (dst = src0 * src1 + dst).
 *
 * @override_id asc_mula__Rvbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [in,out] dst Accumulator and result register of type vector_bfloat16_t: its current elements are read as the
 * addend and replaced by the fused result, with elements whose mask bit is 0 cleared.
 * @param [in] src0 First bfloat16_t multiplicand register; read only, elementwise.
 * @param [in] src1 Second bfloat16_t multiplicand register; read only, elementwise.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit lets the corresponding element participate, a 0 bit
 * excludes it and zeroes that dst element.
 */
__simd_callee__ inline void asc_mula(
    vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Accumulates the elementwise product of the int32_t src0 and src1 registers into the dst register where mask is
 * set (dst = src0 * src1 + dst).
 *
 * @override_id asc_mula__Rvi32_vi32_vi32_vb
 * @archs 3510
 * @param [in,out] dst Accumulator and result register of type vector_int32_t: its current elements are read as the
 * addend and replaced by the fused result, with elements whose mask bit is 0 cleared.
 * @param [in] src0 First int32_t multiplicand register; read only, elementwise.
 * @param [in] src1 Second int32_t multiplicand register; read only, elementwise.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit lets the corresponding element participate, a 0 bit
 * excludes it and zeroes that dst element.
 */
__simd_callee__ inline void asc_mula(vector_int32_t& dst, vector_int32_t src0, vector_int32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Accumulates the elementwise product of the uint32_t src0 and src1 registers into the dst register where mask
 * is set (dst = src0 * src1 + dst).
 *
 * @override_id asc_mula__Rvu32_vu32_vu32_vb
 * @archs 3510
 * @param [in,out] dst Accumulator and result register of type vector_uint32_t: its current elements are read as the
 * addend and replaced by the fused result, with elements whose mask bit is 0 cleared.
 * @param [in] src0 First uint32_t multiplicand register; read only, elementwise.
 * @param [in] src1 Second uint32_t multiplicand register; read only, elementwise.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit lets the corresponding element participate, a 0 bit
 * excludes it and zeroes that dst element.
 */
__simd_callee__ inline void asc_mula(
    vector_uint32_t& dst, vector_uint32_t src0, vector_uint32_t src1, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Accumulates the elementwise product of the float src0 and src1 registers into the dst register where mask is
 * set (dst = src0 * src1 + dst).
 *
 * @override_id asc_mula__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [in,out] dst Accumulator and result register of type vector_float: its current elements are read as the addend
 * and replaced by the fused result, with elements whose mask bit is 0 cleared.
 * @param [in] src0 First float multiplicand register; read only, elementwise.
 * @param [in] src1 Second float multiplicand register; read only, elementwise.
 * @param [in] mask Predicate register of type vector_bool; a 1 bit lets the corresponding element participate, a 0 bit
 * excludes it and zeroes that dst element.
 */
__simd_callee__ inline void asc_mula(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

//=================asc_fma==================
/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Multiplies src0 by src1 element-wise, adds src2 and returns the half vector result selected by mask.
 *
 * @override_id asc_fma__vf16_vf16_vf16_vb
 * @archs 3510
 * @param [in] src0 First half source vector data register operand of the multiplication.
 * @param [in] src1 Second half source vector data register operand of the multiplication.
 * @param [in] src2 half vector data register added to the product as the accumulator of the fused multiply-add.
 * @param [in] mask Mask register whose set bits mark the elements that participate; elements with a 0 bit are set to 0
 * in the returned vector.
 */
__simd_callee__ inline vector_half asc_fma(vector_half src0, vector_half src1, vector_half src2, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Multiplies src0 by src1 element-wise, adds src2 and returns the bfloat16_t vector result selected by mask.
 *
 * @override_id asc_fma__vbf16_vbf16_vbf16_vb
 * @archs 3510
 * @param [in] src0 First bfloat16_t source vector data register operand of the multiplication.
 * @param [in] src1 Second bfloat16_t source vector data register operand of the multiplication.
 * @param [in] src2 bfloat16_t vector data register added to the product as the accumulator of the fused multiply-add.
 * @param [in] mask Mask register whose set bits mark the elements that participate; elements with a 0 bit are set to 0
 * in the returned vector.
 */
__simd_callee__ inline vector_bfloat16_t asc_fma(
    vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bfloat16_t src2, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_fused
 * @brief Multiplies src0 by src1 element-wise, adds src2 and returns the float vector result selected by mask.
 *
 * @override_id asc_fma__vf32_vf32_vf32_vb
 * @archs 3510
 * @param [in] src0 First float source vector data register operand of the multiplication.
 * @param [in] src1 Second float source vector data register operand of the multiplication.
 * @param [in] src2 float vector data register added to the product as the accumulator of the fused multiply-add.
 * @param [in] mask Mask register whose set bits mark the elements that participate; elements with a 0 bit are set to 0
 * in the returned vector.
 */
__simd_callee__ inline vector_float asc_fma(vector_float src0, vector_float src1, vector_float src2, vector_bool mask);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
//=================asc_fms==================
__simd_callee__ inline vector_half asc_fms(vector_half src0, vector_half src1, vector_half src2, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_fms(
    vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bfloat16_t src2, vector_bool mask);

__simd_callee__ inline vector_float asc_fms(vector_float src0, vector_float src1, vector_float src2, vector_bool mask);

//=================asc_fnma==================
__simd_callee__ inline vector_half asc_fnma(vector_half src0, vector_half src1, vector_half src2, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_fnma(
    vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bfloat16_t src2, vector_bool mask);

__simd_callee__ inline vector_float asc_fnma(vector_float src0, vector_float src1, vector_float src2, vector_bool mask);

//=================asc_fnms==================
__simd_callee__ inline vector_half asc_fnms(vector_half src0, vector_half src1, vector_half src2, vector_bool mask);

__simd_callee__ inline vector_bfloat16_t asc_fnms(
    vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bfloat16_t src2, vector_bool mask);

__simd_callee__ inline vector_float asc_fnms(vector_float src0, vector_float src1, vector_float src2, vector_bool mask);
#endif

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar_float2half_rn)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Multiplies every element of float vector src by scalar value and rounds the product to half, writing the
 * results to the even-indexed positions of dst selected by mask.
 *
 * @override_id asc_muls__Rvf16_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the rounded half results at even element indices.
 * @param [in] src Source vector data register of float elements multiplied by value.
 * @param [in] value Scalar multiplier applied to each element of src.
 * @param [in] mask Mask register selecting which elements participate; a 0 bit leaves the corresponding output element
 * zeroed.
 */
__simd_callee__ inline void asc_muls(vector_half& dst, vector_float src, float value, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar_float2half_rn)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Multiplies every element of float vector src by scalar value and rounds the product to half, writing the
 * results to the odd-indexed positions of dst selected by mask.
 *
 * @override_id asc_muls_v2__Rvf16_vf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the rounded half results at odd element indices.
 * @param [in] src Source vector data register of float elements multiplied by value.
 * @param [in] value Scalar multiplier applied to each element of src.
 * @param [in] mask Mask register selecting which elements participate; a 0 bit leaves the corresponding output element
 * zeroed.
 */
__simd_callee__ inline void asc_muls_v2(vector_half& dst, vector_float src, float value, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_exp_sub_half2float)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated exp_sub prototype that writes exp(src0 - src1) of the half inputs into the float destination
 * register.
 *
 * @override_id asc_exp_sub__Rvf32_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_float data register receiving exp(src0 - src1) of the participating elements.
 * @param [in] src0 Minuend vector_half data register.
 * @param [in] src1 Subtrahend vector_half data register.
 * @param [in] mask Mask register marking the participating elements; elements with a 0 bit are set to 0 in the output.
 */
__simd_callee__ inline void asc_exp_sub(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_exp_sub_half2float)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated exp_sub prototype that writes exp(src0 - src1) of the half inputs into the float destination
 * register.
 *
 * @override_id asc_exp_sub_v2__Rvf32_vf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector_float data register receiving exp(src0 - src1) of the participating elements.
 * @param [in] src0 Minuend vector_half data register.
 * @param [in] src1 Subtrahend vector_half data register.
 * @param [in] mask Mask register marking the participating elements; elements with a 0 bit are set to 0 in the output.
 */
__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask);

ASC_DEPRECATED(9.1.0, "2027/09/07", asc_exp_sub)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated exp_sub prototype that writes exp(src0 - src1) of the float inputs into the float destination
 * register.
 *
 * @override_id asc_exp_sub_v2__Rvf32_vf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector_float data register receiving exp(src0 - src1) of the participating elements.
 * @param [in] src0 Minuend vector_float data register.
 * @param [in] src1 Subtrahend vector_float data register.
 * @param [in] mask Mask register marking the participating elements; elements with a 0 bit are set to 0 in the output.
 */
__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_FUSED_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_FUSED_H
#endif
