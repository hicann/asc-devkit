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

#include <vector>
#include <memory>
#include "ccu_types_dl.h"
#include "ccu_variable_dl.hpp"
#include "ccu_func_dl.hpp"
#include "ccu_primitives_impl_dl.h"
#include "ccu_utils_dl.hpp"

namespace AscendC {
namespace ccu {

class Loop {
public:
    Loop(Variable& loopCfg, const Func& func)
    {
        ComposeLoopBody(func);
        mode_ = Mode::VarBased;
        loopParamVar_ = &loopCfg;
    }

    Loop(const CcuLoopConfig& loopCfg, const Func& func)
    {
        ComposeLoopBody(func);
        mode_ = Mode::ConfigBased;
        config_ = loopCfg;
    }

    Loop(Variable& iterNum, Variable& addrOffset, const Func& func)
    {
        ComposeLoopBody(func);
        mode_ = Mode::VarBasedV2;
        iterNumVar_ = &iterNum;
        addrOffsetVar_ = &addrOffset;
        ctxIdVar_ = std::make_shared<Variable>();
    }

private:
    friend class LoopGroup;

    enum class Mode {
        ConfigBased,
        VarBased,
        VarBasedV2,
    };

    CcuLoop Handle() const { return handle_; }

    Mode GetMode() const { return mode_; }

    bool IsVarBased() const { return mode_ == Mode::VarBased; }

    Variable* LoopParamVar() const { return loopParamVar_; }

    Variable* IterNumVar() const { return iterNumVar_; }

    Variable* AddrOffsetVar() const { return addrOffsetVar_; }

    Variable* CtxIdVar() const { return ctxIdVar_.get(); }

    const CcuLoopConfig* Config() const { return &config_; }

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
    Mode mode_{Mode::ConfigBased};
    Variable* loopParamVar_{nullptr};
    Variable* iterNumVar_{nullptr};
    Variable* addrOffsetVar_{nullptr};
    std::shared_ptr<Variable> ctxIdVar_{nullptr};
    CcuLoopConfig config_{};
};

class LoopGroup {
public:
    LoopGroup(Variable& parallelCfg, Variable& offsetCfg, uint32_t maxLoopNum, const std::vector<Loop>& loops)
    {
        CCU_THROW_IF_FAILED(
            ::CcuLoopGroupCreateFromVar(&handle_, maxLoopNum, parallelCfg.handle, offsetCfg.handle),
            "CcuLoopGroupCreateFromVar failed");
        AddLoops(loops);
    }

    LoopGroup(
        Variable& parallelCfgV2, Variable& offsetCfgV2, Variable& varOffsetCfg, uint32_t maxLoopNum,
        const std::vector<Loop>& loops)
    {
        CCU_THROW_IF_FAILED(
            ::CcuLoopGroupCreateFromVarV2(
                &handle_, maxLoopNum, parallelCfgV2.handle, offsetCfgV2.handle, varOffsetCfg.handle),
            "CcuLoopGroupCreateFromVarV2 failed");
        AddLoops(loops);
    }

    LoopGroup(const CcuLoopGroupConfig& loopGroupCfg, uint32_t maxLoopNum, const std::vector<Loop>& loops)
    {
        CcuLoopGroupConfig localCfg = loopGroupCfg;
        CCU_THROW_IF_FAILED(::CcuLoopGroupCreate(&handle_, maxLoopNum, &localCfg), "CcuLoopGroupCreate failed");
        AddLoops(loops);
    }

    CcuLoopGroup Handle() const { return handle_; }

private:
    void AddLoops(const std::vector<Loop>& loops)
    {
        for (const auto& loop : loops) {
            if (loop.GetMode() == Loop::Mode::VarBasedV2) {
                auto* iterNumVar = loop.IterNumVar();
                auto* addrOffsetVar = loop.AddrOffsetVar();
                auto* ctxIdVar = loop.CtxIdVar();
                if (iterNumVar == nullptr || addrOffsetVar == nullptr || ctxIdVar == nullptr) {
                    throw ::AscendC::ccu::detail::CcuException(
                        CcuResult::CCU_E_PARA, "ccu::Loop V2 loop has null parameter");
                }
                CCU_THROW_IF_FAILED(
                    ::CcuLoopGroupAddLoopFromVarV2(
                        handle_, loop.Handle(), iterNumVar->handle, addrOffsetVar->handle, ctxIdVar->handle),
                    "CcuLoopGroupAddLoopFromVarV2 failed");
            } else if (loop.IsVarBased()) {
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

#endif // CCU_LOOP_DL_HPP
