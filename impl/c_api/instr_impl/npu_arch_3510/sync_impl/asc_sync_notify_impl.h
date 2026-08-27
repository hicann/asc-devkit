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
 * \file asc_sync_notify_impl.h
 * \brief
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/instr_impl/npu_arch_3510/sync_impl/asc_sync_notify_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYNC_IMPL_ASC_SYNC_NOTIFY_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYNC_IMPL_ASC_SYNC_NOTIFY_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__aicore__ inline void asc_sync_notify_impl_aic(pipe_t pipe, pipe_t tpipe, event_t id)
{
    if (pipe == pipe_t::PIPE_S) {
        if (tpipe == pipe_t::PIPE_MTE2) {
            set_flag(pipe_t::PIPE_S, pipe_t::PIPE_MTE2, id);
        } else if (tpipe == pipe_t::PIPE_FIX) {
            set_flag(pipe_t::PIPE_S, pipe_t::PIPE_FIX, id);
        }
    } else if (pipe == pipe_t::PIPE_M) {
        if (tpipe == pipe_t::PIPE_MTE1) {
            set_flag(pipe_t::PIPE_M, pipe_t::PIPE_MTE1, id);
        } else if (tpipe == pipe_t::PIPE_FIX) {
            set_flag(pipe_t::PIPE_M, pipe_t::PIPE_FIX, id);
        }
    } else if (pipe == pipe_t::PIPE_MTE1) {
        if (tpipe == pipe_t::PIPE_M) {
            set_flag(pipe_t::PIPE_MTE1, pipe_t::PIPE_M, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            set_flag(pipe_t::PIPE_MTE1, pipe_t::PIPE_MTE2, id);
        } else if (tpipe == pipe_t::PIPE_FIX) {
            set_flag(pipe_t::PIPE_MTE1, pipe_t::PIPE_FIX, id);
        }
    } else if (pipe == pipe_t::PIPE_MTE2) {
        if (tpipe == pipe_t::PIPE_S) {
            set_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_MTE1) {
            set_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_MTE1, id);
        } else if (tpipe == pipe_t::PIPE_FIX) {
            set_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_FIX, id);
        }
    } else if (pipe == pipe_t::PIPE_FIX) {
        if (tpipe == pipe_t::PIPE_S) {
            set_flag(pipe_t::PIPE_FIX, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_M) {
            set_flag(pipe_t::PIPE_FIX, pipe_t::PIPE_M, id);
        } else if (tpipe == pipe_t::PIPE_MTE1) {
            set_flag(pipe_t::PIPE_FIX, pipe_t::PIPE_MTE1, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            set_flag(pipe_t::PIPE_FIX, pipe_t::PIPE_MTE2, id);
        }
    }
}

__aicore__ inline void asc_sync_notify_impl_aiv(pipe_t pipe, pipe_t tpipe, event_t id)
{
    if (pipe == pipe_t::PIPE_S) {
        if (tpipe == pipe_t::PIPE_V) {
            set_flag(pipe_t::PIPE_S, pipe_t::PIPE_V, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            set_flag(pipe_t::PIPE_S, pipe_t::PIPE_MTE2, id);
        } else if (tpipe == pipe_t::PIPE_MTE3) {
            set_flag(pipe_t::PIPE_S, pipe_t::PIPE_MTE3, id);
        }
    } else if (pipe == pipe_t::PIPE_V) {
        if (tpipe == pipe_t::PIPE_S) {
            set_flag(pipe_t::PIPE_V, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            set_flag(pipe_t::PIPE_V, pipe_t::PIPE_MTE2, id);
        } else if (tpipe == pipe_t::PIPE_MTE3) {
            set_flag(pipe_t::PIPE_V, pipe_t::PIPE_MTE3, id);
        }
    } else if (pipe == pipe_t::PIPE_MTE2) {
        if (tpipe == pipe_t::PIPE_S) {
            set_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_V) {
            set_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_V, id);
        } else if (tpipe == pipe_t::PIPE_MTE3) {
            set_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_MTE3, id);
        }
    } else if (pipe == pipe_t::PIPE_MTE3) {
        if (tpipe == pipe_t::PIPE_S) {
            set_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_V) {
            set_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_V, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            set_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_MTE2, id);
        }
    }
}

__aicore__ inline void asc_sync_notify_impl(pipe_t pipe, pipe_t tpipe, event_t id)
{
    if ASC_IS_AIC {
        asc_sync_notify_impl_aic(pipe, tpipe, id);
    } else if ASC_IS_AIV {
        asc_sync_notify_impl_aiv(pipe, tpipe, id);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
