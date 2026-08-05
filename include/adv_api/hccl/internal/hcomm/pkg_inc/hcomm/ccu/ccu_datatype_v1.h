/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCOMM_CCU_DATATYPE_H
#define HCOMM_CCU_DATATYPE_H

#include <memory>

#include "ccu_rep_base_v1.h"
#include "ccu_operator_v1.h"
#include "ccu_rep_context_v1.h"

namespace hcomm {
namespace CcuRep {

class Variable;
class Address;

class CcuPhyRes {
public:
    CcuPhyRes() = default;
    ~CcuPhyRes() = default;
    void Reset(uint16_t id);
    void SetDieId(uint16_t dieId);
    uint16_t Id() const;
    uint16_t DieId() const;

private:
    uint16_t dieId{0};
    uint16_t id{0};
};

class CcuVirRes {
public:
    CcuVirRes(CcuRepContext* context);
    virtual ~CcuVirRes() = default;
    void Reset(uint16_t id);
    void Reset(uint16_t id, uint16_t dieId);
    void SetDieId(uint16_t dieId);
    virtual uint16_t Id() const;
    uint16_t DieId() const;

protected:
    std::shared_ptr<CcuPhyRes> phyRes{nullptr};
    CcuRepContext* context{nullptr};
};

class Variable : public CcuVirRes {
public:
    explicit Variable(CcuRepContext* context = nullptr);
    Variable(const Variable& other);
    void operator=(Variable&& other);

    void operator=(uint64_t immediate);
    void operator=(const Variable& other);
    void operator=(CcuArithmeticOperator<Variable, Variable> op);

    CcuArithmeticOperator<Variable, Variable> operator+(const Variable& varB) const;
    CcuArithmeticOperator<Variable, Address> operator+(const Address& addrB) const;

    void operator+=(const Variable& other);

    CcuRelationalOperator<Variable, uint64_t> operator!=(uint64_t immediate) const;
    CcuRelationalOperator<Variable, uint64_t> operator==(uint64_t immediate) const;
};

class Address : public CcuVirRes {
public:
    explicit Address(CcuRepContext* context = nullptr);
    Address(const Address& other);
    void operator=(Address&& other);

    void operator=(uint64_t immediate);
    void operator=(const Address& other);
    void operator=(const Variable& other);

    void operator=(CcuArithmeticOperator<Variable, Address> op);
    void operator=(CcuArithmeticOperator<Address, Variable> op);
    void operator=(CcuArithmeticOperator<Address, Address> op);

    CcuArithmeticOperator<Variable, Address> operator+(const Variable& varB) const;
    CcuArithmeticOperator<Address, Address> operator+(const Address& addrB) const;

    void operator+=(const Variable& other);
};

class MaskSignal : public CcuVirRes {
public:
    explicit MaskSignal(CcuRepContext* context = nullptr);
};

class CcuBuffer : public CcuVirRes {
public:
    explicit CcuBuffer(CcuRepContext* context = nullptr);
    uint16_t Id() const override;
    static constexpr uint16_t CCUBUFFER_DIE_ID_BIT = 0x8000; // bit15 表示MS所在的IO Die id
};

class CcuBuf : public CcuVirRes {
public:
    explicit CcuBuf(CcuRepContext* context = nullptr);
    uint16_t Id() const override;
    static constexpr uint16_t CCUBUFFER_DIE_ID_BIT = 0x8000; // bit15 表示MS所在的IO Die id
};

class Executor : public CcuVirRes {
public:
    explicit Executor(CcuRepContext* context = nullptr);
};

class Memory {
public:
    Memory() = default;
    Memory(Address addr, Variable token) : addr(addr), token(token) {}
    Address addr;
    Variable token;
};

/*------------------将Memory改为LocalAddr与RemoteAddr------------------------*/
class LocalAddr {
public:
    LocalAddr() = default;
    LocalAddr(Address addr, Variable token) : addr(addr), token(token) {}

    Address addr;
    Variable token;
};

class RemoteAddr {
public:
    RemoteAddr() = default;
    RemoteAddr(Address addr, Variable token) : addr(addr), token(token) {}

    Address addr;
    Variable token;
};

/*------------------将MaskSignal改为LocalNotify------------------------*/
class LocalNotify : public CcuVirRes {
public:
    explicit LocalNotify(CcuRepContext* context = nullptr);
};

class CompletedEvent : public CcuVirRes {
public:
    explicit CompletedEvent(CcuRepContext* context = nullptr);
    void SetMask(uint32_t compeletedMask);
    uint32_t mask{1};
};

// CcuPhyRes implementations
inline uint16_t CcuPhyRes::Id() const { return id; }
inline uint16_t CcuPhyRes::DieId() const { return dieId; }
inline void CcuPhyRes::Reset(uint16_t id) { this->id = id; }
inline void CcuPhyRes::SetDieId(uint16_t dieId) { this->dieId = dieId; }

// CcuVirRes implementations
inline CcuVirRes::CcuVirRes(CcuRepContext* context) : context(context) { phyRes = std::make_shared<CcuPhyRes>(); }

inline void CcuVirRes::Reset(uint16_t id) { phyRes->Reset(id); }

inline void CcuVirRes::Reset(uint16_t id, uint16_t dieId)
{
    phyRes->Reset(id);
    phyRes->SetDieId(dieId);
}

inline void CcuVirRes::SetDieId(uint16_t dieId) { phyRes->SetDieId(dieId); }

inline uint16_t CcuVirRes::Id() const { return phyRes->Id(); }

inline uint16_t CcuVirRes::DieId() const { return phyRes->DieId(); }

// MaskSignal implementation
inline MaskSignal::MaskSignal(CcuRepContext* context) : CcuVirRes(context) {}

}; // namespace CcuRep
}; // namespace hcomm
#endif // HCOMM_CCU_DATATYPE_H
