/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_kernel.h"
#include "ccu_rep_loopcall_v1.h"
#include "ccu_condition_v1.h"
#include "ccu_repeat_v1.h"
#include "ccu_datatype_v1.h"
#include "ccu_rep_context_v1.h"
#include "ccu_types.h"
#include "hccl_ccu_res.h"
#include "ccu_kernel_proxy.h"
#include "ccu/ccu_api/ccu_launch.h"
#include "ccu_dynamic_res_stub.h"
#include "sim_communicator.h"
#include <vector>
#include <string>
#include <map>
#include <functional>

using namespace hcomm::CcuRep;

namespace hcomm {

CcuKernel::CcuKernel(const CcuKernelArg& arg) {}

CcuKernel::~CcuKernel() = default;

HcclResult CcuKernel::Init() { return HCCL_SUCCESS; }

CcuResReq CcuKernel::GetResourceRequest() { return CcuResReq{}; }

CcuResRepository& CcuKernel::GetResRepository()
{
    static CcuResRepository repo;
    return repo;
}

CcuRepResource& CcuKernel::GetResource()
{
    static CcuRepResource res;
    return res;
}

void CcuKernel::SetResRepository(const CcuResRepository& resRepo) {}

void CcuKernel::SetInstrId(uint32_t instrId) {}

uint32_t CcuKernel::GetInstrId() const { return 0; }

uint32_t CcuKernel::GetInstrCount() { return 0; }

void CcuKernel::SetCcuInstrInfo(const CcuInstrInfo& instrInfo) {}

HcclResult CcuKernel::GeneTaskParam(const CcuTaskArg& arg, std::vector<CcuTaskParam>& taskParams)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::Algorithm() { return HCCL_SUCCESS; }

std::vector<uint64_t> CcuKernel::GeneArgs(const CcuTaskArg& arg) { return {}; }

HcclResult CcuKernel::CreateVariable(const ChannelHandle channel, uint32_t varIndex, Variable* var) const
{
    return HCCL_SUCCESS;
}

Variable CcuKernel::CreateVariable() { return Variable(nullptr); }

Variable CcuKernel::CreateContinuousVariable() { return Variable(nullptr); }

LocalAddr CcuKernel::CreateLocalAddr() { return LocalAddr{}; }

RemoteAddr CcuKernel::CreateRemoteAddr() { return RemoteAddr{}; }

RemoteAddr CcuKernel::GetRemoteAddr(const ChannelHandle channel, const uint32_t index) { return RemoteAddr{}; }

LocalNotify CcuKernel::CreateLocalNotify() { return LocalNotify{}; }

CompletedEvent CcuKernel::CreateCompletedEvent() { return CompletedEvent(nullptr); }

CcuBuf CcuKernel::CreateCcuBuf() { return CcuBuf{}; }

Executor CcuKernel::CreateExecutor() { return Executor{}; }

HcclResult CcuKernel::CreateBlockCcuBuf(const uint32_t count, CcuBuf* ccuBufs) { return HCCL_SUCCESS; }

HcclResult CcuKernel::CreateBlockExecutor(const uint32_t count, Executor* ccuExes) { return HCCL_SUCCESS; }

HcclResult CcuKernel::CreateBlockCompletedEvent(const uint32_t count, CompletedEvent* ccuEvents)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::RecordEvent(CompletedEvent event) { return HCCL_SUCCESS; }

HcclResult CcuKernel::WaitEvent(CompletedEvent event) { return HCCL_SUCCESS; }

HcclResult CcuKernel::LocalNotifyRecord(uint32_t coreId, uint32_t dstNotifyIdx, uint32_t mask) { return HCCL_SUCCESS; }

HcclResult CcuKernel::LocalNotifyWait(const uint32_t coreId, const uint32_t notifyIdx, const uint32_t mask)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::NotifyRecord(const ChannelHandle channel, uint32_t remoteNotifyIdx, uint32_t mask)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::NotifyRecord(
    const ChannelHandle channel, uint32_t remoteNotifyIdx, uint32_t remoteVarIdx, const Variable& var, uint32_t mask)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::NotifyWait(const ChannelHandle channel, uint32_t localNotifyIdx, uint32_t mask)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::WriteNb(
    const ChannelHandle channel, const RemoteAddr& rem, const LocalAddr& loc, const Variable& len, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::WriteNb(
    const ChannelHandle channel, const RemoteAddr& rem, const CcuBuf& loc, const Variable& len, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::ReadNb(
    const ChannelHandle channel, const LocalAddr& loc, const RemoteAddr& rem, const Variable& len, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::ReadNb(
    const ChannelHandle channel, const CcuBuf& loc, const RemoteAddr& rem, const Variable& len, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::WriteReduceNb(
    const ChannelHandle channel, const RemoteAddr& rem, const LocalAddr& loc, const Variable& len,
    HcclDataType dataType, HcclReduceOp opType, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::ReadReduceNb(
    const ChannelHandle channel, const LocalAddr& loc, const RemoteAddr& rem, const Variable& len,
    HcclDataType dataType, HcclReduceOp opType, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::LocalCopyNb(const LocalAddr& dst, const LocalAddr& src, const Variable& len, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::LocalCopyNb(const CcuBuf& dst, const LocalAddr& src, const Variable& len, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::LocalCopyNb(const LocalAddr& dst, const CcuBuf& src, const Variable& len, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::LocalReduceNb(
    const LocalAddr& dst, const LocalAddr& src, const Variable& len, HcclDataType dataType, HcclReduceOp opType,
    CompletedEvent event)
{
    return HCCL_SUCCESS;
}

HcclResult CcuKernel::LocalReduceNb(
    const CcuBuf* bufs, uint32_t count, HcclDataType dataType, HcclDataType outputDataType, HcclReduceOp opType,
    const Variable& len, CompletedEvent event)
{
    return HCCL_SUCCESS;
}

void CcuKernel::Load(const Variable& var) {}

void CcuKernel::LoadVariable(const Variable& src, const Variable& var) {}

void CcuKernel::LoadVariable(uint64_t addr, const Variable& var) {}

void CcuKernel::StoreVariable(const Variable& var, uint64_t addr) {}

void CcuKernel::LoadVariable(uint64_t addr, const Variable& var, uint32_t num) {}

LoopCall CcuKernel::Loop(const std::string& label)
{
    LoopCall tmp(this, label);
    return tmp;
}

Address CcuKernel::CreateAddress() { return Address(nullptr); }

LocalAddr CcuKernel::CreateLocalAddr(const Variable& token) { return LocalAddr{}; }

void CcuRepLoopCall::SetInArg(const Variable& var) {}

void CcuRepLoopCall::SetInArg(const std::vector<Variable>& varList) {}

void CcuRepLoopCall::SetInArg(const Memory& mem) {}

void CcuRepLoopCall::SetInArg(const std::vector<Memory>& memList) {}

void CcuRepLoopCall::SetInArg(const LocalAddr& addr) {}

void CcuRepLoopCall::SetInArg(const RemoteAddr& addr) {}

void CcuRepLoopCall::SetInArg(const std::vector<LocalAddr>& addrList) {}

void CcuRepLoopCall::SetInArg(const std::vector<RemoteAddr>& addrList) {}

void LoopCall::AppendToContext() {}

void CcuRepLoopBlock::DefineArg(Variable var) {}

void CcuRepLoopBlock::DefineArg(Memory mem) {}

void CcuRepLoopBlock::DefineArg(LocalAddr addr) {}

void CcuRepLoopBlock::DefineArg(RemoteAddr addr) {}

void CcuRepLoopBlock::DefineArg(const std::vector<Variable> varList) {}

void CcuRepLoopBlock::DefineArg(const std::vector<Memory> memList) {}

void CcuRepLoopBlock::DefineArg(const std::vector<LocalAddr> addrList) {}

void CcuRepLoopBlock::DefineArg(const std::vector<RemoteAddr> addrList) {}

CompletedEvent::CompletedEvent(CcuRepContext* context) : CcuVirRes(context) {}

void CompletedEvent::SetMask(uint32_t compeletedMask) {}

Repeat::Repeat(CcuRepContext* context, CcuRelationalOperator<Variable, uint64_t> rel) {}

Repeat::~Repeat() {}

void Repeat::Break() {}

bool Repeat::Check() const { return true; }

void Repeat::Run() {}

Condition::Condition(CcuRepContext* context, CcuRelationalOperator<Variable, uint64_t> rel) {}

Condition::~Condition() {}

bool Condition::Check() const { return true; }

void Condition::Run() {}

Address::Address(CcuRepContext* context) : CcuVirRes(context) {}

Address::Address(const Address& other) : CcuVirRes(other.context) {}

void Address::operator=(Address&& other) {}

void Address::operator=(uint64_t immediate) {}

void Address::operator=(const Address& other) {}

void Address::operator=(const Variable& other) {}

void Address::operator=(CcuArithmeticOperator<Variable, Address> op) {}

void Address::operator=(CcuArithmeticOperator<Address, Variable> op) {}

void Address::operator=(CcuArithmeticOperator<Address, Address> op) {}

void Address::operator+=(const Variable& other) {}

Variable::Variable(CcuRepContext* context) : CcuVirRes(context) {}

Variable::Variable(const Variable& other) : CcuVirRes(other.context) {}

void Variable::operator=(Variable&& other) {}

void Variable::operator=(uint64_t immediate) {}

void Variable::operator=(const Variable& other) {}

void Variable::operator=(CcuArithmeticOperator<Variable, Variable> op) {}

CcuArithmeticOperator<Variable, Variable> Variable::operator+(const Variable& varB) const
{
    return CcuArithmeticOperator<Variable, Variable>(*this, varB, CcuArithmeticOperatorType::ADDITION);
}

CcuArithmeticOperator<Variable, Address> Variable::operator+(const Address& addrB) const
{
    return CcuArithmeticOperator<Variable, Address>(*this, addrB, CcuArithmeticOperatorType::ADDITION);
}

void Variable::operator+=(const Variable& other) {}

CcuRelationalOperator<Variable, uint64_t> Variable::operator!=(uint64_t immediate) const
{
    return CcuRelationalOperator<Variable, uint64_t>(*this, immediate, CcuRelationalOperatorType::NOT_EQUAL);
}

CcuRelationalOperator<Variable, uint64_t> Variable::operator==(uint64_t immediate) const
{
    return CcuRelationalOperator<Variable, uint64_t>(*this, immediate, CcuRelationalOperatorType::NOT_EQUAL);
}

LoopBlock::LoopBlock(CcuRepContext* context, std::string label)
    : context(context), label(label), repLoopBlock(nullptr), curActiveBlock(nullptr)
{}

LoopBlock::~LoopBlock() {}

LocalNotify::LocalNotify(CcuRepContext* context) : CcuVirRes(context) {}

CcuRepContext::CcuRepContext()
    : activeBlock(nullptr), mainBlock(nullptr), dieId(CCU_MAX_IODIE_NUM), missionId(UINT32_MAX), missionKey(0)
{}

CcuRepContext::~CcuRepContext() {}

LoopCall::LoopCall(CcuRepContext* context, const std::string& label)
    : context(context), label(label), repLoopCall(nullptr)
{}

Executor::Executor(CcuRepContext* context) : CcuVirRes(context) {}

uint64_t CcuRep::GetTokenInfo(uint64_t va, uint64_t size) { return va == 0 ? 0 : va + 0x1234; }

void LoopGroupCall::Run(
    const std::vector<LoopCall>& loopVec, const std::vector<Variable>& loopCfg, const std::vector<Executor>& executors,
    Variable paraCfgIn, Variable offsetCfgIn) const
{}

CcuBuf::CcuBuf(CcuRepContext* context) : CcuVirRes(context) {}

uint16_t CcuBuf::Id() const { return 0; }

} // namespace hcomm

struct CaptureEntry {
    HcclSim::CcuSt::RegisterManager::KernelFunction capture;
    HcclSim::CcuSt::ArgConverter convert;
    HcclSim::CcuSt::ArgDeleter deleter;
};

static std::map<std::string, CaptureEntry>& GetCaptureRegistry()
{
    static std::map<std::string, CaptureEntry> registry;
    return registry;
}

void HcclSim::CcuSt::RegisterCaptureFunction(
    const std::string& name, const RegisterManager::KernelFunction& capture, const ArgConverter& convert,
    const ArgDeleter& deleter)
{
    CaptureEntry entry;
    entry.capture = capture;
    entry.convert = convert;
    entry.deleter = deleter;
    GetCaptureRegistry()[name] = entry;
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static uint32_t g_ccuKernelRegisterCount = 0;

void ResetCcuKernelRegisterCount() { g_ccuKernelRegisterCount = 0; }

uint32_t GetCcuKernelRegisterCount() { return g_ccuKernelRegisterCount; }

CcuKernelHandle RecordCcuKernelRegistrationForTest()
{
    ++g_ccuKernelRegisterCount;
    return g_ccuKernelRegisterCount;
}

HcclResult HcclCcuKernelRegister(HcclComm comm, CcuKernelHandle* kernelHandle, void* kernelCreator, void* kernelArg)
{
    return HCCL_SUCCESS;
}

HcclResult HcclCcuKernelRegisterFinish(HcclComm comm) { return HCCL_SUCCESS; }

HcclResult HcclCcuKernelLaunch(
    HcclComm comm, const ThreadHandle threadHandle, const CcuKernelHandle KernelHandle, void* taskArgs)
{
    return HCCL_SUCCESS;
}

HcclResult HcclCommQueryCcuIns(HcclComm comm, CcuInsHandle* insHandles, uint32_t* insNum)
{
    auto simComm = static_cast<HcclSim::SimCommunicator*>(comm);
    CHK_PTR_NULL(simComm);
    *insHandles = static_cast<CcuInsHandle>(simComm->GetRankId()) + 1;
    *insNum = 1;
    return HCCL_SUCCESS;
}

CcuResult HcommCcuKernelRegisterStart(CcuInsHandle insHandle)
{
    HcclSim::CcuSt::Result r =
        HcclSim::CcuSt::RegisterManager::Global().RegisterStart(static_cast<HcclSim::CcuSt::InstanceHandle>(insHandle));
    return r == HcclSim::CcuSt::Result::SUCCESS ? CCU_SUCCESS : CCU_E_INTERNAL;
}

CcuResult HcommCcuKernelRegister(
    CcuInsHandle insHandle, uint32_t dieId, const char* kernelFuncName, const void* kernelFunc, const void** kernelArgs,
    uint32_t argNum, CcuKernelHandle* kernelHandle)
{
    auto& registry = GetCaptureRegistry();
    auto it = registry.find(kernelFuncName);
    if (it == registry.end()) {
        *kernelHandle = RecordCcuKernelRegistrationForTest();
        return CCU_SUCCESS;
    }

    void* stArg = it->second.convert(kernelArgs[0], argNum);

    HcclSim::CcuSt::KernelHandle handle = 0;
    HcclSim::CcuSt::Result result = HcclSim::CcuSt::RegisterManager::Global().Register(
        static_cast<HcclSim::CcuSt::InstanceHandle>(insHandle), kernelFuncName, it->second.capture, stArg, handle);

    if (it->second.deleter) {
        it->second.deleter(stArg);
    }

    if (result != HcclSim::CcuSt::Result::SUCCESS) {
        return CCU_E_INTERNAL;
    }
    *kernelHandle = static_cast<CcuKernelHandle>(handle);
    return CCU_SUCCESS;
}

CcuResult HcommCcuKernelRegisterEnd(CcuInsHandle insHandle)
{
    HcclSim::CcuSt::Result r =
        HcclSim::CcuSt::RegisterManager::Global().RegisterEnd(static_cast<HcclSim::CcuSt::InstanceHandle>(insHandle));
    return r == HcclSim::CcuSt::Result::SUCCESS ? CCU_SUCCESS : CCU_E_INTERNAL;
}

extern ccu_result asccomm_ccu_kernel_register_start(void* ins_handle)
{
    HcclSim::CcuSt::Result r = HcclSim::CcuSt::RegisterManager::Global().RegisterStart(
        reinterpret_cast<HcclSim::CcuSt::InstanceHandle>(ins_handle));
    return r == HcclSim::CcuSt::Result::SUCCESS ? ccu_result::CCU_SUCCESS : ccu_result::CCU_E_INTERNAL;
}
extern ccu_result asccomm_ccu_kernel_register(
    void* ins_handle, uint32_t die_id, const char* kernel_func_name, const void* kernel_func, const void** kernel_args,
    uint32_t arg_num, ccu_kernel_handle* kernel_handle)
{
    (void)die_id;
    (void)kernel_func;
    if (kernel_handle == nullptr) {
        return ccu_result::CCU_E_PTR;
    }
    if (arg_num != 0U && kernel_args == nullptr) {
        return ccu_result::CCU_E_PTR;
    }
    auto& registry = GetCaptureRegistry();
    auto it = registry.find(kernel_func_name);
    if (it == registry.end()) {
        *kernel_handle = RecordCcuKernelRegistrationForTest();
        return ccu_result::CCU_SUCCESS;
    }
    void* stArg = it->second.convert(kernel_args[0], arg_num);
    HcclSim::CcuSt::KernelHandle handle = 0;
    HcclSim::CcuSt::Result result = HcclSim::CcuSt::RegisterManager::Global().Register(
        reinterpret_cast<HcclSim::CcuSt::InstanceHandle>(ins_handle), kernel_func_name, it->second.capture, stArg,
        handle);
    if (it->second.deleter) {
        it->second.deleter(stArg);
    }
    if (result != HcclSim::CcuSt::Result::SUCCESS) {
        return ccu_result::CCU_E_INTERNAL;
    }
    *kernel_handle = static_cast<ccu_kernel_handle>(handle);
    return ccu_result::CCU_SUCCESS;
}
extern ccu_result asccomm_ccu_kernel_register_end(void* ins_handle)
{
    HcclSim::CcuSt::Result r = HcclSim::CcuSt::RegisterManager::Global().RegisterEnd(
        reinterpret_cast<HcclSim::CcuSt::InstanceHandle>(ins_handle));
    return r == HcclSim::CcuSt::Result::SUCCESS ? ccu_result::CCU_SUCCESS : ccu_result::CCU_E_INTERNAL;
}
// query 桩的版本兼容：asc-devkit 引用的 asc-comm tag（FetchContent 3dd6e71d）尚未包含
// asccomm_ccu_res_desc 与 query 接口（该接口随 asc-comm 查询 PR 进入更新的 tag，本地源码树已含）。
// 两种环境都要能编：tag 版头无该类型 → 本地前向声明一个等大小 POD；新版头已声明该类型与
// 函数原型 → 桩定义必须用真类型签名（C++ 视同名不同参为重载冲突，不能写 void* 版）。
// 按真接口的参数布局（void*/uint32_t/指针均 ABI 等宽），用宏统一两版签名。
#ifndef ASCCOMM_CCU_RES_DESC_TYPE_COUNT
#define ASCCOMM_CCU_RES_DESC_TYPE_COUNT 7U
typedef struct asccomm_ccu_res_desc_tag_stub {
    unsigned int dieId;
    unsigned int resNum[ASCCOMM_CCU_RES_DESC_TYPE_COUNT];
    unsigned int translatorResNum[ASCCOMM_CCU_RES_DESC_TYPE_COUNT];
} asccomm_ccu_res_desc;
#endif
extern ccu_result asccomm_ccu_kernel_query_res_req(
    void* ins_handle, uint32_t die_id, const void* kernel_func, const void** kernel_args, uint32_t arg_num,
    asccomm_ccu_res_desc* res_desc)
{
    (void)ins_handle;
    (void)die_id;
    (void)kernel_func;
    (void)kernel_args;
    (void)arg_num;
    if (res_desc != nullptr) {
        *res_desc = asccomm_ccu_res_desc{};
    }
    return ccu_result::CCU_SUCCESS;
}
CcuResult HcommCcuKernelLaunch(
    ThreadHandle threadHandle, CcuKernelHandle kernelHandle, const void* taskArgs, uint32_t argSize)
{
    return CCU_SUCCESS;
}

extern ccu_result asccomm_ccu_kernel_launch(
    ccu_launch_stream stream, ccu_kernel_handle kernel_handle, const void* task_args, uint32_t arg_num)
{
    (void)stream;
    (void)kernel_handle;
    (void)task_args;
    (void)arg_num;
    return ccu_result::CCU_SUCCESS;
}

#ifdef __cplusplus
}
#endif // __cplusplus
