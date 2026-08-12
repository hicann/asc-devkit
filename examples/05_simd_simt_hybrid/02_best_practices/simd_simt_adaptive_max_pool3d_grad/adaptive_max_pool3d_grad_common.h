/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#pragma once

#include <cstdint>

#include "c_api/asc_simd.h"
#include "kernel_operator.h"

namespace pool3d_common {

template <typename T>
__aicore__ inline T min(T a, T b)
{
    return a < b ? a : b;
}

// Split the NC planes across the blocks actually launched. The first (nc % block_num) blocks take one extra plane, so
// every slice stays contiguous and the load imbalance is at most one plane. nc_start is a prefix sum rather than
// block_idx * nc_count, because nc_count differs between blocks.
__aicore__ inline void split_nc(uint32_t nc, uint32_t& nc_start, uint32_t& nc_count)
{
    const uint32_t block_index = static_cast<uint32_t>(block_idx);
    const uint32_t block_count = static_cast<uint32_t>(block_num);
    const uint32_t base = nc / block_count;
    const uint32_t remainder = nc % block_count;
    nc_count = base + (block_index < remainder ? 1U : 0U);
    nc_start = block_index * base + min(block_index, remainder);
}

// Fill a contiguous range of UB with the same float value using SIMD vector instructions.
__simd_vf__ inline void fill_float_ub(__ubuf__ float* dst, float value, uint32_t count)
{
    constexpr uint32_t one_repeat_size = asc_get_vf_len() / sizeof(float);
    vector_float value_reg;
    vector_bool mask;
    for (uint32_t offset = 0; offset < count; offset += one_repeat_size) {
        uint32_t active_count = count - offset < one_repeat_size ? count - offset : one_repeat_size;
        mask = asc_update_mask_b32(active_count);
        asc_duplicate_scalar(value_reg, value, mask);
        asc_storealign(dst + offset, value_reg, mask);
    }
}

__aicore__ inline void wait_mte2_to_scalar()
{
    asc_sync_notify(PIPE_MTE2, PIPE_S, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_S, EVENT_ID0);
}

__aicore__ inline void wait_mte2_to_v()
{
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
}

__aicore__ inline void wait_scalar_to_v()
{
    asc_sync_notify(PIPE_S, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_S, PIPE_V, EVENT_ID0);
}

__aicore__ inline void wait_v_to_mte3()
{
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
}

__aicore__ inline void wait_v_to_scalar()
{
    asc_sync_notify(PIPE_V, PIPE_S, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_S, EVENT_ID0);
}

__aicore__ inline void wait_mte3_to_v()
{
    asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0);
}

__aicore__ inline void wait_mte3_to_scalar()
{
    asc_sync_notify(PIPE_MTE3, PIPE_S, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_S, EVENT_ID0);
}

} // namespace pool3d_common
