/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "hccl/hccl_types.h"
#include "hccl/base.h"
#include "hccl_res.h"
#include "dtype_common.h"
#include "hccl_common.h"
#include "hccl_rank_graph.h"
#include "acl/acl.h"
#include <memory>
#include <iostream>
#include "sim_communicator.h"
#include "sim_task.h"
#include "sim_world.h"
#include "sim_npu.h"
#include "sim_stream.h"
#include "sim_task_queue.h"
#include "sim_channel.h"
#include "alg_param.h"
#include "hcomm_diag.h"
#include "hccl_comm.h"
#include "hccl_res_expt.h"

using namespace mc2_ops_hccl;
using namespace HcclSim;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

std::mutex g_mutex;
thread_local ThreadHandle curThread;

HcclResult HcclRankGraphGetRankSizeByLayer(HcclComm comm, uint32_t netLayer, uint32_t* rankNum)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    simComm->topoModel_->GetInstSizeByNetLayer(simComm->GetRankId(), netLayer, rankNum);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetLayers(HcclComm comm, uint32_t** netLayers, uint32_t* netLayerNum)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    simComm->topoModel_->GetNetLayers(netLayers, netLayerNum);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetInstSizeListByLayer(
    HcclComm comm, uint32_t netLayer, uint32_t** instSizeList, uint32_t* listSize)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    simComm->topoModel_->GetInstSizeListByNetLayer(netLayer, instSizeList, listSize);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetLinks(
    HcclComm comm, uint32_t netLayer, uint32_t srcRank, uint32_t dstRank, CommLink** linkList, uint32_t* listSize)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    HcclSim::SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(simComm->GetRankId());
    auto devType = npu.GetDevType();
    simComm->topoModel_->GetLinks(devType, netLayer, srcRank, dstRank, linkList, listSize);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetTopoTypeByLayer(HcclComm comm, uint32_t netLayer, CommTopo* topoType)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    HcclSim::SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(simComm->GetRankId());
    auto devType = npu.GetDevType();
    simComm->topoModel_->GetInstTopoTypeByNetLayer(devType, netLayer, topoType);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetRanksByLayer(HcclComm comm, uint32_t netLayer, uint32_t** ranks, uint32_t* rankNum)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    simComm->topoModel_->GetInstRanksByNetLayer(simComm->GetRankId(), netLayer, ranks, rankNum);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetTopoInstsByLayer(
    HcclComm comm, uint32_t netLayer, uint32_t** topoInsts, uint32_t* topoInstNum)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    simComm->topoModel_->GetTopoInstsByLayer(simComm->GetRankId(), netLayer, topoInsts, topoInstNum);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetRanksByTopoInst(
    HcclComm comm, uint32_t netLayer, uint32_t topoInstId, uint32_t** ranks, uint32_t* rankNum)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    simComm->topoModel_->GetRanksByTopoInst(simComm->GetRankId(), netLayer, topoInstId, ranks, rankNum);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetTopoType(HcclComm comm, uint32_t netLayer, uint32_t topoInstId, CommTopo* topoType)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    simComm->topoModel_->GetTopoType(simComm->GetRankId(), netLayer, topoInstId, topoType);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetEndpointInfo(
    HcclComm comm, uint32_t rankId, const EndpointDesc* endpointDesc, EndpointAttr endpointAttr, uint32_t infoLen,
    void* info)
{
    uint32_t* intInfo = static_cast<uint32_t*>(info);
    *intInfo = 0;
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetEndpointNum(HcclComm comm, uint32_t layer, uint32_t topoInstId, uint32_t* num)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    simComm->topoModel_->GetEndpointNum(simComm->GetRankId(), layer, topoInstId, num);
    return HCCL_SUCCESS;
}

HcclResult HcclRankGraphGetEndpointDesc(
    HcclComm comm, uint32_t layer, uint32_t topoInstId, uint32_t* descNum, EndpointDesc* endpointDesc)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    simComm->topoModel_->GetEndpointDesc(simComm->GetRankId(), layer, topoInstId, descNum, endpointDesc);
    return HCCL_SUCCESS;
}

