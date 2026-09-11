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
    "impl/c_api/reg_base_impl/sync_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_SYNC_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_SYNC_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/sync_intf_impl.h"
#endif

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

__aicore__ inline void asc_sync_wait_impl_aic(pipe_t pipe, pipe_t tpipe, event_t id)
{
    if (pipe == pipe_t::PIPE_S) {
        if (tpipe == pipe_t::PIPE_MTE2) {
            wait_flag(pipe_t::PIPE_S, pipe_t::PIPE_MTE2, id);
        } else if (tpipe == pipe_t::PIPE_FIX) {
            wait_flag(pipe_t::PIPE_S, pipe_t::PIPE_FIX, id);
        }
    } else if (pipe == pipe_t::PIPE_M) {
        if (tpipe == pipe_t::PIPE_MTE1) {
            wait_flag(pipe_t::PIPE_M, pipe_t::PIPE_MTE1, id);
        } else if (tpipe == pipe_t::PIPE_FIX) {
            wait_flag(pipe_t::PIPE_M, pipe_t::PIPE_FIX, id);
        }
    } else if (pipe == pipe_t::PIPE_MTE1) {
        if (tpipe == pipe_t::PIPE_M) {
            wait_flag(pipe_t::PIPE_MTE1, pipe_t::PIPE_M, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            wait_flag(pipe_t::PIPE_MTE1, pipe_t::PIPE_MTE2, id);
        } else if (tpipe == pipe_t::PIPE_FIX) {
            wait_flag(pipe_t::PIPE_MTE1, pipe_t::PIPE_FIX, id);
        }
    } else if (pipe == pipe_t::PIPE_MTE2) {
        if (tpipe == pipe_t::PIPE_S) {
            wait_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_MTE1) {
            wait_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_MTE1, id);
        } else if (tpipe == pipe_t::PIPE_FIX) {
            wait_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_FIX, id);
        }
    } else if (pipe == pipe_t::PIPE_FIX) {
        if (tpipe == pipe_t::PIPE_S) {
            wait_flag(pipe_t::PIPE_FIX, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_M) {
            wait_flag(pipe_t::PIPE_FIX, pipe_t::PIPE_M, id);
        } else if (tpipe == pipe_t::PIPE_MTE1) {
            wait_flag(pipe_t::PIPE_FIX, pipe_t::PIPE_MTE1, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            wait_flag(pipe_t::PIPE_FIX, pipe_t::PIPE_MTE2, id);
        }
    }
}

__aicore__ inline void asc_sync_wait_impl_aiv(pipe_t pipe, pipe_t tpipe, event_t id)
{
    if (pipe == pipe_t::PIPE_S) {
        if (tpipe == pipe_t::PIPE_V) {
            wait_flag(pipe_t::PIPE_S, pipe_t::PIPE_V, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            wait_flag(pipe_t::PIPE_S, pipe_t::PIPE_MTE2, id);
        } else if (tpipe == pipe_t::PIPE_MTE3) {
            wait_flag(pipe_t::PIPE_S, pipe_t::PIPE_MTE3, id);
        }
    } else if (pipe == pipe_t::PIPE_V) {
        if (tpipe == pipe_t::PIPE_S) {
            wait_flag(pipe_t::PIPE_V, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            wait_flag(pipe_t::PIPE_V, pipe_t::PIPE_MTE2, id);
        } else if (tpipe == pipe_t::PIPE_MTE3) {
            wait_flag(pipe_t::PIPE_V, pipe_t::PIPE_MTE3, id);
        }
    } else if (pipe == pipe_t::PIPE_MTE2) {
        if (tpipe == pipe_t::PIPE_S) {
            wait_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_V) {
            wait_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_V, id);
        } else if (tpipe == pipe_t::PIPE_MTE3) {
            wait_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_MTE3, id);
        }
    } else if (pipe == pipe_t::PIPE_MTE3) {
        if (tpipe == pipe_t::PIPE_S) {
            wait_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_S, id);
        } else if (tpipe == pipe_t::PIPE_V) {
            wait_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_V, id);
        } else if (tpipe == pipe_t::PIPE_MTE2) {
            wait_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_MTE2, id);
        }
    }
}

