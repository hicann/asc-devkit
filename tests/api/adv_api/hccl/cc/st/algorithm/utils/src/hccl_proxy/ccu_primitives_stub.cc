/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_primitives_stub.h"
#include "ccu_kernel_proxy.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace HcclSim {
namespace CcuSt {
namespace {
thread_local CompilerContext* gCompilerContext = nullptr;

Operation MakeConditionOperation(OpCode code, const Condition& condition)
{
    Operation operation;
    operation.code = code;
    operation.src0 = condition.variable;
    operation.immediate = condition.immediate;
    operation.equal = condition.equal;
    return operation;
}
} // namespace

CompilerContext::CompilerContext(Program& program) : program_(program) {}

ResourceHandle CompilerContext::AllocateVariable()
{
    const ResourceHandle handle = nextHandle_++;
    program_.variables.insert(handle);
    return handle;
}

ResourceHandle CompilerContext::AllocateAddress()
{
    const ResourceHandle handle = nextHandle_++;
    program_.addresses.insert(handle);
    return handle;
}

ResourceHandle CompilerContext::AllocateEvent()
{
    const ResourceHandle handle = nextHandle_++;
    program_.events.insert(handle);
    return handle;
}

ResourceHandle CompilerContext::AllocateBuffer()
{
    const ResourceHandle handle = nextHandle_++;
    program_.buffers.insert(handle);
    return handle;
}

ResourceHandle CompilerContext::AllocateLocalAddress(ResourceHandle& address, ResourceHandle& token)
{
    ResourceHandle handle = nextHandle_++;
    address = AllocateAddress();
    token = AllocateVariable();
    program_.localAddresses[handle] = {address, token};
    return handle;
}

ResourceHandle CompilerContext::AllocateRemoteAddress(ResourceHandle& address, ResourceHandle& token)
{
    ResourceHandle handle = nextHandle_++;
    address = AllocateAddress();
    token = AllocateVariable();
    program_.remoteAddresses[handle] = {address, token};
    return handle;
}

ResourceHandle CompilerContext::GetChannelVariable(ChannelHandle channel, uint32_t index)
{
    const uint32_t peer = ChannelRemoteRank(channel);
    auto key = std::make_pair(peer, index);
    auto iterator = program_.channelVariables.find(key);
    if (iterator != program_.channelVariables.end()) {
        return iterator->second;
    }
    ResourceHandle handle = AllocateVariable();
    program_.channelVariables[key] = handle;
    return handle;
}

void CompilerContext::ConfigureProgram(uint32_t rankId, uint32_t rankSize, const std::vector<ChannelHandle>& channels)
{
    program_.rankId = rankId;
    program_.rankSize = rankSize;
    program_.channels = channels;
}

void CompilerContext::RegisterArgument(ResourceHandle variable, uint32_t argId)
{
    program_.arguments.push_back(ArgDescriptor{argId, variable});
}

void CompilerContext::Append(const Operation& operation)
{
    if (loopBodyTarget_ != nullptr) {
        loopBodyTarget_->push_back(operation);
    } else {
        program_.operations.push_back(operation);
    }
}

ResourceHandle CompilerContext::CreateLoop() { return nextHandle_++; }

void CompilerContext::LoopBodyEnter(ResourceHandle loopHandle)
{
    loopBodies_[loopHandle] = {};
    loopBodyTarget_ = &loopBodies_[loopHandle];
}

void CompilerContext::LoopBodyExit() { loopBodyTarget_ = nullptr; }

std::vector<Operation>& CompilerContext::GetLoopBody(ResourceHandle loopHandle) { return loopBodies_[loopHandle]; }

ResourceHandle CompilerContext::CreateLoopGroup(
    ResourceHandle parallelCfg, ResourceHandle offsetCfg, uint32_t maxLoopNum,
    const std::vector<ResourceHandle>& loopHandles, const std::vector<ResourceHandle>& loopParamHandles)
{
    ResourceHandle groupHandle = nextHandle_++;

    GroupCopyDescriptor descriptor;
    descriptor.destination = groupHandle;
    descriptor.loopGroups.push_back(GroupCopyLoopGroupDescriptor{});
    auto& lg = descriptor.loopGroups.back();
    lg.parallelParamVariable = parallelCfg;
    lg.addressOffsetVariable = offsetCfg;
    lg.maxLoopNum = maxLoopNum;
    lg.loopHandles = loopHandles;
    lg.loopParamHandles = loopParamHandles;

    for (const auto& loopHandle : loopHandles) {
        descriptor.loops.push_back(GroupCopyLoopDescriptor{});
        descriptor.loops.back().index = static_cast<uint32_t>(descriptor.loops.size() - 1);
        descriptor.functions.push_back(GroupCopyFunctionDescriptor{});
        descriptor.functions.back().index = static_cast<uint32_t>(descriptor.functions.size() - 1);
    }

    program_.groupCopies.push_back(descriptor);
    program_.loopBodies = loopBodies_;

    Operation operation;
    operation.code = OpCode::GROUP_COPY;
    operation.dst = groupHandle;
    operation.index = static_cast<uint32_t>(program_.groupCopies.size() - 1);
    Append(operation);

    return groupHandle;
}

void CompilerContext::AppendGroupCopy(
    ResourceHandle destination, ResourceHandle source, ResourceHandle length,
    const std::array<ResourceHandle, 4>& groupSize)
{
    constexpr uint32_t msInterleave = 8;
    constexpr uint32_t loopCount = 8;
    constexpr uint64_t msSize = 4096;
    constexpr uint32_t localCopyMsPerLoop = 8;

    GroupCopyDescriptor descriptor;
    descriptor.destination = destination;
    descriptor.source = source;
    descriptor.length = length;
    descriptor.groupSize = groupSize;
    descriptor.msInterleave = msInterleave;
    descriptor.loopCount = loopCount;
    descriptor.memSlice = localCopyMsPerLoop * msSize;
    for (uint32_t index = 0; index < loopCount; ++index) {
        descriptor.events.push_back(AllocateEvent());
    }
    for (uint32_t index = 0; index < loopCount * msInterleave; ++index) {
        descriptor.buffers.push_back(AllocateBuffer());
    }
    for (uint32_t index = 0; index < 2; ++index) {
        GroupCopyFunctionDescriptor function;
        function.index = index;
        function.bufferBase = index * msInterleave;
        function.event = descriptor.events[index];
        function.body = {
            GroupCopyBodyOp::LOCAL_TO_BUFFER, GroupCopyBodyOp::EVENT_WAIT, GroupCopyBodyOp::BUFFER_TO_LOCAL,
            GroupCopyBodyOp::EVENT_WAIT_FINAL};
        descriptor.functions.push_back(function);
        descriptor.loops.push_back(GroupCopyLoopDescriptor{index, index});
    }
    descriptor.loopGroups.push_back(
        GroupCopyLoopGroupDescriptor{groupSize[0], groupSize[1], 0, groupSize[0], 0, loopCount, {0}});
    descriptor.loopGroups.push_back(
        GroupCopyLoopGroupDescriptor{groupSize[2], 0, groupSize[2], groupSize[0], groupSize[3], loopCount, {0, 1}});
    program_.groupCopies.push_back(descriptor);

    Operation operation;
    operation.code = OpCode::GROUP_COPY;
    operation.dst = destination;
    operation.src0 = source;
    operation.src1 = length;
    operation.index = static_cast<uint32_t>(program_.groupCopies.size() - 1);
    Append(operation);
}

Result CompilerContext::Finalize()
{
    std::vector<size_t> stack;
    for (size_t index = 0; index < program_.operations.size(); ++index) {
        Operation& operation = program_.operations[index];
        if (operation.code == OpCode::IF_BEGIN || operation.code == OpCode::WHILE_BEGIN ||
            operation.code == OpCode::ELSE_BEGIN) {
            stack.push_back(index);
            continue;
        }
        if (operation.code != OpCode::IF_END && operation.code != OpCode::WHILE_END) {
            continue;
        }
        if (stack.empty()) {
            return Result::STATE_ERROR;
        }
        const size_t beginIndex = stack.back();
        stack.pop_back();
        if (operation.code == OpCode::IF_END) {
            if (program_.operations[beginIndex].code != OpCode::IF_BEGIN &&
                program_.operations[beginIndex].code != OpCode::ELSE_BEGIN) {
                return Result::STATE_ERROR;
            }
        } else {
            if (program_.operations[beginIndex].code != OpCode::WHILE_BEGIN) {
                return Result::STATE_ERROR;
            }
        }
        program_.operations[beginIndex].peerIndex = index;
        operation.peerIndex = beginIndex;
    }
    if (!stack.empty()) {
        return Result::STATE_ERROR;
    }

    for (size_t index = 0; index < program_.operations.size(); ++index) {
        if (program_.operations[index].code != OpCode::ELSE_BEGIN) {
            continue;
        }
        size_t ifEndIndex = index - 1;
        if (ifEndIndex >= program_.operations.size() || program_.operations[ifEndIndex].code != OpCode::IF_END) {
            return Result::STATE_ERROR;
        }
        size_t ifBeginIndex = program_.operations[ifEndIndex].peerIndex;
        if (ifBeginIndex >= program_.operations.size() || program_.operations[ifBeginIndex].code != OpCode::IF_BEGIN) {
            return Result::STATE_ERROR;
        }
        program_.operations[ifBeginIndex].peerIndex = index;
    }

    std::sort(
        program_.arguments.begin(), program_.arguments.end(),
        [](const ArgDescriptor& lhs, const ArgDescriptor& rhs) { return lhs.id < rhs.id; });
    for (size_t index = 0; index < program_.arguments.size(); ++index) {
        if (program_.arguments[index].id != index) {
            return Result::CONTRACT_ERROR;
        }
        if (index > 0 && program_.arguments[index - 1].id == program_.arguments[index].id) {
            return Result::CONTRACT_ERROR;
        }
    }
    return Result::SUCCESS;
}

CompilerContext& CompilerContext::Current()
{
    if (gCompilerContext == nullptr) {
        throw std::logic_error("CCU primitive called outside kernel registration");
    }
    return *gCompilerContext;
}

void CompilerContext::SetCurrent(CompilerContext* context) { gCompilerContext = context; }

ControlScope::ControlScope(const Condition& condition, bool isWhile, bool isElse) : isWhile_(isWhile), isElse_(isElse)
{
    if (isElse_) {
        Operation operation;
        operation.code = OpCode::ELSE_BEGIN;
        CompilerContext::Current().Append(operation);
    } else {
        CompilerContext::Current().Append(
            MakeConditionOperation(isWhile ? OpCode::WHILE_BEGIN : OpCode::IF_BEGIN, condition));
    }
}

bool ControlScope::Once()
{
    if (entered_) {
        return false;
    }
    entered_ = true;
    return true;
}

void ControlScope::Close()
{
    if (closed_) {
        return;
    }
    Operation operation;
    operation.code = isWhile_ ? OpCode::WHILE_END : OpCode::IF_END;
    CompilerContext::Current().Append(operation);
    closed_ = true;
}

} // namespace CcuSt
} // namespace HcclSim

