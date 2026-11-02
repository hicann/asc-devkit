/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ins_temp_all_to_all_v_pairwise.h"
#include <algorithm>
#include <map>
#include <set>

namespace mc2_ops_hccl {
constexpr u32 RING_IN_THREAD_OFFSET = 1;
constexpr u32 PAIRWISE_SLOT_NUM = 2; // scratch 2 槽
// 每 ring 对端最大并发 channel 数：channel 切分与子线程数（每卡 1 线程/channel）均由它约束
constexpr u32 RING_MAX_CH_NUM = 2;
// 小数据不切多通道的分界：切分的收益是双通道带宽叠加，代价是每通道一次独立的 ACK/DATA 握手。
// 经验值：单通道发 2MB 已进入带宽饱和区，低于该量级时传输时间与握手延迟同量级，叠加带宽抵不过双份同步开销
constexpr u64 RING_SMALL_DATA_THRESHOLD = 2 * 1024 * 1024;
// cclBuff 与 scratch 共用 hcclBuff 基址，需分离偏移空间避免槽位覆盖：
//   scratch 区 [0, PAIRWISE_SLOT_NUM × stride)
//   cclBuff  区 [PAIRWISE_SLOT_NUM × stride, scratchMultiple_ × stride)
constexpr u32 CCL_BUFF_BASE_SLOT = PAIRWISE_SLOT_NUM;

namespace {
#ifndef AICPU_COMPILE
void FilterPairwiseChannelsByDie(HcclComm comm, u32 myRank, std::vector<HcclChannelDesc>& channels)
{
    std::map<u32, std::set<EndpointAttrDieId>> seenDiesByPeer;
    std::vector<HcclChannelDesc> selectedChannels;
    selectedChannels.reserve(channels.size());
    for (const auto& channel : channels) {
        EndpointDesc localEndpoint = channel.localEndpoint;
        EndpointAttrDieId dieId = 0;
        const HcclResult ret =
            HcclRankGraphGetEndpointInfo(comm, myRank, &localEndpoint, ENDPOINT_ATTR_DIE_ID, sizeof(dieId), &dieId);
        // Match HCCL: keep the link when the endpoint does not expose a die id.
        if (ret != HCCL_SUCCESS || seenDiesByPeer[channel.remoteRank].insert(dieId).second) {
            selectedChannels.push_back(channel);
        }
    }
    channels.swap(selectedChannels);
}
#endif
} // namespace

bool InsTempAllToAllVPairwise::IsSupportedTopology(const TopoInfoWithNetLayerDetails* topoInfo)
{
    constexpr u32 MAX_RANK_SIZE = 128U;
    constexpr u32 UNIT_RANK_SIZE = THREAD_SET_NUM * RANK_NUM_PER_BOARD;
    return topoInfo != nullptr && topoInfo->topoLevelNums > 1U && topoInfo->level0Topo == Level0Shape::MESH_1D &&
           topoInfo->deviceNumPerModule == RANK_NUM_PER_BOARD && topoInfo->level0Symmetric &&
           topoInfo->userRankSize >= UNIT_RANK_SIZE && topoInfo->userRankSize <= MAX_RANK_SIZE &&
           topoInfo->userRankSize % UNIT_RANK_SIZE == 0;
}

InsTempAllToAllVPairwise::InsTempAllToAllVPairwise(
    const OpParam& param, const u32 rankId, const std::vector<std::vector<u32>>& subCommRanks)
    : InsAlgTemplateBase(param, rankId, subCommRanks)
{}

InsTempAllToAllVPairwise::~InsTempAllToAllVPairwise() {}

// 拓扑校验与参数推导：CalcRes/InitParam 两入口共用，保证校验口径一致
HcclResult InsTempAllToAllVPairwise::CheckAndDeriveTopology()
{
    // 防御校验：选择链保证 rankSize 为 16 的倍数（2 流集合 × 8 卡/板），非整除时截断除法会丢板、静默错配
    if (templateRankSize_ % (THREAD_SET_NUM * RANK_NUM_PER_BOARD) != 0) {
        HCCL_ERROR(
            "[InsTempAllToAllVPairwise][CheckAndDeriveTopology] templateRankSize_[%u] not divisible by "
            "THREAD_SET_NUM[%u] * RANK_NUM_PER_BOARD[%u], check selector guard",
            templateRankSize_, THREAD_SET_NUM, RANK_NUM_PER_BOARD);
        return HCCL_E_INTERNAL;
    }
    // 从 templateRankSize_ 推导拓扑参数
    // rankSize = THREAD_SET_NUM × boardNumPerStreamSet_ × RANK_NUM_PER_BOARD
    // 例如：128 = 2 × 8 × 8，64 = 2 × 4 × 8，32 = 2 × 2 × 8
    boardNumPerStreamSet_ = templateRankSize_ / RANK_NUM_PER_BOARD / THREAD_SET_NUM;
    ringStepNum_ = RANK_NUM_PER_BOARD;
    ringInSubThreadNum_ = RANK_NUM_PER_BOARD * RING_MAX_CH_NUM; // 每卡 1 线程/channel
    ringInterSubThreadNum_ = RANK_NUM_PER_BOARD * RING_MAX_CH_NUM;
    // cclBuff 分区：组内 ring 16 槽（8 step × 2 channel）、fullMesh 8 槽、组间 ring 16 槽
    ringInCclBuffBase_ = 0;
    fullmeshCclBuffBase_ = RANK_NUM_PER_BOARD * RING_MAX_CH_NUM;
    ringInterCclBuffBase_ = RANK_NUM_PER_BOARD * RING_MAX_CH_NUM + RANK_NUM_PER_BOARD;
    // cclBuff 总槽数 = (2×RING_MAX_CH_NUM + 1) × RANK_NUM_PER_BOARD，scratch 另占 2 槽
    cclBuffSlotNum_ = 2 * RANK_NUM_PER_BOARD * RING_MAX_CH_NUM + RANK_NUM_PER_BOARD;
    scratchMultiple_ = PAIRWISE_SLOT_NUM + cclBuffSlotNum_;
    HCCL_INFO(
        "[InsTempAllToAllVPairwise][CheckAndDeriveTopology] templateRankSize_[%u] boardNumPerStreamSet_[%u] "
        "THREAD_SET_NUM[%u] ringStepNum_[%u] scratchMultiple_[%llu]",
        templateRankSize_, boardNumPerStreamSet_, THREAD_SET_NUM, ringStepNum_, scratchMultiple_);
    return HCCL_SUCCESS;
}

HcclResult InsTempAllToAllVPairwise::CalcRes(
    HcclComm comm, const OpParam& param, const TopoInfoWithNetLayerDetails* topoInfo,
    AlgResourceRequest& resourceRequest)
{
    CHK_PTR_NULL(topoInfo);
    CHK_PRT_RET(
        !IsSupportedTopology(topoInfo),
        HCCL_WARNING(
            "[InsTempAllToAllVPairwise][CalcRes] unsupported topology, rankSize[%u], "
            "level0Topo[%u], levels[%u], deviceNumPerModule[%u], symmetric[%u].",
            topoInfo->userRankSize, static_cast<u32>(topoInfo->level0Topo), topoInfo->topoLevelNums,
            topoInfo->deviceNumPerModule, static_cast<u32>(topoInfo->level0Symmetric)),
        HCCL_E_NOT_SUPPORT);
    // 拓扑校验与参数推导（与 InitParam 共用入口）
    CHK_RET(CheckAndDeriveTopology());

    // Request all peers, then retain the first link per peer/die as in HCCL.
    std::vector<HcclChannelDesc> myChannelDescs;
    CHK_RET(CalcChannelRequestMesh1D(comm, param, topoInfo, subCommRanks_, myChannelDescs));
#ifndef AICPU_COMPILE
    FilterPairwiseChannelsByDie(comm, topoInfo->userRank, myChannelDescs);
#endif
    resourceRequest.channels.push_back(myChannelDescs);

    // 1 主 + 组内 ring + 组间 ring（fullMesh 复用组内前 RANK_NUM_PER_BOARD 条）
    resourceRequest.slaveThreadNum = ringInSubThreadNum_ + ringInterSubThreadNum_;
    HCCL_INFO("[InsTempAllToAllVPairwise][CalcRes] slaveThreadNum is [%u]", resourceRequest.slaveThreadNum);
    // 每子线程 1 个线程池 notify：idx 0 供 PreSync 主→从唤醒；
    // PostSync 用的是主线程池的 idx（归主线程申请），与子线程池无关
    u32 notifyNumPerSubThread = 1;
    for (u32 index = 0; index < resourceRequest.slaveThreadNum; index++) {
        resourceRequest.notifyNumPerThread.push_back(notifyNumPerSubThread);
    }
    resourceRequest.notifyNumOnMainThread = resourceRequest.slaveThreadNum;
    return HCCL_SUCCESS;
}

u64 InsTempAllToAllVPairwise::CalcScratchMultiple(BufferType inBuffType, BufferType outBuffType)
{
    (void)inBuffType;
    (void)outBuffType;
    // scratch 2 槽 + cclBuff 分区（组内 ring N×8 / fullMesh 8 / 组间 ring N×8，槽位按 round 独占）
    return PAIRWISE_SLOT_NUM + 2 * RANK_NUM_PER_BOARD * RING_MAX_CH_NUM + RANK_NUM_PER_BOARD;
}

// 配对要求对称互指（i 配 j 当且仅当 j 配 i），保证双向对称不死锁，表中数字为本轮对端板号：
// 1) boardNumPerStreamSet_ 为 2 的幂：XOR 配对（i XOR t），round 0 组内全自环，例 N=4，组内：
//      round | 板0 | 板1 | 板2 | 板3
//        0   |  0  |  1  |  2  |  3   ← 全自环轮，RunFullMesh
//        1   |  1  |  0  |  3  |  2
//        2   |  2  |  3  |  0  |  1   （三轮覆盖 C(4,2)=6 对）
//        3   |  3  |  2  |  1  |  0
// 2) 非 2 的幂：统一反射配对 (t - i) mod N，组内组外同式，任意 N 成立，例 N=3，组内：
//      round | 板0 | 板1 | 板2
//        0   |  0  |  2  |  1   ← 板0 自环
//        1   |  1  |  0  |  2   ← 板2 自环
//        2   |  2  |  1  |  0   ← 板1 自环
//    自环板本轮无组内对端，由 KernelRun 在该轮调 RunFullMesh（板内 a2a）填空
// 组间：每轮为流集合间双射，t=0..N-1 恰好覆盖 N² 跨流集合对
u32 InsTempAllToAllVPairwise::GetBoardSendRecvMatrix(u32 round, bool isInterStreamSet) const
{
    // 本流集合内相对 board（0 ~ boardNumPerStreamSet_-1）
    u32 relativeBoard = currBoard_ % boardNumPerStreamSet_;
    u32 targetRelative = 0;
    if ((boardNumPerStreamSet_ & (boardNumPerStreamSet_ - 1)) == 0) {
        targetRelative = relativeBoard ^ round;
    } else {
        targetRelative = (round + boardNumPerStreamSet_ - relativeBoard) % boardNumPerStreamSet_;
    }
    if (isInterStreamSet) {
        // 组间 ring：target 流集合 = 对侧流集合
        u32 targetStreamSet = (currStreamSet_ == 0) ? 1 : 0;
        return targetRelative + targetStreamSet * boardNumPerStreamSet_;
    }
    // 组内 ring：target 流集合 = 本流集合
    return targetRelative + currStreamSet_ * boardNumPerStreamSet_;
}

u32 InsTempAllToAllVPairwise::GetRankSendRecvMatrix(u32 targetBoard, u32 step) const
{
    // XOR 配对：保证同 step 两两 rank 对称互指（i XOR s XOR s = i）
    u32 targetRankIndex = currRankIndex_ ^ step;
    u32 targetRank = targetBoard * RANK_NUM_PER_BOARD + targetRankIndex;
    return targetRank;
}

// cclBuff 槽位布局（槽容量单位 scratchBufferSizePerRank_，共 40 槽，与 rankSize 无关）：
//   [0  .. 16)  组内 ring：slot = step×2 + ch（step 0..7 为对端板内 rank 序，ch 为通道号）
//   [16 .. 24)  fullMesh：板内 7 个 pair 对称分槽，单次使用不跨轮
//   [24 .. 40)  组间 ring：slot = 24 + step×2 + ch
// 每 step 每 channel 独占一槽（+ch 偏移在 TX/PostCopy 调用处补上）：不同 channel 读写不同槽、
// 区间永不重叠；同 channel 跨轮复用时，对端写入门控（本端 ACK）与 PostCopy 同挂 subThreads[ch]，
// 线程 FIFO 保序——不加同步即消除跨轮写入与未完成 PostCopy 的竞态
void InsTempAllToAllVPairwise::CalcCclBuffIdx(
    bool isInterStreamSet, u32 partnerIdx, u32& myCclBuffIdx, u32& remoteCclBuffIdx) const
{
    u32 baseSlot = isInterStreamSet ? ringInterCclBuffBase_ : ringInCclBuffBase_;
    myCclBuffIdx = baseSlot + partnerIdx * RING_MAX_CH_NUM;
    remoteCclBuffIdx = myCclBuffIdx;
}

HcclResult InsTempAllToAllVPairwise::InitParam(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempAllToAllVPairwise][InitParam] Run Start");

