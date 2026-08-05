/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCOMM_CCU_KERNEL_H
#define HCOMM_CCU_KERNEL_H

#include <cstdint>
#include <functional>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "ccu_kernel_signature.h"
#include "ccu_types.h"
#include "ccu_task_arg_v1.h"
#include "ccu_task_param_v1.h"

#include "ccu_kernel_resource.h"
#include "ccu_instr_info_v1.h"
#include "ccu_rep_context_v1.h"

#include "ccu_funccall_v1.h"
#include "ccu_loopcall_v1.h"

#include "hccl_types.h"
#include "hcomm_primitives.h"
#include "ccu_datatype_v1.h"
#include "ccu_interface_assist_v1.h"

#include "ccu_res_repo.h"

// 暂时引用方便算法开发
// #include "ccu_condition_v1.h"  // 已注释，不使用旧API控制流
#include "ccu_loopblock_v1.h"
#include "ccu_loopcall_v1.h"
#include "ccu_loopgroupcall_v1.h"
#include "ccu_assist_pub.h"

#ifndef CCU_PROFILING // 和hccl仓兼容性使用
#define CCU_PROFILING
#endif

using CcuKernelHandle = uint64_t;

namespace hcomm {

struct GroupInfo {
    uint16_t loopParamId;
    uint16_t parallelParamId;
    uint16_t residualId;
};

struct GroupOpConfig {
    uint32_t msInterleave;
    uint32_t loopCount;
    uint64_t memSlice;
};

class CcuKernel : public CcuRep::CcuRepContext {
public:
    explicit CcuKernel(const CcuKernelArg& arg);
    CcuKernel() = default;
    ~CcuKernel() override;
    HcclResult Init();

    CcuResReq GetResourceRequest();
    CcuResRepository& GetResRepository();
    CcuRepResource& GetResource();
    CcuSharedResource& GetExportedRes();
    CcuSharedResource& GetImportedRes();

    void SetResRepository(const CcuResRepository& resRepo);
    void SetInstrId(uint32_t instrId);
    uint32_t GetInstrId() const;
    uint32_t GetInstrCount();
    void SetCcuInstrInfo(const CcuRep::CcuInstrInfo& instrInfo);

    HcclResult GeneTaskParam(const CcuTaskArg& arg, std::vector<CcuTaskParam>& taskParams);

    // 该友元函数用于在context类外创建Variable并被context内的资源管理器管理
    friend CcuRep::Variable CcuRep::CreateVariable(CcuRep::CcuRepContext* context);

    HcclResult AddProfilingInfo(
        const ChannelHandle* channels, uint32_t channelNum, HcclDataType dataType, HcclDataType outputDataType,
        HcclReduceOp opType, const std::string& opName);

    HcclResult AddCcuProfiling(
        GroupInfo groupInfo, const std::vector<ChannelHandle> channelHandle, HcclDataType dataType,
        HcclDataType outputDataType, HcclReduceOp opType, const std::string& opName);
    HcclResult AddCcuProfiling(
        const ChannelHandle* channels, uint32_t channelNum, HcclDataType dataType, HcclDataType outputDataType,
        HcclReduceOp opType, const std::string& opName);
    HcclResult GetCcuProfilingInfo(const CcuTaskArg& arg, std::vector<CcuProfilingInfo>& allCcuProfilingInfo);

    const std::vector<CcuProfilingInfo>& GetAllCcuProfilingInfo() { return allCcuProfilingInfos_; };

protected:
    // 子类实现
    virtual HcclResult Algorithm() = 0;
    virtual std::vector<uint64_t> GeneArgs(const CcuTaskArg& arg) = 0;

    // 使用channel中的Variable
    HcclResult CreateVariable(const ChannelHandle channel, uint32_t varIndex, CcuRep::Variable* var) const;
    CcuRep::Variable CreateVariable();
    CcuRep::Variable CreateContinuousVariable();
    CcuRep::LocalAddr CreateLocalAddr();
    CcuRep::RemoteAddr CreateRemoteAddr();
    CcuRep::RemoteAddr GetRemoteAddr(const ChannelHandle channel, const uint32_t index);
    CcuRep::LocalNotify CreateLocalNotify();
    CcuRep::CompletedEvent CreateCompletedEvent();
    CcuRep::CcuBuf CreateCcuBuf();
    CcuRep::Executor CreateExecutor();

    HcclResult CreateBlockCcuBuf(const uint32_t count, CcuRep::CcuBuf* ccuBufs);
    HcclResult CreateBlockExecutor(const uint32_t count, CcuRep::Executor* ccuExes);
    HcclResult CreateBlockCompletedEvent(const uint32_t count, CcuRep::CompletedEvent* ccuEvents);

    HcclResult RecordEvent(CcuRep::CompletedEvent event);
    HcclResult WaitEvent(CcuRep::CompletedEvent event);

    HcclResult LocalNotifyRecord(const uint32_t coreId, const uint32_t dstNotifyIdx, const uint32_t mask);
    HcclResult LocalNotifyWait(const uint32_t coreId, const uint32_t notifyIdx, const uint32_t mask);

