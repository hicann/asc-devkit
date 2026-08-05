/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_EVENT_DL_HPP
#define CCU_EVENT_DL_HPP

#if CANN_VERSION_NUM >= 90100000
#include "ccu_event.hpp"
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

// Event 退化为纯 handle 持有者：mask 已与 Event 解耦，
// 由调用方在每个 EventRecord/Wait/LocalCopy/Read/Write/... API 上独立传入。
// 旧的 EventMask 代理类、Event::mask 字段、Event::setMask 接口已废弃删除。
class Event final {
public:
    Event() { CCU_THROW_IF_FAILED(CcuEventAlloc(&this->handle), "CcuEventAlloc: failed"); }

    Event(const Event& other) : handle(other.handle) {}

    Event(Event&& other) noexcept : handle(other.handle) {}

    void operator=(Event&& other) { this->handle = other.handle; }

    CcuEventHandle handle{0};

private:
    explicit Event(detail::NoAllocTag) {}
    template <typename U>
    friend class Array;
};

} // namespace ccu
} // namespace AscendC

#endif // CANN_VERSION_NUM >= 90100000
#endif // CCU_EVENT_DL_HPP
