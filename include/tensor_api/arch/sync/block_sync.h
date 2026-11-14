/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BLOCK_SYNC_H
#endif

/*!
 * \file block_sync.h
 * \brief Block, intra-block, inter-block, and sub-block synchronization interfaces.
 */
#ifndef INCLUDE_TENSOR_API_ARCH_SYNC_BLOCK_SYNC_H
#define INCLUDE_TENSOR_API_ARCH_SYNC_BLOCK_SYNC_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
namespace asc {
namespace te {

/** @brief Notifies all AIVs in the AI Core that synchronization on the selected pipeline can begin.
 *
 * @param pipe Pipeline associated with the synchronization flag.
 * @param flag_id Synchronization flag used by the paired wait interface.
 */
__aicore__ inline void sync_block_arrive(pipe_t pipe, int64_t flag_id);
/** @brief Waits for the paired arrive operation on the selected pipeline.
 *
 * @param pipe Pipeline associated with the synchronization flag.
 * @param flag_id Synchronization flag used by the paired arrive interface.
 */
__aicore__ inline void sync_block_wait(pipe_t pipe, int64_t flag_id);
/** @brief Notifies a single AIV in the AI Core that synchronization can begin.
 *
 * @param pipe Pipeline associated with the synchronization counter.
 * @param sync_id Synchronization counter used by the paired wait interface.
 */
__aicore__ inline void sync_intra_arrive(pipe_t pipe, uint64_t sync_id);
/** @brief Waits for the paired intra-core arrive operation.
 *
 * @param pipe Pipeline associated with the synchronization counter.
 * @param sync_id Synchronization counter used by the paired arrive interface.
 */
__aicore__ inline void sync_intra_wait(pipe_t pipe, uint64_t sync_id);
/** @brief Notifies other AI Cores that inter-core synchronization can begin.
 *
 * @param pipe Pipeline associated with the synchronization flag.
 * @param flag_id Synchronization flag used by the paired wait interface.
 */
__aicore__ inline void sync_inter_arrive(pipe_t pipe, int64_t flag_id);
/** @brief Waits for the paired inter-core arrive operation.
 *
 * @param pipe Pipeline associated with the synchronization flag.
 * @param flag_id Synchronization flag used by the paired arrive interface.
 */
__aicore__ inline void sync_inter_wait(pipe_t pipe, int64_t flag_id);
/** @brief Notifies all AIV subblocks in the AI Core that synchronization can begin.
 *
 * @param pipe Pipeline associated with the synchronization flag.
 * @param flag_id Synchronization flag used by the paired wait interface.
 */
__aicore__ inline void sync_subblock_arrive(pipe_t pipe, int64_t flag_id);
/** @brief Waits for the paired subblock arrive operation.
 *
 * @param pipe Pipeline associated with the synchronization flag.
 * @param flag_id Synchronization flag used by the paired arrive interface.
 */
__aicore__ inline void sync_subblock_wait(pipe_t pipe, int64_t flag_id);

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/sync/block_sync_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_SYNC_BLOCK_SYNC_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BLOCK_SYNC_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BLOCK_SYNC_H
#endif
