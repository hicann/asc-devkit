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
 * \file asc_set_saturation_strategy_impl.h
 * \brief
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)

#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_SATURATION_STRATEGY_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_SET_SATURATION_STRATEGY_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__aicore__ inline void asc_set_saturation_strategy_impl(asc_override_strategy strategy)
{
    // CTRL[60]: 0 => USE_API, 1 => USE_GLOBAL. Aligns with AscendC::SetSaturationStrategy.
    constexpr int32_t SATURATION_STRATEGY_BIT = 60;
    int64_t ctrl_value = get_ctrl();
    set_ctrl(
        strategy == asc_override_strategy::USE_API ? sbitset0(ctrl_value, SATURATION_STRATEGY_BIT) :
                                                     sbitset1(ctrl_value, SATURATION_STRATEGY_BIT));
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
