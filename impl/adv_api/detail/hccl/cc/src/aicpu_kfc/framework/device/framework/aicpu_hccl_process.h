/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef __AICPU_HCCL_PROCESS_H__
#define __AICPU_HCCL_PROCESS_H__

#include <memory>
#include <vector>
#include <string>
#include "common.h"
#include "aicpu_communicator.h"
#include "common/aicpu_hccl_def.h"
#include "hdc_pub.h"
#include "stream_pub.h"
#include "aicpu_launch_manager.h"

class AicpuHcclProcess {
public:
    ~AicpuHcclProcess() = default;
    static u32 AicpuRpcResInitV2(HcclOpResParam* commParam, bool isCustom);
    static ReadWriteLockBase& AicpuGetCommMutex();
    static hccl::HcclCommAicpu* AicpuGetCommbyGroup(const std::string& group);
    static HcclResult AicpuGetCommAll(std::vector<std::pair<std::string, hccl::HcclCommAicpu*>>& aicpuCommInfo);
    static void AicpuDestoryCommbyGroup(const std::string& group);
    static void AicpuReleaseCommbyGroup(const std::string& group);
    static HcclResult AicpuRunRpcServerV2(
        hccl::HcclCommAicpu* hcclCommAicpu, OpTilingData* tilingData, HcclOpResParam* commParam);
    static void CallMC2MaintenanceThread(AicpuComContext* ctx);
    static bool GetCommExecStatus(const std::string& group);
    static void CopyCtxInfo(AicpuComContext* ctx);
    static void CopyCtxForBackGroundDfx(const AicpuComContext* ctx);
    static DevType AicpuGetInnerDevType();
    static HcclResult AcquireAicpuComm(const std::string& group, hccl::HcclCommAicpu** aicpuCommPtr);
    static HcclResult HandleOneSideService(const OpTilingData* tilingData);
    static HcclResult InitAsyncFlag(
        const uint32_t* lFlagAddr, const uint32_t* rFlagAddr, hccl::Transport::Buffer* localFlagBufferCheck,
        hccl::Transport::Buffer* localFlagBufferWrite, hccl::Transport::Buffer* remoteFlagBuf);
    static HcclResult WaitAsyncFlag(
        hccl::Transport::Buffer* localFlagBufferCheck, const uint32_t flagValue, uint64_t timeOut);
    static HcclResult AicpuIndOpChannelInit(HcclIndOpChannelRemoteResV3* commParam);
    static HcclResult AicpuIndOpThreadInit(ThreadMgrAicpuParam* param);
    static HcclResult AicpuIndOpNotifyInit(NotifyMgrAicpuParam* param);
    static HcclResult AicpuIndOpCommInit(CommAicpuParam* commAicpuParam);

    // IndOp dfx
    static HcclResult AicpuRegOpInfo(void* opInfo, u32 size);
    static HcclResult AicpuRegOpTaskException(HcommGetOpInfoCallback callback);

private:
    static HcclResult CalcDataSize(
        HcclCMDType op, HcclDataType type, u64 count, u32 rankSize, u64& inputSize, u64& outputSize);
    static HcclResult CalcDataSizeV(hccl::OpParam& param, u32 rankSize);
    static u64 CalcOpTilingVDataDesVDataLen(u32 rankSize);
};

#endif // __AICPU_HCCL_PROCESS_HPP__
