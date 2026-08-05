/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_ARRAY_DL_HPP
#define CCU_ARRAY_DL_HPP

#if CANN_VERSION_NUM >= 90100000
#include "ccu_array.hpp"
#else
#include <cstdint>
#include <new>
#include <vector>

#include "ccu_types_dl.h"
#include "ccu_primitives_impl_dl.h"
#include "ccu_utils_dl.hpp"
#include "ccu_variable_dl.hpp"
#include "ccu_event_dl.hpp"
#include "ccu_buffer_dl.hpp"

namespace AscendC {
namespace ccu {

// 主模板未实现：未特化的资源类型实例化 Array<T> 时编译失败，
// 当前期次仅 Variable / Event / Buffer 提供底层 CcuBlock*Alloc C 接口。
template <typename T>
struct CcuArrayTraits;

template <>
struct CcuArrayTraits<Variable> {
    using Handle = CcuVariableHandle;
    static CcuResult BlockAlloc(Handle* h, uint32_t n) { return CcuBlockVariableAlloc(h, n); }
    static void SetHandle(Variable& v, Handle h) { v.handle = h; }
};

template <>
struct CcuArrayTraits<Event> {
    using Handle = CcuEventHandle;
    static CcuResult BlockAlloc(Handle* h, uint32_t n) { return CcuBlockEventAlloc(h, n); }
    static void SetHandle(Event& e, Handle h) { e.handle = h; }
};

template <>
struct CcuArrayTraits<CcuBuffer> {
    using Handle = CcuBufferHandle;
    static CcuResult BlockAlloc(Handle* h, uint32_t n) { return CcuBlockBufferAlloc(h, n); }
    static void SetHandle(CcuBuffer& b, Handle h) { b.handle = h; }
};

// 连续资源容器：在构造时一次性 BlockAlloc 出 count 个底层句柄并填充给占位元素。
// 元素本身通过 NoAllocTag 私有构造跳过单元 Alloc，避免与 BlockAlloc 双重分配。
template <typename T>
class Array final {
public:
    explicit Array(uint32_t count) : count_(count)
    {
        if (count == 0) {
            return;
        }
        using H = typename CcuArrayTraits<T>::Handle;
        std::vector<H> handles(count);
        elems_ = static_cast<T*>(::operator new(sizeof(T) * count));
        for (uint32_t i = 0; i < count; ++i) {
            ::new (static_cast<void*>(&elems_[i])) T(detail::NoAllocTag{});
        }
        auto ret = CcuArrayTraits<T>::BlockAlloc(handles.data(), count);
        if (ret != CcuResult::CCU_SUCCESS) {
            for (uint32_t i = 0; i < count; ++i) {
                elems_[i].~T();
            }
            ::operator delete(elems_);
            elems_ = nullptr;
            count_ = 0;
            throw ::AscendC::ccu::detail::CcuException(ret, "Array BlockAlloc: failed");
        }
        for (uint32_t i = 0; i < count; ++i) {
            CcuArrayTraits<T>::SetHandle(elems_[i], handles[i]);
        }
    }

    ~Array()
    {
        if (elems_ == nullptr) {
            return;
        }
        for (uint32_t i = 0; i < count_; ++i) {
            elems_[i].~T();
        }
        ::operator delete(elems_);
    }

    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;

    Array(Array&& other) noexcept : elems_(other.elems_), count_(other.count_)
    {
        other.elems_ = nullptr;
        other.count_ = 0;
    }

    Array& operator=(Array&& other) noexcept
    {
        if (this != &other) {
            this->~Array();
            elems_ = other.elems_;
            count_ = other.count_;
            other.elems_ = nullptr;
            other.count_ = 0;
        }
        return *this;
    }

    T& operator[](uint32_t i) { return elems_[i]; }
    const T& operator[](uint32_t i) const { return elems_[i]; }
    T* data() { return elems_; }
    const T* data() const { return elems_; }
    uint32_t size() const { return count_; }

private:
    T* elems_{nullptr};
    uint32_t count_{0};
};

} // namespace ccu
} // namespace AscendC

#endif // CANN_VERSION_NUM >= 90100000

#endif // CCU_ARRAY_DL_HPP
