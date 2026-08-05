/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_LOOP_DL_HPP
#define CCU_LOOP_DL_HPP

#if CANN_VERSION_NUM >= 90100000
#include "ccu_loop.hpp"
#else
#include <vector>
#include "ccu_types_dl.h"
#include "ccu_variable_dl.hpp"
#include "ccu_func_dl.hpp"
#include "ccu_primitives_impl_dl.h"
#include "ccu_utils_dl.hpp"

namespace AscendC {
namespace ccu {

class Loop {
public:
    // var-based：loopCfg 必须是 lvalue（持引用，加入 LoopGroup 时取地址）；
    //            func 在 ctor 内即时翻译，之后不再访问，可接 rvalue (const &)。
    Loop(Variable& loopCfg, const Func& func)
    {
        ComposeLoopBody(func);
        isVarBased_ = true;
        loopParamVar_ = &loopCfg;
    }

    // config-based：loopCfg 拷贝进 config_，func 同上仅在 ctor 内使用，
    //               两者均接 const &，支持完整 inline 写法
    Loop(const CcuLoopConfig& loopCfg, const Func& func)
    {
        ComposeLoopBody(func);
        isVarBased_ = false;
        config_ = loopCfg;
    }

    CcuLoop Handle() const { return handle_; }

    bool IsVarBased() const { return isVarBased_; }

    Variable* LoopParamVar() const { return loopParamVar_; }

    const CcuLoopConfig* Config() const { return &config_; }

private:
    void ComposeLoopBody(const Func& func)
    {
        if (func.NumIn() != 0) {
            throw ::AscendC::ccu::detail::CcuException(
                CcuResult::CCU_E_PARA, "ccu::Loop requires a no-argument ccu::Func");
        }
        CCU_THROW_IF_FAILED(::CcuLoopCreate(&handle_), "CcuLoopCreate failed");
        CCU_THROW_IF_FAILED(::_CcuLoopBodyEnter(handle_), "_CcuLoopBodyEnter failed");
        try {
            func.RunBody(nullptr);
        } catch (...) {
            (void)::_CcuLoopBodyExit(handle_);
            throw;
        }
        CCU_THROW_IF_FAILED(::_CcuLoopBodyExit(handle_), "_CcuLoopBodyExit failed");
    }

    CcuLoop handle_{0};
    bool isVarBased_{false};
    Variable* loopParamVar_{nullptr};
    CcuLoopConfig config_{};
};

class LoopGroup {
public:
    // var-based。maxLoopNum：本 group 实际要 AddLoop 的次数（含展开复用），
    // 用于驱动 kernel 在 CcuLoopGroupCreateFromVar 时按需扩容 LoopEngine 池。
    // 形参顺序：parallelCfg、offsetCfg、maxLoopNum、loops——maxLoopNum 紧跟两个
    // var 配置（第三位），与 config-based 重载里"cfg → maxLoopNum"的相对位置一致。
    LoopGroup(Variable& parallelCfg, Variable& offsetCfg, uint32_t maxLoopNum, const std::vector<Loop>& loops)
    {
        CCU_THROW_IF_FAILED(
            ::CcuLoopGroupCreateFromVar(&handle_, maxLoopNum, parallelCfg.handle, offsetCfg.handle),
            "CcuLoopGroupCreateFromVar failed");
        AddLoops(loops);
    }

    // config-based。maxLoopNum 同上语义；放在 cfg 之后作为 group 元参数。
    LoopGroup(const CcuLoopGroupConfig& loopGroupCfg, uint32_t maxLoopNum, const std::vector<Loop>& loops)
    {
        // 拷贝到 lvalue 以便取地址传给 C 接口；本身不修改原 cfg。
        CcuLoopGroupConfig localCfg = loopGroupCfg;
        CCU_THROW_IF_FAILED(::CcuLoopGroupCreate(&handle_, maxLoopNum, &localCfg), "CcuLoopGroupCreate failed");
        AddLoops(loops);
    }

    CcuLoopGroup Handle() const { return handle_; }

private:
    void AddLoops(const std::vector<Loop>& loops)
    {
        for (const auto& loop : loops) {
            if (loop.IsVarBased()) {
                auto* loopParamVar = loop.LoopParamVar();
                if (loopParamVar == nullptr) {
                    throw ::AscendC::ccu::detail::CcuException(
                        CcuResult::CCU_E_PARA, "ccu::Loop var-based loop has null loop parameter");
                }
                CCU_THROW_IF_FAILED(
                    ::CcuLoopGroupAddLoopFromVar(handle_, loop.Handle(), loopParamVar->handle),
                    "CcuLoopGroupAddLoopFromVar failed");
            } else {
                CCU_THROW_IF_FAILED(
                    ::CcuLoopGroupAddLoop(handle_, loop.Handle(), loop.Config()), "CcuLoopGroupAddLoop failed");
            }
        }
    }

    CcuLoopGroup handle_{0};
};

} // namespace ccu
} // namespace AscendC

#endif // CANN_VERSION_NUM >= 90100000
#endif // CCU_LOOP_DL_HPP
