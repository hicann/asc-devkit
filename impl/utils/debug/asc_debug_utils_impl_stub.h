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
 * \file asc_debug_utils_impl_stub.h
 * \brief stub implementations for unsupported NPU architectures
 */
#ifndef IMPL_UTILS_DEBUG_ASC_DEBUG_UTILS_IMPL_STUB_H
#define IMPL_UTILS_DEBUG_ASC_DEBUG_UTILS_IMPL_STUB_H

#include "impl/utils/sys_macros.h"

namespace __asc_aicore {
__aicore__ inline void asc_entire_dcci_impl(__gm__ uint64_t* ptr) {}

__aicore__ inline uint64_t asc_debug_get_system_cycle_impl() { return 0; }

__aicore__ inline uint32_t asc_debug_get_core_idx_impl() { return 0; }

__aicore__ inline uint64_t asc_debug_get_block_idx_impl() { return 0; }

__aicore__ inline void sync_all_impl() {}

template <uint64_t timeoutCycle>
__aicore__ inline void ringbuf_wait_rts_sync_impl()
{}

template <typename T>
__aicore__ constexpr inline DumpTensorDataType get_dump_datatype_impl()
{
    return DumpTensorDataType::ACL_MAX;
}

} // namespace __asc_aicore

#endif // IMPL_UTILS_DEBUG_ASC_DEBUG_UTILS_IMPL_STUB_H