    // 对称内存路径强制 loopTimes=1，单轮数据量不受 cclBuff 槽容量约束（槽位已按 round 独占扩容）；
    // 本模板未适配零拷贝，显式拒绝防槽越界
    if (tempAlgParams.enableRemoteMemAccess) {
        HCCL_ERROR("[InsTempAllToAllVPairwise][InitParam] symmetric memory not supported, "
                   "single-pair data may exceed cclBuff slot capacity.");
        return HCCL_E_INTERNAL;
    }

    // 拓扑校验与参数推导（与 CalcRes 共用入口，HCCL_ALGO 显式配置绕过 selector 守卫时兜底）
    CHK_RET(CheckAndDeriveTopology());

    dataType_ = param.all2AllVDataDes.sendType;
    dataTypeSize_ = DATATYPE_SIZE_TABLE[dataType_];

    // myAlgRank_ 为逻辑 rank，subCommRanks_ 已扁平化全 rank
    auto iter = std::find(subCommRanks_[0].begin(), subCommRanks_[0].end(), myRank_);
    if (iter != subCommRanks_[0].end()) {
        myAlgRank_ = std::distance(subCommRanks_[0].begin(), iter);
    } else {
        HCCL_ERROR("[InsTempAllToAllVPairwise][InitParam] myRank_[%u] not found in subCommRanks_[0].", myRank_);
        return HCCL_E_INTERNAL;
    }

