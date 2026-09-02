/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "sync.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SYNC_SYNC_H
#endif

#ifndef INCLUDE_C_API_SYNC_SYNC_H
#define INCLUDE_C_API_SYNC_SYNC_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/instr_impl/npu_arch_2201/sync_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/sync_impl.h"
#endif

__aicore__ inline void asc_sync_notify(pipe_t pipe, pipe_t tpipe, event_t id);

__aicore__ inline void asc_sync_wait(pipe_t pipe, pipe_t tpipe, event_t id);

__aicore__ inline void asc_sync_pipe(pipe_t pipe);

__aicore__ inline void asc_sync_vec(int id);

__aicore__ inline void asc_sync_mte3(int id);

__aicore__ inline void asc_sync_mte2(int id);

__aicore__ inline void asc_sync();

__aicore__ inline void asc_sync_block_arrive(pipe_t pipe, int64_t flag_id);

__aicore__ inline void asc_sync_subblock_arrive(pipe_t pipe, int64_t flag_id);

__aicore__ inline void asc_sync_inter_arrive(pipe_t pipe, int64_t flag_id);

__aicore__ inline void asc_sync_data_barrier(mem_dsb_t arg);

__aicore__ inline void asc_sync_intra_arrive(pipe_t pipe, uint64_t sync_id);

__aicore__ inline void asc_sync_intra_wait(pipe_t pipe, uint64_t sync_id);

__aicore__ inline void asc_sync_subblock_wait(pipe_t pipe, int64_t flag_id);

__aicore__ inline void asc_sync_inter_wait(pipe_t pipe, int64_t flag_id);

__aicore__ inline void asc_sync_block_wait(pipe_t pipe, int64_t flag_id);

__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id);

__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode);

__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id);

__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode);

[[deprecated("NOTICE: asc_sync_vec() is deprecated. "
             "Please use asc_sync() instead")]]
__aicore__ inline void asc_sync_vec();

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SYNC_SYNC_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SYNC_SYNC_H
#endif
