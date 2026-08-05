/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_FUNC_DL_HPP
#define CCU_FUNC_DL_HPP

#if CANN_VERSION_NUM >= 90100000
#include "ccu_func.hpp"
#else
#include <cstdint>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "ccu_types_dl.h"
#include "ccu_primitives_impl_dl.h"
#include "ccu_variable_dl.hpp"

namespace AscendC {
namespace ccu {

namespace internal {

// ---- LambdaTraits：只支持非泛型 lambda / 普通成员函数指针 operator() ----
template <typename T>
struct FunctorTraits : FunctorTraits<decltype(&T::operator())> {};

template <typename C, typename R, typename... A>
struct FunctorTraits<R (C::*)(A...) const> {
    static constexpr std::size_t Arity = sizeof...(A);
    using ReturnT = R;
    template <std::size_t I>
    struct Arg {
        using type = typename std::tuple_element<I, std::tuple<A...> >::type;
    };
};

template <typename C, typename R, typename... A>
struct FunctorTraits<R (C::*)(A...)> {
    static constexpr std::size_t Arity = sizeof...(A);
    using ReturnT = R;
};

// ---- 把 lambda 包装成 std::function<void(Variable*)>，按 N 展开 ----
template <typename Lambda, std::size_t... Is>
inline std::function<void(Variable*)> MakeBodyImpl(Lambda body, std::index_sequence<Is...>)
{
    return [body](Variable* formals) { body(formals[Is]...); };
}

template <typename Lambda, std::size_t N>
inline std::function<void(Variable*)> MakeBody(Lambda body)
{
    return MakeBodyImpl(body, std::make_index_sequence<N>{});
}

} // namespace internal

class Func {
public:
    // ctor：从 lambda 推导形参个数 N，类型擦除到 body_。
    // 要求 lambda 返回 void、形参全部为 ccu::Variable（运行期由 kernel 校验形参数量）。
    template <typename Lambda>
    explicit Func(Lambda body)
        : numIn_(internal::FunctorTraits<Lambda>::Arity),
          body_(internal::MakeBody<Lambda, internal::FunctorTraits<Lambda>::Arity>(body))
    {
        static_assert(
            std::is_same<typename internal::FunctorTraits<Lambda>::ReturnT, void>::value,
            "ccu::Func: lambda must return void (no return value supported)");
    }

    Func(const Func&) = delete;
    Func& operator=(const Func&) = delete;

    uint32_t NumIn() const { return numIn_; }
    const void* Key() const { return static_cast<const void*>(this); }

    // 在合成模式下调用：把 N 个 formal Variable 展开传给原 lambda。
    void RunBody(Variable* formals) const { body_(formals); }

private:
    uint32_t numIn_{0};
    std::function<void(Variable*)> body_;
};

// ccu::CallFunc：global / static ccu::Func 引用 NTTP（C++14 合法）。
// 用法：static ccu::Func MyAdd(lambda); ... ccu::CallFunc<MyAdd>(x, y);
template <Func& Obj, typename... Args>
inline CcuResult CallFunc(Args... args)
{
    constexpr std::size_t kArgN = sizeof...(Args);
    if (static_cast<uint32_t>(kArgN) != Obj.NumIn()) {
        return CcuResult::CCU_E_PARA;
    }

    uint64_t handle = 0;
    CCU_THROW_IF_FAILED(::CcuFuncBlockLookup(Obj.Key(), &handle), "CallFunc: CcuFuncBlockLookup failed");
    if (handle == 0) {
        // 第一次进入：合成 FuncBlock
        CCU_THROW_IF_FAILED(::CcuFuncBlockBegin(Obj.Key(), &handle), "CallFunc: CcuFuncBlockBegin failed");

        // 形参：按 lambda 形参个数 alloc，并注册为 FuncBlock 的 in args
        std::vector<Variable> formals(Obj.NumIn());
        for (uint32_t i = 0; i < Obj.NumIn(); i++) {
            CCU_THROW_IF_FAILED(::CcuVariableAlloc(&formals[i].handle), "CallFunc: CcuVariableAlloc failed");
            CCU_THROW_IF_FAILED(::CcuFuncDefineInArg(handle, formals[i].handle), "CallFunc: CcuFuncDefineInArg failed");
        }

        // 执行 lambda（在合成模式下 emit 到 FuncBlock）
        Obj.RunBody(formals.empty() ? nullptr : formals.data());

        CCU_THROW_IF_FAILED(::CcuFuncBlockEnd(handle), "CallFunc: CcuFuncBlockEnd failed");
    }

    // emit 一条 FuncCall 跳转
    std::vector< ::CcuVariableHandle> actuals(kArgN);
    {
        std::size_t i = 0;
        // C++14 兼容的参数包展开
        using expand_t = int[];
        (void)expand_t{0, ((void)(actuals[i++] = args.handle), 0)...};
        (void)i;
    }
    return ::CcuFuncCall(handle, actuals.empty() ? nullptr : actuals.data(), static_cast<uint32_t>(kArgN));
}

} // namespace ccu
} // namespace AscendC

#endif // CANN_VERSION_NUM >= 90100000
#endif // CCU_FUNC_DL_HPP
