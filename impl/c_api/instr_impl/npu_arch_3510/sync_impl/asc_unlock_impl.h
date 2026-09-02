/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file asc_unlock_impl.h
 * \brief asc_unlock接口的实现
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#pragma message( \
    "impl/c_api/instr_impl/npu_arch_3510/sync_impl/asc_unlock_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"c_api/asc_simd.h\"\" and use public functions or variables defined in interface headers files.")
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYNC_IMPL_ASC_UNLOCK_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYNC_IMPL_ASC_UNLOCK_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl/utils_impl.h"

__aicore__ inline void asc_unlock_impl(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode)
{
    if (mode == ASC_LOCK_BLOCK) {
        if ASC_IS_AIC {
            if (pipe == pipe_t::PIPE_S) {
                rls_buf(pipe_t::PIPE_S, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_M) {
                rls_buf(pipe_t::PIPE_M, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_MTE1) {
                rls_buf(pipe_t::PIPE_MTE1, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_MTE2) {
                rls_buf(pipe_t::PIPE_MTE2, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_FIX) {
                rls_buf(pipe_t::PIPE_FIX, mutex_id, false);
            }
        } else if ASC_IS_AIV {
            if (pipe == pipe_t::PIPE_S) {
                rls_buf(pipe_t::PIPE_S, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_MTE2) {
                rls_buf(pipe_t::PIPE_MTE2, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_MTE3) {
                rls_buf(pipe_t::PIPE_MTE3, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_V) {
                rls_buf(pipe_t::PIPE_V, mutex_id, false);
            }
        }
    } else {
        if ASC_IS_AIC {
            if (pipe == pipe_t::PIPE_S) {
                rls_buf(pipe_t::PIPE_S, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_M) {
                rls_buf(pipe_t::PIPE_M, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_MTE1) {
                rls_buf(pipe_t::PIPE_MTE1, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_MTE2) {
                rls_buf(pipe_t::PIPE_MTE2, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_FIX) {
                rls_buf(pipe_t::PIPE_FIX, mutex_id, true);
            }
        } else if ASC_IS_AIV {
            if (pipe == pipe_t::PIPE_S) {
                rls_buf(pipe_t::PIPE_S, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_MTE2) {
                rls_buf(pipe_t::PIPE_MTE2, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_MTE3) {
                rls_buf(pipe_t::PIPE_MTE3, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_V) {
                rls_buf(pipe_t::PIPE_V, mutex_id, true);
            }
        }
    }
}

__aicore__ inline void asc_unlock_impl(pipe_t pipe, uint8_t mutex_id)
{
    asc_unlock_impl(pipe, mutex_id, ASC_LOCK_BLOCK);
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
