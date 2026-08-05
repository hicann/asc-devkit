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
#include "ccu_condition_v1.h"
#include "ccu_rep_loopcall_v1.h"
#include "ccu_repeat_v1.h"
#include <vector>
#include <string>

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

uint64_t CcuRep::GetTokenInfo(uint64_t va, uint64_t size) { return 0; }

void LoopGroupCall::Run(
    const std::vector<LoopCall>& loopVec, const std::vector<Variable>& loopCfg, const std::vector<Executor>& executors,
    Variable paraCfgIn, Variable offsetCfgIn) const
{}

CcuBuf::CcuBuf(CcuRepContext* context) : CcuVirRes(context) {}

uint16_t CcuBuf::Id() const { return 0; }

} // namespace hcomm

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

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

HcclResult HcclCommQueryCcuIns(HcclComm comm, CcuInsHandle* insHandles, uint32_t* insNum) { return HCCL_SUCCESS; }

CcuResult HcommCcuKernelRegisterStart(CcuInsHandle insHandle) { return CCU_SUCCESS; }

CcuResult HcommCcuKernelRegister(
    CcuInsHandle insHandle, uint32_t dieId, const char* kernelFuncName, const void* kernelFunc, const void** kernelArgs,
    uint32_t argNum, CcuKernelHandle* kernelHandle)
{
    return CCU_SUCCESS;
}

CcuResult HcommCcuKernelRegisterEnd(CcuInsHandle insHandle) { return CCU_SUCCESS; }

CcuResult HcommCcuKernelLaunch(
    ThreadHandle threadHandle, CcuKernelHandle kernelHandle, const void* taskArgs, uint32_t argSize)
{
    return CCU_SUCCESS;
}

#ifdef __cplusplus
}
#endif // __cplusplus
