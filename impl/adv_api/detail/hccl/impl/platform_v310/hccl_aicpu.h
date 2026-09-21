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
 * \file hccl_aicpu.h
 * \brief
 */
#ifndef IMPL_V310_HCCL_AICPU_H
#define IMPL_V310_HCCL_AICPU_H

#include "../../common/hccl_aicpu_impl.h"

namespace AscendC {
template <const auto& config>
__aicore__ inline uint32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::GetRankDim()
{
    ASCENDC_HCCL_API_ASSERT(
        hcclContext_ != nullptr, { return UINT32_MAX; }, "Call GetRankDim failed, ensure InitV2 has been called!");
    return hcclContext_->rankSize;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::InitV2(
    GM_ADDR context, const void* initTiling)
{
    if (initTiling != nullptr) {
        const Mc2InitTilingInner* initTilingPtr = static_cast<const Mc2InitTilingInner*>(initTiling);
        debugMode_ = initTilingPtr->debugMode;
        devType_ = initTilingPtr->devType;
        tilingBaseAddr_ = reinterpret_cast<uint64_t>(initTiling);
        hcclContext_ = (__gm__ OpResCtx*)context;
        ASCENDC_HCCL_API_ASSERT(hcclContext_ != nullptr, { return; }, "Init Hccl failed, context addr is nullptr.");
        InitInner(hcclContext_->workspace, HcclTilingVersion::CONTEXT_DECOUPLE_VERSION);
        return;
    }
    // initTiling is nullptr, get opType from context
    hcclContext_ = (__gm__ OpResCtx*)context;
    ASCENDC_HCCL_API_ASSERT(hcclContext_ != nullptr, { return; }, "Init Hccl failed, context addr is nullptr.");
    InitInner(hcclContext_->workspace, HcclTilingVersion::CONTEXT_DECOUPLE_VERSION);
    const uint32_t opType = hcclContext_->opType[0];
    ASCENDC_HCCL_API_ASSERT(
        opType < static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL), { return; },
        "Call InitV2 failed, ensure cmdType is valid");
    ccOpParamTable_[opType] = hcclContext_->algInfo[0].opParam;
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SetCcTilingV2(uint64_t offset)
{
    ASCENDC_HCCL_API_ASSERT(
        curVersion_ == HcclTilingVersion::CONTEXT_DECOUPLE_VERSION, { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure Hccl::InitV2 func has been called successfully!");
    ASCENDC_HCCL_API_ASSERT(
        tilingBaseAddr_ != 0UL, { return HCCL_FAILED; },
        "Call SetCcTilingV2 failed, InitV2 must be called with valid initTiling!");
    const uint32_t opType = (reinterpret_cast<Mc2CcTilingInner*>(tilingBaseAddr_ + offset))->opType;
    ASCENDC_HCCL_API_ASSERT(
        opType < static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL), { return HCCL_FAILED; },
        "Call SetCcTiling failed, ensure cmdType is valid");
    ccOpTilingDataTable_[opType] = offset;
    for (uint8_t i = 0; i < HCCL_ALG_SUPPORT_NUM; i++) {
        if (hcclContext_->algInfo[i].offset == offset) {
            ccOpParamTable_[opType] = hcclContext_->algInfo[i].opParam;
            break;
        }
    }
    return HCCL_SUCCESS;
}
template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::BatchWrite(
    GM_ADDR batchWriteInfo, uint32_t itemNum, uint16_t queueID)
{
    return -1;
}

template <const auto& config>
template <bool commit>
__aicore__ inline HcclHandle HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::AlltoAllvWrite(
    GM_ADDR usrIn, GM_ADDR sendOffsets, GM_ADDR sendSizes, uint64_t remoteWinOffset, uint64_t localDataSize)
{
    return -1;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::InterHcclGroupSync(
    int8_t srcGroupID, HcclHandle srcHandleID)
{
    return;
}

template <const auto& config>
__aicore__ inline GM_ADDR HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::GetWindowsInAddr(uint32_t rankId)
{
    return 0UL;
}

template <const auto& config>
__aicore__ inline GM_ADDR HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::GetWindowsOutAddr(uint32_t rankId)
{
    return 0UL;
}

template <const auto& config>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Init(
    GM_ADDR context, __gm__ void* initTiling)
{
    return;
}

template <const auto& config>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::SetCcTiling(
    __gm__ void* ccOpTilingData)
{
    return HCCL_FAILED;
}

template <const auto& config>
template <ScopeType type>
__aicore__ inline void HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::QueueBarrier(uint16_t queueID)
{
    return;
}

template <const auto& config>
template <bool sync>
__aicore__ inline int32_t HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config>::Iterate(
    HcclHandle handleId, uint16_t* seqSlices, uint16_t seqSliceLen)
{
    return HCCL_FAILED;
}
} // namespace AscendC

#endif