HcclResult HcclGetCommName(HcclComm commHandle, char* commName)
{
    CHK_PTR_NULL(commName);
    auto simComm = static_cast<HcclSim::SimCommunicator*>(commHandle);
    CHK_PTR_NULL(simComm);
    s32 ret = strncpy_s(
        commName, ROOTINFO_INDENTIFIER_MAX_LENGTH, simComm->GetIdentifier().c_str(), simComm->GetIdentifier().size());
    if (ret != EOK) {
        HCCL_ERROR("[%s] str copy fail. return %d", __func__, ret);
        return HCCL_E_INTERNAL;
    }
    return HCCL_SUCCESS;
}

HcclResult HcclGetRankSize(HcclComm comm, uint32_t* rankSize)
{
    CHK_PTR_NULL(rankSize);
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    *rankSize = simComm->GetRankSize();
    HCCL_INFO("[%s] rankSize: %u", __func__, *rankSize);
    return HCCL_SUCCESS;
}

HcclResult HcclGetRankId(HcclComm comm, uint32_t* rank)
{
    CHK_PTR_NULL(rank);
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    *rank = simComm->GetRankId();
    HCCL_INFO("[%s] rankId: %u", __func__, *rank);
    return HCCL_SUCCESS;
}

// Inner后缀算子尚未实现走原始流程
HcclResult HcclScatterInner(
    void* sendBuf, void* recvBuf, uint64_t recvCount, HcclDataType dataType, uint32_t root, HcclComm comm,
    aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclAllReduceInner(
    void* sendBuf, void* recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, HcclComm comm,
    aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclBroadcastInner(
    void* buf, uint64_t count, HcclDataType dataType, uint32_t root, HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclReduceScatterInner(
    void* sendBuf, void* recvBuf, uint64_t recvCount, HcclDataType dataType, HcclReduceOp op, HcclComm comm,
    aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclReduceScatterVInner(
    void* sendBuf, const void* sendCounts, const void* sendDispls, void* recvBuf, uint64_t recvCount,
    HcclDataType dataType, HcclReduceOp op, HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclAllGatherInner(
    void* sendBuf, void* recvBuf, uint64_t sendCount, HcclDataType dataType, HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclAllGatherVInner(
    void* sendBuf, uint64_t sendCount, void* recvBuf, const void* recvCounts, const void* recvDispls,
    HcclDataType dataType, HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclSendInner(
    void* sendBuf, uint64_t count, HcclDataType dataType, uint32_t destRank, HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclRecvInner(
    void* recvBuf, uint64_t count, HcclDataType dataType, uint32_t srcRank, HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclAlltoAllVCInner(
    const void* sendBuf, const void* sendCountMatrix, HcclDataType sendType, const void* recvBuf, HcclDataType recvType,
    HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclAlltoAllVInner(
    const void* sendBuf, const void* sendCounts, const void* sdispls, HcclDataType sendType, const void* recvBuf,
    const void* recvCounts, const void* rdispls, HcclDataType recvType, HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclAlltoAllInner(
    const void* sendBuf, uint64_t sendCount, HcclDataType sendType, const void* recvBuf, uint64_t recvCount,
    HcclDataType recvType, HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclReduceInner(
    void* sendBuf, void* recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, uint32_t root, HcclComm comm,
    aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclBatchSendRecvInner(HcclSendRecvItem* sendRecvInfo, uint32_t itemNum, HcclComm comm, aclrtStream stream)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclCreateOpResCtxInner(
    HcclComm comm, uint8_t opType, HcclDataType srcDataType, HcclDataType dstDataType, HcclReduceOp reduceType,
    uint64_t count, char* algConfig, uint32_t commEngine, void** opResCtx)
{
    HCCL_ERROR("[%s] not support", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult HcclGetHeterogMode(HcclComm comm, HcclHeterogMode* mode)
{
    *mode = HcclHeterogMode::HCCL_HETEROG_MODE_HOMOGENEOUS;
    return HCCL_SUCCESS;
}

HcclResult HcclGetHcclBuffer(HcclComm comm, void** buffer, uint64_t* size)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    return simComm->GetHcclBuffer(buffer, size);
}

HcclResult HcclChannelAcquire(
    HcclComm comm, CommEngine engine, const HcclChannelDesc* channelDescList, uint32_t listNum,
    ChannelHandle* channelList)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    std::string channelTag = "channelTag";
    return simComm->ChannelCommCreate(
        simComm->GetIdentifier(), channelTag.c_str(), engine, channelDescList, listNum, channelList);
}

HcclResult HcclEngineCtxCreate(HcclComm comm, const char* ctxTag, CommEngine engine, uint64_t size, void** ctx)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    return simComm->contextManager_->CreateCommEngineCtx(std::string(ctxTag), engine, size, ctx);
}

HcclResult HcclChannelGetHcclBuffer(HcclComm comm, ChannelHandle channel, void** buffer, uint64_t* size)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    return simComm->ChannelCommGetHcclBuffer(channel, buffer, size);
}

HcclResult HcclThreadAcquire(
    HcclComm comm, CommEngine engine, uint32_t threadNum, uint32_t notifyNumPerThread, ThreadHandle* threads)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    return simComm->independentOpThreadMgr_->HcclThreadAcquire(engine, threadNum, notifyNumPerThread, threads);
}

HcclResult HcclThreadAcquireWithStream(
    HcclComm comm, CommEngine engine, aclrtStream stream, uint32_t notifyNum, ThreadHandle* thread)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    return simComm->independentOpThreadMgr_->HcclThreadAcquireWithStream(engine, stream, notifyNum, thread);
}

HcclResult HcclEngineCtxGet(HcclComm comm, const char* engineTag, CommEngine engine, void** ctx, uint64_t* size)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    return simComm->contextManager_->GetCommEngineCtx(std::string(engineTag), engine, ctx, size);
}

HcclResult HcclEngineCtxCopy(
    HcclComm comm, CommEngine engine, const char* ctxTag, const void* srcCtx, uint64_t size, uint64_t dstCtxOffset)
{
    // HOST场景下srcCtx就是创建的EngineCtx内存地址，无需拷贝
    if (engine == CommEngine::COMM_ENGINE_AICPU_TS || engine == CommEngine::COMM_ENGINE_AICPU) {
        uint64_t size = 0;
        void* ctx = nullptr;
        auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
        CHK_PTR_NULL(simComm);
        simComm->contextManager_->GetCommEngineCtx(std::string(ctxTag), engine, &ctx, &size);
        if (ctx != nullptr && size > 0) {
            memcpy(ctx, srcCtx, size);
        }
    }
    return HCCL_SUCCESS;
}

HcclResult HcclCommInitClusterInfo(const char* clusterInfo, uint32_t rank, HcclComm* comm)
{
    auto topoMeta = HcclSim::SimWorld::Global()->GetTopoMetaInfo();
    return HcclSim::Sim_HcclCommInitClusterInfo(topoMeta, rank, comm);
}

HcclResult HcclCommDestroy(HcclComm comm)
{
    auto* simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    delete simComm;
    return HCCL_SUCCESS;
}

int32_t HcommThreadNotifyWaitOnThread(ThreadHandle thread, uint32_t notifyIdx, uint32_t timeout)
{
    // timeout 暂时未使用
    static_cast<void>(timeout);

    // 1.获取当前rankId,NpuPos和stream
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);

    // 2.从thread获得notifyId
    uint32_t notifyId = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetNotifyIdByIndex(notifyIdx);

    // 3.下发task
    auto task = std::make_shared<HcclSim::TaskStubLocalWaitFrom>(notifyId);
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommThreadNotifyRecordOnThread(ThreadHandle thread, ThreadHandle dstThread, uint32_t dstNotifyIdx)
{
    // 1.获取当前rankId,NpuPos和stream
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);

    // 2.从thread获得notifyId
    uint32_t notifyId = reinterpret_cast<HcclSim::SimHcclThread*>(dstThread)->GetNotifyIdByIndex(dstNotifyIdx);

    // 3.下发task
    auto task = std::make_shared<HcclSim::TaskStubLocalPostTo>(notifyId);
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommLocalCopyOnThread(ThreadHandle thread, void* dst, const void* src, uint64_t len)
{
    CHK_PTR_NULL(dst);
    CHK_PTR_NULL(src);

    // 1.获取当前rankId和stream
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);

    // 2.从模型用rankid查询NpuPos，从NpuPos获得SimNpu
    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curRank);

    // 3.查询DataSlice
    // 出参准备
    HcclSim::DataSlice srcSlice;
    HcclSim::DataSlice dstSlice;

    CHK_RET(npu.GetSlice(reinterpret_cast<uint64_t>(src), len, srcSlice));
    CHK_RET(npu.GetSlice(reinterpret_cast<uint64_t>(dst), len, dstSlice));

    auto task = std::make_shared<HcclSim::TaskStubLocalCopy>(srcSlice, dstSlice);
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommWriteOnThread(ThreadHandle thread, ChannelHandle channel, void* dst, const void* src, uint64_t len)
{
    CHK_PTR_NULL(dst);
    CHK_PTR_NULL(src);

    // 1.获取当前rankId,NpuPos和stream
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);

    // 2.获取远端和本地rankId
    uint32_t locRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLocRankId();
    uint32_t rmtRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetRmtRankId();

    // 3.从模型用rankid查询NpuPos，从NpuPos获得SimNpu
    // src地址rank
    HcclSim::SimNpu& srcNpu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(locRank);
    // dst地址rank
    HcclSim::SimNpu& dstNpu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(rmtRank);

    // 4.查询DataSlice
    // 出参准备
    HcclSim::DataSlice srcSlice;
    HcclSim::DataSlice dstSlice;

    CHK_RET(srcNpu.GetSlice(reinterpret_cast<uint64_t>(src), len, srcSlice));
    CHK_RET(dstNpu.GetSlice(reinterpret_cast<uint64_t>(dst), len, dstSlice));

    // 5.通过抽象链接类型判断链接协议
    HcclSim::LinkInfo link(reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLinkType());

    // 6.下发task
    auto task = std::make_shared<HcclSim::TaskStubWrite>(rmtRank, link, srcSlice, dstSlice);
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommReadOnThread(ThreadHandle thread, ChannelHandle channel, void* dst, const void* src, uint64_t len)
{
    CHK_PTR_NULL(dst);
    CHK_PTR_NULL(src);

    // 1.获取当前rankId,NpuPos和stream
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);

    // 2.获取远端和本地rankId
    uint32_t locRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLocRankId();
    uint32_t rmtRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetRmtRankId();

    // 3.从模型用rankid查询NpuPos，从NpuPos获得SimNpu
    // src地址rank
    HcclSim::SimNpu& srcNpu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(rmtRank);
    // dst地址rank
    HcclSim::SimNpu& dstNpu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(locRank);

    // 4.查询DataSlice
    // 出参准备
    HcclSim::DataSlice srcSlice;
    HcclSim::DataSlice dstSlice;

    CHK_RET(srcNpu.GetSlice(reinterpret_cast<uint64_t>(src), len, srcSlice));
    CHK_RET(dstNpu.GetSlice(reinterpret_cast<uint64_t>(dst), len, dstSlice));

    // 5.通过抽象链接类型判断链接协议
    HcclSim::LinkInfo link(reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLinkType());

    // 6.下发task
    auto task = std::make_shared<HcclSim::TaskStubRead>(rmtRank, link, dstSlice, srcSlice);
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommChannelNotifyRecordOnThread(ThreadHandle thread, ChannelHandle channel, uint32_t remoteNotifyIdx)
{
    // 1.获取当前rankId,NpuPos和stream
    thread = (thread == 0) ? curThread : thread; // DPU模式使用AICPU的thread
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();

    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curRank);

    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);

    // 2.获取远端和本地rankId
    uint32_t rmtRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetRmtRankId();

    // 3.通过抽象链接类型判断链接协议
    HcclSim::LinkInfo link(reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLinkType());

    // 4.通过channel获得remoteNotify id
    uint32_t rmtNotifyId = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetRmtNotifyIdByIndex(remoteNotifyIdx);

    // 5.下发task
    auto task = std::make_shared<TaskStubPost>(rmtRank, link, rmtNotifyId, NotifyTypeStub::READY, "POST");
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommChannelNotifyWaitOnThread(
    ThreadHandle thread, ChannelHandle channel, uint32_t localNotifyIdx, uint32_t timeout)
{
    // timeout 不参与 taskstubwait的构造
    static_cast<void>(timeout);

    // 1.获取当前rankId,NpuPos和stream
    thread = (thread == 0) ? curThread : thread;
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();

    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curRank);

    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);

    // 2.获取远端和本地rankId
    uint32_t rmtRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetRmtRankId();

    // 3.通过抽象链接类型判断链接协议
    HcclSim::LinkInfo link(reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLinkType());

    // 4.通过channel获得remoteNotify id
    uint32_t localNotifyId = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLocNotifyIdByIndex(localNotifyIdx);

    // 5.下发task
    auto task = std::make_shared<TaskStubWait>(rmtRank, link, localNotifyId, NotifyTypeStub::READY, "WAIT");
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommLocalReduceOnThread(
    ThreadHandle thread, void* dst, const void* src, uint64_t count, HcommDataType dataType, HcommReduceOp reduceOp)
{
    CHK_PTR_NULL(dst);
    CHK_PTR_NULL(src);

    // 1.获取当前rankId和stream
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);

    // 2.从模型用rankid查询NpuPos，从NpuPos获得SimNpu
    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimNpu& npu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(curRank);

    // 3.查询DataSlice
    // 出参准备
    HcclSim::DataSlice srcSlice;
    HcclSim::DataSlice dstSlice;

    CHK_RET(npu.GetSlice(reinterpret_cast<uint64_t>(src), count, static_cast<HcclDataType>(dataType), srcSlice));
    CHK_RET(npu.GetSlice(reinterpret_cast<uint64_t>(dst), count, static_cast<HcclDataType>(dataType), dstSlice));

    // 4.下发task
    auto task = std::make_shared<TaskStubLocalReduce>(
        srcSlice, dstSlice, static_cast<HcclDataType>(dataType), static_cast<HcclReduceOp>(reduceOp));
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommWriteReduceOnThread(
    ThreadHandle thread, ChannelHandle channel, void* dst, const void* src, uint64_t count, HcommDataType dataType,
    HcommReduceOp reduceOp)
{
    CHK_PTR_NULL(dst);
    CHK_PTR_NULL(src);

    // 1.获取当前rankId,NpuPos和stream
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);

    // 2.获取远端和本地rankId
    uint32_t locRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLocRankId();
    uint32_t rmtRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetRmtRankId();

    // 3.从模型用rankid查询NpuPos，从NpuPos获得SimNpu
    // src地址rank
    HcclSim::SimNpu& srcNpu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(locRank);
    // dst地址rank
    HcclSim::SimNpu& dstNpu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(rmtRank);

    // 4.查询DataSlice
    // 出参准备
    HcclSim::DataSlice srcSlice;
    HcclSim::DataSlice dstSlice;

    CHK_RET(srcNpu.GetSlice(reinterpret_cast<uint64_t>(src), count, static_cast<HcclDataType>(dataType), srcSlice));
    CHK_RET(dstNpu.GetSlice(reinterpret_cast<uint64_t>(dst), count, static_cast<HcclDataType>(dataType), dstSlice));

    // 5.通过抽象链接类型判断链接协议
    HcclSim::LinkInfo link(reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLinkType());

    // 6.下发task
    auto task = std::make_shared<HcclSim::TaskStubWriteReduce>(
        rmtRank, link, srcSlice, dstSlice, static_cast<HcclDataType>(dataType), static_cast<HcclReduceOp>(reduceOp));
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommReadReduceOnThread(
    ThreadHandle thread, ChannelHandle channel, void* dst, const void* src, uint64_t count, HcommDataType dataType,
    HcommReduceOp reduceOp)
{
    CHK_PTR_NULL(dst);
    CHK_PTR_NULL(src);

    // 1.获取当前rankId,NpuPos和stream
    uint32_t curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    NpuPos pos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();

    // 2.获取远端和本地rankId
    uint32_t locRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLocRankId();
    uint32_t rmtRank = reinterpret_cast<HcclSim::SimChannel*>(channel)->GetRmtRankId();

    // 3.从模型用rankid查询NpuPos，从NpuPos获得SimNpu
    // src地址rank
    HcclSim::SimNpu& srcNpu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(rmtRank);
    // dst地址rank
    HcclSim::SimNpu& dstNpu = HcclSim::SimWorld::Global()->GetSimNpuByRankId(locRank);

    // 4.查询DataSlice
    // 出参准备
    HcclSim::DataSlice srcSlice;
    HcclSim::DataSlice dstSlice;

    CHK_RET(srcNpu.GetSlice(reinterpret_cast<uint64_t>(src), count, static_cast<HcclDataType>(dataType), srcSlice));
    CHK_RET(dstNpu.GetSlice(reinterpret_cast<uint64_t>(dst), count, static_cast<HcclDataType>(dataType), dstSlice));

    // 5.通过抽象链接类型判断链接协议
    HcclSim::LinkInfo link(reinterpret_cast<HcclSim::SimChannel*>(channel)->GetLinkType());

    // 6.下发task
    auto task = std::make_shared<HcclSim::TaskStubWriteReduce>(
        rmtRank, link, dstSlice, srcSlice, static_cast<HcclDataType>(dataType), static_cast<HcclReduceOp>(reduceOp));
    HcclSim::SimTaskQueue::Global()->AppendTask(pos, stream, task);

    return HCCL_SUCCESS;
}

int32_t HcommAclrtNotifyRecordOnThread(ThreadHandle thread, uint64_t dstNotifyId)
{
    auto curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    auto npuPos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);
    auto task = std::make_shared<HcclSim::TaskStubLocalPostTo>(dstNotifyId);
    HcclSim::SimTaskQueue::Global()->AppendTask(npuPos, stream, task);
    return HCCL_SUCCESS;
}

int32_t HcommAclrtNotifyWaitOnThread(ThreadHandle thread, uint64_t notifyId, uint32_t timeOut)
{
    auto curRank = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetCurRank();
    auto npuPos = HcclSim::SimWorld::Global()->GetNpuPosByRankId(curRank);
    HcclSim::SimStream* stream = reinterpret_cast<HcclSim::SimHcclThread*>(thread)->GetStream();
    CHK_PTR_NULL(stream);
    auto task = std::make_shared<HcclSim::TaskStubLocalWaitFrom>(notifyId);
    HcclSim::SimTaskQueue::Global()->AppendTask(npuPos, stream, task);
    return HCCL_SUCCESS;
}

HcclResult CommTaskPrepare(char* key, uint32_t keyLen)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult CommTaskLaunch(ThreadHandle* threads, uint32_t threadNum)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult CommWriteReduceWithNotify(
    ThreadHandle thread, ChannelHandle channel, void* dst, const void* src, uint64_t count, HcclDataType dataType,
    HcclReduceOp reduceOp, uint32_t remoteNotifyIdx)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return HCCL_E_NOT_SUPPORT;
}

bool HcommIsSupportHcommBatchTransferOnThread() { return false; }

int32_t HcommWriteWithNotifyOnThread(
    ThreadHandle thread, ChannelHandle channel, void* dst, const void* src, uint64_t len, uint32_t remoteNotifyIdx)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return HCCL_E_NOT_SUPPORT;
}

HcclResult CommFence(ThreadHandle thread, ChannelHandle channel)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return HCCL_E_NOT_SUPPORT;
}

int32_t HcommBatchModeStart(const char* batchTag)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return HCCL_SUCCESS;
}

