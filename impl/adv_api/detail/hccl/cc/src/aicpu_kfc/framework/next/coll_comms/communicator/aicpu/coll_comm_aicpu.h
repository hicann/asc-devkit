/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef __COLL_COMM_AICPU_H__
#define __COLL_COMM_AICPU_H__

#include "common.h"
#include "aicpu_init_param.h"
#include "hcomm_primitives.h"
#include "transport_pub.h"
#include "thread.h"
#include "local_notify.h"
#include "ub_transport_lite_impl.h"
#include "p2p_transport_lite_impl.h"
#include "task_exception.h"
#include "aicpu_launch_manager.h"
#include "channel_param.h"
#include "hdc_pub.h"
#include "ns_recovery/aicpu/ns_recovery_lite.h"
#include <atomic>
#include "hcclCommDfxLite.h"
#include "error_message_v2.h"
#include "kfc.h"
#include "aicpu_hdc.h"
#include "roce_transport_lite_impl.h"
#include "hccl/hccl_types.h"

using namespace hccl;

class CollCommAicpu {
public:
    ~CollCommAicpu();
    HcclResult InitAicpuIndOp(CommAicpuParam* commAicpuParam);
    HcclResult InitThreads(ThreadMgrAicpuParam* param);
    HcclResult AllocChannelResource(HcclChannelUrmaRes* commParam);
    HcclResult NotifyFree(NotifyMgrAicpuParam* param);
    HcclResult NotifyAlloc(NotifyMgrAicpuParam* param);
    const std::vector<std::shared_ptr<Thread>>& GetAllThread() { return threads_; };
    const HcclTopoInfo& GetTopoInfo() { return topoInfo_; }
    const std::string& GetIdentifier() { return identifier_; }

    // taskException
    bool IsErrorReported() { return isErrorReported_; }
    void SetErrorReported(bool isErrorReported) { isErrorReported_ = isErrorReported; }
    HcclResult SendErrorMessageReportToHost(Hccl::ErrorMessageReport& errMsgInfo);
    HcclResult RegisterProfCallBack();
    HcclCommDfxLite* GetHcclCommDfxLite() { return &dfx_; };
    ReadWriteLockBase& GetThreadMutex() { return threadMutex_; }

    // h2d - d2h通道信息交互
    HcclResult BackGroundGetCmd(Hccl::KfcCommand& cmd);
    HcclResult BackGroundSetStatus(Hccl::KfcStatus state);
    u32 UpdateIndex();

    HcclCommStatus GetCommmStatus() { return commStatus_; }
    void SetCommmStatus(HcclCommStatus status);

    // N秒快恢
    hccl::NsRecoveryLitePtr GetNsRecoveryLitePtr();
    HcclResult Clean();
    HcclResult Resume(HcclChannelUrmaRes* commParam);

    HcclResult CheckIndOpExecStatus(bool timeout);

private:
    // 初始化
    void InitIndopEnv(CommAicpuParam* commAicpuParam);
    HcclResult InitHDCommunicate(CommAicpuParam* commAicpuParam);

    HcclResult InitUrmaChannel(HcclChannelUrmaRes* commParam);
    HcclResult ParsePackData(std::vector<char>& data, ChannelHandle& handle);
    HcclResult RegisterChannelAddDfxTaskInfo(ChannelHandle channel);
    HcclResult RegisterThreadAddDfxTaskInfo(ThreadHandle thread);
    void InitBackGroundThread();
    HcclResult ResumePackData(std::vector<char>& data, ChannelHandle& handle);
    HcclResult ProcessUrmaRes(HcclChannelUrmaRes* commParam, bool isInit);

    u32 devId_{0};
    // 通用的通道
    std::shared_ptr<hccl::HDCommunicate> kfcControlTransferH2D_{nullptr};
    std::shared_ptr<hccl::HDCommunicate> kfcStatusTransferD2H_{nullptr};

    std::string identifier_;
    HcclCommStatus commStatus_{HcclCommStatus::HCCL_COMM_STATUS_INVALID};
    HcclTopoInfo topoInfo_;
    ReadWriteLockBase threadMutex_;
    std::vector<std::shared_ptr<Thread>> threads_;
    std::vector<std::unique_ptr<LocalNotify>> notifys_;
    // A5 独立算子
    std::unordered_map<ChannelHandle, std::unique_ptr<Hccl::UbTransportLiteImpl>> ubTransportMap_;
    std::unordered_map<ChannelHandle, std::unique_ptr<Hccl::P2PTransportLiteImpl>> p2pTransportMap_;
    std::unordered_map<ChannelHandle, std::unique_ptr<Hccl::RoceTransportLiteImpl>> roceTransportMap_;

    // N秒快恢相关
    hccl::NsRecoveryLitePtr nsRecoveryLitePtr_{nullptr};

    // dfx
    bool isErrorReported_{false}; // 是否上报了taskException信息
    HcclCommDfxLite dfx_;
    u32 index_{0};
};

#endif // __COLL_COMM_AICPU_H__
