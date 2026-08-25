/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef AICPU_LAUNCH_MANAGER_H
#define AICPU_LAUNCH_MANAGER_H

#include "hccl_common.h"
#include "stream_pub.h"
#include "aicpu_operator_pub.h"
#include "thread.h"
#include "hccl/hccl_res.h"
#include "hccl_independent_common.h"
#include "local_notify.h"
#include "aicpu_init_param.h"

constexpr uint32_t THREAD_UNIQUE_ID_MAX_SIZE = 1024;
constexpr uint32_t NOTIFY_UNIQUE_ID_MAX_SIZE = THREAD_UNIQUE_ID_MAX_SIZE * hccl::HCCL_THREAD_NOTIFY_MAX_NUM;
constexpr uint32_t NOTIFY_DEVICE_ID_MAX_SIZE = 21 * hccl::HCCL_THREAD_NOTIFY_MAX_NUM;
constexpr uint32_t NAME_SIZE = 64;
struct ThreadMgrAicpuParam {
    u32 threadNum;
    char hcomId[HCOMID_MAX_SIZE];
    char threadParam[LOCAL_STREAM_MAX_NUM][THREAD_UNIQUE_ID_MAX_SIZE]; // 含序列化后thread信息，约40KB
    void* deviceHandle;
    u32 rsv1;
    s32 deviceLogicId{-1}; // 基础通信使用
    u32 deviceType{0};     // 基础通信使用
};

struct NotifyMgrAicpuParam {
    u32 notifyNum;
    char hcomId[HCOMID_MAX_SIZE];
    char notifyParam[NOTIFY_UNIQUE_ID_MAX_SIZE]; // 含序列化后notify信息
    void* deviceHandle;
    bool freeFlag;
    u32 rsv1;
};

namespace hccl {

struct ApiParamDef {
    uint64_t commContext{};
    char kernelName[NAME_SIZE] = {};
    char soName[NAME_SIZE] = {};
    char opName[NAME_SIZE] = {};

    ApiParamDef(const char* kName, const char* sName, const char* oName)
    {
        const s32 kernelRet = strncpy_s(kernelName, NAME_SIZE, kName, NAME_SIZE - 1);
        const s32 soRet = strncpy_s(soName, NAME_SIZE, sName, NAME_SIZE - 1);
        const s32 opRet = strncpy_s(opName, NAME_SIZE, oName, NAME_SIZE - 1);
        if (UNLIKELY(kernelRet != EOK || soRet != EOK || opRet != EOK)) {
            HCCL_ERROR(
                "[%s] failed to initialize API names, kernel ret[%d], so ret[%d], op ret[%d].", __func__, kernelRet,
                soRet, opRet);
        }
    }
};

struct ThreadKernelLaunchConfig {
    std::string commId;       // 通信ID
    aclrtBinHandle binHandle; // 自定义二进制句柄
    std::string kernelName;   // 核函数名称
    bool needDeviceInfo;      // 是否需要设备信息
    uint32_t timeoutSec;      // 超时时间（秒）
    bool needProfiling;       // 是否需要性能分析
    bool isSupplementNotify;  // 是否是补充notify kernel

    ThreadKernelLaunchConfig(
        const std::string& cid, aclrtBinHandle binHandle, const std::string& name, bool needDev, uint32_t timeout,
        bool profiling, bool isSupplementNotify)
        : commId(cid),
          binHandle(binHandle),
          kernelName(name),
          needDeviceInfo(needDev),
          timeoutSec(timeout),
          needProfiling(profiling),
          isSupplementNotify(isSupplementNotify)
    {}
};

class AicpuLaunchMgr {
public:
    AicpuLaunchMgr() = default;
    ~AicpuLaunchMgr() = default;
    template <typename OpParam, typename ApiParam>
    static HcclResult KernelLaunch(OpParam& opParam, ApiParam& apiParam, rtStream_t aicpuInitStream);
    static HcclResult ThreadKernelLaunchImpl(
        std::vector<std::shared_ptr<Thread>>& newThreads, std::unique_ptr<ThreadHandle[]>& aicpuHandle,
        const ThreadKernelLaunchConfig& config);
    static HcclResult ThreadKernelLaunchForComm(
        std::vector<std::shared_ptr<Thread>>& newThreads, const std::string& commId,
        std::unique_ptr<ThreadHandle[]>& aicpuHandle, aclrtBinHandle binHandle);
    static HcclResult ThreadKernelLaunchForBase(
        std::vector<std::shared_ptr<Thread>>& newThreads, std::unique_ptr<ThreadHandle[]>& aicpuHandle,
        aclrtBinHandle binHandle);
    static HcclResult SupplementNotifyKernelLaunch(
        std::vector<std::shared_ptr<Thread>>& newThreads, const std::string& commId,
        std::unique_ptr<ThreadHandle[]>& aicpuHandle, aclrtBinHandle binHandle);
    static HcclResult ThreadKernelLaunchDestroy(
        ThreadHandle* threadHandles, uint32_t listNum, aclrtBinHandle binHandle);
    static HcclResult NotifyKernelLaunchAlloc(
        std::vector<std::unique_ptr<LocalNotify>>& newNotifys, const std::string& commId,
        std::unique_ptr<NotifyHandle[]>& hostHandle, aclrtBinHandle binCustomHandle);
    static HcclResult NotifyKernelLaunchFree(
        std::vector<NotifyHandle>& aicpuNotifys, uint32_t notifyNum, const std::string& commId,
        aclrtBinHandle binCustomHandle);
    template <typename OpParam>
    static HcclResult KernelLaunchAicpuCustom(
        OpParam& opParam, std::string kernelName, rtStream_t aicpuInitStream, aclrtBinHandle binCustomHandle);

private:
    HcclResult AiCpuStreamAllocAndGet(rtStream_t& aiCpuStream);
    static HcclResult PrepareAicpuNotifyParam(
        NotifyMgrAicpuParam& opParam, const std::string& commId, size_t notifyNum, bool freeFlag, void* deviceHandle);
    static HcclResult LaunchNotifyKernel(NotifyMgrAicpuParam& opParam, aclrtBinHandle binCustomHandle);
    Stream opStream_;
};
} // namespace hccl
#endif
