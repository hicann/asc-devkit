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
#error "gather/ub_gather.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_GATHER_UB_GATHER_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_GATHER_UB_GATHER_H
#define INCLUDE_C_API_REG_COMPUTE_GATHER_UB_GATHER_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/ub_gather_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUi8_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the int8_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in int8_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_int8_t asc_gather(__ubuf__ int8_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUu8_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the uint8_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in uint8_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_uint8_t asc_gather(__ubuf__ uint8_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and returns the
 * collected vector, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUi16_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the int16_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in int16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_int16_t asc_gather(__ubuf__ int16_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and returns the
 * collected vector, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUu16_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the uint16_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in uint16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_uint16_t asc_gather(__ubuf__ uint16_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and returns the
 * collected vector, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUi32_vu32_vb
 * @archs 3510
 * @param [in] src Start address in UB of the int32_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in int32_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_int32_t asc_gather(__ubuf__ int32_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and returns the
 * collected vector, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUu32_vu32_vb
 * @archs 3510
 * @param [in] src Start address in UB of the uint32_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in uint32_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_uint32_t asc_gather(__ubuf__ uint32_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and returns the
 * collected vector, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUf16_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the half source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in half elements, giving the element gathered
 * into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_half asc_gather(__ubuf__ half* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and returns the
 * collected vector, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUf32_vu32_vb
 * @archs 3510
 * @param [in] src Start address in UB of the float source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in float elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_float asc_gather(__ubuf__ float* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and returns the
 * collected vector, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUbf16_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the bfloat16_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in bfloat16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_bfloat16_t asc_gather(__ubuf__ bfloat16_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUf8e4m3_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source vector; the actual read address must be 32-byte
 * aligned and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in fp8_e4m3fn_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_gather(
    __ubuf__ fp8_e4m3fn_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUhf8_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the hifloat8_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in hifloat8_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_hifloat8_t asc_gather(__ubuf__ hifloat8_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUf8e5m2_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the fp8_e5m2_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in fp8_e5m2_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_gather(__ubuf__ fp8_e5m2_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUf8e8m0_vu16_vb
 * @archs 3510
 * @param [in] src Start address in UB of the fp8_e8m0_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in fp8_e8m0_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_gather(__ubuf__ fp8_e8m0_t* src, vector_uint16_t index, vector_bool mask);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline vector_fp8_e6m2_t asc_gather(__ubuf__ fp8_e6m2_t* src, vector_uint16_t index, vector_bool mask);
#endif

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUi16_vu32_vb
 * @archs 3510
 * @param [in] src Start address in UB of the int16_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in int16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_int16_t asc_gather(__ubuf__ int16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUu16_vu32_vb
 * @archs 3510
 * @param [in] src Start address in UB of the uint16_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in uint16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_uint16_t asc_gather(__ubuf__ uint16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUf16_vu32_vb
 * @archs 3510
 * @param [in] src Start address in UB of the half source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in half elements, giving the element gathered
 * into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_half asc_gather(__ubuf__ half* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and returns them; because the
 * index element is wider than the destination element the results occupy the even destination lanes and the odd lanes
 * are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__PUbf16_vu32_vb
 * @archs 3510
 * @param [in] src Start address in UB of the bfloat16_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in bfloat16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline vector_bfloat16_t asc_gather(__ubuf__ bfloat16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers the b8 elements of the UB vector at src into the b16 register dst using the element offsets in index,
 * placing each 8-bit source element in the low 8 bits of the destination element and zero-filling the high 8 bits,
 * gated by mask.
 *
 * @override_id asc_gather__Rvi16_PUi8_vu16_vb
 * @archs 3510
 * @param [out] dst Destination b16 vector data register (vector_int16_t) receiving the zero-extended b8 elements; lanes
 * whose mask bit is 0 are written as zero.
 * @param [in] src Start address in UB of the int8_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in int8_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_int16_t& dst, __ubuf__ int8_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers the b8 elements of the UB vector at src into the b16 register dst using the element offsets in index,
 * placing each 8-bit source element in the low 8 bits of the destination element and zero-filling the high 8 bits,
 * gated by mask.
 *
 * @override_id asc_gather__Rvu16_PUu8_vu16_vb
 * @archs 3510
 * @param [out] dst Destination b16 vector data register (vector_uint16_t) receiving the zero-extended b8 elements;
 * lanes whose mask bit is 0 are written as zero.
 * @param [in] src Start address in UB of the uint8_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in uint8_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_uint16_t& dst, __ubuf__ uint8_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvi8_PUi8_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) receiving the gathered elements; results are
 * written to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the int8_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in int8_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_int8_t& dst, __ubuf__ int8_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvu8_PUu8_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) receiving the gathered elements; results are
 * written to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the uint8_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in uint8_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_uint8_t& dst, __ubuf__ uint8_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and writes them
 * into dst, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvi16_PUi16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) receiving one gathered element per lane; lanes
 * whose mask bit is 0 are written as zero.
 * @param [in] src Start address in UB of the int16_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in int16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and writes them
 * into dst, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvu16_PUu16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) receiving one gathered element per lane; lanes
 * whose mask bit is 0 are written as zero.
 * @param [in] src Start address in UB of the uint16_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in uint16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and writes them
 * into dst, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvi32_PUi32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) receiving one gathered element per lane; lanes
 * whose mask bit is 0 are written as zero.
 * @param [in] src Start address in UB of the int32_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in int32_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_int32_t& dst, __ubuf__ int32_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and writes them
 * into dst, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvu32_PUu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) receiving one gathered element per lane; lanes
 * whose mask bit is 0 are written as zero.
 * @param [in] src Start address in UB of the uint32_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in uint32_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_uint32_t& dst, __ubuf__ uint32_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and writes them
 * into dst, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvf16_PUf16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) receiving one gathered element per lane; lanes whose
 * mask bit is 0 are written as zero.
 * @param [in] src Start address in UB of the half source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in half elements, giving the element gathered
 * into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(vector_half& dst, __ubuf__ half* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and writes them
 * into dst, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvf32_PUf32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) receiving one gathered element per lane; lanes whose
 * mask bit is 0 are written as zero.
 * @param [in] src Start address in UB of the float source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in float elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(vector_float& dst, __ubuf__ float* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers one element per lane from the UB vector at src using the element offsets held in index and writes them
 * into dst, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvbf16_PUbf16_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) receiving one gathered element per lane; lanes
 * whose mask bit is 0 are written as zero.
 * @param [in] src Start address in UB of the bfloat16_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in bfloat16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvf8e4m3_PUf8e4m3_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) receiving the gathered elements; results are
 * written to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source vector; the actual read address must be 32-byte
 * aligned and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in fp8_e4m3fn_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvhf8_PUhf8_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) receiving the gathered elements; results are
 * written to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the hifloat8_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in hifloat8_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvf8e5m2_PUf8e5m2_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) receiving the gathered elements; results are
 * written to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the fp8_e5m2_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in fp8_e5m2_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, vector_uint16_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvf8e8m0_PUf8e8m0_vu16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) receiving the gathered elements; results are
 * written to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the fp8_e8m0_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint16_t per-lane offsets, counted in fp8_e8m0_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, vector_uint16_t index, vector_bool mask);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline void asc_gather(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, vector_uint16_t index, vector_bool mask);
#endif

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvi16_PUi16_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) receiving the gathered elements; results are
 * written to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the int16_t source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in int16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvu16_PUu16_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) receiving the gathered elements; results are
 * written to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the uint16_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in uint16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvf16_PUf16_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) receiving the gathered elements; results are written
 * to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the half source vector; the actual read address must be 32-byte aligned and
 * must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in half elements, giving the element gathered
 * into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(vector_half& dst, __ubuf__ half* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Gathers elements from the UB vector at src using the element offsets in index and writes them into dst;
 * because the index element is wider than the destination element the results occupy the even destination lanes and the
 * odd lanes are zeroed, with mask selecting the participating input elements.
 *
 * @override_id asc_gather__Rvbf16_PUbf16_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) receiving the gathered elements; results are
 * written to the even lanes with the odd lanes zeroed, and lanes whose mask bit is 0 are zero.
 * @param [in] src Start address in UB of the bfloat16_t source vector; the actual read address must be 32-byte aligned
 * and must stay inside the UB address space.
 * @param [in] index Vector register of uint32_t per-lane offsets, counted in bfloat16_t elements, giving the element
 * gathered into each destination lane.
 * @param [in] mask Mask register whose set bits select the input elements that are gathered; destination lanes whose
 * mask bit is 0 are written as 0.
 */