    HcclResult NotifyRecord(const ChannelHandle channel, uint32_t remoteNotifyIdx, uint32_t mask = 1);

    HcclResult NotifyRecord(
        const ChannelHandle channel, uint32_t remoteNotifyIdx, uint32_t remoteVarIdx, const CcuRep::Variable& var,
        uint32_t mask = 1);
    HcclResult NotifyWait(const ChannelHandle channel, uint32_t localNotifyIdx, uint32_t mask = 1);

    // 数据操作
    HcclResult WriteNb(
        const ChannelHandle channel, const CcuRep::RemoteAddr& rem, const CcuRep::LocalAddr& loc,
        const CcuRep::Variable& len, CcuRep::CompletedEvent event);
    HcclResult WriteNb(
        const ChannelHandle channel, const CcuRep::RemoteAddr& rem, const CcuRep::CcuBuf& loc,
        const CcuRep::Variable& len, CcuRep::CompletedEvent event);

    HcclResult ReadNb(
        const ChannelHandle channel, const CcuRep::LocalAddr& loc, const CcuRep::RemoteAddr& rem,
        const CcuRep::Variable& len, CcuRep::CompletedEvent event);
    HcclResult ReadNb(
        const ChannelHandle channel, const CcuRep::CcuBuf& loc, const CcuRep::RemoteAddr& rem,
        const CcuRep::Variable& len, CcuRep::CompletedEvent event);

    HcclResult WriteReduceNb(
        const ChannelHandle channel, const CcuRep::RemoteAddr& rem, const CcuRep::LocalAddr& loc,
        const CcuRep::Variable& len, HcclDataType dataType, HcclReduceOp opType, CcuRep::CompletedEvent event);
    HcclResult ReadReduceNb(
        const ChannelHandle channel, const CcuRep::LocalAddr& loc, const CcuRep::RemoteAddr& rem,
        const CcuRep::Variable& len, HcclDataType dataType, HcclReduceOp opType, CcuRep::CompletedEvent event);

    HcclResult LocalCopyNb(
        const CcuRep::LocalAddr& dst, const CcuRep::LocalAddr& src, const CcuRep::Variable& len,
        CcuRep::CompletedEvent event); // dst和src是否都是local
    HcclResult LocalCopyNb(
        const CcuRep::CcuBuf& dst, const CcuRep::LocalAddr& src, const CcuRep::Variable& len,
        CcuRep::CompletedEvent event);
    HcclResult LocalCopyNb(
        const CcuRep::LocalAddr& dst, const CcuRep::CcuBuf& src, const CcuRep::Variable& len,
        CcuRep::CompletedEvent event);

    HcclResult LocalReduceNb(
        const CcuRep::LocalAddr& dst, const CcuRep::LocalAddr& src, const CcuRep::Variable& len, HcclDataType dataType,
        HcclReduceOp opType, CcuRep::CompletedEvent event);
    HcclResult LocalReduceNb(
        const CcuRep::CcuBuf* bufs, uint32_t count, HcclDataType dataType, HcclDataType outputDataType,
        HcclReduceOp opType, const CcuRep::Variable& len, CcuRep::CompletedEvent event);

    // 参数操作
    void Load(const CcuRep::Variable& var);

    // Variable src中存放内存地址，从地址中加载数据到Variable var中
    void LoadVariable(const CcuRep::Variable& src, const CcuRep::Variable& var);

    void LoadVariable(uint64_t addr, const CcuRep::Variable& var);
    void StoreVariable(const CcuRep::Variable& var, uint64_t addr);
    void LoadVariable(uint64_t addr, const CcuRep::Variable& var, uint32_t num);
    // 控制逻辑
    // 宏定义IF、WHILE
    CcuRep::FuncCall Func(const std::string& label);
    CcuRep::FuncCall Func(const CcuRep::Variable& funcAddr);
    CcuRep::LoopCall Loop(const std::string& label);

private:
    CcuRep::Address CreateAddress();
    CcuRep::LocalAddr CreateLocalAddr(const CcuRep::Variable& token);

protected:
    std::vector<ChannelHandle> channels_;
    GroupOpConfig moConfig_{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFFFFFFFFFF};

private:
    template <typename T>
    T CreateResAssist(std::array<std::vector<T>, CCU_MAX_IODIE_NUM>& resRecord);
    template <typename T>
    std::vector<T> CreateBlockResAssist(const uint32_t count, std::array<std::vector<T>, CCU_MAX_IODIE_NUM>& resRecord);

private:
    CcuRepResource res_{};
    CcuResRepository resRepo_{};

    CcuRep::CcuInstrInfo instrInfo_{};

    uint32_t loadArgIndex_{0};

    CcuSharedResource exportedRes_{};
    CcuSharedResource importedRes_{};
    std::vector<GroupInfo> groupOpSizeInfo_;
    std::vector<CcuProfilingInfo> allCcuProfilingInfos_;
};

// kernel构造函数的lambda函数
using KernelCreator = std::function<std::unique_ptr<hcomm::CcuKernel>(const CcuKernelArg&)>;

} // namespace hcomm

#endif // HCOMM_CCU_KERNEL_H
