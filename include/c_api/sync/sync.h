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
#include "impl/c_api/memory_base_impl/sync_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/sync_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Sets the hardware event flag of a source pipeline so that a paired asc_sync_wait on a destination pipeline can
 * proceed.
 *
 * @override_id asc_sync_notify__pipe_pipe_event
 * @archs 2201 3510
 * @param [in] pipe Source pipeline type whose preceding instructions must finish before the flag is set, of type
 * pipe_t; PIPE_ALL is not allowed.
 * @param [in] tpipe Destination pipeline type whose paired asc_sync_wait is to be released, of type pipe_t with the
 * same value set as pipe; PIPE_ALL is not allowed.
 * @param [in] id Synchronization event ID of type event_t; each (pipe, tpipe) combination owns eight independent event
 * IDs EVENT_ID0 through EVENT_ID7.
 */
__aicore__ inline void asc_sync_notify(pipe_t pipe, pipe_t tpipe, event_t id);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Blocks a destination pipeline until the paired asc_sync_notify on the source pipeline has set the event flag.
 *
 * @override_id asc_sync_wait__pipe_pipe_event
 * @archs 2201 3510
 * @param [in] pipe Source pipeline type whose preceding instructions are awaited, of type pipe_t; PIPE_ALL is not
 * allowed.
 * @param [in] tpipe Destination pipeline type whose subsequent instructions are blocked, of type pipe_t with the same
 * value set as pipe; PIPE_ALL is not allowed.
 * @param [in] id Synchronization event ID of type event_t; each (pipe, tpipe) combination owns eight independent event
 * IDs EVENT_ID0 through EVENT_ID7.
 */
__aicore__ inline void asc_sync_wait(pipe_t pipe, pipe_t tpipe, event_t id);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Blocks the subsequent instructions of one pipeline (or of all pipelines with PIPE_ALL) until that pipeline's
 * prior instructions complete.
 *
 * @override_id asc_sync_pipe__pipe
 * @archs 2201 3510
 * @param [in] pipe Pipeline category to block, of type pipe_t: PIPE_V, PIPE_M, PIPE_MTE1, PIPE_MTE2, PIPE_MTE3,
 * PIPE_FIX or PIPE_ALL; PIPE_S is not supported.
 */
__aicore__ inline void asc_sync_pipe(pipe_t pipe);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Synchronizes the PIPE_V pipeline so that all its preceding instructions complete before later PIPE_MTE2,
 * PIPE_MTE3 and PIPE_S instructions start, using the given sync ID.
 *
 * @override_id asc_sync_vec__i
 * @archs 2201 3510
 * @param [in] id Intra-core multi-pipeline sync ID in the range [0, 7]; it has the same meaning as the event_t enum
 * values used by asc_sync_notify.
 */
__aicore__ inline void asc_sync_vec(int id);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Synchronizes the MTE3 pipeline so that later MTE2 and vector instructions wait for its prior transfers.
 *
 * @override_id asc_sync_mte3__i
 * @archs 2201 3510
 * @param [in] id In-core multi-pipeline synchronization ID in the range [0, 7], with the same meaning as the event_t
 * enumerator used by asc_sync_notify (EVENT_ID0 to EVENT_ID7).
 */
__aicore__ inline void asc_sync_mte3(int id);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Synchronizes the MTE2 pipeline so that later MTE3 and vector instructions wait for its prior transfers.
 *
 * @override_id asc_sync_mte2__i
 * @archs 2201 3510
 * @param [in] id In-core multi-pipeline synchronization ID in the range [0, 7], with the same meaning as the event_t
 * enumerator used by asc_sync_notify (EVENT_ID0 to EVENT_ID7).
 */
__aicore__ inline void asc_sync_mte2(int id);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Synchronizes all hardware pipelines of the current core by blocking subsequent instructions until prior work
 * in every pipeline completes.
 *
 * @override_id asc_sync__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_sync();

/**
 * @ingroup simd_c_api.sync.inter_core_sync
 * @brief Notifies the scheduler that the AIC side of a single AI Core has reached the sync point, pairing with
 * asc_sync_block_wait for AIC-to-all-AIV synchronization.
 *
 * @override_id asc_sync_block_arrive__pipe_i64
 * @archs 2201 3510
 * @param [in] pipe Pipeline whose preceding instructions must complete before the notification is sent to the
 * scheduler, of type pipe_t; the value takes effect on every supported NPU architecture for this interface.
 * @param [in] flag_id Synchronization tag identifying one signal group, range [0, 15], where each flag_id owns an
 * independent 4-bit counter.
 */