namespace AscendC {
namespace ccu {
namespace {
using HcclSim::CcuSt::CompilerContext;
using HcclSim::CcuSt::OpCode;
using HcclSim::CcuSt::Operation;

void AppendSimple(OpCode code, ResourceHandle destination, ResourceHandle source = 0)
{
    Operation operation;
    operation.code = code;
    operation.dst = destination;
    operation.src0 = source;
    CompilerContext::Current().Append(operation);
}
} // namespace

Variable::Variable() : handle(CompilerContext::Current().AllocateVariable()) {}
Variable::Variable(ResourceHandle existingHandle) : handle(existingHandle) {}

void Variable::operator=(const Variable& other) const { AppendSimple(OpCode::ASSIGN_VAR, handle, other.handle); }

void Variable::operator=(const VariableExpr& expr) const
{
    Operation addOp;
    addOp.code = OpCode::ADD_VAR;
    addOp.dst = handle;
    addOp.src0 = expr.LhsHandle();
    addOp.src1 = expr.RhsHandle();
    CompilerContext::Current().Append(addOp);
}

void Variable::operator=(uint64_t immediate) const
{
    Operation operation;
    operation.code = OpCode::ASSIGN_IMM;
    operation.dst = handle;
    operation.immediate = immediate;
    CompilerContext::Current().Append(operation);
}

void Variable::operator+=(const Variable& other) const
{
    Operation operation;
    operation.code = OpCode::ADD_VAR;
    operation.dst = handle;
    operation.src0 = handle;
    operation.src1 = other.handle;
    CompilerContext::Current().Append(operation);
}

VariableExpr Variable::operator+(const Variable& other) const { return VariableExpr(handle, other.handle); }

Condition Variable::operator==(uint64_t immediate) const { return Condition{handle, immediate, true}; }

Condition Variable::operator!=(uint64_t immediate) const { return Condition{handle, immediate, false}; }

Address::Address() : handle(CompilerContext::Current().AllocateAddress()) {}
Address::Address(ResourceHandle existingHandle) : handle(existingHandle) {}

void Address::operator=(const Variable& variable) const
{
    AppendSimple(OpCode::ADDRESS_ASSIGN_VAR, handle, variable.handle);
}

void Address::operator=(const Address& address) const
{
    AppendSimple(OpCode::ADDRESS_ASSIGN_ADDRESS, handle, address.handle);
}

void Address::operator=(const AddressExpr& expr) const
{
    Operation operation;
    operation.code = OpCode::ADDRESS_ADD_VAR;
    operation.dst = handle;
    operation.src0 = expr.RhsHandle();
    CompilerContext::Current().Append(operation);
}

void Address::operator+=(const Variable& variable) const
{
    AppendSimple(OpCode::ADDRESS_ADD_VAR, handle, variable.handle);
}

void Address::operator+=(const Address& other) const
{
    Operation operation;
    operation.code = OpCode::ADDRESS_ASSIGN_ADDRESS;
    operation.dst = handle;
    operation.src0 = handle;
    operation.src1 = other.handle;
    CompilerContext::Current().Append(operation);
}

AddressExpr Address::operator+(const Address& other) const { return AddressExpr(handle, other.handle, true); }

AddressExpr Address::operator+(const Variable& variable) const { return AddressExpr(handle, variable.handle, true); }

LocalAddr::LocalAddr() : addr(0), token(0)
{
    ResourceHandle addressHandle = 0;
    ResourceHandle tokenHandle = 0;
    handle = CompilerContext::Current().AllocateLocalAddress(addressHandle, tokenHandle);
    addr.handle = addressHandle;
    token.handle = tokenHandle;
}

LocalAddr::LocalAddr(ResourceHandle localHandle, ResourceHandle addressHandle, ResourceHandle tokenHandle)
    : addr(addressHandle), token(tokenHandle), handle(localHandle)
{}

RemoteAddr::RemoteAddr() : addr(0), token(0)
{
    ResourceHandle addressHandle = 0;
    ResourceHandle tokenHandle = 0;
    handle = CompilerContext::Current().AllocateRemoteAddress(addressHandle, tokenHandle);
    addr.handle = addressHandle;
    token.handle = tokenHandle;
}

RemoteAddr::RemoteAddr(ResourceHandle remoteHandle, ResourceHandle addressHandle, ResourceHandle tokenHandle)
    : addr(addressHandle), token(tokenHandle), handle(remoteHandle)
{}

Event::Event() : handle(CompilerContext::Current().AllocateEvent()) {}

template <>
Variable GetResByChannel<Variable>(ChannelHandle channel, uint32_t index)
{
    return Variable(CompilerContext::Current().GetChannelVariable(channel, index));
}

Result LoadArg(Variable variable, uint32_t argId)
{
    Operation operation;
    operation.code = OpCode::LOAD_ARG;
    operation.dst = variable.handle;
    operation.index = argId;
    CompilerContext::Current().Append(operation);
    CompilerContext::Current().RegisterArgument(variable.handle, argId);
    return Result::SUCCESS;
}

Result Load(Variable address, Variable destination)
{
    Operation operation;
    operation.code = OpCode::LOAD_MEMORY;
    operation.dst = destination.handle;
    operation.src0 = address.handle;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result Load(uint64_t address, Variable destination)
{
    Variable addrVar;
    addrVar = address;
    return Load(addrVar, destination);
}

Result Load(Variable address, Array<Variable>& destinations, uint32_t count)
{
    if (count > destinations.size()) {
        return Result::PARAM_ERROR;
    }
    for (uint32_t index = 0; index < count; ++index) {
        Operation operation;
        operation.code = OpCode::LOAD_MEMORY;
        operation.dst = destinations[index].handle;
        operation.src0 = address.handle;
        operation.byteOffset = static_cast<uint64_t>(index) * sizeof(uint64_t);
        CompilerContext::Current().Append(operation);
    }
    return Result::SUCCESS;
}

Result Load(uint64_t address, Array<Variable>& destinations, uint32_t count)
{
    Variable addrVar;
    addrVar = address;
    return Load(addrVar, destinations, count);
}

Result Store(Variable address, Variable source)
{
    Operation operation;
    operation.code = OpCode::STORE_MEMORY;
    operation.dst = address.handle;
    operation.src0 = source.handle;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result Store(uint64_t address, Variable source)
{
    Variable addrVar;
    addrVar = address;
    return Store(addrVar, source);
}

Result WriteVariableWithNotify(
    ChannelHandle channel, Variable variable, uint32_t remoteVarIndex, uint32_t remoteNotifyIndex, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::WRITE_VARIABLE_NOTIFY;
    operation.channel = channel;
    operation.src0 = variable.handle;
    operation.index = remoteVarIndex;
    operation.immediate = remoteNotifyIndex;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result NotifyWait(ChannelHandle channel, uint32_t localNotifyIndex, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::NOTIFY_WAIT;
    operation.channel = channel;
    operation.index = localNotifyIndex;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result NotifyRecord(ChannelHandle channel, uint32_t remoteNotifyIndex, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::NOTIFY_RECORD;
    operation.channel = channel;
    operation.index = remoteNotifyIndex;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result EventRecord(Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::EVENT_RECORD;
    operation.dst = event.handle;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result EventWait(Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::EVENT_WAIT;
    operation.src0 = event.handle;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result Write(ChannelHandle channel, RemoteAddr remote, LocalAddr local, Variable length, Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::WRITE;
    operation.channel = channel;
    operation.dst = remote.handle;
    operation.src0 = local.handle;
    operation.src1 = length.handle;
    operation.immediate = event.handle;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result LocalCopy(LocalAddr destination, LocalAddr source, Variable length, Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::LOCAL_COPY;
    operation.dst = destination.handle;
    operation.src0 = source.handle;
    operation.src1 = length.handle;
    operation.immediate = event.handle;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result LocalCopy(CcuBuffer destination, LocalAddr source, Variable length, Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::LOCAL_COPY;
    operation.dst = destination.handle;
    operation.src0 = source.handle;
    operation.src1 = length.handle;
    operation.immediate = event.handle;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result LocalCopy(LocalAddr destination, CcuBuffer source, Variable length, Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::LOCAL_COPY;
    operation.dst = destination.handle;
    operation.src0 = source.handle;
    operation.src1 = length.handle;
    operation.immediate = event.handle;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

CcuBuffer::CcuBuffer() : handle(CompilerContext::Current().AllocateBuffer()) {}

namespace {
void AppendRead(
    OpCode code, ChannelHandle channel, ResourceHandle local, ResourceHandle remote, ResourceHandle length,
    ResourceHandle event, uint16_t mask)
{
    Operation operation;
    operation.code = code;
    operation.channel = channel;
    operation.dst = local;
    operation.src0 = remote;
    operation.src1 = length;
    operation.immediate = event;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
}
} // namespace

Result Read(ChannelHandle channel, LocalAddr local, RemoteAddr remote, Variable length, Event event, uint16_t mask)
{
    AppendRead(OpCode::READ, channel, local.handle, remote.handle, length.handle, event.handle, mask);
    return Result::SUCCESS;
}

Result Read(ChannelHandle channel, CcuBuffer local, RemoteAddr remote, Variable length, Event event, uint16_t mask)
{
    AppendRead(OpCode::READ, channel, local.handle, remote.handle, length.handle, event.handle, mask);
    return Result::SUCCESS;
}

Result ReadReduce(
    ChannelHandle channel, LocalAddr local, RemoteAddr remote, Variable length, HcclDataType dataType,
    HcclReduceOp opType, Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::READ_REDUCE;
    operation.channel = channel;
    operation.dst = local.handle;
    operation.src0 = remote.handle;
    operation.src1 = length.handle;
    operation.immediate = event.handle;
    operation.mask = mask;
    operation.dataType = dataType;
    operation.opType = opType;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result LocalReduce(
    LocalAddr dst, LocalAddr src, Variable len, HcclDataType dataType, HcclReduceOp opType, Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::LOCAL_REDUCE;
    operation.dst = dst.handle;
    operation.src0 = src.handle;
    operation.src1 = len.handle;
    operation.immediate = event.handle;
    operation.mask = mask;
    operation.dataType = dataType;
    operation.opType = opType;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result LocalReduce(
    CcuBuffer* buffers, uint32_t count, HcclDataType dataType, HcclDataType outputDataType, HcclReduceOp opType,
    Variable len, Event event, uint16_t mask)
{
    if (buffers == nullptr || count == 0) {
        return Result::PARAM_ERROR;
    }
    // hcomm semantics: reduce all buffers into buffers[0]
    for (uint32_t i = 1; i < count; ++i) {
        Operation operation;
        operation.code = OpCode::LOCAL_REDUCE;
        operation.dst = buffers[0].handle;
        operation.src0 = buffers[i].handle;
        operation.src1 = len.handle;
        operation.immediate = event.handle;
        operation.mask = mask;
        operation.dataType = dataType;
        operation.outputDataType = outputDataType;
        operation.opType = opType;
        CompilerContext::Current().Append(operation);
    }
    return Result::SUCCESS;
}

Result Write(ChannelHandle channel, RemoteAddr remote, CcuBuffer local, Variable length, Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::WRITE;
    operation.channel = channel;
    operation.dst = remote.handle;
    operation.src0 = local.handle;
    operation.src1 = length.handle;
    operation.immediate = event.handle;
    operation.mask = mask;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Result WriteReduce(
    ChannelHandle channel, RemoteAddr remote, LocalAddr local, Variable length, HcclDataType dataType,
    HcclReduceOp opType, Event event, uint16_t mask)
{
    Operation operation;
    operation.code = OpCode::WRITE_REDUCE;
    operation.channel = channel;
    operation.dst = remote.handle;
    operation.src0 = local.handle;
    operation.src1 = length.handle;
    operation.immediate = event.handle;
    operation.mask = mask;
    operation.dataType = dataType;
    operation.opType = opType;
    CompilerContext::Current().Append(operation);
    return Result::SUCCESS;
}

Loop::Loop() : handle(0) {}

Loop::Loop(Variable& loopParam, const Func& func)
{
    handle = CompilerContext::Current().CreateLoop();
    loopParamHandle = loopParam.handle;
    CompilerContext::Current().LoopBodyEnter(handle);
    func.RunBody();
    CompilerContext::Current().LoopBodyExit();
}

LoopGroup::LoopGroup() : handle(0) {}

LoopGroup::LoopGroup(Variable& parallelCfg, Variable& offsetCfg, uint32_t maxLoopNum, const std::vector<Loop>& loops)
{
    std::vector<ResourceHandle> loopHandles;
    std::vector<ResourceHandle> loopParamHandles;
    for (const auto& loop : loops) {
        loopHandles.push_back(loop.handle);
        loopParamHandles.push_back(loop.loopParamHandle);
    }
    handle = CompilerContext::Current().CreateLoopGroup(
        parallelCfg.handle, offsetCfg.handle, maxLoopNum, loopHandles, loopParamHandles);
}

} // namespace ccu
} // namespace AscendC
