/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_ADDRESS_DL_HPP
#define CCU_ADDRESS_DL_HPP

#if CANN_VERSION_NUM >= 90100000
#include "ccu_address.hpp"
#else
#include <cstdint>
#include <type_traits>

#include "ccu_types_dl.h"
#include "ccu_utils_dl.hpp"
#include "ccu_primitives_impl_dl.h"
#include "ccu_variable_dl.hpp"

namespace AscendC {
namespace ccu {

class LocalAddr;
class RemoteAddr;
template <typename U>
class Array;

class Address final {
public:
    Address() { CCU_THROW_IF_FAILED(CcuAddressAlloc(&this->handle), "CcuAddressAlloc: failed"); }

    Address(const Address& other) { this->handle = other.handle; }

    Address(Address&& other) noexcept { this->handle = other.handle; }

    void operator=(const Address& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuAddressAssignAddr(this->handle, other.handle),
            "Address::operator=(Address): CcuAddressAssignAddr failed");
    }

    void operator=(Address&& other) { this->handle = other.handle; }

    void operator=(uint64_t immediate) const
    {
        CCU_THROW_IF_FAILED(
            CcuAddressAssignImm(this->handle, immediate), "Address::operator=(uint64_t): CcuAddressAssignImm failed");
    }

    void operator=(const Variable& var) const
    {
        CCU_THROW_IF_FAILED(
            CcuAddressAssignVar(this->handle, var.handle), "Address::operator=(Variable): CcuAddressAssignVar failed");
    }

    void operator=(detail::CcuArithmeticOperator<Address, Address> op) const
    {
        CCU_THROW_IF_FAILED(
            CcuAddressAddAddrToAddr(this->handle, op.lhs.handle, op.rhs.handle),
            "Address::operator=(Addr+Addr): CcuAddressAddAddrToAddr failed");
    }

    void operator=(detail::CcuArithmeticOperator<Address, Variable> op) const
    {
        CCU_THROW_IF_FAILED(
            CcuAddressAddVarToAddr(this->handle, op.lhs.handle, op.rhs.handle),
            "Address::operator=(Addr+Var): CcuAddressAddVarToAddr failed");
    }

    void operator=(detail::CcuArithmeticOperator<Variable, Address> op) const
    {
        CCU_THROW_IF_FAILED(
            CcuAddressAddVarToAddr(this->handle, op.rhs.handle, op.lhs.handle),
            "Address::operator=(Var+Addr): CcuAddressAddVarToAddr failed");
    }

    // addr + addr
    detail::CcuArithmeticOperator<Address, Address> operator+(const Address& that) const
    {
        return detail::CcuArithmeticOperator<Address, Address>(
            *this, that, detail::CcuArithmeticOperatorType::ADDITION);
    }

    // addr + variable
    detail::CcuArithmeticOperator<Address, Variable> operator+(const Variable& var) const
    {
        return detail::CcuArithmeticOperator<Address, Variable>(
            *this, var, detail::CcuArithmeticOperatorType::ADDITION);
    }

    void operator+=(const Variable& var) const
    {
        CCU_THROW_IF_FAILED(
            CcuAddressAddAssignVar(this->handle, var.handle),
            "Address::operator+=(Variable): CcuAddressAddAssignVar failed");
    }

    // addr += addr
    void operator+=(const Address& other) const
    {
        CCU_THROW_IF_FAILED(
            CcuAddressAddAddrToAddr(this->handle, this->handle, other.handle),
            "Address::operator+=(Address): CcuAddressAddAddrToAddr failed");
    }

    CcuAddressHandle handle{0};

private:
    explicit Address(detail::NoAllocTag) {}
    template <typename U>
    friend class Array;
    friend class LocalAddr;
    friend class RemoteAddr;
};

// variable + addr（交换律）
inline detail::CcuArithmeticOperator<Variable, Address> operator+(const Variable& var, const Address& addr)
{
    return detail::CcuArithmeticOperator<Variable, Address>(var, addr, detail::CcuArithmeticOperatorType::ADDITION);
}

} // namespace ccu
} // namespace AscendC

template <>
inline void AscendC::ccu::detail::CcuArithmeticOperator<AscendC::ccu::Address, AscendC::ccu::Address>::Check() const
{}
template <>
inline void AscendC::ccu::detail::CcuArithmeticOperator<AscendC::ccu::Address, AscendC::ccu::Variable>::Check() const
{}
template <>
inline void AscendC::ccu::detail::CcuArithmeticOperator<AscendC::ccu::Variable, AscendC::ccu::Address>::Check() const
{}
#endif // CANN_VERSION_NUM >= 90100000

#endif // CCU_ADDRESS_DL_HPP
