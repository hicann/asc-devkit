/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_VARIABLE_DL_HPP
#define CCU_VARIABLE_DL_HPP

#include <cstdint>
#include <type_traits>
#include "ccu_types_dl.h"
#include "ccu_utils_dl.hpp"
#include "ccu_primitives_impl_dl.h"

namespace AscendC {
namespace ccu {

class Variable;
class Address;
class LocalAddr;
class RemoteAddr;
template <typename U>
class Array;
template <typename T>
T GetResByChannel(ChannelHandle channel, uint32_t index);

struct CondExpr {
    Variable* var;
    Variable* rhsVar;
    uint64_t imm;
    CcuConditionType cond;
    bool isVarCompare;
};

class Variable final {
public:
    Variable() { CCU_THROW_IF_FAILED(CcuVariableAlloc(&this->handle), "CcuVariableAlloc: failed"); }

    Variable(const Variable& other) { this->handle = other.handle; }

    Variable(Variable&& other) noexcept { this->handle = other.handle; }

    void operator=(const Variable& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableAssignVar(this->handle, other.handle),
            "Variable::operator=(Variable): CcuVariableAssignVar failed");
    }

    void operator=(Variable&& other) { this->handle = other.handle; }

    void operator=(uint64_t immediate) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableAssignImm(this->handle, immediate),
            "Variable::operator=(uint64_t): CcuVariableAssignImm failed");
    }

    void operator=(detail::CcuArithmeticOperator<Variable, Variable> op) const
    {
        switch (op.type) {
            case detail::CcuArithmeticOperatorType::ADDITION:
                CCU_THROW_IF_FAILED(
                    CcuVariableAddVarToVar(this->handle, op.lhs.handle, op.rhs.handle),
                    "Variable::operator=(Var+Var): CcuVariableAddVarToVar failed");
                break;
            case detail::CcuArithmeticOperatorType::SUBTRACTION:
                CCU_THROW_IF_FAILED(
                    CcuVariableSubVarToVar(this->handle, op.lhs.handle, op.rhs.handle),
                    "Variable::operator=(Var-Var): CcuVariableSubVarToVar failed");
                break;
            case detail::CcuArithmeticOperatorType::MULTIPLICATION:
                CCU_THROW_IF_FAILED(
                    CcuVariableMulVarToVar(this->handle, op.lhs.handle, op.rhs.handle),
                    "Variable::operator=(Var*Var): CcuVariableMulVarToVar failed");
                break;
            default:
                throw detail::CcuException(
                    CcuResult::CCU_E_PARA, "Variable::operator=: invalid arithmetic operator type");
        }
    }

    void operator=(detail::CcuArithmeticOperator<Variable, uint16_t> op) const
    {
        switch (op.type) {
            case detail::CcuArithmeticOperatorType::ADDITION:
                CCU_THROW_IF_FAILED(
                    CcuVariableAddImmToVar(this->handle, op.lhs.handle, op.rhs),
                    "Variable::operator=(Var+Imm): CcuVariableAddImmToVar failed");
                break;
            case detail::CcuArithmeticOperatorType::SUBTRACTION:
                CCU_THROW_IF_FAILED(
                    CcuVariableSubImmToVar(this->handle, op.lhs.handle, op.rhs),
                    "Variable::operator=(Var-Imm): CcuVariableSubImmToVar failed");
                break;
            case detail::CcuArithmeticOperatorType::MULTIPLICATION:
                CCU_THROW_IF_FAILED(
                    CcuVariableMulImmToVar(this->handle, op.lhs.handle, op.rhs),
                    "Variable::operator=(Var*Imm): CcuVariableMulImmToVar failed");
                break;
            default:
                throw detail::CcuException(
                    CcuResult::CCU_E_PARA, "Variable::operator=: invalid arithmetic operator type");
        }
    }

    void operator+=(const Variable& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableAddVarToVar(this->handle, this->handle, other.handle),
            "Variable::operator+=(Variable): CcuVariableAddVarToVar failed");
    }

    void operator-=(const Variable& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableSubVarToVar(this->handle, this->handle, other.handle),
            "Variable::operator-=(Variable): CcuVariableSubVarToVar failed");
    }

    void operator*=(const Variable& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableMulVarToVar(this->handle, this->handle, other.handle),
            "Variable::operator*=(Variable): CcuVariableMulVarToVar failed");
    }

    void operator+=(uint16_t immediate) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableAddImmToVar(this->handle, this->handle, immediate),
            "Variable::operator+=(uint16_t): CcuVariableAddImmToVar failed");
    }

    void operator-=(uint16_t immediate) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableSubImmToVar(this->handle, this->handle, immediate),
            "Variable::operator-=(uint16_t): CcuVariableSubImmToVar failed");
    }

    void operator*=(uint16_t immediate) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableMulImmToVar(this->handle, this->handle, immediate),
            "Variable::operator*=(uint16_t): CcuVariableMulImmToVar failed");
    }

    detail::CcuArithmeticOperator<Variable, Variable> operator+(const Variable& that) const
    {
        return detail::CcuArithmeticOperator<Variable, Variable>(
            *this, that, detail::CcuArithmeticOperatorType::ADDITION);
    }

    detail::CcuArithmeticOperator<Variable, Variable> operator-(const Variable& that) const
    {
        return detail::CcuArithmeticOperator<Variable, Variable>(
            *this, that, detail::CcuArithmeticOperatorType::SUBTRACTION);
    }

    detail::CcuArithmeticOperator<Variable, Variable> operator*(const Variable& that) const
    {
        return detail::CcuArithmeticOperator<Variable, Variable>(
            *this, that, detail::CcuArithmeticOperatorType::MULTIPLICATION);
    }

    detail::CcuArithmeticOperator<Variable, uint16_t> operator+(uint16_t immediate) const
    {
        return detail::CcuArithmeticOperator<Variable, uint16_t>(
            *this, immediate, detail::CcuArithmeticOperatorType::ADDITION);
    }

    detail::CcuArithmeticOperator<Variable, uint16_t> operator-(uint16_t immediate) const
    {
        return detail::CcuArithmeticOperator<Variable, uint16_t>(
            *this, immediate, detail::CcuArithmeticOperatorType::SUBTRACTION);
    }

    detail::CcuArithmeticOperator<Variable, uint16_t> operator*(uint16_t immediate) const
    {
        return detail::CcuArithmeticOperator<Variable, uint16_t>(
            *this, immediate, detail::CcuArithmeticOperatorType::MULTIPLICATION);
    }

    void operator=(detail::CcuLogicOperator<Variable, Variable> op) const
    {
        switch (op.type) {
            case detail::CcuLogicOperatorType::AND:
                CCU_THROW_IF_FAILED(
                    CcuVariableAndVarToVar(this->handle, op.lhs.handle, op.rhs.handle),
                    "Variable::operator=(Var&Var): CcuVariableAndVarToVar failed");
                break;
            case detail::CcuLogicOperatorType::OR:
                CCU_THROW_IF_FAILED(
                    CcuVariableOrVarToVar(this->handle, op.lhs.handle, op.rhs.handle),
                    "Variable::operator=(Var|Var): CcuVariableOrVarToVar failed");
                break;
            case detail::CcuLogicOperatorType::XOR:
                CCU_THROW_IF_FAILED(
                    CcuVariableXorVarToVar(this->handle, op.lhs.handle, op.rhs.handle),
                    "Variable::operator=(Var^Var): CcuVariableXorVarToVar failed");
                break;
            default:
                throw detail::CcuException(CcuResult::CCU_E_PARA, "Variable::operator=: invalid logic operator type");
        }
    }

    void operator=(detail::CcuLogicUnaryOperator<Variable> op) const
    {
        switch (op.type) {
            case detail::CcuLogicOperatorType::NOT:
                CCU_THROW_IF_FAILED(
                    CcuVariableNotVar(this->handle, op.lhs.handle),
                    "Variable::operator=(~Var): CcuVariableNotVar failed");
                break;
            default:
                throw detail::CcuException(
                    CcuResult::CCU_E_PARA, "Variable::operator=: invalid unary logic operator type");
        }
    }

    detail::CcuLogicOperator<Variable, Variable> operator&(const Variable& that) const
    {
        return detail::CcuLogicOperator<Variable, Variable>(*this, that, detail::CcuLogicOperatorType::AND);
    }

    detail::CcuLogicOperator<Variable, Variable> operator|(const Variable& that) const
    {
        return detail::CcuLogicOperator<Variable, Variable>(*this, that, detail::CcuLogicOperatorType::OR);
    }

    detail::CcuLogicOperator<Variable, Variable> operator^(const Variable& that) const
    {
        return detail::CcuLogicOperator<Variable, Variable>(*this, that, detail::CcuLogicOperatorType::XOR);
    }

    detail::CcuLogicUnaryOperator<Variable> operator~() const
    {
        return detail::CcuLogicUnaryOperator<Variable>(*this, detail::CcuLogicOperatorType::NOT);
    }

    void operator&=(const Variable& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableAndVarToVar(this->handle, this->handle, other.handle),
            "Variable::operator&=(Variable): CcuVariableAndVarToVar failed");
    }

    void operator|=(const Variable& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableOrVarToVar(this->handle, this->handle, other.handle),
            "Variable::operator|=(Variable): CcuVariableOrVarToVar failed");
    }

    void operator^=(const Variable& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableXorVarToVar(this->handle, this->handle, other.handle),
            "Variable::operator^=(Variable): CcuVariableXorVarToVar failed");
    }

    CondExpr operator==(uint64_t immediate) { return CondExpr{this, nullptr, immediate, CCU_CONDITION_EQ, false}; }

    CondExpr operator!=(uint64_t immediate) { return CondExpr{this, nullptr, immediate, CCU_CONDITION_NE, false}; }

    CondExpr operator<(uint64_t immediate) { return CondExpr{this, nullptr, immediate, CCU_CONDITION_LT, false}; }

    CondExpr operator<=(uint64_t immediate) { return CondExpr{this, nullptr, immediate, CCU_CONDITION_LE, false}; }

    CondExpr operator>(uint64_t immediate) { return CondExpr{this, nullptr, immediate, CCU_CONDITION_GT, false}; }

    CondExpr operator>=(uint64_t immediate) { return CondExpr{this, nullptr, immediate, CCU_CONDITION_GE, false}; }

    CondExpr operator==(Variable& other) { return CondExpr{this, &other, 0, CCU_CONDITION_EQ, true}; }

    CondExpr operator!=(Variable& other) { return CondExpr{this, &other, 0, CCU_CONDITION_NE, true}; }

    CondExpr operator<(Variable& other) { return CondExpr{this, &other, 0, CCU_CONDITION_LT, true}; }

    CondExpr operator<=(Variable& other) { return CondExpr{this, &other, 0, CCU_CONDITION_LE, true}; }

    CondExpr operator>(Variable& other) { return CondExpr{this, &other, 0, CCU_CONDITION_GT, true}; }

    CondExpr operator>=(Variable& other) { return CondExpr{this, &other, 0, CCU_CONDITION_GE, true}; }

    CcuVariableHandle handle{0};

private:
    explicit Variable(detail::NoAllocTag) {}
    template <typename U>
    friend class Array;
    friend class LocalAddr;
    friend class RemoteAddr;
    friend class Address;
    template <typename T>
    friend T GetResByChannel(ChannelHandle channel, uint32_t index);
};

} // namespace ccu
} // namespace AscendC

template <>
inline void AscendC::ccu::detail::CcuArithmeticOperator<AscendC::ccu::Variable, AscendC::ccu::Variable>::Check() const
{}

template <>
inline void AscendC::ccu::detail::CcuArithmeticOperator<AscendC::ccu::Variable, uint16_t>::Check() const
{}

template <>
inline void AscendC::ccu::detail::CcuLogicOperator<AscendC::ccu::Variable, AscendC::ccu::Variable>::Check() const
{}

template <>
inline void AscendC::ccu::detail::CcuLogicUnaryOperator<AscendC::ccu::Variable>::Check() const
{}

#endif // CCU_VARIABLE_DL_HPP
