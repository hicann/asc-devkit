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
 * \file hccl_aicpu_def.h
 * \brief
 */
#ifndef IMPL_V220_HCCL_AICPU_DEF_H
#define IMPL_V220_HCCL_AICPU_DEF_H

namespace AscendC {

template <const auto& config>
class HcclImpl<HcclServerType::HCCL_SERVER_TYPE_AICPU, config> {
public:
    template <bool commit = false>
    __aicore__ inline HcclHandle AllReduce(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, uint8_t repeat = 1);

    template <bool commit = false>
    __aicore__ inline HcclHandle AllGather(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t sendCount, HcclDataType dataType, uint64_t strideCount,
        uint8_t repeat = 1);

    template <bool commit = false>
    __aicore__ inline HcclHandle ReduceScatter(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t recvCount, HcclDataType dataType, HcclReduceOp op,
        uint64_t strideCount, uint8_t repeat = 1);

    template <bool commit = false>
    __aicore__ inline HcclHandle AlltoAll(
        GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t dataCount, HcclDataType dataType, uint64_t strideCount = 0,
        uint8_t repeat = 1);

    template <bool commit = false>
    __aicore__ inline HcclHandle AlltoAllV(
        GM_ADDR sendBuf, void* sendCounts, void* sdispls, HcclDataType sendType, GM_ADDR recvBuf, void* recvCounts,
        void* rdispls, HcclDataType recvType, uint8_t repeat = 1);

    template <bool commit = false>
    __aicore__ inline HcclHandle BatchWrite(GM_ADDR batchWriteInfo, uint32_t itemNum, uint16_t queueID);

    template <bool commit = false>
    __aicore__ inline HcclHandle AlltoAllvWrite(
        GM_ADDR usrIn, GM_ADDR sendOffsets, GM_ADDR sendSizes, uint64_t remoteWinOffset, uint64_t localDataSize);

public:
    __aicore__ inline void Init(GM_ADDR context, __gm__ void* initTiling = nullptr);

    __aicore__ inline void InitV2(GM_ADDR context, const void* initTiling);

    __aicore__ inline int32_t SetCcTiling(__gm__ void* ccOpTilingData);

    __aicore__ inline int32_t SetCcTilingV2(uint64_t offset);

    __aicore__ inline void Commit(HcclHandle handleId);

    __aicore__ inline int32_t Wait(HcclHandle handleId);

    __aicore__ inline int32_t Query(HcclHandle handleId);

    __aicore__ inline void InterHcclGroupSync(int8_t srcGroupID, HcclHandle srcHandleID);

    template <ScopeType type = ScopeType::ALL>
    __aicore__ inline void QueueBarrier(uint16_t queueID);

    template <bool sync = true>
    __aicore__ inline int32_t Iterate(HcclHandle handleId, uint16_t* seqSlices, uint16_t seqSliceLen);

    template <bool sync = true>
    __aicore__ inline void Finalize();

public:
    __aicore__ inline GM_ADDR GetWindowsInAddr(uint32_t rankId);

    __aicore__ inline GM_ADDR GetWindowsOutAddr(uint32_t rankId);

    __aicore__ inline uint32_t GetRankId();

    __aicore__ inline uint32_t GetRankDim();

    __aicore__ inline uint16_t GetQueueNum() { return queueNum_; }

private:
    // Generic implementation for corresponding interface of each Prepare primitive. Return identifier(handleId) of
    // corresponding comm task. HandleId >= 0 when successful, otherwise return -1.
    template <bool commit = false>
    __aicore__ inline HcclHandle CommonPrepareImpl(const CommonPrepareParam& param);

    __aicore__ inline bool CheckCommonPrepareParamValid(const CommonPrepareParam& param);

    __aicore__ inline bool CheckAlltoAllVPrepareParamValid(const CommonPrepareParam& param);

    // Clear the finishedTurnCnt before aicore exists to ensure the correctness of next launch.
    __aicore__ inline void ResetFinishedTurnCnt();

    template <bool sync>
    __aicore__ inline void SendFinalizeMsg();

    __aicore__ inline void SendMsgToServer(
        uint16_t queId, const CommonPrepareParam& para, int8_t srcGroupID = -1,
        HcclHandle srcHandleID = INVALID_HANDLE_ID);

    __aicore__ inline void SendMsgToServer(const AlltoAllVParamExt& para);

    __aicore__ inline uint16_t GetStepSizeByHandle(HcclHandle handle);

    __aicore__ inline uint16_t GetStepCntsPerRepeatByHandle(HcclHandle handle);

    __aicore__ inline void SetCommitTurnCntToGm(uint8_t msgPos, uint64_t turnCnt, HcclHandle handleId);

    __aicore__ inline uint64_t WaitFinishCntFromGm(uint8_t msgPos, uint64_t expectedCnt);

    __aicore__ inline void InitWorkingFlag();

    __aicore__ inline void InitInner(uint64_t msgAddr, HcclTilingVersion version);

private:
    uint64_t ccOpTilingDataTable_[static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL)] = {0UL};
    uint64_t ccOpParamTable_[static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL)] = {0UL};
#if __NPU_ARCH__ == 3510
    __gm__ OpResCtx* hcclContext_;
#else
    __gm__ HcclCombineOpParam* hcclContext_;
#endif
    __gm__ HcclMsgArea* hcclMsgArea_;
    uint64_t tilingBaseAddr_;
    uint16_t queueNum_ = 0U;
    uint16_t handleId2CurrSliceId_[HCCL_MAX_HANDLE_ID] = {0U};
    uint16_t handleIdCommitTurnCnt_[HCCL_MAX_HANDLE_ID] = {0U};
    uint16_t handleIdWaitCallNum_[HCCL_MAX_HANDLE_ID] = {0U};
    uint8_t handleId2CmdType_[HCCL_MAX_HANDLE_ID] = {0U};
    int8_t handleIdMsgPosition_[HCCL_MAX_HANDLE_ID];
    uint8_t handleIdRepeat_[HCCL_MAX_HANDLE_ID] = {0U};
    uint8_t curMsgPosition_[MAX_QUE_NUM] = {0U};
    HcclHandle curHandleId_ = INVALID_HANDLE_ID;

    // Current msg position where Api write, starts from 0 and increases automatically, with a maximum of
    // HCCL_MSG_CNT-1. When HCCL_MSG_CNT is reached, take the remainder and recycling message area.
    // Prepare/BatchPrepare/Finalize/InterHcclGroupSync (supported in future versions) only use one message.
    HcclTilingVersion curVersion_ = HcclTilingVersion::INVALID_TILING_VERSION;
    uint8_t workingFlag_ = false;
    uint8_t debugMode_ = 0U;
    uint8_t devType_;
};
} // namespace AscendC

#endif
