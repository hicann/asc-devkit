/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_BUFFER_DL_HPP
#define CCU_BUFFER_DL_HPP

#if CANN_VERSION_NUM >= 90100000
#include "ccu_buffer.hpp"
#else
#include <cstdint>
#include <type_traits>
#include "ccu_types_dl.h"
#include "ccu_primitives_impl_dl.h"
#include "ccu_utils_dl.hpp"

namespace AscendC {
namespace ccu {

template <typename U>
class Array;

class CcuBuffer final {
public:
    CcuBuffer() { CCU_THROW_IF_FAILED(CcuBufferAlloc(&this->handle), "CcuBufferAlloc: failed"); }

    CcuBuffer(const CcuBuffer& other) { this->handle = other.handle; }

    CcuBuffer(CcuBuffer&& other) noexcept { this->handle = other.handle; }

    void operator=(CcuBuffer&& other) { this->handle = other.handle; }

    CcuBufferHandle handle{0};

private:
    explicit CcuBuffer(detail::NoAllocTag) {}
    template <typename U>
    friend class Array;
};

} // namespace ccu
} // namespace AscendC

#endif // CANN_VERSION_NUM >= 90100000

#endif // CCU_BUFFER_DL_HPP
