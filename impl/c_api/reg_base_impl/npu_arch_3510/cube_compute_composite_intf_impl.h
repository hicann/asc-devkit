/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/reg_base_impl/npu_arch_3510/cube_compute_composite_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_CUBE_COMPUTE_COMPOSITE_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_CUBE_COMPUTE_COMPOSITE_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val);
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_mx_sync; parameters below identify this variant.
 * @param a_matrix Left-matrix type: __ca__ fp4x2_e1m2_t*.
 * @param b_matrix Right-matrix type: __cb__ fp4x2_e2m1_t*.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp4x2_e1m2_t*/cb fp4x2_e2m1_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_mx_sync; parameters below identify this variant.
 * @param a_matrix Left-matrix type: __ca__ fp4x2_e2m1_t*.
 * @param b_matrix Right-matrix type: __cb__ fp4x2_e1m2_t*.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp4x2_e2m1_t*/cb fp4x2_e1m2_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_mx_sync; parameters below identify this variant.
 * @param a_matrix Left-matrix type: __ca__ fp4x2_e2m1_t*.
 * @param b_matrix Right-matrix type: __cb__ fp4x2_e2m1_t*.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp4x2_e2m1_t*/cb fp4x2_e2m1_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_mx_sync; parameters below identify this variant.
 * @param a_matrix Left-matrix type: __ca__ fp8_e4m3fn_t*.
 * @param b_matrix Right-matrix type: __cb__ fp8_e4m3fn_t*.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp8_e4m3fn_t*/cb fp8_e4m3fn_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_mx_sync; parameters below identify this variant.
 * @param a_matrix Left-matrix type: __ca__ fp8_e4m3fn_t*.
 * @param b_matrix Right-matrix type: __cb__ fp8_e5m2_t*.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp8_e4m3fn_t*/cb fp8_e5m2_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_mx_sync; parameters below identify this variant.
 * @param a_matrix Left-matrix type: __ca__ fp8_e5m2_t*.
 * @param b_matrix Right-matrix type: __cb__ fp8_e4m3fn_t*.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp8_e5m2_t*/cb fp8_e4m3fn_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_mx_sync; parameters below identify this variant.
 * @param a_matrix Left-matrix type: __ca__ fp8_e5m2_t*.
 * @param b_matrix Right-matrix type: __cb__ fp8_e5m2_t*.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad_mx and asc_sync)
__aicore__ inline void asc_mmad_mx_sync(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad_mx(
            c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp8_e5m2_t*/cb fp8_e5m2_t*.
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val);
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_sync; parameters below identify this variant.
 * @param c_matrix Accumulator type: __cc__ float*.
 * @param a_matrix Left-matrix type: __ca__ fp8_e4m3fn_t*.
 * @param b_matrix Right-matrix type: __cb__ fp8_e4m3fn_t*.
 * @note 10-parameter form.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp8_e4m3fn_t*/cb fp8_e4m3fn_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_sync; parameters below identify this variant.
 * @param c_matrix Accumulator type: __cc__ float*.
 * @param a_matrix Left-matrix type: __ca__ fp8_e4m3fn_t*.
 * @param b_matrix Right-matrix type: __cb__ fp8_e5m2_t*.
 * @note 10-parameter form.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp8_e4m3fn_t*/cb fp8_e5m2_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_sync; parameters below identify this variant.
 * @param c_matrix Accumulator type: __cc__ float*.
 * @param a_matrix Left-matrix type: __ca__ fp8_e5m2_t*.
 * @param b_matrix Right-matrix type: __cb__ fp8_e4m3fn_t*.
 * @note 10-parameter form.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp8_e5m2_t*/cb fp8_e4m3fn_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_sync; parameters below identify this variant.
 * @param c_matrix Accumulator type: __cc__ float*.
 * @param a_matrix Left-matrix type: __ca__ fp8_e5m2_t*.
 * @param b_matrix Right-matrix type: __cb__ fp8_e5m2_t*.
 * @note 10-parameter form.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix/b_matrix are ca fp8_e5m2_t*/cb fp8_e5m2_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_sync; parameters below identify this variant.
 * @param c_matrix Accumulator type: __cc__ float*.
 * @param a_matrix Left-matrix type: __ca__ half*.
 * @param b_matrix Right-matrix type: __cb__ half*.
 * @param disable_gemv GEMV-disable control specific to this overload.
 * @note 10-parameter form.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: c_matrix uses __cc__ float*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_sync; parameters below identify this variant.
 * @param c_matrix Accumulator type: __cc__ float*.
 * @param a_matrix Left-matrix type: __ca__ float*.
 * @param b_matrix Right-matrix type: __cb__ float*.
 * @param disable_gemv GEMV-disable control specific to this overload.
 * @note 10-parameter form.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: a_matrix uses __ca__ float*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_sync; parameters below identify this variant.
 * @param c_matrix Accumulator type: __cc__ int32_t*.
 * @param a_matrix Left-matrix type: __ca__ int8_t*.
 * @param b_matrix Right-matrix type: __cb__ int8_t*.
 * @param disable_gemv GEMV-disable control specific to this overload.
 * @note 10-parameter form.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim,
    uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val); // 3510 sync overload: c_matrix uses __cc__ int32_t*.
        asc_sync_post_process();
    }
}

/**
 * @brief 3510 sync overload of asc_mmad_sync; parameters below identify this variant.
 * @param c_matrix Accumulator type: __cc__ float*.
 * @param a_matrix Left-matrix type: __ca__ hifloat8_t*.
 * @param b_matrix Right-matrix type: __cb__ hifloat8_t*.
 * @note 10-parameter form.
 */
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mmad and asc_sync)
__aicore__ inline void asc_mmad_sync(
    __cc__ float* c_matrix, __ca__ hifloat8_t* a_matrix, __cb__ hifloat8_t* b_matrix, uint16_t left_height,
    uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source,
    bool c_matrix_init_val)
{
    if ASC_IS_AIC {
        mad(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source,
            c_matrix_init_val);  // 3510 sync overload: a_matrix uses __ca__ hifloat8_t*.
        asc_sync_post_process(); // 3510 sync completion: a_matrix uses __ca__ hifloat8_t*.
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
