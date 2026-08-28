/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef HCCL_CCU_PRIMITIVES_STUB_H
#define HCCL_CCU_PRIMITIVES_STUB_H

#include "ccu_st_types.h"

#include <functional>
#include <vector>
#include "hccl_types.h"

namespace HcclSim {
namespace CcuSt {

class CompilerContext {
public:
    explicit CompilerContext(Program& program);

    ResourceHandle AllocateVariable();
    ResourceHandle AllocateAddress();
    ResourceHandle AllocateEvent();
    ResourceHandle AllocateBuffer();
    ResourceHandle AllocateLocalAddress(ResourceHandle& address, ResourceHandle& token);
    ResourceHandle AllocateRemoteAddress(ResourceHandle& address, ResourceHandle& token);
    ResourceHandle GetChannelVariable(ChannelHandle channel, uint32_t index);
    void ConfigureProgram(uint32_t rankId, uint32_t rankSize, const std::vector<ChannelHandle>& channels);
    void RegisterArgument(ResourceHandle variable, uint32_t argId);
    void Append(const Operation& operation);
    void AppendGroupCopy(
        ResourceHandle destination, ResourceHandle source, ResourceHandle length,
        const std::array<ResourceHandle, 4>& groupSize);
    Result Finalize();

    ResourceHandle CreateLoop();
    void LoopBodyEnter(ResourceHandle loopHandle);
    void LoopBodyExit();
    std::vector<Operation>& GetLoopBody(ResourceHandle loopHandle);

    ResourceHandle CreateLoopGroup(
        ResourceHandle parallelCfg, ResourceHandle offsetCfg, uint32_t maxLoopNum,
        const std::vector<ResourceHandle>& loopHandles, const std::vector<ResourceHandle>& loopParamHandles);

    static CompilerContext& Current();
    static void SetCurrent(CompilerContext* context);

private:
    Program& program_;
    ResourceHandle nextHandle_{1};
    std::vector<Operation>* loopBodyTarget_{nullptr};
    std::map<ResourceHandle, std::vector<Operation>> loopBodies_;
};

struct Condition {
    ResourceHandle variable{0};
    uint64_t immediate{0};
    bool equal{true};
};

class ControlScope {
public:
    ControlScope(const Condition& condition, bool isWhile, bool isElse = false);
    bool Once();
    void Close();

private:
    bool entered_{false};
    bool closed_{false};
    bool isWhile_{false};
    bool isElse_{false};
};

} // namespace CcuSt
} // namespace HcclSim

namespace AscendC {
namespace ccu {

using HcclSim::CcuSt::ChannelHandle;
using HcclSim::CcuSt::Condition;
using HcclSim::CcuSt::ResourceHandle;
using HcclSim::CcuSt::Result;

class VariableExpr {
public:
    VariableExpr(ResourceHandle lhs, ResourceHandle rhs) : lhsHandle(lhs), rhsHandle(rhs) {}
    ResourceHandle LhsHandle() const { return lhsHandle; }
    ResourceHandle RhsHandle() const { return rhsHandle; }

private:
    ResourceHandle lhsHandle{0};
    ResourceHandle rhsHandle{0};
};

class Variable {
public:
    Variable();
    explicit Variable(ResourceHandle existingHandle);
    Variable(const Variable& other) = default;
    Variable(Variable&& other) noexcept = default;
    Variable& operator=(Variable&& other) noexcept = default;

    void operator=(const Variable& other) const;
    void operator=(const VariableExpr& expr) const;
    void operator=(uint64_t immediate) const;
    void operator+=(const Variable& other) const;
    VariableExpr operator+(const Variable& other) const;
    Condition operator==(uint64_t immediate) const;
    Condition operator!=(uint64_t immediate) const;

    ResourceHandle handle{0};

private:
    template <typename T>
    friend T GetResByChannel(ChannelHandle channel, uint32_t index);
};

class AddressExpr {
public:
    AddressExpr(ResourceHandle lhs, ResourceHandle rhs, bool lhsIsAddr)
        : lhsHandle(lhs), rhsHandle(rhs), lhsIsAddress(lhsIsAddr)
    {}
    ResourceHandle LhsHandle() const { return lhsHandle; }
    ResourceHandle RhsHandle() const { return rhsHandle; }
    bool LhsIsAddress() const { return lhsIsAddress; }

private:
    ResourceHandle lhsHandle{0};
    ResourceHandle rhsHandle{0};
    bool lhsIsAddress{true};
};

class Address {
public:
    Address();
    explicit Address(ResourceHandle existingHandle);
    Address(const Address& other) = default;
    Address(Address&& other) noexcept = default;
    Address& operator=(Address&& other) noexcept = default;

    void operator=(const Variable& variable) const;
    void operator=(const Address& address) const;
    void operator=(const AddressExpr& expr) const;
    void operator+=(const Variable& variable) const;
    void operator+=(const Address& other) const;
    AddressExpr operator+(const Address& other) const;
    AddressExpr operator+(const Variable& variable) const;

    ResourceHandle handle{0};

private:
    friend class LocalAddr;
    friend class RemoteAddr;
};

inline AddressExpr operator+(const Variable& var, const Address& addr)
{
    return AddressExpr(var.handle, addr.handle, false);
}

class LocalAddr {
public:
    LocalAddr();
    LocalAddr(const LocalAddr& other) = default;
    LocalAddr(LocalAddr&& other) noexcept = default;
    LocalAddr& operator=(LocalAddr&& other) noexcept = default;

