/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_AICPU_COMMUNICATOR_IMPL_LITE_H_
#define HCCL_AICPU_COMMUNICATOR_IMPL_LITE_H_

#include <memory>
#include "ins_executor.h"
#include "data_type.h"
#include "prim_translator.h"

#include "lite_res_mgr_fetcher.h"
#include "coll_alg_component_lite.h"
#include "types.h"
#include "coll_alg_info.h"
#include "kernel_param_lite.h"
#include "rma_buffer_lite.h"
#include "queue_notify_lite_mgr.h"
#include "cnt_1ton_notify_lite_mgr.h"
#include "host_device_sync_notify_lite_mgr.h"
#include "profiling_reporter_lite.h"
#include "profiling_handler_lite.h"
#include "data_buffer.h"
#include "rmt_data_buffer_mgr.h"
#include "aicpu_hdc_handler.h"
#include "one_sided_component_lite.h"
#include "error_message_v2.h"

namespace Hccl {

class CommunicatorImplLite : public ResMgrFetcher {
public:
    explicit CommunicatorImplLite(u32 idIndex);
    ~CommunicatorImplLite() override = default;

    int LoadWithOpBasedMode(HcclKernelParamLite* kernelParam);
    int UpdateComm(HcclKernelParamLite* kernelParam);

    HostDeviceSyncNotifyLiteMgr* GetHostDeviceSyncNotifyLiteMgr() override;
    StreamLiteMgr* GetStreamLiteMgr() override;
    QueueNotifyLiteMgr* GetQueueNotifyLiteMgr() override;
    Cnt1tonNotifyLiteMgr* GetCnt1tonNotifyLiteMgr() override;
    CntNto1NotifyLiteMgr* GetCntNto1NotifyLiteMgr() override;
    ConnectedLinkMgr* GetConnectedLinkMgr() override;
    DevId GetDevPhyId() override;
    u32 GetExecTimeOut() override;

    MemTransportLiteMgr* GetTransportLiteMgr() override { return transportLiteMgr.get(); }

    u64 GetLocAddr(BufferType type) override { return locBuffer[type]; }

    CollOperator GetCurrentOp() override { return currentOp; }

    void SetCurrentOpMode(OpMode opMode) { currentOp.opMode = opMode; }

    uint32_t GetCommIdIndex() const { return idIndex_; }

    RmaBufferLite* GetRmaBufferLite(BufferType type) override
    {
        if (rmaBufferLiteVec[type] != nullptr) {
            return rmaBufferLiteVec[type].get();
        } else {
            return nullptr;
        }
    }

    u64 GetCounterAddr() override { return opCounterAddr; }

    MirrorTaskManagerLite* GetMirrorTaskMgrLite() override { return mirrorTaskMgrLite.get(); }

    string GetId() const { return commId; }

    u32 GetRankSize() const { return rankSize; }

    RankId GetMyRank() const { return myRank; }

    KfcCommand BackGroundGetCmd();

    void BackGroundSetStatus(KfcStatus status, KfcErrType errorCode = KfcErrType::NONE);

    void SetIsCommReady(bool flag) { isCommReady = flag; }

    bool IsCommReady() const { return isCommReady; }

    void SetNeedClean(bool flag) { needClean = flag; }

    bool IsNeedClean() const { return needClean; }

    void SetIsSuspended(bool status) { isSuspended = status; }

    bool IsSuspended() const { return isSuspended; }

    const ProfilingReporterLite* GetProfilingReporterLite() const { return profilingReporterLite.get(); }

    bool IsFirstUsed() const { return isFirstUsed; }

    void SetIsFirstUsedToFalse() { isFirstUsed = false; }

    bool IsUsed() const { return isUsed; }

    void SetIsUsed(bool used) { isUsed = used; }

    std::mutex& GetAicpuMc2Mutex() { return aicpuMc2Mutex; }

    InsExecutor* GetInsExecutor() { return insExecutor.get(); }

    ProfilingReporterLite* GetProfilingReporterLite() { return profilingReporterLite.get(); }

    HcclResult SendErrorMessageReportToHost(ErrorMessageReport& errMsgInfo);
    u32 GetUserStreamId() const { return userStreamId_; }

    bool IsErrorReported() const { return isErrorReported_; }

    void SetErrorReported() { isErrorReported_ = true; }

    void ResetErrorReported() { isErrorReported_ = false; }

    void UnfoldOp(HcclKernelParamLite* kernelParam);
    void RegisterRtsqCallback();