__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode)
{
    if (mode == ASC_LOCK_BLOCK) {
        if ASC_IS_AIC {
            if (pipe == pipe_t::PIPE_S) {
                get_buf(pipe_t::PIPE_S, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_M) {
                get_buf(pipe_t::PIPE_M, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_MTE1) {
                get_buf(pipe_t::PIPE_MTE1, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_MTE2) {
                get_buf(pipe_t::PIPE_MTE2, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_FIX) {
                get_buf(pipe_t::PIPE_FIX, mutex_id, false);
            }
        } else if ASC_IS_AIV {
            if (pipe == pipe_t::PIPE_S) {
                get_buf(pipe_t::PIPE_S, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_MTE2) {
                get_buf(pipe_t::PIPE_MTE2, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_MTE3) {
                get_buf(pipe_t::PIPE_MTE3, mutex_id, false);
            } else if (pipe == pipe_t::PIPE_V) {
                get_buf(pipe_t::PIPE_V, mutex_id, false);
            }
        }
    } else {
        if ASC_IS_AIC {
            if (pipe == pipe_t::PIPE_S) {
                get_buf(pipe_t::PIPE_S, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_M) {
                get_buf(pipe_t::PIPE_M, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_MTE1) {
                get_buf(pipe_t::PIPE_MTE1, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_MTE2) {
                get_buf(pipe_t::PIPE_MTE2, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_FIX) {
                get_buf(pipe_t::PIPE_FIX, mutex_id, true);
            }
        } else if ASC_IS_AIV {
            if (pipe == pipe_t::PIPE_S) {
                get_buf(pipe_t::PIPE_S, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_MTE2) {
                get_buf(pipe_t::PIPE_MTE2, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_MTE3) {
                get_buf(pipe_t::PIPE_MTE3, mutex_id, true);
            } else if (pipe == pipe_t::PIPE_V) {
                get_buf(pipe_t::PIPE_V, mutex_id, true);
            }
        }
    }
}

__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id) { asc_lock(pipe, mutex_id, ASC_LOCK_BLOCK); }

__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode)
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

__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id) { asc_unlock(pipe, mutex_id, ASC_LOCK_BLOCK); }

__aicore__ inline void asc_sync_vec(int id)
{
    set_flag(pipe_t::PIPE_V, pipe_t::PIPE_MTE2, static_cast<event_t>(id));
    set_flag(pipe_t::PIPE_V, pipe_t::PIPE_MTE3, static_cast<event_t>(id));
    set_flag(pipe_t::PIPE_V, pipe_t::PIPE_S, static_cast<event_t>(id));
    wait_flag(pipe_t::PIPE_V, pipe_t::PIPE_MTE2, static_cast<event_t>(id));
    wait_flag(pipe_t::PIPE_V, pipe_t::PIPE_MTE3, static_cast<event_t>(id));
    wait_flag(pipe_t::PIPE_V, pipe_t::PIPE_S, static_cast<event_t>(id));
}

__aicore__ inline void asc_sync_pipe(pipe_t pipe)
{
    if ASC_IS_AIC {
        if (pipe == pipe_t::PIPE_M) {
            pipe_barrier(pipe_t::PIPE_M);
        } else if (pipe == pipe_t::PIPE_MTE1) {
            pipe_barrier(pipe_t::PIPE_MTE1);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            pipe_barrier(pipe_t::PIPE_MTE2);
        } else if (pipe == pipe_t::PIPE_ALL) {
            asc_sync_post_process();
        } else if (pipe == pipe_t::PIPE_FIX) {
            pipe_barrier(pipe_t::PIPE_FIX);
        }
    } else if ASC_IS_AIV {
        if (pipe == pipe_t::PIPE_MTE2) {
            pipe_barrier(pipe_t::PIPE_MTE2);
        } else if (pipe == pipe_t::PIPE_MTE3) {
            pipe_barrier(pipe_t::PIPE_MTE3);
        } else if (pipe == pipe_t::PIPE_ALL) {
            asc_sync_post_process();
        }
    }
}

__aicore__ inline void asc_sync_notify(pipe_t pipe, pipe_t tpipe, event_t id)
{
    if ASC_IS_AIC {
        asc_sync_notify_impl_aic(pipe, tpipe, id);
    } else if ASC_IS_AIV {
        asc_sync_notify_impl_aiv(pipe, tpipe, id);
    }
}

__aicore__ inline void asc_sync_wait(pipe_t pipe, pipe_t tpipe, event_t id)
{
    if ASC_IS_AIC {
        asc_sync_wait_impl_aic(pipe, tpipe, id);
    } else if ASC_IS_AIV {
        asc_sync_wait_impl_aiv(pipe, tpipe, id);
    }
}

__aicore__ inline void asc_sync_mte3(int id)
{
    set_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_MTE2, static_cast<event_t>(id));
    set_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_V, static_cast<event_t>(id));
    wait_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_MTE2, static_cast<event_t>(id));
    wait_flag(pipe_t::PIPE_MTE3, pipe_t::PIPE_V, static_cast<event_t>(id));
    pipe_barrier(pipe_t::PIPE_MTE3);
}

__aicore__ inline void asc_sync_mte2(int id)
{
    set_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_MTE3, static_cast<event_t>(id));
    set_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_V, static_cast<event_t>(id));
    wait_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_MTE3, static_cast<event_t>(id));
    wait_flag(pipe_t::PIPE_MTE2, pipe_t::PIPE_V, static_cast<event_t>(id));
    pipe_barrier(pipe_t::PIPE_MTE2);
}

__aicore__ inline void asc_sync() { asc_sync_post_process(); }

__aicore__ inline void asc_sync_block_arrive(pipe_t pipe, int64_t flag_id)
{
    constexpr uint16_t SYNC_MODE_SHIFT_VALUE = 4;
    constexpr uint16_t SYNC_FLAG_SHIFT_VALUE = 8;
    uint16_t mode = 0x02;
    uint64_t config = (0x1 + ((mode & 0x3) << SYNC_MODE_SHIFT_VALUE) + ((flag_id & 0xf) << SYNC_FLAG_SHIFT_VALUE));

    if ASC_IS_AIC {
        if (pipe == pipe_t::PIPE_M) {
            ffts_cross_core_sync(pipe_t::PIPE_M, config);
        } else if (pipe == pipe_t::PIPE_MTE1) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE1, config);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE2, config);
        } else if (pipe == pipe_t::PIPE_FIX) {
            ffts_cross_core_sync(pipe_t::PIPE_FIX, config);
        }
    } else if ASC_IS_AIV {
        if (pipe == pipe_t::PIPE_MTE2) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE2, config);
        } else if (pipe == pipe_t::PIPE_MTE3) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE3, config);
        } else if (pipe == pipe_t::PIPE_V) {
            ffts_cross_core_sync(pipe_t::PIPE_V, config);
        }
    }
}

