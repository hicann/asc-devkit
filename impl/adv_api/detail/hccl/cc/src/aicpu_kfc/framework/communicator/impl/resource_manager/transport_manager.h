/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TRANSPORT_MANAGER_H
#define TRANSPORT_MANAGER_H

#include <mutex>
#include <unordered_map>
#include <atomic>
#include <fstream>
#include "hccl/base.h"
#include "hccl_socket_manager.h"
#include "dispatcher.h"
#include "mem_device_pub.h"
#include "transport_pub.h"
#include "ccl_buffer_manager.h"
#include "externalinput_pub.h"
#include "sal_pub.h"
#include "thread/threads_guard.h"
#include "hccl_hash_utils.h"
#include "workflow_pub.h"
#include "comm_base_pub.h"
#include "coll_alg_param.h"
#include "multi_qpInfo_manager.h"
namespace hccl {

constexpr u32 AICPU_RETRY_BACKUP_PORT = 16667;     // aicpu重执行备份默认端口
constexpr u32 MASSIVE_IBV_CONNECTION_COUNT = 1000; // bsr大于这个链路数量就切换链路类型
constexpr u32 SEND_QP_DEPTH_FOR_BSR = 512;         // 使用Transport NpuDirect链路的时候设置send深度为512
constexpr u32 RECV_QP_DEPTH_FOR_BSR = 128;         // // 使用Transport NpuDirect链路的时候设置recv深度为128
constexpr u32 MAX_THREAD_NUM = 8;                  // BatchSendRecv建链时单个线程池的最大线程数量

struct TransportData {
    LinkMode linkMode{LinkMode::LINK_RESERVED_MODE};
    std::vector<HcclIpAddress> remoteIpAddr;
    u32 remoteUserrank{INVALID_VALUE_RANKID};
    u32 remoteWorldRank{INVALID_VALUE_RANKID};
    s32 remoteDeviceId{-1};
    DevType deviceType{DevType::DEV_TYPE_COUNT};
    DeviceMem inputMem{DeviceMem()};
    DeviceMem outputMem{DeviceMem()};
    bool supportDataReceivedAck{false};
    u32 remoteSocketPort;

    TransportData(
        LinkMode linkMode, const std::vector<HcclIpAddress>& remoteIpAddr, u32 remoteUserrank, u32 remoteWorldRank,
        s32 remoteDeviceId, DevType deviceType, const DeviceMem& inputMem, const DeviceMem& outputMem,
        bool supportDataReceivedAck, u32 remoteSocketPort)
        : linkMode(linkMode),
          remoteIpAddr(remoteIpAddr),
          remoteUserrank(remoteUserrank),
          remoteWorldRank(remoteWorldRank),
          remoteDeviceId(remoteDeviceId),
          deviceType(deviceType),
          inputMem(inputMem),
          outputMem(outputMem),
          supportDataReceivedAck(supportDataReceivedAck),
          remoteSocketPort(remoteSocketPort) {};

    bool operator==(const TransportData& that) const
    {
        return (linkMode == that.linkMode) && (remoteIpAddr == that.remoteIpAddr) &&
               (remoteUserrank == that.remoteUserrank) && (remoteWorldRank == that.remoteWorldRank) &&
               (remoteDeviceId == that.remoteDeviceId) && (deviceType == that.deviceType) &&
               (inputMem == that.inputMem) && (outputMem == that.outputMem) &&
               (supportDataReceivedAck == that.supportDataReceivedAck) && (remoteSocketPort == that.remoteSocketPort);
    }
};

struct SubCommLinkPara {
    struct SingleSubCommTransport& singleSubCommTransport;
    std::vector<std::pair<u32, u32>> remoteRankMap;
    u32 remoteRankIdStartIndex;
    u32 remoteRankIdNum;
    std::vector<std::unique_ptr<std::thread>> linkThreads;
    std::vector<HcclResult> linkResult; // TransportManager::CreateLink返回值出参

