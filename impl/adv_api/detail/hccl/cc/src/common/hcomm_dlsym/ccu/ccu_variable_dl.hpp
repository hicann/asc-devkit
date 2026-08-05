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

#if CANN_VERSION_NUM >= 90100000
#include "ccu_variable.hpp"
#else
#include <cstdint>
#include <type_traits>
#include "ccu_types_dl.h"
#include "ccu_utils_dl.hpp"
#include "ccu_primitives_impl_dl.h"

namespace AscendC {
namespace ccu {

class Variable;
class LocalAddr;
class RemoteAddr;
template <typename U>
class Array;
template <typename T>
T GetResByChannel(ChannelHandle channel, uint32_t index);

struct CondExpr {
    Variable* var;
    uint64_t imm;
    CcuConditionType cond;
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
        CCU_THROW_IF_FAILED(
            CcuVariableAddVarToVar(this->handle, op.lhs.handle, op.rhs.handle),
            "Variable::operator=(Var+Var): CcuVariableAddVarToVar failed");
    }

    void operator+=(const Variable& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuVariableAddVarToVar(this->handle, this->handle, other.handle),
            "Variable::operator+=(Variable): CcuVariableAddVarToVar failed");
    }

    detail::CcuArithmeticOperator<Variable, Variable> operator+(const Variable& that) const
    {
        return detail::CcuArithmeticOperator<Variable, Variable>(
            *this, that, detail::CcuArithmeticOperatorType::ADDITION);
    }

    CondExpr operator==(uint64_t immediate) { return CondExpr{this, immediate, CCU_CONDITION_EQ}; }

    CondExpr operator!=(uint64_t immediate) { return CondExpr{this, immediate, CCU_CONDITION_NE}; }

    CcuVariableHandle handle{0};

private:
    explicit Variable(detail::NoAllocTag) {}
    template <typename U>
    friend class Array;
    friend class LocalAddr;
    friend class RemoteAddr;
    template <typename T>
    friend T GetResByChannel(ChannelHandle channel, uint32_t index);
};

} // namespace ccu
} // namespace AscendC

template <>
inline void AscendC::ccu::detail::CcuArithmeticOperator<AscendC::ccu::Variable, AscendC::ccu::Variable>::Check() const
{}

#endif // CANN_VERSION_NUM >= 90100000
#endif // CCU_VARIABLE_DL_HPP
