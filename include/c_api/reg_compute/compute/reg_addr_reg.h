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
#error "compute/reg_addr_reg.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_ADDR_REG_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_ADDR_REG_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_ADDR_REG_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_addr_reg_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with a single innermost-dimension element offset in b32 mode so that
 * subsequent load/store operations auto-increment the Unified Buffer address.
 *
 * @override_id asc_update_addr_reg_b32__u32
 * @archs 3510
 * @param [in] offset Offset of the first (innermost) dimension in elements; in b32 mode one element advances the UB
 * address by 4 byte(s), e.g. b32 offset=64 addresses 64 float elements, one 256-byte VL.
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with a single innermost-dimension element offset in b16 mode so that
 * subsequent load/store operations auto-increment the Unified Buffer address.
 *
 * @override_id asc_update_addr_reg_b16__u32
 * @archs 3510
 * @param [in] offset Offset of the first (innermost) dimension in elements; in b16 mode one element advances the UB
 * address by 2 byte(s) for any 2-byte element type.
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with a single innermost-dimension element offset in b8 mode so that
 * subsequent load/store operations auto-increment the Unified Buffer address.
 *
 * @override_id asc_update_addr_reg_b8__u32
 * @archs 3510
 * @param [in] offset Offset of the first (innermost) dimension in elements; in b8 mode one element advances the UB
 * address by 1 byte(s) for any 1-byte element type.
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with 2 element offsets covering 2 tensor dimensions in b32 mode, so that
 * asc_loadalign/asc_storealign auto-increment the Unified Buffer address during iteration.
 *
 * @override_id asc_update_addr_reg_b32__u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first (innermost, e.g. W) dimension in elements; in b32 mode one element advances
 * the UB address by 4 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b32 mode one element advances the UB address by 4 byte(s).
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset0, uint32_t offset1);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with 2 element offsets covering 2 tensor dimensions in b16 mode, so that
 * asc_loadalign/asc_storealign auto-increment the Unified Buffer address during iteration.
 *
 * @override_id asc_update_addr_reg_b16__u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first (innermost, e.g. W) dimension in elements; in b16 mode one element advances
 * the UB address by 2 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b16 mode one element advances the UB address by 2 byte(s).
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset0, uint32_t offset1);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with 2 element offsets covering 2 tensor dimensions in b8 mode, so that
 * asc_loadalign/asc_storealign auto-increment the Unified Buffer address during iteration.
 *
 * @override_id asc_update_addr_reg_b8__u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first (innermost, e.g. W) dimension in elements; in b8 mode one element advances
 * the UB address by 1 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b8 mode one element advances the UB address by 1 byte(s).
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset0, uint32_t offset1);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with 3 element offsets covering 3 tensor dimensions in b32 mode, so that
 * asc_loadalign/asc_storealign auto-increment the Unified Buffer address during iteration.
 *
 * @override_id asc_update_addr_reg_b32__u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first (innermost, e.g. W) dimension in elements; in b32 mode one element advances
 * the UB address by 4 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b32 mode one element advances the UB address by 4 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b32 mode one element advances the UB address by 4 byte(s).
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b32(uint32_t offset0, uint32_t offset1, uint32_t offset2);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with 3 element offsets covering 3 tensor dimensions in b16 mode, so that
 * asc_loadalign/asc_storealign auto-increment the Unified Buffer address during iteration.
 *
 * @override_id asc_update_addr_reg_b16__u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first (innermost, e.g. W) dimension in elements; in b16 mode one element advances
 * the UB address by 2 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b16 mode one element advances the UB address by 2 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b16 mode one element advances the UB address by 2 byte(s).
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b16(uint32_t offset0, uint32_t offset1, uint32_t offset2);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with 3 element offsets covering 3 tensor dimensions in b8 mode, so that
 * asc_loadalign/asc_storealign auto-increment the Unified Buffer address during iteration.
 *
 * @override_id asc_update_addr_reg_b8__u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first (innermost, e.g. W) dimension in elements; in b8 mode one element advances
 * the UB address by 1 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b8 mode one element advances the UB address by 1 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b8 mode one element advances the UB address by 1 byte(s).
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b8(uint32_t offset0, uint32_t offset1, uint32_t offset2);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with 4 element offsets covering 4 tensor dimensions in b32 mode, so that
 * asc_loadalign/asc_storealign auto-increment the Unified Buffer address during iteration.
 *
 * @override_id asc_update_addr_reg_b32__u32_u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first (innermost, e.g. W) dimension in elements; in b32 mode one element advances
 * the UB address by 4 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b32 mode one element advances the UB address by 4 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b32 mode one element advances the UB address by 4 byte(s).
 * @param [in] offset3 Offset of the fourth dimension (outermost, cumulative stride, e.g. N*C*H*W in an N,C,H,W layout)
 * in elements, i.e. the cumulative stride of that dimension; in b32 mode one element advances the UB address by 4
 * byte(s).
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b32(
    uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with 4 element offsets covering 4 tensor dimensions in b16 mode, so that
 * asc_loadalign/asc_storealign auto-increment the Unified Buffer address during iteration.
 *
 * @override_id asc_update_addr_reg_b16__u32_u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first (innermost, e.g. W) dimension in elements; in b16 mode one element advances
 * the UB address by 2 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b16 mode one element advances the UB address by 2 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b16 mode one element advances the UB address by 2 byte(s).
 * @param [in] offset3 Offset of the fourth dimension (outermost, cumulative stride, e.g. N*C*H*W in an N,C,H,W layout)
 * in elements, i.e. the cumulative stride of that dimension; in b16 mode one element advances the UB address by 2
 * byte(s).
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b16(
    uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3);

/**
 * @ingroup simd_c_api.reg_compute.reg_addr_reg
 * @brief Returns an address register seeded with 4 element offsets covering 4 tensor dimensions in b8 mode, so that
 * asc_loadalign/asc_storealign auto-increment the Unified Buffer address during iteration.
 *
 * @override_id asc_update_addr_reg_b8__u32_u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first (innermost, e.g. W) dimension in elements; in b8 mode one element advances
 * the UB address by 1 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b8 mode one element advances the UB address by 1 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements,
 * i.e. the cumulative stride of that dimension; in b8 mode one element advances the UB address by 1 byte(s).
 * @param [in] offset3 Offset of the fourth dimension (outermost, cumulative stride, e.g. N*C*H*W in an N,C,H,W layout)
 * in elements, i.e. the cumulative stride of that dimension; in b8 mode one element advances the UB address by 1
 * byte(s).
 */
__simd_callee__ inline addr_reg asc_update_addr_reg_b8(
    uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b32)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 1 element offset(s) (offset) in b32 mode,
 * and is the equivalent of asc_update_addr_reg_b32.
 *
 * @override_id asc_create_iter_reg_b32__u32
 * @archs 3510
 * @param [in] offset Offset of the single innermost dimension in elements; in b32 mode one element advances the UB
 * address by 4 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b32(uint32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b16)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 1 element offset(s) (offset) in b16 mode,
 * and is the equivalent of asc_update_addr_reg_b16.
 *
 * @override_id asc_create_iter_reg_b16__u32
 * @archs 3510
 * @param [in] offset Offset of the single innermost dimension in elements; in b16 mode one element advances the UB
 * address by 2 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b16(uint32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b8)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 1 element offset(s) (offset) in b8 mode,
 * and is the equivalent of asc_update_addr_reg_b8.
 *
 * @override_id asc_create_iter_reg_b8__u32
 * @archs 3510
 * @param [in] offset Offset of the single innermost dimension in elements; in b8 mode one element advances the UB
 * address by 1 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b8(uint32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b32)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 2 element offset(s) (offset0, offset1) in
 * b32 mode, and is the equivalent of asc_update_addr_reg_b32.
 *
 * @override_id asc_create_iter_reg_b32__u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first dimension (innermost, e.g. W) in elements; in b32 mode one element advances
 * the UB address by 4 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements; in
 * b32 mode one element advances the UB address by 4 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b16)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 2 element offset(s) (offset0, offset1) in
 * b16 mode, and is the equivalent of asc_update_addr_reg_b16.
 *
 * @override_id asc_create_iter_reg_b16__u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first dimension (innermost, e.g. W) in elements; in b16 mode one element advances
 * the UB address by 2 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements; in
 * b16 mode one element advances the UB address by 2 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b8)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 2 element offset(s) (offset0, offset1) in
 * b8 mode, and is the equivalent of asc_update_addr_reg_b8.
 *
 * @override_id asc_create_iter_reg_b8__u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first dimension (innermost, e.g. W) in elements; in b8 mode one element advances
 * the UB address by 1 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements; in
 * b8 mode one element advances the UB address by 1 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b32)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 3 element offset(s) (offset0, offset1,
 * offset2) in b32 mode, and is the equivalent of asc_update_addr_reg_b32.
 *
 * @override_id asc_create_iter_reg_b32__u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first dimension (innermost, e.g. W) in elements; in b32 mode one element advances
 * the UB address by 4 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements; in
 * b32 mode one element advances the UB address by 4 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements;
 * in b32 mode one element advances the UB address by 4 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b32(uint32_t offset0, uint32_t offset1, uint32_t offset2);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b16)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 3 element offset(s) (offset0, offset1,
 * offset2) in b16 mode, and is the equivalent of asc_update_addr_reg_b16.
 *
 * @override_id asc_create_iter_reg_b16__u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first dimension (innermost, e.g. W) in elements; in b16 mode one element advances
 * the UB address by 2 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements; in
 * b16 mode one element advances the UB address by 2 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements;
 * in b16 mode one element advances the UB address by 2 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b16(uint32_t offset0, uint32_t offset1, uint32_t offset2);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b8)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 3 element offset(s) (offset0, offset1,
 * offset2) in b8 mode, and is the equivalent of asc_update_addr_reg_b8.
 *
 * @override_id asc_create_iter_reg_b8__u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first dimension (innermost, e.g. W) in elements; in b8 mode one element advances
 * the UB address by 1 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements; in
 * b8 mode one element advances the UB address by 1 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements;
 * in b8 mode one element advances the UB address by 1 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b8(uint32_t offset0, uint32_t offset1, uint32_t offset2);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b32)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 4 element offset(s) (offset0, offset1,
 * offset2, offset3) in b32 mode, and is the equivalent of asc_update_addr_reg_b32.
 *
 * @override_id asc_create_iter_reg_b32__u32_u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first dimension (innermost, e.g. W) in elements; in b32 mode one element advances
 * the UB address by 4 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements; in
 * b32 mode one element advances the UB address by 4 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements;
 * in b32 mode one element advances the UB address by 4 byte(s).
 * @param [in] offset3 Offset of the fourth dimension (outermost, cumulative stride, e.g. N*C*H*W in an N,C,H,W layout)
 * in elements; in b32 mode one element advances the UB address by 4 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b32(
    uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b16)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 4 element offset(s) (offset0, offset1,
 * offset2, offset3) in b16 mode, and is the equivalent of asc_update_addr_reg_b16.
 *
 * @override_id asc_create_iter_reg_b16__u32_u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first dimension (innermost, e.g. W) in elements; in b16 mode one element advances
 * the UB address by 2 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements; in
 * b16 mode one element advances the UB address by 2 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements;
 * in b16 mode one element advances the UB address by 2 byte(s).
 * @param [in] offset3 Offset of the fourth dimension (outermost, cumulative stride, e.g. N*C*H*W in an N,C,H,W layout)
 * in elements; in b16 mode one element advances the UB address by 2 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b16(
    uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_update_addr_reg_b8)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated initializer that returns an address register seeded with 4 element offset(s) (offset0, offset1,
 * offset2, offset3) in b8 mode, and is the equivalent of asc_update_addr_reg_b8.
 *
 * @override_id asc_create_iter_reg_b8__u32_u32_u32_u32
 * @archs 3510
 * @param [in] offset0 Offset of the first dimension (innermost, e.g. W) in elements; in b8 mode one element advances
 * the UB address by 1 byte(s).
 * @param [in] offset1 Offset of the second dimension (cumulative stride, e.g. H*W in an N,C,H,W layout) in elements; in
 * b8 mode one element advances the UB address by 1 byte(s).
 * @param [in] offset2 Offset of the third dimension (cumulative stride, e.g. C*H*W in an N,C,H,W layout) in elements;
 * in b8 mode one element advances the UB address by 1 byte(s).
 * @param [in] offset3 Offset of the fourth dimension (outermost, cumulative stride, e.g. N*C*H*W in an N,C,H,W layout)
 * in elements; in b8 mode one element advances the UB address by 1 byte(s).
 */
__simd_callee__ inline addr_reg asc_create_iter_reg_b8(
    uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_ADDR_REG_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_ADDR_REG_H
#endif
