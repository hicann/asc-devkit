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
 * \file asc_set_saturation_flag_impl.h
 * \brief
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)

#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_SATURATION_FLAG_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_SATURATION_FLAG_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__aicore__ inline void asc_set_saturation_flag_impl(asc_saturation_mode saturation_mode, bool enable_sat)
{
    // Enum underlying values are CTRL bit indices. INT: 1=sat; FLOAT/FLOAT8/CAST: 0=sat.
    const int64_t bit = static_cast<int64_t>(saturation_mode);
    const bool is_bit_set = (saturation_mode == asc_saturation_mode::INT) ? enable_sat : !enable_sat;
    set_ctrl(is_bit_set ? sbitset1(get_ctrl(), bit) : sbitset0(get_ctrl(), bit));
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