    // rankSize = THREAD_SET_NUM × boardNumPerStreamSet_ × RANK_NUM_PER_BOARD
    currBoard_ = myAlgRank_ / RANK_NUM_PER_BOARD;
    currRankIndex_ = myAlgRank_ % RANK_NUM_PER_BOARD;
    currStreamSet_ = currBoard_ / boardNumPerStreamSet_;

    scratchBufferSizePerRank_ = tempAlgParams.inputSliceStride;
    threadNum_ = templateResource.threads.size();

    u32 expectedThreadNum = 1 + ringInSubThreadNum_ + ringInterSubThreadNum_;
    if (threadNum_ != expectedThreadNum) {
        HCCL_ERROR("[InsTempAllToAllVPairwise] threadNum_ is [%u], but expected [%u]", threadNum_, expectedThreadNum);
        return HCCL_E_INTERNAL;
    }

    // 子流分组：组内 ringInSubThreadNum_ 条（RANK_NUM_PER_BOARD × 2 ch），组间同理
    // fullMesh 复用组内前 RANK_NUM_PER_BOARD 条（subThreadsRingIn_[0..RANK_NUM_PER_BOARD-1]，每 rank 1 条）
    u32 ringInterThreadOffset = RING_IN_THREAD_OFFSET + ringInSubThreadNum_;
    subThreadsRingIn_.assign(
        templateResource.threads.begin() + RING_IN_THREAD_OFFSET,
        templateResource.threads.begin() + RING_IN_THREAD_OFFSET + ringInSubThreadNum_);
    subThreadsRingInter_.assign(
        templateResource.threads.begin() + ringInterThreadOffset,
        templateResource.threads.begin() + ringInterThreadOffset + ringInterSubThreadNum_);

