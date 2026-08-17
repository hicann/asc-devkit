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
    "This is an internal header file and must not be used directly. Please include \"c_api/experimental/asc_get_pipe_idle_slot_count.h\" instead."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_GET_PIPE_IDLE_SLOT_COUNT_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_ASC_GET_PIPE_IDLE_SLOT_COUNT_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__aicore__ inline uint8_t asc_get_pipe_mte2_idle_slot_count() { return static_cast<uint8_t>(get_iqent(PIPE_MTE2)); }

__aicore__ inline uint8_t asc_get_pipe_mte3_idle_slot_count()
{
    if ASC_IS_AIV {
        return static_cast<uint8_t>(get_iqent(PIPE_MTE3));
    }
    return 0;
}

__aicore__ inline uint8_t asc_get_pipe_v_idle_slot_count()
{
    if ASC_IS_AIV {
        return static_cast<uint8_t>(get_iqent(PIPE_V));
    }
    return 0;
}

__aicore__ inline uint8_t asc_get_pipe_m_idle_slot_count()
{
    if ASC_IS_AIC {
        return static_cast<uint8_t>(get_iqent(PIPE_M));
    }
    return 0;
}

__aicore__ inline uint8_t asc_get_pipe_mte1_idle_slot_count()
{
    if ASC_IS_AIC {
        return static_cast<uint8_t>(get_iqent(PIPE_MTE1));
    }
    return 0;
}

__aicore__ inline uint8_t asc_get_pipe_fix_idle_slot_count()
{
    if ASC_IS_AIC {
        return static_cast<uint8_t>(get_iqent(PIPE_FIX));
    }
    return 0;
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
