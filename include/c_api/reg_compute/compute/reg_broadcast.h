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
#error "compute/reg_broadcast.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_BROADCAST_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_BROADCAST_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_BROADCAST_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_broadcast_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the uint8_t scalar value into the mask-selected lanes of vector_uint8_t dst and zeroes the lanes
 * that mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvu8_u8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value uint8_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint8_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the int8_t scalar value into the mask-selected lanes of vector_int8_t dst and zeroes the lanes that
 * mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvi8_i8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value int8_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int8_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the fp8_e4m3fn_t scalar value into the mask-selected lanes of vector_fp8_e4m3fn_t dst and zeroes
 * the lanes that mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvf8e4m3_f8e4m3_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value fp8_e4m3fn_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e4m3fn_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the fp8_e5m2_t scalar value into the mask-selected lanes of vector_fp8_e5m2_t dst and zeroes the
 * lanes that mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvf8e5m2_f8e5m2_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value fp8_e5m2_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e5m2_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e5m2_t& dst, fp8_e5m2_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the fp8_e8m0_t scalar value into the mask-selected lanes of vector_fp8_e8m0_t dst and zeroes the
 * lanes that mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvf8e8m0_f8e8m0_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value fp8_e8m0_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e8m0_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e8m0_t& dst, fp8_e8m0_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the uint16_t scalar value into the mask-selected lanes of vector_uint16_t dst and zeroes the lanes
 * that mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvu16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value uint16_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint16_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_uint16_t& dst, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the int16_t scalar value into the mask-selected lanes of vector_int16_t dst and zeroes the lanes
 * that mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvi16_i16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value int16_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int16_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_int16_t& dst, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the half scalar value into the mask-selected lanes of vector_half dst and zeroes the lanes that
 * mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvf16_f16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value half scalar to broadcast; for the value-returning overload its type also fixes the return type, so
 * pass an explicitly typed expression such as static_cast<half>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_half& dst, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the bfloat16_t scalar value into the mask-selected lanes of vector_bfloat16_t dst and zeroes the
 * lanes that mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvbf16_bf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value bfloat16_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<bfloat16_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_bfloat16_t& dst, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the uint32_t scalar value into the mask-selected lanes of vector_uint32_t dst and zeroes the lanes
 * that mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvu32_u32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value uint32_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint32_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_uint32_t& dst, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the int32_t scalar value into the mask-selected lanes of vector_int32_t dst and zeroes the lanes
 * that mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvi32_i32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value int32_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int32_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_int32_t& dst, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the float scalar value into the mask-selected lanes of vector_float dst and zeroes the lanes that
 * mask does not select.
 *
 * @override_id asc_duplicate_scalar__Rvf32_f32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register written with the broadcast result: the lanes marked in mask receive
 * value and all other lanes are set to zero; its element type selects this overload.
 * @param [in] value float scalar to broadcast; for the value-returning overload its type also fixes the return type, so
 * pass an explicitly typed expression such as static_cast<float>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_float& dst, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_uint8_t in which every lane selected by mask holds the broadcast uint8_t scalar value and
 * every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__u8_vb
 * @archs 3510
 * @param [in] value uint8_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint8_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_uint8_t asc_duplicate_scalar(uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_int8_t in which every lane selected by mask holds the broadcast int8_t scalar value and
 * every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__i8_vb
 * @archs 3510
 * @param [in] value int8_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int8_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_fp8_e4m3fn_t in which every lane selected by mask holds the broadcast fp8_e4m3fn_t scalar
 * value and every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__f8e4m3_vb
 * @archs 3510
 * @param [in] value fp8_e4m3fn_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e4m3fn_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_duplicate_scalar(fp8_e4m3fn_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_fp8_e5m2_t in which every lane selected by mask holds the broadcast fp8_e5m2_t scalar
 * value and every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__f8e5m2_vb
 * @archs 3510
 * @param [in] value fp8_e5m2_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e5m2_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_duplicate_scalar(fp8_e5m2_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_fp8_e8m0_t in which every lane selected by mask holds the broadcast fp8_e8m0_t scalar
 * value and every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__f8e8m0_vb
 * @archs 3510
 * @param [in] value fp8_e8m0_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e8m0_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_duplicate_scalar(fp8_e8m0_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_uint16_t in which every lane selected by mask holds the broadcast uint16_t scalar value
 * and every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__u16_vb
 * @archs 3510
 * @param [in] value uint16_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint16_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_uint16_t asc_duplicate_scalar(uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_int16_t in which every lane selected by mask holds the broadcast int16_t scalar value and
 * every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__i16_vb
 * @archs 3510
 * @param [in] value int16_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int16_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_int16_t asc_duplicate_scalar(int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_half in which every lane selected by mask holds the broadcast half scalar value and every
 * unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__f16_vb
 * @archs 3510
 * @param [in] value half scalar to broadcast; for the value-returning overload its type also fixes the return type, so
 * pass an explicitly typed expression such as static_cast<half>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_half asc_duplicate_scalar(half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_bfloat16_t in which every lane selected by mask holds the broadcast bfloat16_t scalar
 * value and every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__bf16_vb
 * @archs 3510
 * @param [in] value bfloat16_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<bfloat16_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_bfloat16_t asc_duplicate_scalar(bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_uint32_t in which every lane selected by mask holds the broadcast uint32_t scalar value
 * and every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__u32_vb
 * @archs 3510
 * @param [in] value uint32_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint32_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_uint32_t asc_duplicate_scalar(uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_int32_t in which every lane selected by mask holds the broadcast int32_t scalar value and
 * every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__i32_vb
 * @archs 3510
 * @param [in] value int32_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int32_t>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_int32_t asc_duplicate_scalar(int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_float in which every lane selected by mask holds the broadcast float scalar value and
 * every unselected lane is zero.
 *
 * @override_id asc_duplicate_scalar__f32_vb
 * @archs 3510
 * @param [in] value float scalar to broadcast; for the value-returning overload its type also fixes the return type, so
 * pass an explicitly typed expression such as static_cast<float>(1).
 * @param [in] mask Mask register selecting the destination lanes to fill with value; a lane whose mask bit is 0 is set
 * to 0 in dst.
 */
__simd_callee__ inline vector_float asc_duplicate_scalar(float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the unsigned 8-bit integer scalar value into the mask-selected lanes of dst and preserves the lanes
 * whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvu8_u8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of unsigned 8-bit integer data, read and updated in place: mask bit 1
 * lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type uint8_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_uint8_t& dst, uint8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the signed 8-bit integer scalar value into the mask-selected lanes of dst and preserves the lanes
 * whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvi8_i8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of signed 8-bit integer data, read and updated in place: mask bit 1
 * lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type int8_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_int8_t& dst, int8_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the 8-bit FP8 E4M3FN scalar value into the mask-selected lanes of dst and preserves the lanes whose
 * mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvf8e4m3_f8e4m3_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 8-bit FP8 E4M3FN data, read and updated in place: mask bit 1 lanes
 * receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type fp8_e4m3fn_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the 8-bit FP8 E5M2 scalar value into the mask-selected lanes of dst and preserves the lanes whose
 * mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvf8e5m2_f8e5m2_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 8-bit FP8 E5M2 data, read and updated in place: mask bit 1 lanes
 * receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type fp8_e5m2_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_fp8_e5m2_t& dst, fp8_e5m2_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the 8-bit FP8 E8M0 (exponent only) scalar value into the mask-selected lanes of dst and preserves
 * the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvf8e8m0_f8e8m0_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 8-bit FP8 E8M0 (exponent only) data, read and updated in place:
 * mask bit 1 lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type fp8_e8m0_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_fp8_e8m0_t& dst, fp8_e8m0_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the unsigned 16-bit integer scalar value into the mask-selected lanes of dst and preserves the
 * lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvu16_u16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of unsigned 16-bit integer data, read and updated in place: mask bit
 * 1 lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type uint16_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_uint16_t& dst, uint16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the signed 16-bit integer scalar value into the mask-selected lanes of dst and preserves the lanes
 * whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvi16_i16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of signed 16-bit integer data, read and updated in place: mask bit 1
 * lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type int16_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_int16_t& dst, int16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the 16-bit floating point (half) scalar value into the mask-selected lanes of dst and preserves the
 * lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvf16_f16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 16-bit floating point (half) data, read and updated in place: mask
 * bit 1 lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type half holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_half& dst, half value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the 16-bit brain float (bfloat16_t) scalar value into the mask-selected lanes of dst and preserves
 * the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvbf16_bf16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 16-bit brain float (bfloat16_t) data, read and updated in place:
 * mask bit 1 lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type bfloat16_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_bfloat16_t& dst, bfloat16_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the unsigned 32-bit integer scalar value into the mask-selected lanes of dst and preserves the
 * lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvu32_u32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of unsigned 32-bit integer data, read and updated in place: mask bit
 * 1 lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type uint32_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_uint32_t& dst, uint32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the signed 32-bit integer scalar value into the mask-selected lanes of dst and preserves the lanes
 * whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvi32_i32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of signed 32-bit integer data, read and updated in place: mask bit 1
 * lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type int32_t holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_int32_t& dst, int32_t value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the 32-bit floating point (float) scalar value into the mask-selected lanes of dst and preserves
 * the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_scalar_merge__Rvf32_f32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 32-bit floating point (float) data, read and updated in place:
 * mask bit 1 lanes receive value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] value Scalar of type float holding the value that is broadcast across the selected lanes of dst.
 * @param [in] mask Mask register over dst lanes: bit 1 writes value into that lane, bit 0 keeps that lane's previous
 * value.
 */
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_float& dst, float value, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the uint8_t scalar value into every lane of vector_uint8_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvu8_u8
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value uint8_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint8_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_uint8_t& dst, uint8_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the int8_t scalar value into every lane of vector_int8_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvi8_i8
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value int8_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int8_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst, int8_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the uint16_t scalar value into every lane of vector_uint16_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvu16_u16
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value uint16_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint16_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_uint16_t& dst, uint16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the int16_t scalar value into every lane of vector_int16_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvi16_i16
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value int16_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int16_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_int16_t& dst, int16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the uint32_t scalar value into every lane of vector_uint32_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvu32_u32
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value uint32_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint32_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_uint32_t& dst, uint32_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the int32_t scalar value into every lane of vector_int32_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value int32_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int32_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_int32_t& dst, int32_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the half scalar value into every lane of vector_half dst.
 *
 * @override_id asc_duplicate_scalar__Rvf16_f16
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value half scalar to broadcast; for the value-returning overload its type also fixes the return type, so
 * pass an explicitly typed expression such as static_cast<half>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_half& dst, half value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the float scalar value into every lane of vector_float dst.
 *
 * @override_id asc_duplicate_scalar__Rvf32_f32
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value float scalar to broadcast; for the value-returning overload its type also fixes the return type, so
 * pass an explicitly typed expression such as static_cast<float>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_float& dst, float value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the bfloat16_t scalar value into every lane of vector_bfloat16_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvbf16_bf16
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value bfloat16_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<bfloat16_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_bfloat16_t& dst, bfloat16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the fp8_e4m3fn_t scalar value into every lane of vector_fp8_e4m3fn_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvf8e4m3_f8e4m3
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value fp8_e4m3fn_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e4m3fn_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e4m3fn_t& dst, fp8_e4m3fn_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the fp8_e5m2_t scalar value into every lane of vector_fp8_e5m2_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvf8e5m2_f8e5m2
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value fp8_e5m2_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e5m2_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e5m2_t& dst, fp8_e5m2_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the fp8_e8m0_t scalar value into every lane of vector_fp8_e8m0_t dst.
 *
 * @override_id asc_duplicate_scalar__Rvf8e8m0_f8e8m0
 * @archs 3510
 * @param [out] dst Destination vector data register written with the scalar in every lane; its element type selects
 * this overload.
 * @param [in] value fp8_e8m0_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e8m0_t>(1).
 */
__simd_callee__ inline void asc_duplicate_scalar(vector_fp8_e8m0_t& dst, fp8_e8m0_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_uint8_t with all lanes set to the uint8_t scalar value.
 *
 * @override_id asc_duplicate_scalar__u8
 * @archs 3510
 * @param [in] value uint8_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint8_t>(1).
 */
__simd_callee__ inline vector_uint8_t asc_duplicate_scalar(uint8_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_int8_t with all lanes set to the int8_t scalar value.
 *
 * @override_id asc_duplicate_scalar__i8
 * @archs 3510
 * @param [in] value int8_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int8_t>(1).
 */
__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_uint16_t with all lanes set to the uint16_t scalar value.
 *
 * @override_id asc_duplicate_scalar__u16
 * @archs 3510
 * @param [in] value uint16_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint16_t>(1).
 */
__simd_callee__ inline vector_uint16_t asc_duplicate_scalar(uint16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_int16_t with all lanes set to the int16_t scalar value.
 *
 * @override_id asc_duplicate_scalar__i16
 * @archs 3510
 * @param [in] value int16_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int16_t>(1).
 */
__simd_callee__ inline vector_int16_t asc_duplicate_scalar(int16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_uint32_t with all lanes set to the uint32_t scalar value.
 *
 * @override_id asc_duplicate_scalar__u32
 * @archs 3510
 * @param [in] value uint32_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<uint32_t>(1).
 */
__simd_callee__ inline vector_uint32_t asc_duplicate_scalar(uint32_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_int32_t with all lanes set to the int32_t scalar value.
 *
 * @override_id asc_duplicate_scalar__i32
 * @archs 3510
 * @param [in] value int32_t scalar to broadcast; for the value-returning overload its type also fixes the return type,
 * so pass an explicitly typed expression such as static_cast<int32_t>(1).
 */
__simd_callee__ inline vector_int32_t asc_duplicate_scalar(int32_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_half with all lanes set to the half scalar value.
 *
 * @override_id asc_duplicate_scalar__f16
 * @archs 3510
 * @param [in] value half scalar to broadcast; for the value-returning overload its type also fixes the return type, so
 * pass an explicitly typed expression such as static_cast<half>(1).
 */
__simd_callee__ inline vector_half asc_duplicate_scalar(half value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_float with all lanes set to the float scalar value.
 *
 * @override_id asc_duplicate_scalar__f32
 * @archs 3510
 * @param [in] value float scalar to broadcast; for the value-returning overload its type also fixes the return type, so
 * pass an explicitly typed expression such as static_cast<float>(1).
 */
__simd_callee__ inline vector_float asc_duplicate_scalar(float value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_bfloat16_t with all lanes set to the bfloat16_t scalar value.
 *
 * @override_id asc_duplicate_scalar__bf16
 * @archs 3510
 * @param [in] value bfloat16_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<bfloat16_t>(1).
 */
__simd_callee__ inline vector_bfloat16_t asc_duplicate_scalar(bfloat16_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_fp8_e4m3fn_t with all lanes set to the fp8_e4m3fn_t scalar value.
 *
 * @override_id asc_duplicate_scalar__f8e4m3
 * @archs 3510
 * @param [in] value fp8_e4m3fn_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e4m3fn_t>(1).
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_duplicate_scalar(fp8_e4m3fn_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_fp8_e5m2_t with all lanes set to the fp8_e5m2_t scalar value.
 *
 * @override_id asc_duplicate_scalar__f8e5m2
 * @archs 3510
 * @param [in] value fp8_e5m2_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e5m2_t>(1).
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_duplicate_scalar(fp8_e5m2_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Returns a new vector_fp8_e8m0_t with all lanes set to the fp8_e8m0_t scalar value.
 *
 * @override_id asc_duplicate_scalar__f8e8m0
 * @archs 3510
 * @param [in] value fp8_e8m0_t scalar to broadcast; for the value-returning overload its type also fixes the return
 * type, so pass an explicitly typed expression such as static_cast<fp8_e8m0_t>(1).
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_duplicate_scalar(fp8_e8m0_t value);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the uint8_t source register vector_uint8_t into the mask-selected positions
 * of dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the broadcast result; its element
 * type must match src.
 * @param [in] src Source vector data register (vector_uint8_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the int8_t source register vector_int8_t into the mask-selected positions of
 * dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the broadcast result; its element
 * type must match src.
 * @param [in] src Source vector data register (vector_int8_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the fp8_e4m3fn_t source register vector_fp8_e4m3fn_t into the mask-selected
 * positions of dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the broadcast result; its
 * element type must match src.
 * @param [in] src Source vector data register (vector_fp8_e4m3fn_t); its lowest element, that is the element at index
 * 0, is the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the fp8_e5m2_t source register vector_fp8_e5m2_t into the mask-selected
 * positions of dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the broadcast result; its element
 * type must match src.
 * @param [in] src Source vector data register (vector_fp8_e5m2_t); its lowest element, that is the element at index 0,
 * is the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the fp8_e8m0_t source register vector_fp8_e8m0_t into the mask-selected
 * positions of dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvf8e8m0_vf8e8m0_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the broadcast result; its element
 * type must match src.
 * @param [in] src Source vector data register (vector_fp8_e8m0_t); its lowest element, that is the element at index 0,
 * is the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the uint16_t source register vector_uint16_t into the mask-selected positions
 * of dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the broadcast result; its element
 * type must match src.
 * @param [in] src Source vector data register (vector_uint16_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the int16_t source register vector_int16_t into the mask-selected positions
 * of dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the broadcast result; its element
 * type must match src.
 * @param [in] src Source vector data register (vector_int16_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the half source register vector_half into the mask-selected positions of dst
 * and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the broadcast result; its element type
 * must match src.
 * @param [in] src Source vector data register (vector_half); its lowest element, that is the element at index 0, is the
 * value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the bfloat16_t source register vector_bfloat16_t into the mask-selected
 * positions of dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the broadcast result; its element
 * type must match src.
 * @param [in] src Source vector data register (vector_bfloat16_t); its lowest element, that is the element at index 0,
 * is the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the uint32_t source register vector_uint32_t into the mask-selected positions
 * of dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the broadcast result; its element
 * type must match src.
 * @param [in] src Source vector data register (vector_uint32_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the int32_t source register vector_int32_t into the mask-selected positions
 * of dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the broadcast result; its element
 * type must match src.
 * @param [in] src Source vector data register (vector_int32_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the float source register vector_float into the mask-selected positions of
 * dst and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the broadcast result; its element type
 * must match src.
 * @param [in] src Source vector data register (vector_float); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline void asc_duplicate(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the uint8_t source register vector_uint8_t to the mask-selected positions of
 * a returned vector_uint8_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vu8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint8_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_uint8_t asc_duplicate(vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the int8_t source register vector_int8_t to the mask-selected positions of a
 * returned vector_int8_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vi8_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int8_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_int8_t asc_duplicate(vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the fp8_e4m3fn_t source register vector_fp8_e4m3fn_t to the mask-selected
 * positions of a returned vector_fp8_e4m3fn_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vf8e4m3_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_fp8_e4m3fn_t); its lowest element, that is the element at index
 * 0, is the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_duplicate(vector_fp8_e4m3fn_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the fp8_e5m2_t source register vector_fp8_e5m2_t to the mask-selected
 * positions of a returned vector_fp8_e5m2_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vf8e5m2_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_fp8_e5m2_t); its lowest element, that is the element at index 0,
 * is the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_duplicate(vector_fp8_e5m2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the fp8_e8m0_t source register vector_fp8_e8m0_t to the mask-selected
 * positions of a returned vector_fp8_e8m0_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vf8e8m0_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_fp8_e8m0_t); its lowest element, that is the element at index 0,
 * is the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_duplicate(vector_fp8_e8m0_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the uint16_t source register vector_uint16_t to the mask-selected positions
 * of a returned vector_uint16_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vu16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint16_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_uint16_t asc_duplicate(vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the int16_t source register vector_int16_t to the mask-selected positions of
 * a returned vector_int16_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vi16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int16_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_int16_t asc_duplicate(vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the half source register vector_half to the mask-selected positions of a
 * returned vector_half result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vf16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_half); its lowest element, that is the element at index 0, is the
 * value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_half asc_duplicate(vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the bfloat16_t source register vector_bfloat16_t to the mask-selected
 * positions of a returned vector_bfloat16_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vbf16_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_bfloat16_t); its lowest element, that is the element at index 0,
 * is the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_bfloat16_t asc_duplicate(vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the uint32_t source register vector_uint32_t to the mask-selected positions
 * of a returned vector_uint32_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vu32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_uint32_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_uint32_t asc_duplicate(vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the int32_t source register vector_int32_t to the mask-selected positions of
 * a returned vector_int32_t result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vi32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_int32_t); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_int32_t asc_duplicate(vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest element of the float source register vector_float to the mask-selected positions of a
 * returned vector_float result and zeroes the positions that mask does not select.
 *
 * @override_id asc_duplicate__vf32_vb
 * @archs 3510
 * @param [in] src Source vector data register (vector_float); its lowest element, that is the element at index 0, is
 * the value that is broadcast.
 * @param [in] mask Mask register selecting the destination positions that receive the broadcast value; positions not
 * selected are written as zero and the mask never filters the element read from src.
 */
__simd_callee__ inline vector_float asc_duplicate(vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the unsigned 8-bit integer src into the mask-selected lanes of dst and
 * sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvu8_vu8_vb
 * @archs 3510
 * @param [out] dst Destination vector register of unsigned 8-bit integer data: mask bit 1 lanes receive the broadcast
 * value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the signed 8-bit integer src into the mask-selected lanes of dst and
 * sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvi8_vi8_vb
 * @archs 3510
 * @param [out] dst Destination vector register of signed 8-bit integer data: mask bit 1 lanes receive the broadcast
 * value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 8-bit FP8 E4M3FN src into the mask-selected lanes of dst and sets
 * every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [out] dst Destination vector register of 8-bit FP8 E4M3FN data: mask bit 1 lanes receive the broadcast value
 * and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 8-bit FP8 E5M2 src into the mask-selected lanes of dst and sets
 * every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [out] dst Destination vector register of 8-bit FP8 E5M2 data: mask bit 1 lanes receive the broadcast value and
 * mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 8-bit FP8 E8M0 (exponent only) src into the mask-selected lanes of
 * dst and sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvf8e8m0_vf8e8m0_vb
 * @archs 3510
 * @param [out] dst Destination vector register of 8-bit FP8 E8M0 (exponent only) data: mask bit 1 lanes receive the
 * broadcast value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the unsigned 16-bit integer src into the mask-selected lanes of dst
 * and sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector register of unsigned 16-bit integer data: mask bit 1 lanes receive the broadcast
 * value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the signed 16-bit integer src into the mask-selected lanes of dst and
 * sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvi16_vi16_vb
 * @archs 3510
 * @param [out] dst Destination vector register of signed 16-bit integer data: mask bit 1 lanes receive the broadcast
 * value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 16-bit floating point (half) src into the mask-selected lanes of
 * dst and sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvf16_vf16_vb
 * @archs 3510
 * @param [out] dst Destination vector register of 16-bit floating point (half) data: mask bit 1 lanes receive the
 * broadcast value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 16-bit brain float (bfloat16_t) src into the mask-selected lanes
 * of dst and sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvbf16_vbf16_vb
 * @archs 3510
 * @param [out] dst Destination vector register of 16-bit brain float (bfloat16_t) data: mask bit 1 lanes receive the
 * broadcast value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the unsigned 32-bit integer src into the mask-selected lanes of dst
 * and sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector register of unsigned 32-bit integer data: mask bit 1 lanes receive the broadcast
 * value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the signed 32-bit integer src into the mask-selected lanes of dst and
 * sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvi32_vi32_vb
 * @archs 3510
 * @param [out] dst Destination vector register of signed 32-bit integer data: mask bit 1 lanes receive the broadcast
 * value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 32-bit floating point (float) src into the mask-selected lanes of
 * dst and sets every unselected lane to zero.
 *
 * @override_id asc_duplicate_highest__Rvf32_vf32_vb
 * @archs 3510
 * @param [out] dst Destination vector register of 32-bit floating point (float) data: mask bit 1 lanes receive the
 * broadcast value and mask bit 0 lanes are forced to zero.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 forces that
 * lane to 0.
 */
__simd_callee__ inline void asc_duplicate_highest(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the unsigned 8-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvu8_vu8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of unsigned 8-bit integer data, read and updated in place: mask bit 1
 * lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the signed 8-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvi8_vi8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of signed 8-bit integer data, read and updated in place: mask bit 1
 * lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the 8-bit FP8 E4M3FN src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 8-bit FP8 E4M3FN data, read and updated in place: mask bit 1 lanes
 * receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the 8-bit FP8 E5M2 src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 8-bit FP8 E5M2 data, read and updated in place: mask bit 1 lanes
 * receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the 8-bit FP8 E8M0 (exponent only) src into the mask-selected lanes of
 * dst and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvf8e8m0_vf8e8m0_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 8-bit FP8 E8M0 (exponent only) data, read and updated in place:
 * mask bit 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the unsigned 16-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvu16_vu16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of unsigned 16-bit integer data, read and updated in place: mask bit
 * 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the signed 16-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvi16_vi16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of signed 16-bit integer data, read and updated in place: mask bit 1
 * lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the 16-bit floating point (half) src into the mask-selected lanes of
 * dst and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvf16_vf16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 16-bit floating point (half) data, read and updated in place: mask
 * bit 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the 16-bit brain float (bfloat16_t) src into the mask-selected lanes of
 * dst and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvbf16_vbf16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 16-bit brain float (bfloat16_t) data, read and updated in place:
 * mask bit 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the unsigned 32-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvu32_vu32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of unsigned 32-bit integer data, read and updated in place: mask bit
 * 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the signed 32-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvi32_vi32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of signed 32-bit integer data, read and updated in place: mask bit 1
 * lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the lowest-index element of the 32-bit floating point (float) src into the mask-selected lanes of
 * dst and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_merge__Rvf32_vf32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 32-bit floating point (float) data, read and updated in place:
 * mask bit 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose lowest-index (first) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_merge(vector_float& dst, vector_float src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the unsigned 8-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvu8_vu8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of unsigned 8-bit integer data, read and updated in place: mask bit 1
 * lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint8_t& dst, vector_uint8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the signed 8-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvi8_vi8_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of signed 8-bit integer data, read and updated in place: mask bit 1
 * lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(vector_int8_t& dst, vector_int8_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 8-bit FP8 E4M3FN src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvf8e4m3_vf8e4m3_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 8-bit FP8 E4M3FN data, read and updated in place: mask bit 1 lanes
 * receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(
    vector_fp8_e4m3fn_t& dst, vector_fp8_e4m3fn_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 8-bit FP8 E5M2 src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvf8e5m2_vf8e5m2_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 8-bit FP8 E5M2 data, read and updated in place: mask bit 1 lanes
 * receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(
    vector_fp8_e5m2_t& dst, vector_fp8_e5m2_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 8-bit FP8 E8M0 (exponent only) src into the mask-selected lanes of
 * dst and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvf8e8m0_vf8e8m0_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 8-bit FP8 E8M0 (exponent only) data, read and updated in place:
 * mask bit 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(
    vector_fp8_e8m0_t& dst, vector_fp8_e8m0_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the unsigned 16-bit integer src into the mask-selected lanes of dst
 * and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvu16_vu16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of unsigned 16-bit integer data, read and updated in place: mask bit
 * 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint16_t& dst, vector_uint16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the signed 16-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvi16_vi16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of signed 16-bit integer data, read and updated in place: mask bit 1
 * lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(vector_int16_t& dst, vector_int16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 16-bit floating point (half) src into the mask-selected lanes of
 * dst and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvf16_vf16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 16-bit floating point (half) data, read and updated in place: mask
 * bit 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(vector_half& dst, vector_half src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 16-bit brain float (bfloat16_t) src into the mask-selected lanes
 * of dst and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvbf16_vbf16_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 16-bit brain float (bfloat16_t) data, read and updated in place:
 * mask bit 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(
    vector_bfloat16_t& dst, vector_bfloat16_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the unsigned 32-bit integer src into the mask-selected lanes of dst
 * and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvu32_vu32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of unsigned 32-bit integer data, read and updated in place: mask bit
 * 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(vector_uint32_t& dst, vector_uint32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the signed 32-bit integer src into the mask-selected lanes of dst and
 * preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvi32_vi32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of signed 32-bit integer data, read and updated in place: mask bit 1
 * lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(vector_int32_t& dst, vector_int32_t src, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.reg_broadcast
 * @brief Broadcasts the highest-index element of the 32-bit floating point (float) src into the mask-selected lanes of
 * dst and preserves the lanes whose mask bit is 0.
 *
 * @override_id asc_duplicate_highest_merge__Rvf32_vf32_vb
 * @archs 3510
 * @param [in,out] dst Destination vector register of 32-bit floating point (float) data, read and updated in place:
 * mask bit 1 lanes receive the broadcast value while mask bit 0 lanes keep the value they had before the call.
 * @param [in] src Source vector register whose highest-index (last) element is the value broadcast to dst; mask never
 * selects the read element.
 * @param [in] mask Mask register over dst lanes: bit 1 writes the broadcast value into that lane, bit 0 keeps that
 * lane's previous value.
 */
__simd_callee__ inline void asc_duplicate_highest_merge(vector_float& dst, vector_float src, vector_bool mask);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_BROADCAST_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_BROADCAST_H
#endif