int32_t HcommBatchModeEnd(const char* batchTag)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return HCCL_SUCCESS;
}

int32_t HcommAcquireComm(const char* commId) { return 0; }

int32_t HcommReleaseComm(const char* commId) { return 0; }

// stub for host dpu
int32_t HcommWriteWithNotifyNbiOnThread(
    ThreadHandle thread, ChannelHandle channel, void* dst, const void* src, uint64_t len, uint32_t remoteNotifyIdx)
{
    HcommWriteOnThread(curThread, channel, dst, src, len);
    HcommChannelNotifyRecordOnThread(curThread, channel, remoteNotifyIdx);
    return 0;
}

HcclResult HcclDevMemAcquire(HcclComm comm, const char* memTag, uint64_t* size, void** addr, bool* newCreated)
{
    // 不真实分配host和dpu间通信的共享内存
    HCCL_WARNING("[%s] not support.", __func__);
    return HCCL_SUCCESS;
}

int32_t HcommFenceOnThread(ThreadHandle thread)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return 0;
}

int32_t HcommThreadSynchronize(ThreadHandle thread)
{
    // 这里记录当前thread，因为dpu侧拿不到thread了
    curThread = thread;
    return 0;
}

typedef std::function<int32_t(uint64_t, int32_t)> CallbackStub;
std::map<std::string, CallbackStub> dpuCallbackMap;
int32_t HcclTaskRegister(HcclComm comm, const char* msgTag, Callback cb)
{
    std::string msgTagStr(msgTag);
    std::lock_guard<std::mutex> lock(g_mutex);
    dpuCallbackMap[msgTagStr] = std::function<int32_t(uint64_t, int32_t)>(cb);
    return 0;
}