    SubCommLinkPara(
        struct SingleSubCommTransport& singleSubCommTransport, std::vector<std::pair<u32, u32>>& remoteRankMap,
        u32 remoteRankIdStartIndex, u32 remoteRankIdNum)
        : singleSubCommTransport(singleSubCommTransport),
          remoteRankMap(remoteRankMap),
          remoteRankIdStartIndex(remoteRankIdStartIndex),
          remoteRankIdNum(remoteRankIdNum)
    {}

    ~SubCommLinkPara()
    {
        for (auto& linkThread : linkThreads) {
            if (linkThread != nullptr && linkThread->joinable()) {
                linkThread->join();
            }
        }
    }
};

struct LinkPoolPara {
    struct SingleSubCommTransport& singleSubCommTransport;
    std::string poolName;
    // 记录pair<remoteRank, idx>, idx表示remoteRank对应的建链信息在transportRequests中的索引位置
    std::vector<std::pair<u32, u32>> taskList;

    std::atomic<u32> taskIndex{0};
    std::atomic<bool> abortFlag{false};

    std::vector<std::unique_ptr<std::thread>> linkThreads;
    std::vector<HcclResult> linkResults;

    LinkPoolPara(
        struct SingleSubCommTransport& transport, const std::string& name,
        const std::vector<std::pair<u32, u32>>& tasks)
        : singleSubCommTransport(transport), poolName(name), taskList(tasks)
    {
        u32 threadNum = std::min(MAX_THREAD_NUM, static_cast<u32>(taskList.size()));
        linkThreads.resize(threadNum);
        linkResults.resize(taskList.size(), HCCL_SUCCESS);
    }

    ~LinkPoolPara()
    {
        for (auto& linkThread : linkThreads) {
            if (linkThread != nullptr && linkThread->joinable()) {
                linkThread->join();
            }
        }
    }
};
} // namespace hccl

namespace std {

template <>
class hash<hccl::TransportData> {
public:
    size_t operator()(const hccl::TransportData& transportData) const
    {
        auto linkMode = hash<s32>{}(static_cast<s32>(transportData.linkMode));
        auto remoteIpAddrFamily = hash<s32>{}(transportData.remoteIpAddr[0].GetFamily());
        auto remoteIpAddr = hash<string>{}(string(transportData.remoteIpAddr[0].GetReadableAddress()));
        auto remoteUserrank = hash<u32>{}(transportData.remoteUserrank);
        auto remoteWorldRank = hash<u32>{}(transportData.remoteWorldRank);
        auto remoteDeviceId = hash<s32>{}(transportData.remoteDeviceId);
        auto deviceType = hash<s32>{}(static_cast<s32>(transportData.deviceType));
        auto inputMemPtr = hash<u64>{}(reinterpret_cast<u64>(transportData.inputMem.ptr()));
        auto inputMemSize = hash<u64>{}(transportData.inputMem.size());
        auto outputMemPtr = hash<u64>{}(reinterpret_cast<u64>(transportData.outputMem.ptr()));
        auto outputMemSize = hash<u64>{}(transportData.outputMem.size());
        auto supportDataReceivedAck = hash<bool>{}(transportData.supportDataReceivedAck);
        auto remoteSocketPort = hash<u32>{}(transportData.remoteSocketPort);

        return hccl::HashCombine(
            {linkMode, remoteIpAddrFamily, remoteIpAddr, remoteUserrank, remoteWorldRank, remoteDeviceId, deviceType,
             inputMemPtr, inputMemSize, outputMemPtr, outputMemSize, supportDataReceivedAck, remoteSocketPort});
    }
};
} // namespace std

