/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hccl_ccu_v0_def.h
 * \brief
 */
#ifndef IMPL_V310_HCCL_CCU_D100_DEF_H
#define IMPL_V310_HCCL_CCU_D100_DEF_H

namespace AscendC {

enum class AlgorithmType : uint8_t {
    CcuAllGatherMesh1D = 0,
    CcuAllGatherMeshMem2Mem1D,
    CcuAllGatherMesh2D,
    CcuSchedAllGatherConcurMeshNHRMultiLink = 3,
    CcuReduceScatterMesh1D = 50,
    CcuReduceScatterMeshMem2Mem1D,
    CcuReduceScatterMesh2D,
    CcuAllReduceMesh1D = 100,
    CcuAllReduceMeshMem2Mem1D,
    CcuAllReduceMesh2DOneShot,
    CcuReduceMesh1D,
    CcuReduceMesh2D,
    CcuAlltoAllMesh1D,
    CcuAlltoAllVMesh1D,
    CcuHalfAll2AllVMesh1D
};

template <const auto& config>
class HcclImpl<HcclServerType::HCCL_SERVER_TYPE_CCU, config> {
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
    __aicore__ inline HcclHandle AlltoAllvWrite(
        GM_ADDR usrIn, GM_ADDR sendOffsets, GM_ADDR sendSizes, uint64_t remoteWinOffset, uint64_t localDataSize);

    __aicore__ inline void Init(GM_ADDR context, __gm__ void* initTiling = nullptr);

    __aicore__ inline void InitV2(GM_ADDR context, const void* initTiling);

    __aicore__ inline int32_t SetCcTiling(__gm__ void* ccOpTilingData);

    __aicore__ inline int32_t SetCcTilingV2(uint64_t offset);

    __aicore__ inline void Commit(HcclHandle handleId);

    __aicore__ inline int32_t Wait(HcclHandle handleId);

    template <bool sync = true>
    __aicore__ inline void Finalize();

    __aicore__ inline uint32_t GetRankId() { return (newCcuFlag_ ? hcclNewContext_->rankId : hcclContext_->rankId); }

    __aicore__ inline uint32_t GetRankDim()
    {
        return (newCcuFlag_ ? hcclNewContext_->rankSize : hcclContext_->rankNum);
    }

private:
    __aicore__ inline void InitWorkingFlag();

    template <bool commit = false>
    __aicore__ inline HcclHandle CommonPrepareImpl(const CommonPrepareParam& param);

    __aicore__ inline void CommitMsgInner(const HcclHandle handleId);

    __aicore__ inline void CcuSendMsg(uint8_t resourceId, HcclHandle handleId);

    __aicore__ inline GM_ADDR GetCommitCkeAddr(uint8_t msgId, uint8_t missionIndex = 0);

    __aicore__ inline GM_ADDR GetWaitCkeAddr(uint8_t msgId, uint8_t missionIndex = 0);

    __aicore__ inline uint8_t GetKfcMissionNum(HcclHandle handleId) const;

    __aicore__ inline bool IsFinish(uint8_t reqId);

    __aicore__ inline void InitHandleInfo(uint8_t handleId);

    __aicore__ inline void InitCcuParam(const HcclHandle handleId);

    __aicore__ inline void InitInner(GM_ADDR context, HcclTilingVersion version);

    __aicore__ inline void CommitMsg(HcclHandle handleId);

    __aicore__ inline void CcuPrepareForOp(const HcclHandle handleId);
    __aicore__ inline void CcuPrepareForAllGather(__gm__ CommonPrepareParamCcu* commParam);
    __aicore__ inline void CcuPrepareForAllReduce(__gm__ CommonPrepareParamCcu* commParam);
    __aicore__ inline void CcuPrepareForAllToAll(__gm__ CommonPrepareParamCcu* commParam);
    __aicore__ inline void CcuPrepareForAllToAllV(
        __gm__ CommonPrepareParamCcu* commParam, __gm__ AlltoAllVParamCcu* allToAllVParam);
    __aicore__ inline void CcuPrepareForAllToAllVWrite(__gm__ CommonPrepareParamCcu* commParam);
    __aicore__ inline void CcuPrepareForReduceScatter(__gm__ CommonPrepareParamCcu* commParam);
    __aicore__ inline void CcuPrepareForAllReduceM2M(__gm__ CommonPrepareParamCcu* commParam);
    __aicore__ inline void CcuPrepareForAllGatherM2M(__gm__ CommonPrepareParamCcu* commParam);
    __aicore__ inline void CcuPrepareForConcurrentAllGatherM2M(__gm__ CommonPrepareParamCcu* commParam);
    __aicore__ inline void CcuPrepareForReduceScatterM2M(__gm__ CommonPrepareParamCcu* commParam);

private:
    __gm__ HcclCombineOpParam* hcclContext_;
    __gm__ HcclApi::OpResCtx* hcclNewContext_;

    HcclHandle curHandleId_ = INVALID_HANDLE_ID;
    bool newCcuFlag_ = false;

    uint8_t workingFlag_ = false;
    bool isInited_ = false;
    __gm__ uint64_t* finishCntGM_; // For soft sync
    uint64_t finishNum_;
    uint64_t finishNumTemp_;

    CCUMsg ccuMsg_;

    uint64_t ccOpTilingDataTable_[static_cast<uint32_t>(HcclCMDType::HCCL_CMD_ALL)] = {0UL};
    HcclTilingVersion curVersion_ = HcclTilingVersion::INVALID_TILING_VERSION;
    uint64_t tilingBaseAddr_;
    CcuPrepareParam ccuParam_;
    uint8_t handleReqId_[HCCL_MAX_HANDLE_ID];
    uint8_t handleRepeatCnt_[HCCL_MAX_HANDLE_ID];
    uint8_t handleCommitCnt_[HCCL_MAX_HANDLE_ID];
    uint8_t handleFinishCnt_[HCCL_MAX_HANDLE_ID];
    uint8_t handleNeedCommitCnt_[HCCL_MAX_HANDLE_ID];

    __gm__ CommonPrepareParamCcu* handleParamGM_;

    __gm__ AlltoAllVParamCcu* allToAllVParam_;

    uint8_t globalCurResId_ = 0;  // 全局的消息ID
    uint8_t globalCurWaitId_ = 0; // 全局的消息ID

    uint32_t globalCurCommitCnt_ = 0;
    uint32_t globalCurWaitCnt_ = 0;

    uint8_t ccuUsedXnNum_ = 0;

    bool msgQueueIsAvailable_[CCU_MAX_MSG_NUM];
    uint64_t xnData_[CCU_USED_XN_NUM];
};
} // namespace AscendC

#endif