__aicore__ inline void asc_sync_subblock_arrive(pipe_t pipe, int64_t flag_id)
{
    constexpr uint16_t SYNC_MODE_SHIFT_VALUE = 4;
    constexpr uint16_t SYNC_FLAG_SHIFT_VALUE = 8;
    uint16_t mode = 0x01;
    uint64_t config = (0x1 + ((mode & 0x3) << SYNC_MODE_SHIFT_VALUE) + ((flag_id & 0xf) << SYNC_FLAG_SHIFT_VALUE));

    if ASC_IS_AIC {
        if (pipe == pipe_t::PIPE_M) {
            ffts_cross_core_sync(pipe_t::PIPE_M, config);
        } else if (pipe == pipe_t::PIPE_MTE1) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE1, config);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE2, config);
        } else if (pipe == pipe_t::PIPE_FIX) {
            ffts_cross_core_sync(pipe_t::PIPE_FIX, config);
        }
    } else if ASC_IS_AIV {
        if (pipe == pipe_t::PIPE_MTE2) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE2, config);
        } else if (pipe == pipe_t::PIPE_MTE3) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE3, config);
        } else if (pipe == pipe_t::PIPE_V) {
            ffts_cross_core_sync(pipe_t::PIPE_V, config);
        }
    }
}

__aicore__ inline void asc_sync_inter_arrive(pipe_t pipe, int64_t flag_id)
{
    constexpr uint16_t SYNC_MODE_SHIFT_VALUE = 4;
    constexpr uint16_t SYNC_FLAG_SHIFT_VALUE = 8;
    uint16_t mode = 0x00;
    uint64_t config = (0x1 + ((mode & 0x3) << SYNC_MODE_SHIFT_VALUE) + ((flag_id & 0xf) << SYNC_FLAG_SHIFT_VALUE));

    if ASC_IS_AIC {
        if (pipe == pipe_t::PIPE_M) {
            ffts_cross_core_sync(pipe_t::PIPE_M, config);
        } else if (pipe == pipe_t::PIPE_MTE1) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE1, config);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE2, config);
        } else if (pipe == pipe_t::PIPE_FIX) {
            ffts_cross_core_sync(pipe_t::PIPE_FIX, config);
        }
    } else if ASC_IS_AIV {
        if (pipe == pipe_t::PIPE_MTE2) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE2, config);
        } else if (pipe == pipe_t::PIPE_MTE3) {
            ffts_cross_core_sync(pipe_t::PIPE_MTE3, config);
        } else if (pipe == pipe_t::PIPE_V) {
            ffts_cross_core_sync(pipe_t::PIPE_V, config);
        }
    }
}