int32_t HcommSendRequest(uint64_t handle, const char* msgTag, const void* src, size_t sizeByte, uint32_t* msgId)
{
    std::string msgTagStr(msgTag);
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = dpuCallbackMap.find(msgTagStr);
    if (it != dpuCallbackMap.end()) {
        it->second(reinterpret_cast<uint64_t>(src), sizeByte);
    } else {
        HCCL_ERROR("[HcommSendRequest] cannot find callback for %s", msgTag);
    }
    return 0;
}

int32_t HcommChannelFenceOnThread(ThreadHandle thread, ChannelHandle channel)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return 0;
}

int32_t HcommWaitResponse(uint64_t handle, void* dst, size_t sizeByte, uint32_t* msgId)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return 0;
}

HcclResult HcclChannelGetRemoteMems(
    HcclComm comm, ChannelHandle channel, uint32_t* memNum, CommMem** remoteMems, char*** memTags)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return HCCL_SUCCESS;
}

HcclResult HcclCommMemReg(HcclComm comm, const char* memTag, const CommMem* mem, HcclMemHandle* memHandle)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return HCCL_SUCCESS;
}

HcclResult HcclThreadExportToCommEngine(
    HcclComm comm, uint32_t threadNum, const ThreadHandle* threads, CommEngine dstCommEngine,
    ThreadHandle* exportedThreads)
{
    *exportedThreads = *const_cast<const ThreadHandle*>(threads);
    return HCCL_SUCCESS;
}

