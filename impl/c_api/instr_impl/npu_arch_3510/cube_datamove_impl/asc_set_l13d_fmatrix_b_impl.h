/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file asc_set_l13d_fmatrix_b_impl.h
 * \brief
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)

#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CUBE_DATAMOVE_IMPL_ASC_SET_L13D_FMATRIX_B_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CUBE_DATAMOVE_IMPL_ASC_SET_L13D_FMATRIX_B_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl/utils_impl.h"

constexpr uint8_t ASC_L13D_FMATRIX_B_H_SHIFT = 16;
constexpr uint8_t ASC_L13D_FMATRIX_B_PAD_LEFT_SHIFT = 32;
constexpr uint8_t ASC_L13D_FMATRIX_B_PAD_RIGHT_SHIFT = 40;
constexpr uint8_t ASC_L13D_FMATRIX_B_PAD_TOP_SHIFT = 48;
constexpr uint8_t ASC_L13D_FMATRIX_B_PAD_BOTTOM_SHIFT = 56;

__aicore__ inline void asc_set_l13d_fmatrix_b_impl(asc_l13d_fmatrix_config& config)
{
    if ASC_IS_AIC {
        set_fmatrix_b(config.config);
    }
}

__aicore__ inline void asc_set_l13d_fmatrix_b_impl(
    uint16_t fmatrix_w, uint16_t fmatrix_h, uint8_t pad_left, uint8_t pad_right, uint8_t pad_top, uint8_t pad_bottom)
{
    if ASC_IS_AIC {
        uint64_t config = static_cast<uint64_t>(fmatrix_w) |
                          (static_cast<uint64_t>(fmatrix_h) << ASC_L13D_FMATRIX_B_H_SHIFT) |
                          (static_cast<uint64_t>(pad_left) << ASC_L13D_FMATRIX_B_PAD_LEFT_SHIFT) |
                          (static_cast<uint64_t>(pad_right) << ASC_L13D_FMATRIX_B_PAD_RIGHT_SHIFT) |
                          (static_cast<uint64_t>(pad_top) << ASC_L13D_FMATRIX_B_PAD_TOP_SHIFT) |
                          (static_cast<uint64_t>(pad_bottom) << ASC_L13D_FMATRIX_B_PAD_BOTTOM_SHIFT);
        set_fmatrix_b(config);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