__aicore__ inline void asc_sync_data_barrier(mem_dsb_t arg)
{
    if ASC_IS_AIV {
        if (arg == mem_dsb_t::DSB_ALL) {
            dsb(mem_dsb_t::DSB_ALL);
        } else if (arg == mem_dsb_t::DSB_DDR) {
            dsb(mem_dsb_t::DSB_DDR);
        } else if (arg == mem_dsb_t::DSB_UB) {
            dsb(mem_dsb_t::DSB_UB);
        } else if (arg == mem_dsb_t::DSB_SEQ) {
            dsb(mem_dsb_t::DSB_SEQ);
        }
    } else if ASC_IS_AIC {
        if (arg == mem_dsb_t::DSB_ALL) {
            dsb(mem_dsb_t::DSB_ALL);
        } else if (arg == mem_dsb_t::DSB_DDR) {
            dsb(mem_dsb_t::DSB_DDR);
        } else if (arg == mem_dsb_t::DSB_SEQ) {
            dsb(mem_dsb_t::DSB_SEQ);
        }
    }
}

__aicore__ inline void asc_sync_intra_arrive(pipe_t pipe, uint64_t sync_id)
{
    if ASC_IS_AIC {
        if (pipe == pipe_t::PIPE_S) {
            set_intra_block(pipe_t::PIPE_S, sync_id);
        } else if (pipe == pipe_t::PIPE_M) {
            set_intra_block(pipe_t::PIPE_M, sync_id);
        } else if (pipe == pipe_t::PIPE_MTE1) {
            set_intra_block(pipe_t::PIPE_MTE1, sync_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            set_intra_block(pipe_t::PIPE_MTE2, sync_id);
        } else if (pipe == pipe_t::PIPE_FIX) {
            set_intra_block(pipe_t::PIPE_FIX, sync_id);
        }
    } else if ASC_IS_AIV {
        if (pipe == pipe_t::PIPE_S) {
            set_intra_block(pipe_t::PIPE_S, sync_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            set_intra_block(pipe_t::PIPE_MTE2, sync_id);
        } else if (pipe == pipe_t::PIPE_MTE3) {
            set_intra_block(pipe_t::PIPE_MTE3, sync_id);
        } else if (pipe == pipe_t::PIPE_V) {
            set_intra_block(pipe_t::PIPE_V, sync_id);
        }
    }
}

__aicore__ inline void asc_sync_intra_wait(pipe_t pipe, uint64_t sync_id)
{
    if ASC_IS_AIC {
        if (pipe == pipe_t::PIPE_S) {
            wait_intra_block(pipe_t::PIPE_S, sync_id);
        } else if (pipe == pipe_t::PIPE_M) {
            wait_intra_block(pipe_t::PIPE_M, sync_id);
        } else if (pipe == pipe_t::PIPE_MTE1) {
            wait_intra_block(pipe_t::PIPE_MTE1, sync_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            wait_intra_block(pipe_t::PIPE_MTE2, sync_id);
        } else if (pipe == pipe_t::PIPE_FIX) {
            wait_intra_block(pipe_t::PIPE_FIX, sync_id);
        }
    } else if ASC_IS_AIV {
        if (pipe == pipe_t::PIPE_S) {
            wait_intra_block(pipe_t::PIPE_S, sync_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            wait_intra_block(pipe_t::PIPE_MTE2, sync_id);
        } else if (pipe == pipe_t::PIPE_MTE3) {
            wait_intra_block(pipe_t::PIPE_MTE3, sync_id);
        } else if (pipe == pipe_t::PIPE_V) {
            wait_intra_block(pipe_t::PIPE_V, sync_id);
        }
    }
}

__aicore__ inline void asc_sync_subblock_wait(pipe_t pipe, int64_t flag_id)
{
    if ASC_IS_AIC {
        if (pipe == pipe_t::PIPE_S) {
            wait_flag_dev(pipe_t::PIPE_S, flag_id);
        } else if (pipe == pipe_t::PIPE_M) {
            wait_flag_dev(pipe_t::PIPE_M, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE1) {
            wait_flag_dev(pipe_t::PIPE_MTE1, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            wait_flag_dev(pipe_t::PIPE_MTE2, flag_id);
        } else if (pipe == pipe_t::PIPE_FIX) {
            wait_flag_dev(pipe_t::PIPE_FIX, flag_id);
        }
    } else if ASC_IS_AIV {
        if (pipe == pipe_t::PIPE_S) {
            wait_flag_dev(pipe_t::PIPE_S, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            wait_flag_dev(pipe_t::PIPE_MTE2, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE3) {
            wait_flag_dev(pipe_t::PIPE_MTE3, flag_id);
        } else if (pipe == pipe_t::PIPE_V) {
            wait_flag_dev(pipe_t::PIPE_V, flag_id);
        }
    }
}

__aicore__ inline void asc_sync_inter_wait(pipe_t pipe, int64_t flag_id)
{
    if ASC_IS_AIC {
        if (pipe == pipe_t::PIPE_S) {
            wait_flag_dev(pipe_t::PIPE_S, flag_id);
        } else if (pipe == pipe_t::PIPE_M) {
            wait_flag_dev(pipe_t::PIPE_M, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE1) {
            wait_flag_dev(pipe_t::PIPE_MTE1, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            wait_flag_dev(pipe_t::PIPE_MTE2, flag_id);
        } else if (pipe == pipe_t::PIPE_FIX) {
            wait_flag_dev(pipe_t::PIPE_FIX, flag_id);
        }
    } else if ASC_IS_AIV {
        if (pipe == pipe_t::PIPE_S) {
            wait_flag_dev(pipe_t::PIPE_S, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            wait_flag_dev(pipe_t::PIPE_MTE2, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE3) {
            wait_flag_dev(pipe_t::PIPE_MTE3, flag_id);
        } else if (pipe == pipe_t::PIPE_V) {
            wait_flag_dev(pipe_t::PIPE_V, flag_id);
        }
    }
}

__aicore__ inline void asc_sync_block_wait(pipe_t pipe, int64_t flag_id)
{
    if ASC_IS_AIC {
        if (pipe == pipe_t::PIPE_S) {
            wait_flag_dev(pipe_t::PIPE_S, flag_id);
        } else if (pipe == pipe_t::PIPE_M) {
            wait_flag_dev(pipe_t::PIPE_M, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE1) {
            wait_flag_dev(pipe_t::PIPE_MTE1, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            wait_flag_dev(pipe_t::PIPE_MTE2, flag_id);
        } else if (pipe == pipe_t::PIPE_FIX) {
            wait_flag_dev(pipe_t::PIPE_FIX, flag_id);
        }
    } else if ASC_IS_AIV {
        if (pipe == pipe_t::PIPE_S) {
            wait_flag_dev(pipe_t::PIPE_S, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE2) {
            wait_flag_dev(pipe_t::PIPE_MTE2, flag_id);
        } else if (pipe == pipe_t::PIPE_MTE3) {
            wait_flag_dev(pipe_t::PIPE_MTE3, flag_id);
        } else if (pipe == pipe_t::PIPE_V) {
            wait_flag_dev(pipe_t::PIPE_V, flag_id);
        }
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
