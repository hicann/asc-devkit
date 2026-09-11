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
 * \file kernel_operator_determine_compute_sync_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_determine_compute_sync_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H__
#endif
#include "../../../include/basic_api/kernel_operator_common_intf.h"
#ifndef ASCENDC_MODULE_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H

namespace AscendC {
__aicore__ inline void InitDetermineComputeWorkspaceCalc(
    GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
{
    PipeBarrier<PIPE_ALL>();
    event_t eventID;
    auto blockNum = GetBlockNum();
    if (GetBlockIdx() == 0) {
        DuplicateImpl((__ubuf__ int32_t*)ubWorkspace.GetPhyAddr(), 0, B32_DATA_NUM_PER_BLOCK * blockNum);
        eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);
        DataCopy(gmWorkspace, ubWorkspace, B32_DATA_NUM_PER_BLOCK * blockNum);
    }
    ubWorkspace.SetValue(blockNum * B32_DATA_NUM_PER_BLOCK, 1);
    PipeBarrier<PIPE_ALL>();
}

__aicore__ inline bool CheckUBWorkspace(LocalTensor<int32_t>& ubWorkspace, int64_t blockIdx, int64_t blockNum)
{
    int32_t repeatTime = ubWorkspace.GetValue(blockNum * B32_DATA_NUM_PER_BLOCK);
    int64_t offset = 0;
    // example: core num is n, current core id is i, current repeat time is k:
    // matched if workspace values are kkkk...k0...000: [k] * (i), [0] * (n-i)
    for (; offset < blockIdx * B32_DATA_NUM_PER_BLOCK; offset += B32_DATA_NUM_PER_BLOCK) {
        if (ubWorkspace.GetValue(offset) != repeatTime) {
            return false;
        }
    }
    for (; offset < blockNum * B32_DATA_NUM_PER_BLOCK; offset += B32_DATA_NUM_PER_BLOCK) {
        if (ubWorkspace.GetValue(offset) != 0) {
            return false;
        }
    }
    return true;
}

__aicore__ inline void WaitPreBlockCalc(const GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
{
    PipeBarrier<PIPE_ALL>();
    event_t eventID;
    auto blockIdx = GetBlockIdx();
    auto blockNum = GetBlockNum();
    bool matchFlag;
    do {
        DataCopy(ubWorkspace, gmWorkspace, blockNum * B32_DATA_NUM_PER_BLOCK);
        eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_S));
        SetFlag<HardEvent::MTE2_S>(eventID);
        WaitFlag<HardEvent::MTE2_S>(eventID);
        matchFlag = CheckUBWorkspace(ubWorkspace, blockIdx, blockNum);
        eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE2));
        SetFlag<HardEvent::S_MTE2>(eventID);
        WaitFlag<HardEvent::S_MTE2>(eventID);
    } while (!matchFlag);
    PipeBarrier<PIPE_ALL>();
}

__aicore__ inline void NotifyNextBlockCalc(GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
{
    PipeBarrier<PIPE_ALL>();
    event_t eventID;
    auto blockIdx = GetBlockIdx();
    auto blockNum = GetBlockNum();
    int32_t repeatTime = ubWorkspace.GetValue(blockNum * B32_DATA_NUM_PER_BLOCK);
    if (blockIdx + 1 == blockNum) {
        DuplicateImpl((__ubuf__ int32_t*)ubWorkspace.GetPhyAddr(), 0, blockNum * B32_DATA_NUM_PER_BLOCK);
        eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);
        DataCopy(gmWorkspace, ubWorkspace, blockNum * B32_DATA_NUM_PER_BLOCK);
    } else {
        auto offset = blockIdx * B32_DATA_NUM_PER_BLOCK;
        eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventID);
        WaitFlag<HardEvent::S_V>(eventID);
        DuplicateImpl((__ubuf__ int32_t*)ubWorkspace[offset].GetPhyAddr(), repeatTime, B32_DATA_NUM_PER_BLOCK);
        eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);
        DataCopy(gmWorkspace[offset], ubWorkspace[offset], B32_DATA_NUM_PER_BLOCK);
    }
    ubWorkspace.SetValue(blockNum * B32_DATA_NUM_PER_BLOCK, repeatTime + 1);
    PipeBarrier<PIPE_ALL>();
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DETERMINE_COMPUTE_SYNC_IMPL_H__
#endif
