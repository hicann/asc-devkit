/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_LOCAL_ADDR_DL_HPP
#define CCU_LOCAL_ADDR_DL_HPP

#if CANN_VERSION_NUM >= 90100000
#include "ccu_local_addr.hpp"
#else
#include <type_traits>
#include "ccu_types_dl.h"
#include "ccu_variable_dl.hpp"
#include "ccu_address_dl.hpp"

namespace AscendC {
namespace ccu {

template <typename U>
class Array;

class LocalAddr final {
public:
    LocalAddr() : addr(detail::NoAllocTag{}), token(detail::NoAllocTag{})
    {
        CCU_THROW_IF_FAILED(
            CcuLocalAddrAlloc(&this->handle, &this->addr.handle, &this->token.handle), "CcuLocalAddrAlloc: failed");
    }

    LocalAddr(const LocalAddr& other) : addr(detail::NoAllocTag{}), token(detail::NoAllocTag{})
    {
        this->handle = other.handle;
        this->addr.handle = other.addr.handle;
        this->token.handle = other.token.handle;
    }
    LocalAddr(LocalAddr&& other) noexcept : addr(detail::NoAllocTag{}), token(detail::NoAllocTag{})
    {
        this->handle = other.handle;
        this->addr.handle = other.addr.handle;
        this->token.handle = other.token.handle;
    }
    void operator=(const LocalAddr& other)
    {
        this->addr = other.addr;
        this->token = other.token;
    }
    void operator=(LocalAddr&& other)
    {
        this->handle = other.handle;
        this->addr.handle = other.addr.handle;
        this->token.handle = other.token.handle;
    }

    Address addr;
    Variable token;
    CcuLocalAddrHandle handle{0};

private:
    explicit LocalAddr(detail::NoAllocTag) : addr(detail::NoAllocTag{}), token(detail::NoAllocTag{}) {}
    template <typename U>
    friend class Array;
};

} // namespace ccu
} // namespace AscendC

#endif // CANN_VERSION_NUM >= 90100000
#endif // CCU_LOCAL_ADDR_DL_HPP