namespace hccl {
// 独立算子内存
struct IndOpMem {
    std::vector<HostMem> userHostMem;
    std::vector<DeviceMem> userDeviceMem;
};

struct TransportIOMem {
    DeviceMem cclInputMem;
    DeviceMem cclOutputMem;
    DeviceMem paramInputMem;
    DeviceMem paramOutputMem;
    DeviceMem scratchMem;
    DeviceMem aivInputMem;
    DeviceMem aivOutputMem;
    DeviceMem expMem;
    DeviceMem userMem;
    IndOpMem indOpMem;
};

class TransportManager {
public:
    TransportManager(
        CCLBufferManager& cclBufferManager, const std::unique_ptr<HcclSocketManager>& socketManager,
        HcclDispatcher dispatcher, const std::unique_ptr<NotifyPool>& notifyPool,
        const std::vector<RankInfo>& rankInfoList, RankId userRank, const std::string& identifier, s32 deviceLogicId,
        NICDeployment nicDeployment, bool isHaveCpuRank, const void* transportResourceInfoAddr,
        size_t transportResourceInfoSize, bool isUseRankPort, bool isUsedRdmaLevel0,
        const std::vector<u32>& nicRanksPort, const std::vector<u32>& vnicRanksPort, bool useSuperPodMode,
        const std::vector<HcclIpAddress>& devIpAddr, const HcclIpAddress& hostIp, const HcclIpAddress& localVnicIp,
        std::map<HcclIpAddress, HcclNetDevCtx>& netDevCtxMap);

    ~TransportManager();

    HcclResult CreateVirturalTransport(SingleSubCommTransport& singleSubCommTransport);
    HcclResult Alloc(
        const std::string& tag, const TransportIOMem& transMem, OpCommTransport& opTransportResponse,
        bool isAicpuModeEn, bool isBackup = false, bool isZeroCopy = false,
        const HcclCMDType& opType = HcclCMDType::HCCL_CMD_INVALID, bool isCapture = false, bool isIndOp = false,
        bool isNpuDirectRoce = false, const OpParam* opParam = nullptr);
    HcclResult IncreAlloc(
        const std::string& tag, const TransportIOMem& transMem, OpCommTransport& opTransportReq,
        OpCommTransport& opTransportResponse, bool isAicpuModeEn, bool isBackup = false, bool isCapture = false,
        const HcclCMDType& opType = HcclCMDType::HCCL_CMD_INVALID);
    HcclResult GetRemoteRankList(
        OpCommTransport& opTransportResponse, std::vector<u32>& rankList, TransportType transportType);
    HcclResult GetIncreRemoteRankList(
        OpCommTransport& opTransportReq, OpCommTransport& opTransportResponse, std::vector<u32>& rankList,
        TransportType transportType);
    HcclResult AddremoteUserRankToList(
        TransportRequest& transportRequest, std::vector<u32>& rankList, TransportType transportType);
    TransportManager(TransportManager const&) = delete;            // Copy construct
    TransportManager(TransportManager&&) = delete;                 // Move construct
    TransportManager& operator=(TransportManager const&) = delete; // Copy assign
    TransportManager& operator=(TransportManager&&) = delete;      // Move assign
    void SetQpQosAttr(u32 trafficClass, u32 serviceLevel);         // 设置TC/SL配置

    HcclResult SetStopFlag(bool value);
    bool GetStopFlag();
    void SetIsStandardCard(bool isStandardCard);

