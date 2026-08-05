/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_UTILS_DL_HPP
#define CCU_UTILS_DL_HPP

#if CANN_VERSION_NUM >= 90100000
#include "ccu_utils.hpp"
#else
#include <exception>
#include <string>
#include "ccu_types_dl.h"

namespace AscendC {
namespace ccu {
namespace detail {

struct NoAllocTag {};

class CcuException : public std::exception {
public:
    CcuException(CcuResult code, const char* what) : code_(code), what_(BuildMessage(code, what)) {}

    const char* what() const noexcept override { return what_.c_str(); }
    CcuResult code() const noexcept { return code_; }

private:
    static std::string BuildMessage(CcuResult code, const char* what)
    {
        std::string msg = "[ccu] ";
        msg += (what != nullptr) ? what : "(unknown)";
        msg += " (CcuResult=";
        msg += std::to_string(static_cast<int>(code));
        msg += ")";
        return msg;
    }

    CcuResult code_;
    std::string what_;
};
enum class CcuArithmeticOperatorType { ADDITION, INVALID };

template <typename lhsT, typename rhsT>
class CcuOperator {
public:
    CcuOperator(lhsT lhs, rhsT rhs) : lhs(lhs), rhs(rhs) {}
    lhsT lhs;
    rhsT rhs;
};

template <typename lhsT, typename rhsT>
class CcuArithmeticOperator : public CcuOperator<lhsT, rhsT> {
public:
    CcuArithmeticOperator(lhsT lhs, rhsT rhs, CcuArithmeticOperatorType type)
        : CcuOperator<lhsT, rhsT>(lhs, rhs), type(type)
    {
        Check();
    }
    void Check() const
    {
        // 默认通用模板：未特化的 (lhsT, rhsT) 组合属于编译期允许、运行期非法。
        // 抛 CcuException 而非字符串字面量，可被 `catch (const std::exception&)` 捕获。
        throw ::AscendC::ccu::detail::CcuException(
            CcuResult::CCU_E_PARA, "CcuArithmeticOperator: invalid operand types");
    }

    CcuArithmeticOperatorType type{CcuArithmeticOperatorType::INVALID};
};

} // namespace detail
} // namespace ccu
} // namespace AscendC

#define CCU_THROW_IF_FAILED(ret, msg)                                    \
    do {                                                                 \
        auto _ccu_ret = (ret);                                           \
        if (_ccu_ret != CcuResult::CCU_SUCCESS) {                        \
            throw ::AscendC::ccu::detail::CcuException(_ccu_ret, (msg)); \
        }                                                                \
    } while (0)

#endif // CANN_VERSION_NUM >= 90100000
#endif // CCU_UTILS_DL_HPP