    void CheckOpExecStatus() const;
    bool CheckNeedUpdateRes(HcclKernelParamLite* kernelParam);
    void UpdateCommParam(HcclKernelParamLite* kernelParam);
    void UpdateLocBuffer(HcclKernelParamLite* kernelParam);
    void UpdateRes(HcclKernelParamLite* kernelParam);
    void UpdateTransports(HcclKernelParamLite* kernelParam);
    void UpdateHDCommnicate(HcclKernelParamLite* kernelParam);
    void CreateCollAlgComponentLite();
    void InitCurrentOp(HcclKernelParamLite* kernelParam);
    void UpdateUserStreamId(HcclKernelParamLite* kernelParam);
    std::shared_ptr<InsQueue> GetInsQueue(HcclKernelParamLite* kernelParam);
    void SetDfxOpInfo(uint64_t beginTime);

private:
    std::unique_ptr<StreamLiteMgr> streamLiteMgr = std::make_unique<StreamLiteMgr>();
    std::unique_ptr<QueueNotifyLiteMgr> queueNotifyLiteMgr = std::make_unique<QueueNotifyLiteMgr>();
    std::unique_ptr<Cnt1tonNotifyLiteMgr> cnt1tonNotifyLiteMgr = std::make_unique<Cnt1tonNotifyLiteMgr>();
    std::unique_ptr<CntNto1NotifyLiteMgr> cntNto1NotifyLiteMgr = std::make_unique<CntNto1NotifyLiteMgr>();
    std::unique_ptr<ConnectedLinkMgr> connectedLinkMgr = std::make_unique<ConnectedLinkMgr>();
    std::unique_ptr<PrimTranslator> primTranslator = std::make_unique<PrimTranslator>();
    std::unique_ptr<InsExecutor> insExecutor = std::make_unique<InsExecutor>(this);
    std::unique_ptr<MirrorTaskManagerLite> mirrorTaskMgrLite = std::make_unique<MirrorTaskManagerLite>();
    std::unique_ptr<ProfilingReporterLite> profilingReporterLite =
        std::make_unique<ProfilingReporterLite>(mirrorTaskMgrLite.get(), &ProfilingHandlerLite::GetInstance());

    HcclResult InitProfilingReporterLite();

    std::unique_ptr<MemTransportLiteMgr> transportLiteMgr =
        std::make_unique<MemTransportLiteMgr>(mirrorTaskMgrLite.get());

    std::unique_ptr<HostDeviceSyncNotifyLiteMgr> hostDeviceSyncNotifyLiteMgr =
        std::make_unique<HostDeviceSyncNotifyLiteMgr>();

    std::unique_ptr<CollAlgComponentLite> algComponentLite{};

    void RestoreOpRes(const string& opTag, const string& tagKey, u64 addr, u64 bufSize);

    void RestoreAllTransports(u64 addr, u64 bufSize);
    unique_ptr<HDCommunicateLite> kfcControlTransferH2D = std::make_unique<HDCommunicateLite>();
    unique_ptr<HDCommunicateLite> kfcStatusTransferD2H = std::make_unique<HDCommunicateLite>();
    unique_ptr<AicpuHdcHandler> hdcHandler{};
    std::mutex hdcShmLock_;

    u32 idIndex_;
    u32 myRank{0};
    u32 rankSize{0};
    u32 devPhyId{0};
    u32 hcclExecTimeout{1836};
    u64 scratchSize{0};
    u64 locBuffer[BufferType::__COUNT__]{};
    u64 opCounterAddr{0};
    u32 opIndex_;
    std::string commId;
    bool isUpdateComm{false};
    CollOperator currentOp;

    bool isCommReady{false}; // 是否初始化完成
    bool needClean{false};   // 是否有待清理资源
    bool isSuspended{false}; // 是否处于暂停状态

    void InitRmaBufferLite(HcclAicpuLocBufLite& bufLite, BufferType type);
    void UpdateDynamicOpData(HcclKernelParamLite* kernelParam);

    std::vector<std::unique_ptr<RmaBufferLite>> rmaBufferLiteVec;
    DevType devType;

    std::unordered_map<std::string, AlgTopoInfo> algTopoInfoMap;

    std::unique_ptr<CollAlgInfo> collAlgInfo;
    std::unique_ptr<RmtDataBufferMgr> rmtDataBufferMgr;

    std::unordered_set<std::string> offloadOpSet;

    void CreateOneSidedComponentLite();
    std::unique_ptr<OneSidedComponentLite> oneSidedComponentLite{};
    std::shared_ptr<InsQueue> GetOneSidedInsQueue(HcclKernelParamLite* kernelParam);

    bool isUsed{false};
    bool isFirstUsed{true};
    std::mutex aicpuMc2Mutex;
    u32 userStreamId_{0};
    bool isErrorReported_{false};

    std::unordered_set<std::string> loadedOpSet{};
};

} // namespace Hccl

#endif // HCCL_AICPU_COMMUNICATOR_IMPL_LITE_H_