    void SetPortConfig(bool devPortSwitchOn);
    HcclResult CheckLinkNumAndSwitchLinkType(
        TransportType& type, MachinePara& machinePara, const std::vector<std::shared_ptr<HcclSocket>> sockets);
    void SetOpType(HcclCMDType opType);
    HcclResult SetGroupMode(bool groupMode);
    std::map<u32, TransportType> GetRemoteTransportMap();

private:
    HcclResult GetIOMem(
        const TransportIOMem& transMem, const TransportMemType inputMemType, const TransportMemType outputMemType,
        DeviceMem& inputMem, DeviceMem& outputMem, DeviceMem& expMem);
    u32 GetHostPort(s32 devicePhyId);
    u32 GetRemoteNicPort(s32 devicePhyId, u32 dstUserRank, bool isInterRdma);
    bool IsSupportInterHccs(const u32 dstRank);
    void UpdateIsInterRdma(const u32 remoteRank, bool& isInterRdma, bool forceRdma);
    HcclResult MakeRemoteLinkInfo(
        const u32 remoteRank, bool isInterRdma, u32 socketsPerLink, HcclRankLinkInfo& remoteLinkInfo);
    HcclResult CreateDestSockets(
        const std::string& tag, RankId remoteRank, u64 taskNum,
        std::vector<std::shared_ptr<HcclSocket>>& connectSockets, HcclNetDevCtx& netDevCtx, bool& isInterRdma,
        bool forceRdma = false, bool isBackup = false, u32 subCommIndex = 0,
        TransportLinkType linkType = TransportLinkType::RESERVED);
    u32 GetSocketsPerLink(u64 taskNum, u32 remoteRankId = INVALID_VALUE_RANKID);
    HcclResult SetMachinePara(
        const std::string& tag, MachineType machineType, const std::string& serverId, u32 dstRank,
        const bool supportDataReceivedAck, const LinkMode linkMode,
        const std::vector<std::shared_ptr<HcclSocket>>& socketList, const DeviceMem& inputMem,
        const DeviceMem& outputMem, const DeviceMem& expMem, bool isAicpuModeEn, bool isBackup, bool isCapture,
        u32 notifyNum, u32 trafficClass, u32 serviceLevel, MachinePara& machinePara, RankInfo& localRank,
        RankInfo& remoteRank, const HcclNetDevCtx& netDevCtx, TransportLinkType linkType = TransportLinkType::RESERVED,
        const IndOpMem& indOpMem = IndOpMem(), bool isIndOp = false,
        const HcclCMDType& opType = HcclCMDType::HCCL_CMD_INVALID, bool isNpuDirectRoce = false);
    HcclResult GetTransportType(const u32 dstRank, bool isUsedRdma, TransportType& transportType);
    void SetTransportParam(TransportPara& para, MachinePara& machinePara);
    HcclResult TransportInit(
        const u32 dstRank, MachinePara& machinePara, std::shared_ptr<Transport>& link, bool useOneDoorbell,
        bool isUsedRdma, TransportType type);
    HcclResult AllocSliceMem(DeviceMem& inputMem, DeviceMem& outputMem, u32 remoteUserRank);
    HcclResult CreateLink(
        const std::string& tag, const ErrContextPub& error_context, const MachineType machineType,
        const std::string& serverId, const u32 remoteRank, const bool supportDataReceivedAck, const LinkMode linkMode,
        const bool enableUseOneDoorbell, const std::string threadStr,
        const std::vector<std::shared_ptr<HcclSocket>> sockets, const DeviceMem inputMem, const DeviceMem outputMem,
        bool isUsedRdma, std::shared_ptr<Transport>& link, bool isAicpuModeEn, HcclResult& retOut,
        const HcclNetDevCtx& netDevCtx, u32 notifyNum = 0, bool isBackup = false, bool isCapture = false,
        const DeviceMem expMem = DeviceMem(), TransportLinkType linkType = TransportLinkType::RESERVED,
        bool isIndOp = false, const IndOpMem indOpMem = IndOpMem(),
        const HcclCMDType& opType = HcclCMDType::HCCL_CMD_INVALID, bool isNpuDirectRoce = false);
    bool IsHccsTransport(u32 remoteRank, TransportLinkType linkType);
    HcclResult ConstructTransTag(
        const std::string& tag, std::string& transTag, bool isInterRdma, u32 subCommIndex = 0, bool isHccs = false);
    HcclResult ExceptionHandle(const std::string& tag, OpCommTransport& opTransportResponse);
    HcclResult createSubCommLinkThreads(
        const std::string& tag, const TransportIOMem& transMem, struct SubCommLinkPara& subCommLinkPara,
        bool isAicpuModeEn, bool isBackup, u32 subCommIndex, bool isCapture = false,
        const HcclCMDType& opType = HcclCMDType::HCCL_CMD_INVALID, bool isIndOp = false);
    HcclResult waitSubCommLinkThreadsComplete(struct SubCommLinkPara& subCommLinkPara);
    HcclResult checkSubCommLinkThreadsStatus(
        const std::string& tag, struct SubCommLinkPara& subCommLinkPara, bool isBackup);
    HcclResult AllocSubCommLinks(
        const std::string& tag, const TransportIOMem& transMem, struct SingleSubCommTransport& singleSubCommTransport,
        bool isAicpuModeEn, bool isBackup, u32 subCommIndex, bool isCapture = false,
        const HcclCMDType& opType = HcclCMDType::HCCL_CMD_INVALID, bool isIndOp = false);
    HcclResult IsInterServer(const u32 dstRank, bool& isInterServer);
    HcclResult PrintErrorInfo(NicType nicType);
    HcclResult CreateBatchSendRecvLinks(
        const std::string& tag, const TransportIOMem& transMem, struct LinkPoolPara& linkPoolPara, bool isAicpuModeEn,
        bool isBackup, u32 subCommIndex, bool isCapture = false,
        const HcclCMDType& opType = HcclCMDType::HCCL_CMD_INVALID, bool isIndOp = false);
    HcclResult WaitBatchSendRecvThreadsComplete(struct LinkPoolPara& linkPoolPara);
    HcclResult CheckBatchSendRecvLinkStatus(
        const std::string& tag, struct SingleSubCommTransport& singleSubCommTransport, bool isBackup);
    HcclResult AllocBatchSendRecvLinks(
        HcclSendRecvItem* sendRecvItemsPtr, u32 itemNum, const std::string& tag, const TransportIOMem& transMem,
        struct SingleSubCommTransport& singleSubCommTransport, bool isAicpuModeEn, bool isBackup, u32 subCommIndex,
        bool isCapture = false, const HcclCMDType& opType = HcclCMDType::HCCL_CMD_INVALID, bool isIndOp = false);
    HcclResult PrepareTaskLists(
        HcclSendRecvItem* sendRecvItemsPtr, u32 itemNum, const SingleSubCommTransport& singleSubCommTransport,
        std::vector<std::pair<u32, u32>>& senderList, std::vector<std::pair<u32, u32>>& receiverList);