HcclResult HcclConfigGetInfo(HcclComm comm, HcclConfigType cfgType, uint32_t infoLen, void* info)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return HCCL_SUCCESS;
}

HcclResult HcomGetCommHandleByGroup(const char* group, HcclComm* commHandle)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return HCCL_SUCCESS;
}

HcclResult HcclEngineCtxDestroy(HcclComm comm, const char* ctxTag, CommEngine engine)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return HCCL_SUCCESS;
}

HcclResult HcommThreadJoin(ThreadHandle thread, uint32_t timeout)
{
    HCCL_WARNING("[%s] not support.", __func__);
    return HCCL_SUCCESS;
}

int32_t HcommWriteReduceWithNotifyOnThread(
    ThreadHandle thread, ChannelHandle channel, void* dst, const void* src, uint64_t count, HcommDataType dataType,
    HcommReduceOp reduceOp, uint32_t remoteNotifyIdx)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return -1;
}

int32_t HcommWriteNbi(ChannelHandle channel, void* dst, const void* src, uint64_t len)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return -1;
}

int32_t HcommWriteWithNotifyNbi(
    ChannelHandle channel, void* dst, const void* src, uint64_t len, uint32_t remoteNotifyIdx)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return -1;
}

int32_t HcommReadNbi(ChannelHandle channel, void* dst, const void* src, uint64_t len)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return -1;
}

int32_t HcommChannelNotifyRecord(ChannelHandle channel, uint32_t remoteNotifyIdx)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return -1;
}

int32_t HcommChannelNotifyWait(ChannelHandle channel, uint32_t localNotifyIdx, uint32_t timeout)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return -1;
}

int32_t HcommFlush()
{
    HCCL_ERROR("[%s] not support.", __func__);
    return -1;
}

int32_t HcommChannelFence(ChannelHandle channel)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return -1;
}

HcclResult HcommSymWinGetPeerPointer(HcclCommSymWindow winHandle, size_t offset, uint32_t peerRank, void** ptr)
{
    HCCL_ERROR("[%s] not support.", __func__);
    return HCCL_E_NOT_SUPPORT;
}

#ifdef __cplusplus
}
#endif // __cplusplus