    Address addr;
    Variable token;
    ResourceHandle handle{0};

private:
    LocalAddr(ResourceHandle localHandle, ResourceHandle addressHandle, ResourceHandle tokenHandle);
};

class RemoteAddr {
public:
    RemoteAddr();
    RemoteAddr(const RemoteAddr& other) = default;
    RemoteAddr(RemoteAddr&& other) noexcept = default;
    RemoteAddr& operator=(RemoteAddr&& other) noexcept = default;

    Address addr;
    Variable token;
    ResourceHandle handle{0};

private:
    RemoteAddr(ResourceHandle remoteHandle, ResourceHandle addressHandle, ResourceHandle tokenHandle);
};

class Event {
public:
    Event();
    ResourceHandle handle{0};
};

class CcuBuffer;

class Func {
public:
    template <typename Callable>
    explicit Func(Callable&& fn) : body_(std::forward<Callable>(fn))
    {}

    void RunBody() const
    {
        if (body_)
            body_();
    }

private:
    std::function<void()> body_;
};

template <typename T>
class Array {
public:
    explicit Array(uint32_t count) : values_(count) {}

    T& operator[](uint32_t index) { return values_[index]; }
    const T& operator[](uint32_t index) const { return values_[index]; }
    uint32_t size() const { return static_cast<uint32_t>(values_.size()); }

private:
    std::vector<T> values_;
};

template <typename T>
T GetResByChannel(ChannelHandle channel, uint32_t index) = delete;

template <>
Variable GetResByChannel<Variable>(ChannelHandle channel, uint32_t index);
Result LoadArg(Variable variable, uint32_t argId);
Result Load(Variable address, Variable destination);
Result Load(Variable address, Array<Variable>& destinations, uint32_t count);
Result Load(uint64_t address, Variable destination);
Result Load(uint64_t address, Array<Variable>& destinations, uint32_t count);
Result Store(Variable address, Variable source);
Result Store(uint64_t address, Variable source);
Result WriteVariableWithNotify(
    ChannelHandle channel, Variable variable, uint32_t remoteVarIndex, uint32_t remoteNotifyIndex, uint16_t mask = 1);
Result NotifyWait(ChannelHandle channel, uint32_t localNotifyIndex, uint16_t mask = 1);
Result NotifyRecord(ChannelHandle channel, uint32_t remoteNotifyIndex, uint16_t mask = 1);
Result EventRecord(Event event, uint16_t mask = 1);
Result EventWait(Event event, uint16_t mask = 1);
Result Write(
    ChannelHandle channel, RemoteAddr remote, LocalAddr local, Variable length, Event event, uint16_t mask = 1);
Result LocalCopy(LocalAddr destination, LocalAddr source, Variable length, Event event, uint16_t mask = 1);
Result LocalCopy(CcuBuffer destination, LocalAddr source, Variable length, Event event, uint16_t mask = 1);
Result LocalCopy(LocalAddr destination, CcuBuffer source, Variable length, Event event, uint16_t mask = 1);
Result Read(ChannelHandle channel, LocalAddr local, RemoteAddr remote, Variable length, Event event, uint16_t mask = 1);
Result Read(ChannelHandle channel, CcuBuffer local, RemoteAddr remote, Variable length, Event event, uint16_t mask = 1);
Result ReadReduce(
    ChannelHandle channel, LocalAddr local, RemoteAddr remote, Variable length, HcclDataType dataType,
    HcclReduceOp opType, Event event, uint16_t mask = 1);
Result LocalReduce(
    LocalAddr dst, LocalAddr src, Variable len, HcclDataType dataType, HcclReduceOp opType, Event event,
    uint16_t mask = 1);
Result LocalReduce(
    CcuBuffer* buffers, uint32_t count, HcclDataType dataType, HcclDataType outputDataType, HcclReduceOp opType,
    Variable len, Event event, uint16_t mask = 1);
Result Write(
    ChannelHandle channel, RemoteAddr remote, CcuBuffer local, Variable length, Event event, uint16_t mask = 1);
Result WriteReduce(
    ChannelHandle channel, RemoteAddr remote, LocalAddr local, Variable length, HcclDataType dataType,
    HcclReduceOp opType, Event event, uint16_t mask = 1);

} // namespace ccu
} // namespace AscendC

#define CCU_IF(expr) \
    for (::HcclSim::CcuSt::ControlScope _ccuIfScope((expr), false); _ccuIfScope.Once(); _ccuIfScope.Close())

#define CCU_WHILE(expr) \
    for (::HcclSim::CcuSt::ControlScope _ccuWhileScope((expr), true); _ccuWhileScope.Once(); _ccuWhileScope.Close())

#define CCU_ELSE                                                                                             \
    for (::HcclSim::CcuSt::ControlScope _ccuElseScope(::HcclSim::CcuSt::Condition{0, 0, true}, false, true); \
         _ccuElseScope.Once(); _ccuElseScope.Close())

// Merged from shim/ccu_primitives_dl.hpp — types with IR capture support
namespace AscendC {
namespace ccu {
class CcuBuffer {
public:
    CcuBuffer();
    explicit CcuBuffer(ResourceHandle h) : handle(h) {}
    ResourceHandle handle{0};
};

class Loop {
public:
    Loop();
    Loop(Variable& loopParam, const Func& func);
    Loop(const Loop&) = default;
    ResourceHandle handle{0};
    ResourceHandle loopParamHandle{0};
};

class LoopGroup {
public:
    LoopGroup();
    LoopGroup(Variable& parallelCfg, Variable& offsetCfg, uint32_t maxLoopNum, const std::vector<Loop>& loops);
    ResourceHandle handle{0};
};
} // namespace ccu
} // namespace AscendC

#endif // HCCL_CCU_PRIMITIVES_STUB_H