__aicore__ inline void asc_sync_block_arrive(pipe_t pipe, int64_t flag_id);

/**
 * @ingroup simd_c_api.sync.inter_core_sync
 * @brief Notifies the scheduler that this AIV reached the sync point, pairing with asc_sync_subblock_wait for
 * synchronization among all AIVs of one AI Core.
 *
 * @override_id asc_sync_subblock_arrive__pipe_i64
 * @archs 2201 3510
 * @param [in] pipe Pipeline whose preceding instructions must complete before notifying the scheduler, of type pipe_t;
 * the interface is callable on AIV only and the accepted values follow the AIV table in the constraints.
 * @param [in] flag_id Synchronization tag identifying one signal group, range [0, 15], where each flag_id owns an
 * independent 4-bit counter.
 */
__aicore__ inline void asc_sync_subblock_arrive(pipe_t pipe, int64_t flag_id);

/**
 * @ingroup simd_c_api.sync.inter_core_sync
 * @brief Notifies the scheduler that this core reached the sync point for cross-AI-Core synchronization of all AICs or
 * all AIVs.
 *
 * @override_id asc_sync_inter_arrive__pipe_i64
 * @archs 2201 3510
 * @param [in] pipe Pipeline whose preceding instructions must complete before notifying the scheduler, of type pipe_t;
 * the value takes effect on every supported NPU architecture for this interface.
 * @param [in] flag_id Synchronization tag identifying one signal group, range [0, 15], where each flag_id owns an
 * independent 4-bit counter.
 */
__aicore__ inline void asc_sync_inter_arrive(pipe_t pipe, int64_t flag_id);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Blocks subsequent instructions until all previously issued memory accesses within a selected memory scope have
 * completed.
 *
 * @override_id asc_sync_data_barrier__memdsb
 * @archs 2201 3510
 * @param [in] arg Memory scope to wait on, of type mem_dsb_t: DSB_ALL waits for all memory accesses (in practice GM and
 * UB only), DSB_DDR waits for GM accesses, DSB_UB waits for UB accesses, and DSB_SEQ is a reserved value not yet
 * enabled.
 */
__aicore__ inline void asc_sync_data_barrier(mem_dsb_t arg);

/**
 * @ingroup simd_c_api.sync.inter_core_sync
 * @brief Notifies the scheduler from one core of a single AI Core for AIC-to-single-AIV synchronization.
 *
 * @override_id asc_sync_intra_arrive__pipe_u64
 * @archs 3510
 * @param [in] pipe Pipeline whose preceding instructions must complete before the notification is sent, of type pipe_t;
 * this argument takes effect for the arrive side and the supported values are tabulated per core type.
 * @param [in] sync_id Synchronization tag identifying one signal group, where an AI Core's AIC owns 32 sync_id values
 * (0 to 31) and each AIV owns 16 (0 to 15), with an independent 4-bit counter per value.
 */
__aicore__ inline void asc_sync_intra_arrive(pipe_t pipe, uint64_t sync_id);

/**
 * @ingroup simd_c_api.sync.inter_core_sync
 * @brief Blocks a pipeline until the paired asc_sync_intra_arrive side has signalled, completing AIC-to-single-AIV
 * synchronization.
 *
 * @override_id asc_sync_intra_wait__pipe_u64
 * @archs 3510
 * @param [in] pipe Pipeline whose subsequent instructions are blocked until the sync_id counter becomes non-zero, of
 * type pipe_t, with supported values tabulated per core type.
 * @param [in] sync_id Synchronization tag identifying one signal group, where the AIC owns 32 sync_id values (0 to 31)
 * and each AIV owns 16 (0 to 15), each with an independent 4-bit counter.
 */
__aicore__ inline void asc_sync_intra_wait(pipe_t pipe, uint64_t sync_id);

/**
 * @ingroup simd_c_api.sync.inter_core_sync
 * @brief Blocks a pipeline until all AIVs of the AI Core have arrived, completing synchronization among all subblocks.
 *
 * @override_id asc_sync_subblock_wait__pipe_i64
 * @archs 2201 3510
 * @param [in] pipe Pipeline whose subsequent instructions are blocked once the flag_id counter becomes non-zero, of
 * type pipe_t; the argument takes effect on 3510, where exactly this pipeline is blocked, while on 2201 it does not
 * take effect and the wait blocks regardless of the value passed.
 * @param [in] flag_id Synchronization tag identifying one signal group, range [0, 15], where each flag_id owns an
 * independent 4-bit counter.
 */
__aicore__ inline void asc_sync_subblock_wait(pipe_t pipe, int64_t flag_id);

