/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "cann_dispatch_bridge.h"

#include <dlfcn.h>

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "log.h"

namespace mc2_ops_hccl {
namespace cann_abi {
namespace {

constexpr const char* kDefaultCannLib = "libscatter_aicpu_kernel.so";
constexpr const char* kSymInstance = "_ZN8ops_hccl21CollAlgExecRegistryV28InstanceEv";
constexpr const char* kSymGetAlgExec = "_ZN8ops_hccl21CollAlgExecRegistryV210GetAlgExecE11HcclCMDTypeRKNSt7__cxx11"
                                       "12basic_stringIcSt11char_traitsIcESaIcEEE";
constexpr const char* kSymInitBatchTransfer = "_ZN8ops_hccl39InitHcommBatchTransferOnThreadSupportedEb";

using InstanceFn = void* (*)();
using GetAlgExecFn = std::unique_ptr<InsCollAlgBase> (*)(void* self, HcclCMDType cmdType, const std::string& algName);
using InitBatchTransferFn = HcclResult (*)(bool isSupported);

struct CannSyms {
    void* handle = nullptr;
    InstanceFn instance = nullptr;
    GetAlgExecFn getAlgExec = nullptr;
    InitBatchTransferFn initBatchTransfer = nullptr;
    bool ok = false;
};

const CannSyms& LoadCannSyms()
{
    static CannSyms syms = []() -> CannSyms {
        CannSyms s;
        const char* libName = kDefaultCannLib;
        // 优先复用已加载实例(RTLD_LOCAL 降低符号冲突面)。
        s.handle = dlopen(libName, RTLD_NOW | RTLD_LOCAL | RTLD_NOLOAD);
        if (s.handle == nullptr) {
            s.handle = dlopen(libName, RTLD_NOW | RTLD_LOCAL);
        }
        if (s.handle == nullptr) {
            HCCL_ERROR("[CannBridge] dlopen %s failed: %s", libName, dlerror());
            return s;
        }
        s.instance = reinterpret_cast<InstanceFn>(dlsym(s.handle, kSymInstance));
        s.getAlgExec = reinterpret_cast<GetAlgExecFn>(dlsym(s.handle, kSymGetAlgExec));
        if (s.instance == nullptr || s.getAlgExec == nullptr) {
            HCCL_ERROR(
                "[CannBridge] dlsym failed: Instance=%p GetAlgExec=%p err=%s", reinterpret_cast<void*>(s.instance),
                reinterpret_cast<void*>(s.getAlgExec), dlerror());
            return s;
        }
        dlerror();
        s.initBatchTransfer = reinterpret_cast<InitBatchTransferFn>(dlsym(s.handle, kSymInitBatchTransfer));
        const char* initBatchTransferError = dlerror();
        if (s.initBatchTransfer == nullptr || initBatchTransferError != nullptr) {
            s.initBatchTransfer = nullptr;
            HCCL_DEBUG(
                "[MC2_BATCH_TRANSFER][CannInit][SkipNoSymbol] symbol[%s], lib[%s], error[%s].", kSymInitBatchTransfer,
                libName, initBatchTransferError == nullptr ? "none" : initBatchTransferError);
        }
        s.ok = true;
        HCCL_RUN_INFO(
            "[CannBridge] loaded %s handle=%p Instance=%p GetAlgExec=%p InitBatchTransfer=%p", libName, s.handle,
            reinterpret_cast<void*>(s.instance), reinterpret_cast<void*>(s.getAlgExec),
            reinterpret_cast<void*>(s.initBatchTransfer));
        return s;
    }();
    return syms;
}

HcclResult InitCannBatchTransferIfSupported(
    const CannSyms& syms, const OpParam& ascParam, const AlgResourceCtxSerializable& ascResCtx)
{
    if (syms.initBatchTransfer == nullptr) {
        return HCCL_SUCCESS;
    }

    HcclResult ret = syms.initBatchTransfer(ascResCtx.isHcommBatchTransferOnThreadSupported);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR(
            "[MC2_BATCH_TRANSFER][CannInit] failed, ret[%d], opType[%u], algName[%s], supported[%d].",
            static_cast<int>(ret), static_cast<u32>(ascParam.opType), ascParam.algName,
            static_cast<int>(ascResCtx.isHcommBatchTransferOnThreadSupported));
        return ret;
    }

    static std::atomic<bool> logged{false};
    if (!logged.exchange(true)) {
        HCCL_DEBUG(
            "[MC2_BATCH_TRANSFER][CannInit] opType[%u], algName[%s], supported[%d], ret[%d].",
            static_cast<u32>(ascParam.opType), ascParam.algName,
            static_cast<int>(ascResCtx.isHcommBatchTransferOnThreadSupported), static_cast<int>(ret));
    }
    return HCCL_SUCCESS;
}

} // namespace
} // namespace cann_abi

HcclResult LaunchViaCann(const OpParam& ascParam, const AlgResourceCtxSerializable& ascResCtx)
{
    const cann_abi::CannSyms& syms = cann_abi::LoadCannSyms();
    if (!syms.ok) {
        HCCL_ERROR(
            "[CannBridge] CANN kernel lib not ready, cannot dispatch. opType=%u algName[%s]",
            static_cast<u32>(ascParam.opType), ascParam.algName);
        return HCCL_E_NOT_SUPPORT;
    }

    // 需要保证 OpParam / AlgResourceCtxSerializable 布局与 CANN 对齐，直接传入，无需逐字段转换。
    CHK_RET(cann_abi::InitCannBatchTransferIfSupported(syms, ascParam, ascResCtx));
    void* self = syms.instance();
    std::unique_ptr<cann_abi::InsCollAlgBase> executor =
        syms.getAlgExec(self, ascParam.opType, std::string(ascParam.algName));
    if (executor == nullptr) {
        HCCL_ERROR(
            "[CannBridge] GetAlgExec returned null. opType=%u algName[%s]", static_cast<u32>(ascParam.opType),
            ascParam.algName);
        return HCCL_E_NOT_SUPPORT;
    }

    HcclResult ret = executor->Orchestrate(ascParam, ascResCtx);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR(
            "[CannBridge] CANN Orchestrate failed, ret=%d. opType=%u algName[%s]", static_cast<int>(ret),
            static_cast<u32>(ascParam.opType), ascParam.algName);
    }
    return ret;
}

} // namespace mc2_ops_hccl