    std::mutex mutex_; // 用于控制互斥资源的访问
    CCLBufferManager& cclBufferManager_;
    const std::unique_ptr<HcclSocketManager>& socketManager_;
    HcclDispatcher dispatcher_;
    const std::unique_ptr<NotifyPool>& notifyPool_;
    const std::vector<RankInfo>& rankInfoList_;
    RankId userRank_;
    std::string identifier_;
    s32 deviceLogicId_;
    NICDeployment nicDeployment_;
    bool isHaveCpuRank_{false};
    const void* transportResourceInfoAddr_;
    size_t transportResourceInfoSize_;
    bool isUseRankPort_{false};
    bool isUsedRdmaLevel0_{false};
    const std::vector<u32>& nicRanksPort_;
    const std::vector<u32>& vnicRanksPort_;
    bool useSuperPodMode_{false};
    const std::vector<HcclIpAddress>& devIpAddr_;
    const HcclIpAddress& hostIp_;
    const HcclIpAddress& localVnicIp_;
    std::map<HcclIpAddress, HcclNetDevCtx>& netDevCtxMap_;
    bool devPortSwitchOn_{false};
    std::map<u32, TransportType> remoteTransportMap_;

    std::unordered_map<TransportData, LINK> transportMap_;
    std::vector<u32> enableP2PDevices_;

    std::vector<std::string> socketTagVec_;
    std::vector<DeviceMem> extraMem_;

    bool isGroupMode_ = false;

    std::atomic<bool> stopFlag_{false};
    HcclWorkflowMode workflowMode_{HcclWorkflowMode::HCCL_WORKFLOW_MODE_OP_BASE};
    u64 rankConsistentDataLength_ = 0;
    u32 trafficClass_;
    u32 serviceLevel_;
    u32 ibvCount_ = 0;
    std::mutex ibvCountMutex_;
    HcclCMDType opType_ = HcclCMDType::HCCL_CMD_INVALID;
    bool isStandardCard_ = false;
    std::unique_ptr<MulQpInfo> mulQpinfo_ = {nullptr};
    std::mutex createSocketMutex_; // BatchSendRecv建链调用CreateDestSockets时，保护socketTagVec_等资源
};
} // namespace hccl

#endif /* TRANSPORT_MANAGER_H */