/**
 * @ingroup simd_c_api.sync.inter_core_sync
 * @brief Blocks a pipeline until all participating AI Cores have arrived, completing cross-core synchronization of all
 * AICs or all AIVs.
 *
 * @override_id asc_sync_inter_wait__pipe_i64
 * @archs 2201 3510
 * @param [in] pipe Pipeline whose subsequent instructions are blocked once the flag_id counter becomes non-zero, of
 * type pipe_t; the argument takes effect on 3510, where exactly this pipeline is blocked, while on 2201 it does not
 * take effect and the wait blocks regardless of the value passed.
 * @param [in] flag_id Synchronization tag identifying one signal group, range [0, 15], where each flag_id owns an
 * independent 4-bit counter.
 */
__aicore__ inline void asc_sync_inter_wait(pipe_t pipe, int64_t flag_id);

/**
 * @ingroup simd_c_api.sync.inter_core_sync
 * @brief Blocks a pipeline until the paired asc_sync_block_arrive side has signalled, completing AIC-to-all-AIV
 * synchronization in one AI Core.
 *
 * @override_id asc_sync_block_wait__pipe_i64
 * @archs 2201 3510
 * @param [in] pipe Pipeline whose subsequent instructions are blocked once the flag_id counter becomes non-zero, of
 * type pipe_t; the argument takes effect on 3510, where exactly this pipeline is blocked, while on 2201 it does not
 * take effect and the wait blocks regardless of the value passed.
 * @param [in] flag_id Synchronization tag identifying one signal group, range [0, 15], where each flag_id owns an
 * independent 4-bit counter.
 */
__aicore__ inline void asc_sync_block_wait(pipe_t pipe, int64_t flag_id);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Acquires the mutex identified by mutex_id and blocks later instructions of the given pipe until every unlock
 * with the same mutex_id has been released.
 *
 * @override_id asc_lock__pipe_u8
 * @archs 3510
 * @param [in] pipe Pipeline whose subsequent instructions are blocked; a pipe_t enum value limited to the pipes
 * supported by the current core type.
 * @param [in] mutex_id Mutex identifier in the range [0, 31] that groups matching asc_lock and asc_unlock calls.
 */
__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Acquires the mutex identified by mutex_id with an explicit execute mode, optionally blocking later
 * instructions of the given pipe.
 *
 * @override_id asc_lock__pipe_u8_mutexmode
 * @archs 3510
 * @param [in] pipe Pipeline whose subsequent instructions are blocked when the blocking mode is selected; a pipe_t enum
 * value.
 * @param [in] mutex_id Mutex identifier in the range [0, 31] that groups matching asc_lock and asc_unlock calls.
 * @param [in] mode Optional asc_mutex_execute_mode value defaulting to ASC_LOCK_BLOCK; ASC_LOCK_NON_BLOCK (1) does not
 * block the pipe's execution.
 */
__aicore__ inline void asc_lock(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Releases the mutex identified by mutex_id after the preceding instructions of the given pipe finish executing.
 *
 * @override_id asc_unlock__pipe_u8
 * @archs 3510
 * @param [in] pipe Execution pipeline whose preceding instructions must complete before the mutex is released; a pipe_t
 * enum value.
 * @param [in] mutex_id Mutex identifier in the range [0, 31] shared with the matching asc_lock call.
 */
__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id);

/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Releases the mutex identified by mutex_id with an explicit execute mode after the relevant instructions of the
 * given pipe complete.
 *
 * @override_id asc_unlock__pipe_u8_mutexmode
 * @archs 3510
 * @param [in] pipe Execution pipeline whose preceding instructions gate the release, or whose prior instructions plus
 * same-mutex unlocks are awaited in non-blocking mode.
 * @param [in] mutex_id Mutex identifier in the range [0, 31] shared with the matching asc_lock call.
 * @param [in] mode Optional asc_mutex_execute_mode value defaulting to ASC_LOCK_BLOCK; ASC_LOCK_NON_BLOCK waits also
 * for all unlocks with the same mutex_id.
 */
__aicore__ inline void asc_unlock(pipe_t pipe, uint8_t mutex_id, const asc_mutex_execute_mode mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sync)
/**
 * @ingroup simd_c_api.sync.intra_core_sync
 * @brief Synchronizes all pipelines by issuing a PIPE_ALL barrier with no sync ID; the documented PIPE_V description
 * applies to the overload that takes an id, and this parameterless form is deprecated in favour of asc_sync.
 *
 * @override_id asc_sync_vec__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_sync_vec();

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SYNC_SYNC_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SYNC_SYNC_H
#endif