    HCCL_INFO(
        "[InsTempAllToAllVPairwise][InitParam] myAlgRank_[%u] currBoard_[%u] currRankIndex_[%u] "
        "currStreamSet_[%u] threadNum_[%u]",
        myAlgRank_, currBoard_, currRankIndex_, currStreamSet_, threadNum_);
    return HCCL_SUCCESS;
}

void InsTempAllToAllVPairwise::GetNotifyIdxMainToSub(std::vector<u32>& notifyIdxMainToSub)
{
    // main→sub 全部用 idx 0：PreSync 一次性唤醒所有子流
    notifyIdxMainToSub.assign(ringInSubThreadNum_ + ringInterSubThreadNum_, 0);
}

void InsTempAllToAllVPairwise::GetNotifyIdxSubToMain(std::vector<u32>& notifyIdxSubToMain)
{
    // sub→main 每子流独立 idx：PostSync 逐一等待全部子流完工
    notifyIdxSubToMain.clear();
    for (u32 i = 0; i < ringInSubThreadNum_ + ringInterSubThreadNum_; i++) {
        notifyIdxSubToMain.push_back(i);
    }
}

HcclResult InsTempAllToAllVPairwise::PostCopy(
    const TemplateDataParams& tempAlgParams, const ThreadHandle& thread, u32 myCclBuffIdx, u32 remoteRank,
    const std::vector<u64>& recvOffsetSplit, const std::vector<u64>& recvSizeSplit)
{
    // 对端 SendRecvWrite 已把数据写入本端 cclBuff 的 myCclBuffIdx 槽，此处拷到 OUTPUT 的 rdispls[remoteRank] 处
    u64 cclBuffOffsetBase = (CCL_BUFF_BASE_SLOT + myCclBuffIdx) * scratchBufferSizePerRank_;
    for (u32 ch = 0; ch < recvOffsetSplit.size(); ch++) {
        if (recvSizeSplit[ch] == 0) {
            continue;
        }
        DataSlice cclBuffSlice = DataSlice(
            tempAlgParams.buffInfo.hcclBuff.addr, cclBuffOffsetBase + recvOffsetSplit[ch], recvSizeSplit[ch],
            recvSizeSplit[ch] / dataTypeSize_);
        DataSlice usrOutSlice = DataSlice(
            tempAlgParams.buffInfo.outputPtr, tempAlgParams.rdispls[remoteRank] * dataTypeSize_ + recvOffsetSplit[ch],
            recvSizeSplit[ch], recvSizeSplit[ch] / dataTypeSize_);
        HCCL_INFO(
            "[PostCopy] ch[%u] src hcclBuff+offset[%llu] size[%llu] -> "
            "dst outputPtr+offset[%llu] size[%llu]",
            ch, cclBuffOffsetBase + recvOffsetSplit[ch], recvSizeSplit[ch],
            tempAlgParams.rdispls[remoteRank] * dataTypeSize_ + recvOffsetSplit[ch], recvSizeSplit[ch]);
        CHK_RET(LocalCopy(thread, cclBuffSlice, usrOutSlice));
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllToAllVPairwise::RunRingStep(
    const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 targetRank, u32 myCclBuffIdx,
    u32 remoteCclBuffIdx, const std::vector<ThreadHandle>& subThreads)
{
    const std::map<u32, std::vector<ChannelInfo>>& channels = templateResource.channels;
    if (channels.find(targetRank) == channels.end() || channels.at(targetRank).empty()) {
        HCCL_ERROR("[InsTempAllToAllVPairwise][RunRingStep] targetRank[%u] has no channel", targetRank);
        return HCCL_E_INTERNAL;
    }
    const std::vector<ChannelInfo>& channelList = channels.at(targetRank);
    // 每个对端 rank 最多用 RING_MAX_CH_NUM 条 channel，分别绑到 subThreads[0]/[1] 并发
    u32 validChNum = std::min(static_cast<u32>(channelList.size()), RING_MAX_CH_NUM);

    // 小数据不切分：send 和 recv 均 <= 2MB 时，全量给 portGroupSize 最大的 channel。
    // 只用局部列表承载选中 channel，不改动共享 channels map 顺序（保持建链原样，
    // fullMesh 取 [0] 与后续轮次不受影响）
    u64 sendSize = tempAlgParams.sendCounts[targetRank] * dataTypeSize_;
    u64 recvSize = tempAlgParams.recvCounts[targetRank] * dataTypeSize_;
    std::vector<ChannelInfo> smallDataChList;
    if (validChNum > 1 && sendSize <= RING_SMALL_DATA_THRESHOLD && recvSize <= RING_SMALL_DATA_THRESHOLD) {
        validChNum = 1;
        // 选 portGroupSize 最大的 channel，CalcDataSplitByPortGroupCommon 按列表顺序取前 validChNum 个
        u32 maxPortIdx = 0;
        for (u32 i = 1; i < channelList.size(); i++) {
            if (channelList[i].portGroupSize > channelList[maxPortIdx].portGroupSize) {
                maxPortIdx = i;
            }
        }
        smallDataChList.push_back(channelList[maxPortIdx]);
        HCCL_INFO(
            "[InsTempAllToAllVPairwise][RunRingStep] small data sendSize[%llu] recvSize[%llu] "
            "<= 2MB, no split, use ch with max portGroupSize",
            sendSize, recvSize);
    }
    const std::vector<ChannelInfo>& effChList = smallDataChList.empty() ? channelList : smallDataChList;

    HCCL_INFO(
        "[InsTempAllToAllVPairwise][RunRingStep] targetRank[%u] myCclBuffIdx[%u] "
        "remoteCclBuffIdx[%u] channelNum[%u] validChNum[%u]",
        targetRank, myCclBuffIdx, remoteCclBuffIdx, static_cast<u32>(channelList.size()), validChNum);

    // 按 portGroup 带宽自适应切分 send/recv（validChNum=1 时退化为单片）
    CHK_RET(CalcDataSplitByPortGroupCommon(
        tempAlgParams.sendCounts[targetRank], dataTypeSize_, effChList, sendCountsSplit_, sendSizeSplit_,
        sendOffsetSplit_, validChNum));
    CHK_RET(CalcDataSplitByPortGroupCommon(
        tempAlgParams.recvCounts[targetRank], dataTypeSize_, effChList, recvCountsSplit_, recvSizeSplit_,
        recvOffsetSplit_, validChNum));

    std::vector<DataSlice> emptySrcSlices;
    std::vector<DataSlice> emptyDstSlices;
    SlicesList rxSlicesList(emptySrcSlices, emptyDstSlices);

    for (u32 ch = 0; ch < validChNum; ch++) {
        const bool hasSend = sendSizeSplit_[ch] > 0;
        const bool hasRecv = recvSizeSplit_[ch] > 0;
        if (!hasSend && !hasRecv) {
            continue;
        }
        const ChannelInfo& channelSendRecv = effChList[ch];

        if (hasSend) {
            // 发送：写入对端槽的偏移须与对端读出偏移一致。对端 PostCopy 按其 recvCounts[me]
            // 切分读（== 我的 sendCounts[target]，同一切分函数+同一 channel 确定性输出），
            // 故此处用 sendOffsetSplit_ 而非本端 recvOffsetSplit_（非对称 pair 二者不同）
            DataSlice txSrcSlice = DataSlice(
                tempAlgParams.buffInfo.inputPtr,
                tempAlgParams.sdispls[targetRank] * dataTypeSize_ + sendOffsetSplit_[ch], sendSizeSplit_[ch],
                sendCountsSplit_[ch]);
            DataSlice txDstSlice = DataSlice(
                channelSendRecv.remoteCclMem.addr,
                (CCL_BUFF_BASE_SLOT + remoteCclBuffIdx + ch) * scratchBufferSizePerRank_ + sendOffsetSplit_[ch],
                sendSizeSplit_[ch], sendCountsSplit_[ch]);
            std::vector<DataSlice> txSrcSlices{txSrcSlice};
            std::vector<DataSlice> txDstSlices{txDstSlice};
            HCCL_INFO(
                "[RunRingStep] ch[%u] TX src inputPtr+offset[%llu] size[%llu] -> "
                "dst remoteCclMem+offset[%llu] size[%llu]",
                ch, tempAlgParams.sdispls[targetRank] * dataTypeSize_ + sendOffsetSplit_[ch], sendSizeSplit_[ch],
                (CCL_BUFF_BASE_SLOT + remoteCclBuffIdx + ch) * scratchBufferSizePerRank_ + sendOffsetSplit_[ch],
                sendSizeSplit_[ch]);

            SlicesList txSlicesList(txSrcSlices, txDstSlices);
            if (hasRecv) {
                SendRecvInfo sendRecvInfo{{channelSendRecv, channelSendRecv}, {txSlicesList, rxSlicesList}, dataType_};
                CHK_RET(SendRecvWrite(sendRecvInfo, subThreads[ch]));
            } else {
                DataInfo sendInfo{channelSendRecv, {txSrcSlices, txDstSlices}, dataType_};
                CHK_RET(SendWrite(sendInfo, subThreads[ch]));
            }
        } else {
            // 纯收：等对端 SendWrite 落槽，握手之外无本端数据搬运
            DataInfo recvInfo{channelSendRecv, rxSlicesList, dataType_};
            CHK_RET(RecvWrite(recvInfo, subThreads[ch]));
        }

        // 子流自拷 PostCopy（接收侧）：独立于发送判定，send=0/recv>0 时数据由对端 SendWrite 写入
        if (hasRecv) {
            std::vector<u64> recvOffset{recvOffsetSplit_[ch]};
            std::vector<u64> recvSize{recvSizeSplit_[ch]};
            CHK_RET(PostCopy(tempAlgParams, subThreads[ch], myCclBuffIdx + ch, targetRank, recvOffset, recvSize));
        }
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllToAllVPairwise::RunRing(
    const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource, u32 round, bool isInterStreamSet)
{
    u32 targetBoard = GetBoardSendRecvMatrix(round, isInterStreamSet);
    HCCL_INFO(
        "[InsTempAllToAllVPairwise][RunRing] round[%u] isInterStreamSet[%d] targetBoard[%u]", round, isInterStreamSet,
        targetBoard);

    const std::vector<ThreadHandle>& subThreads = isInterStreamSet ? subThreadsRingInter_ : subThreadsRingIn_;

    // 对端板内各 rank 并发派发（每 rank 绑线程对 subThreads[step*2]/[step*2+1]）
    // 同步由 KernelRun 外层 PreSync/PostSync 统一管理，此处仅派发任务
    for (u32 step = 0; step < ringStepNum_; step++) {
        u32 targetRank = GetRankSendRecvMatrix(targetBoard, step);
        if (targetRank == myAlgRank_) {
            continue; // 自发自收在 fullMesh/直拷处理
        }
        if (targetRank >= templateRankSize_) {
            // 配对矩阵推导保证不越界，触发即内部缺陷；跳过会丢该 pair 数据，直接报错退出
            HCCL_ERROR(
                "[InsTempAllToAllVPairwise][RunRing] targetRank[%u] out of range, myAlgRank_[%u], "
                "round[%u] isInterStreamSet[%d] targetBoard[%u]",
                targetRank, myAlgRank_, round, isInterStreamSet, targetBoard);
            return HCCL_E_INTERNAL;
        }

        u32 myCclBuffIdx = 0;
        u32 remoteCclBuffIdx = 0;
        CalcCclBuffIdx(isInterStreamSet, step, myCclBuffIdx, remoteCclBuffIdx);

        std::vector<ThreadHandle> rankThreads{subThreads[step * 2], subThreads[step * 2 + 1]};
        CHK_RET(RunRingStep(tempAlgParams, templateResource, targetRank, myCclBuffIdx, remoteCclBuffIdx, rankThreads));
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllToAllVPairwise::RunFullMesh(
    const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    const std::map<u32, std::vector<ChannelInfo>>& channels = templateResource.channels;
    // 板内 a2a：RANK_NUM_PER_BOARD 流并发，每流 1 self LocalCopy + 其余 SendRecvWrite + PostCopy
    for (u32 i = 0; i < RANK_NUM_PER_BOARD; i++) {
        u32 targetRank = currBoard_ * RANK_NUM_PER_BOARD + i;
        const ThreadHandle& fullMeshThread = subThreadsRingIn_[i]; // fullMesh 复用组内前 RANK_NUM_PER_BOARD 条线程
        if (targetRank == myAlgRank_) {
            // 自发自收：直拷 input→output，跳 scratch
            DataSlice usrInSlices = DataSlice(
                tempAlgParams.buffInfo.inputPtr, tempAlgParams.sdispls[myAlgRank_] * dataTypeSize_,
                tempAlgParams.sendCounts[myAlgRank_] * dataTypeSize_, tempAlgParams.sendCounts[myAlgRank_]);
            DataSlice usrOutSlices = DataSlice(
                tempAlgParams.buffInfo.outputPtr, tempAlgParams.rdispls[myAlgRank_] * dataTypeSize_,
                tempAlgParams.recvCounts[myAlgRank_] * dataTypeSize_, tempAlgParams.recvCounts[myAlgRank_]);
            CHK_RET(LocalCopy(fullMeshThread, usrInSlices, usrOutSlices));
        } else {
            // 板内其他卡：四象限分派 + PostCopy（子流自拷），非对称 pair 握手与 RunRingStep 同构
            if (channels.find(targetRank) == channels.end() || channels.at(targetRank).empty()) {
                HCCL_ERROR("[InsTempAllToAllVPairwise][RunFullMesh] targetRank[%u] has no channel", targetRank);
                return HCCL_E_INTERNAL;
            }
            const ChannelInfo& channelSendRecv = channels.at(targetRank)[0];
            const bool hasSend = tempAlgParams.sendCounts[targetRank] > 0;
            const bool hasRecv = tempAlgParams.recvCounts[targetRank] > 0;
            if (!hasSend && !hasRecv) {
                continue;
            }
            u32 myCclBuffIdx = 0;
            u32 remoteCclBuffIdx = 0;
            // fullMesh 非 self：两端按各自视角的 partnerIdx 在 fullMesh 分区对称分配槽位
            u32 rmtRankIdx = i;
            u32 myPartnerIdx = (rmtRankIdx < currRankIndex_) ? rmtRankIdx : (rmtRankIdx - 1);
            u32 rmtPartnerIdx = (currRankIndex_ < rmtRankIdx) ? currRankIndex_ : (currRankIndex_ - 1);
            myCclBuffIdx = fullmeshCclBuffBase_ + myPartnerIdx;
            remoteCclBuffIdx = fullmeshCclBuffBase_ + rmtPartnerIdx;

            if (hasSend) {
                // 发送 → 对端 cclBuff 槽（偏移基于 CCL_BUFF_BASE_SLOT，与 scratch 分离）
                DataSlice txSrcSlice = DataSlice(
                    tempAlgParams.buffInfo.inputPtr, tempAlgParams.sdispls[targetRank] * dataTypeSize_,
                    tempAlgParams.sendCounts[targetRank] * dataTypeSize_, tempAlgParams.sendCounts[targetRank]);
                DataSlice txDstSlice = DataSlice(
                    channelSendRecv.remoteCclMem.addr,
                    (CCL_BUFF_BASE_SLOT + remoteCclBuffIdx) * scratchBufferSizePerRank_,
                    tempAlgParams.sendCounts[targetRank] * dataTypeSize_, tempAlgParams.sendCounts[targetRank]);
                std::vector<DataSlice> txSrcSlices{txSrcSlice};
                std::vector<DataSlice> txDstSlices{txDstSlice};
                HCCL_INFO(
                    "[RunFullMesh] TX targetRank[%u] src inputPtr+offset[%llu] size[%llu] -> "
                    "dst remoteCclMem+offset[%llu] size[%llu]",
                    targetRank, tempAlgParams.sdispls[targetRank] * dataTypeSize_,
                    tempAlgParams.sendCounts[targetRank] * dataTypeSize_,
                    (CCL_BUFF_BASE_SLOT + remoteCclBuffIdx) * scratchBufferSizePerRank_,
                    tempAlgParams.sendCounts[targetRank] * dataTypeSize_);

                if (hasRecv) {
                    std::vector<DataSlice> emptySrcSlices;
                    std::vector<DataSlice> emptyDstSlices;
                    SlicesList txSlicesList(txSrcSlices, txDstSlices);
                    SlicesList rxSlicesList(emptySrcSlices, emptyDstSlices);
                    SendRecvInfo sendRecvInfo{
                        {channelSendRecv, channelSendRecv}, {txSlicesList, rxSlicesList}, dataType_};
                    CHK_RET(SendRecvWrite(sendRecvInfo, fullMeshThread));
                } else {
                    // 纯发：对端同 pair 走 RecvWrite
                    DataInfo sendInfo{channelSendRecv, SlicesList(txSrcSlices, txDstSlices), dataType_};
                    CHK_RET(SendWrite(sendInfo, fullMeshThread));
                }
            } else {
                // 纯收：等对端 SendWrite 落槽
                std::vector<DataSlice> emptySrcSlices;
                std::vector<DataSlice> emptyDstSlices;
                DataInfo recvInfo{channelSendRecv, SlicesList(emptySrcSlices, emptyDstSlices), dataType_};
                CHK_RET(RecvWrite(recvInfo, fullMeshThread));
            }

            // 子流自拷：cclBuff myCclBuffIdx 槽 → OUTPUT（接收侧，独立于发送判定）
            if (hasRecv) {
                std::vector<u64> recvOffset{0};
                std::vector<u64> recvSize{tempAlgParams.recvCounts[targetRank] * dataTypeSize_};
                CHK_RET(PostCopy(tempAlgParams, fullMeshThread, myCclBuffIdx, targetRank, recvOffset, recvSize));
            }
        }
    }
    return HCCL_SUCCESS;
}

HcclResult InsTempAllToAllVPairwise::KernelRun(
    const OpParam& param, const TemplateDataParams& tempAlgParams, const TemplateResource& templateResource)
{
    HCCL_INFO("[InsTempAllToAllVPairwise][KernelRun] Run Start");
    CHK_RET(InitParam(param, tempAlgParams, templateResource));

    const std::vector<ThreadHandle>& threads = templateResource.threads;

    // 全量前同步
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> allSubThreads(threads.begin() + 1, threads.end());
        GetNotifyIdxMainToSub(notifyIdxMainToSub_);
        CHK_RET(PreSyncInterThreads(threads[0], allSubThreads, notifyIdxMainToSub_));
    }

    // 主流程
    const u32 relativeBoard = currBoard_ % boardNumPerStreamSet_;
    for (u32 round = 0; round < boardNumPerStreamSet_; round++) {
        CHK_RET(RunRing(tempAlgParams, templateResource, round, true));
        const u32 intraTarget = GetBoardSendRecvMatrix(round, false) % boardNumPerStreamSet_;
        if (intraTarget == relativeBoard) {
            CHK_RET(RunFullMesh(tempAlgParams, templateResource));
        } else {
            CHK_RET(RunRing(tempAlgParams, templateResource, round, false));
        }
    }

    // 全量后同步
    if (threadNum_ > 1) {
        std::vector<ThreadHandle> allSubThreads(threads.begin() + 1, threads.end());
        GetNotifyIdxSubToMain(notifyIdxSubToMain_);
        CHK_RET(PostSyncInterThreads(threads[0], allSubThreads, notifyIdxSubToMain_));
    }

    HCCL_INFO("[InsTempAllToAllVPairwise][KernelRun] Run End");
    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
