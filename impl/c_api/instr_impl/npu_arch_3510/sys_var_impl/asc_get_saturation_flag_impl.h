/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file asc_get_saturation_flag_impl.h
 * \brief
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)

#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_GET_SATURATION_FLAG_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_GET_SATURATION_FLAG_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__aicore__ inline bool asc_get_saturation_flag_impl(asc_saturation_mode saturation_mode)
{
    // Enum underlying values are CTRL bit indices. INT: 1=sat; FLOAT/FLOAT8/CAST: 0=sat.
    const uint8_t bit = static_cast<uint8_t>(saturation_mode);
    const bool is_bit_set = ((get_ctrl() >> bit) & 1) != 0;
    return (saturation_mode == asc_saturation_mode::INT) ? is_bit_set : !is_bit_set;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