__simd_callee__ inline void asc_gather(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of int8_t elements gathered from src at the byte
 * offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUi8_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the int8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_int8_t asc_gather_datablock(
    __ubuf__ int8_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of uint8_t elements gathered from src at the byte
 * offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUu8_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the uint8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_uint8_t asc_gather_datablock(
    __ubuf__ uint8_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of int16_t elements gathered from src at the byte
 * offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUi16_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the int16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_int16_t asc_gather_datablock(
    __ubuf__ int16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of uint16_t elements gathered from src at the byte
 * offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUu16_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the uint16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_uint16_t asc_gather_datablock(
    __ubuf__ uint16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of int32_t elements gathered from src at the byte
 * offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUi32_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the int32_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_int32_t asc_gather_datablock(
    __ubuf__ int32_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of uint32_t elements gathered from src at the byte
 * offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUu32_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the uint32_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_uint32_t asc_gather_datablock(
    __ubuf__ uint32_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of half elements gathered from src at the byte
 * offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUf16_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the half source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_half asc_gather_datablock(__ubuf__ half* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of float elements gathered from src at the byte
 * offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUf32_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the float source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_float asc_gather_datablock(__ubuf__ float* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of bfloat16_t elements gathered from src at the
 * byte offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUbf16_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the bfloat16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_bfloat16_t asc_gather_datablock(
    __ubuf__ bfloat16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp8_e4m3fn_t elements gathered from src at the
 * byte offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUf8e4m3_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp8_e4m3fn_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_gather_datablock(
    __ubuf__ fp8_e4m3fn_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of hifloat8_t elements gathered from src at the
 * byte offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUhf8_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the hifloat8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_hifloat8_t asc_gather_datablock(
    __ubuf__ hifloat8_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp8_e5m2_t elements gathered from src at the
 * byte offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUf8e5m2_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp8_e5m2_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_gather_datablock(
    __ubuf__ fp8_e5m2_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp4x2_e2m1_t elements gathered from src at the
 * byte offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUf4e2m1x2_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp4x2_e2m1_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_gather_datablock(
    __ubuf__ fp4x2_e2m1_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp4x2_e1m2_t elements gathered from src at the
 * byte offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUf4e1m2x2_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp4x2_e1m2_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_gather_datablock(
    __ubuf__ fp4x2_e1m2_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of packed int4b_t element pairs gathered from src
 * at the byte offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUi4x2_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the int4b_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_int4x2_t asc_gather_datablock(
    __ubuf__ int4b_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp8_e8m0_t elements gathered from src at the
 * byte offsets in the first eight index elements, where mask selects which DataBlocks are updated.
 *
 * @override_id asc_gather_datablock__PUf8e8m0_vu32_vb
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp8_e8m0_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_gather_datablock(
    __ubuf__ fp8_e8m0_t* src, vector_uint32_t index, vector_bool mask);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline vector_fp8_e6m2_t asc_gather_datablock(
    __ubuf__ fp8_e6m2_t* src, vector_uint32_t index, vector_bool mask);
#endif

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of int8_t elements gathered from src at the byte
 * offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUi8_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the int8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_int8_t asc_gather_datablock(__ubuf__ int8_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of uint8_t elements gathered from src at the byte
 * offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUu8_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the uint8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_uint8_t asc_gather_datablock(__ubuf__ uint8_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of int16_t elements gathered from src at the byte
 * offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUi16_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the int16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_int16_t asc_gather_datablock(__ubuf__ int16_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of uint16_t elements gathered from src at the byte
 * offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUu16_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the uint16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_uint16_t asc_gather_datablock(__ubuf__ uint16_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of int32_t elements gathered from src at the byte
 * offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUi32_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the int32_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_int32_t asc_gather_datablock(__ubuf__ int32_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of uint32_t elements gathered from src at the byte
 * offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUu32_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the uint32_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_uint32_t asc_gather_datablock(__ubuf__ uint32_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of half elements gathered from src at the byte
 * offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUf16_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the half source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_half asc_gather_datablock(__ubuf__ half* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of float elements gathered from src at the byte
 * offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUf32_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the float source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_float asc_gather_datablock(__ubuf__ float* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of bfloat16_t elements gathered from src at the
 * byte offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUbf16_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the bfloat16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_bfloat16_t asc_gather_datablock(__ubuf__ bfloat16_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp8_e4m3fn_t elements gathered from src at the
 * byte offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUf8e4m3_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp8_e4m3fn_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_gather_datablock(__ubuf__ fp8_e4m3fn_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of hifloat8_t elements gathered from src at the
 * byte offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUhf8_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the hifloat8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_hifloat8_t asc_gather_datablock(__ubuf__ hifloat8_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp8_e5m2_t elements gathered from src at the
 * byte offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUf8e5m2_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp8_e5m2_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_gather_datablock(__ubuf__ fp8_e5m2_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp4x2_e2m1_t elements gathered from src at the
 * byte offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUf4e2m1x2_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp4x2_e2m1_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_gather_datablock(__ubuf__ fp4x2_e2m1_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp4x2_e1m2_t elements gathered from src at the
 * byte offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUf4e1m2x2_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp4x2_e1m2_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_gather_datablock(__ubuf__ fp4x2_e1m2_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of packed int4b_t element pairs gathered from src
 * at the byte offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUi4x2_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the int4b_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_int4x2_t asc_gather_datablock(__ubuf__ int4b_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Returns a vector register holding eight 32-byte DataBlocks of fp8_e8m0_t elements gathered from src at the
 * byte offsets in the first eight index elements.
 *
 * @override_id asc_gather_datablock__PUf8e8m0_vu32
 * @archs 3510
 * @param [in] src Unified Buffer start address of the fp8_e8m0_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_gather_datablock(__ubuf__ fp8_e8m0_t* src, vector_uint32_t index);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline vector_fp8_e6m2_t asc_gather_datablock(__ubuf__ fp8_e6m2_t* src, vector_uint32_t index);
#endif

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of int8_t elements from src into dst at the byte offsets in the first eight
 * index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvi8_PUi8_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the int8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_int8_t& dst, __ubuf__ int8_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of uint8_t elements from src into dst at the byte offsets in the first eight
 * index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvu8_PUu8_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the uint8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_uint8_t& dst, __ubuf__ uint8_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of int16_t elements from src into dst at the byte offsets in the first eight
 * index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvi16_PUi16_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the int16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of uint16_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvu16_PUu16_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the uint16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of int32_t elements from src into dst at the byte offsets in the first eight
 * index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvi32_PUi32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the int32_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_int32_t& dst, __ubuf__ int32_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of uint32_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvu32_PUu32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the uint32_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_uint32_t& dst, __ubuf__ uint32_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of int64_t elements from src into dst at the byte offsets in the first eight
 * index elements, updating only the destination DataBlocks enabled by mask; the b64 element type is software-simulated
 * by the compiler.
 *
 * @override_id asc_gather_datablock__Rvi64_PUi64_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int64_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the int64_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_int64_t& dst, __ubuf__ int64_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of uint64_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask; the b64 element type is
 * software-simulated by the compiler.
 *
 * @override_id asc_gather_datablock__Rvu64_PUu64_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint64_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the uint64_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_uint64_t& dst, __ubuf__ uint64_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of half elements from src into dst at the byte offsets in the first eight
 * index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvf16_PUf16_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the half source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_half& dst, __ubuf__ half* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of float elements from src into dst at the byte offsets in the first eight
 * index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvf32_PUf32_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the float source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_float& dst, __ubuf__ float* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of bfloat16_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvbf16_PUbf16_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the bfloat16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp8_e4m3fn_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvf8e4m3_PUf8e4m3_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the gathered DataBlocks; only
 * the DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the fp8_e4m3fn_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of hifloat8_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvhf8_PUhf8_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the hifloat8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp8_e5m2_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvf8e5m2_PUf8e5m2_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the fp8_e5m2_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp4x2_e2m1_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvf4e2m1x2_PUf4e2m1x2_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the gathered DataBlocks; only
 * the DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the fp4x2_e2m1_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp4x2_e1m2_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvf4e1m2x2_PUf4e1m2x2_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the gathered DataBlocks; only
 * the DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the fp4x2_e1m2_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of packed int4b_t element pairs from src into dst at the byte offsets in the
 * first eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvi4x2_PUi4x2_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the int4b_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_int4x2_t& dst, __ubuf__ int4b_t* src, vector_uint32_t index, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp8_e8m0_t elements from src into dst at the byte offsets in the first
 * eight index elements, updating only the destination DataBlocks enabled by mask.
 *
 * @override_id asc_gather_datablock__Rvf8e8m0_PUf8e8m0_vu32_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the gathered DataBlocks; only the
 * DataBlocks whose mask bits are set are updated.
 * @param [in] src Unified Buffer start address of the fp8_e8m0_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 * @param [in] mask Mask register interpreted as b32 with one 4-bit group per DataBlock; only bits 0, 4, 8, 12, 16, 20,
 * 24 and 28 are used and they enable destination DataBlocks 0 to 7 respectively.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, vector_uint32_t index, vector_bool mask);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, vector_uint32_t index, vector_bool mask);
#endif

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of int8_t elements from src into dst at the byte offsets in the first eight
 * index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvi8_PUi8_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the int8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_int8_t& dst, __ubuf__ int8_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of uint8_t elements from src into dst at the byte offsets in the first eight
 * index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvu8_PUu8_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the uint8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_uint8_t& dst, __ubuf__ uint8_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of int16_t elements from src into dst at the byte offsets in the first eight
 * index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvi16_PUi16_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the int16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_int16_t& dst, __ubuf__ int16_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of uint16_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvu16_PUu16_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the uint16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_uint16_t& dst, __ubuf__ uint16_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of int32_t elements from src into dst at the byte offsets in the first eight
 * index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvi32_PUi32_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the int32_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_int32_t& dst, __ubuf__ int32_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of uint32_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvu32_PUu32_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the uint32_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_uint32_t& dst, __ubuf__ uint32_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of int64_t elements from src into dst at the byte offsets in the first eight
 * index elements, with no mask selection applied; the b64 element type is software-simulated by the compiler.
 *
 * @override_id asc_gather_datablock__Rvi64_PUi64_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int64_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the int64_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_int64_t& dst, __ubuf__ int64_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of uint64_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied; the b64 element type is software-simulated by the compiler.
 *
 * @override_id asc_gather_datablock__Rvu64_PUu64_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint64_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the uint64_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_uint64_t& dst, __ubuf__ uint64_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of half elements from src into dst at the byte offsets in the first eight
 * index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvf16_PUf16_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the half source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_half& dst, __ubuf__ half* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of float elements from src into dst at the byte offsets in the first eight
 * index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvf32_PUf32_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the float source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_float& dst, __ubuf__ float* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of bfloat16_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvbf16_PUbf16_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the bfloat16_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp8_e4m3fn_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvf8e4m3_PUf8e4m3_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the gathered DataBlocks; no
 * mask selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the fp8_e4m3fn_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of hifloat8_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvhf8_PUhf8_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the hifloat8_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp8_e5m2_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvf8e5m2_PUf8e5m2_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the fp8_e5m2_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp4x2_e2m1_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvf4e2m1x2_PUf4e2m1x2_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the gathered DataBlocks; no
 * mask selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the fp4x2_e2m1_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp4x2_e1m2_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvf4e1m2x2_PUf4e1m2x2_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the gathered DataBlocks; no
 * mask selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the fp4x2_e1m2_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of packed int4b_t element pairs from src into dst at the byte offsets in the
 * first eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvi4x2_PUi4x2_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the int4b_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(vector_int4x2_t& dst, __ubuf__ int4b_t* src, vector_uint32_t index);

/**
 * @ingroup simd_c_api.reg_compute.ub_gather
 * @brief Collects eight 32-byte DataBlocks of fp8_e8m0_t elements from src into dst at the byte offsets in the first
 * eight index elements, with no mask selection applied.
 *
 * @override_id asc_gather_datablock__Rvf8e8m0_PUf8e8m0_vu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the gathered DataBlocks; no mask
 * selection is applied to this prototype.
 * @param [in] src Unified Buffer start address of the fp8_e8m0_t source operand; it must be 32-byte aligned.
 * @param [in] index Vector register of uint32 byte offsets relative to src, one per DataBlock; only the first eight
 * elements are used and every offset must be non-negative and 32-byte aligned, while equal values are allowed.
 */
__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, vector_uint32_t index);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)
__simd_callee__ inline void asc_gather_datablock(
    vector_fp8_e6m2_t& dst, __ubuf__ fp8_e6m2_t* src, vector_uint32_t index);
#endif

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_GATHER_UB_GATHER_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_GATHER_UB_GATHER_H
#endif
