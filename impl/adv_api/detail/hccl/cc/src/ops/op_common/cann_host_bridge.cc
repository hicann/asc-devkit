/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include "cann_host_bridge.h"

#include <dlfcn.h>

#include <memory>
#include <string>

#include "log.h"
#include "executor_v2_base.h"
#include "selector/execute_selector.h"

namespace mc2_ops_hccl {
namespace cann_abi {
namespace {

constexpr const char* kDefaultCannLib = "libhccl.so";

// ExecuteSelector 对象就地构造缓冲区的对齐字节数。
constexpr std::size_t kSelectorBufAlign = 16U;
// ExecuteSelector 就地构造缓冲区的字节数。
constexpr std::size_t kSelectorBufSize = 64U;

// CANN 侧 ExecuteSelector 的真实 sizeof/alignof 由 libhccl.so 决定, 无法在运行期 dlsym 获取。
// 本地 selector/execute_selector.h 中的 ExecuteSelector 是与 CANN 侧保持同步的镜像类,
// 以它作为编译期代理: 一旦 CANN 侧(及本地镜像)给该类新增数据成员或更严的对齐要求,
// 下面的断言会编译失败, 强制同步调大 selBuf, 避免就地构造越界踩栈。
static_assert(
    sizeof(mc2_ops_hccl::ExecuteSelector) <= kSelectorBufSize,
    "selBuf too small for ExecuteSelector; CANN ABI changed, enlarge kSelectorBufSize");
static_assert(
    kSelectorBufAlign % alignof(mc2_ops_hccl::ExecuteSelector) == 0,
    "selBuf alignment insufficient for ExecuteSelector; raise kSelectorBufAlign");

// ops_hccl::ExecuteSelector::ExecuteSelector()
constexpr const char* kSymSelectorCtor = "_ZN8ops_hccl15ExecuteSelectorC1Ev";
// ops_hccl::ExecuteSelector::Run(OpParam&, TopoInfoWithNetLayerDetails*, std::string&) const
constexpr const char* kSymSelectorRun =
    "_ZNK8ops_hccl15ExecuteSelector3RunERNS_7OpParamEPNS_27TopoInfoWithNetLayerDetailsERSs";
// ops_hccl::CollAlgExecRegistryV2::Instance() — 静态成员, 返回单例引用(取地址作 self)。
constexpr const char* kSymRegistryInstance = "_ZN8ops_hccl21CollAlgExecRegistryV28InstanceEv";
// ops_hccl::CollAlgExecRegistryV2::GetAlgExec(HcclCMDType, std::string const&)
constexpr const char* kSymRegistryGetAlgExec = "_ZN8ops_hccl21CollAlgExecRegistryV210GetAlgExecE11HcclCMDTypeRKSs";

// ops_hccl::HcclGetAlgRes — 新版(当前 hccl 项目)签名, 末尾新增 const ResPackGraphMode&:
//   (void*(HcclComm), OpParam&, unique_ptr<InsCollAlgBase>&, TopoInfoWithNetLayerDetails*,
//    unique_ptr<AlgResourceCtxSerializable>&, void**, bool&, const ResPackGraphMode&)
constexpr const char* kSymHcclGetAlgRes =
    "_ZN8ops_hccl13HcclGetAlgResEPvRNS_7OpParamERSt10unique_ptrINS_14InsCollAlgBaseESt14default_deleteIS4_EEPNS_"
    "27TopoInfoWithNetLayerDetailsERS3_INS_26AlgResourceCtxSerializableES5_ISB_EEPS0_RbRKNS_16ResPackGraphModeE";

// 函数指针类型。
// ExecuteSelector 构造: 无入参, 在调用方提供的内存上原地构造(类无数据成员)。
using SelectorCtorFn = void (*)(void* self);
// ExecuteSelector::Run const 成员函数: AArch64 下 this 作隐式首参。
using SelectorRunFn = HcclResult (*)(
    const void* self, mc2_ops_hccl::OpParam& param, mc2_ops_hccl::TopoInfoWithNetLayerDetails* topoInfo,
    std::string& algName);
// CollAlgExecRegistryV2::Instance(): 静态成员, 返回 registry 引用 → 取其地址(void*)。
using RegInstanceFn = void* (*)();
// CollAlgExecRegistryV2::GetAlgExec(HcclCMDType, const std::string&): 非静态成员, this 作隐式首参。
// 返回 unique_ptr(non-trivial) 由编译器按 AArch64 间接返回(x8)约定自动处理。
using RegGetAlgExecFn = std::unique_ptr<InsCollAlgBase> (*)(void* self, HcclCMDType opType, const std::string& algName);
// HcclGetAlgRes 自由函数: 直接按导出签名映射(comm 为 void*/HcclComm)。
using HcclGetAlgResFn = HcclResult (*)(
    HcclComm comm, mc2_ops_hccl::OpParam& param, std::unique_ptr<InsCollAlgBase>& executor,
    mc2_ops_hccl::TopoInfoWithNetLayerDetails* topoInfo, std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost,
    void** resCtxSequence, bool& isResourceReused, const mc2_ops_hccl::ResPackGraphMode& resPack);
using HcclGetAlgResLegacyFn = HcclResult (*)(
    HcclComm comm, mc2_ops_hccl::OpParam& param, std::unique_ptr<InsCollAlgBase>& executor,
    mc2_ops_hccl::TopoInfoWithNetLayerDetails* topoInfo, std::unique_ptr<AlgResourceCtxSerializable>& resCtxHost,
    void** resCtxSequence, bool& isResourceReused);

struct CannSyms {
    void* handle = nullptr;
    SelectorCtorFn selectorCtor = nullptr;
    SelectorRunFn selectorRun = nullptr;
    RegInstanceFn regInstance = nullptr;
    RegGetAlgExecFn regGetAlgExec = nullptr;
    HcclGetAlgResFn getAlgRes = nullptr;
    bool selectorOk = false;
    bool getAlgExecOk = false;
    bool getAlgResOk = false;
};

const CannSyms& LoadCannSyms()
{
    static CannSyms syms = []() -> CannSyms {
        CannSyms s;
        const char* libName = kDefaultCannLib;
        s.handle = dlopen(libName, RTLD_NOW | RTLD_LOCAL | RTLD_NOLOAD);
        if (s.handle == nullptr) {
            s.handle = dlopen(libName, RTLD_NOW | RTLD_LOCAL);
        }
        if (s.handle == nullptr) {
            HCCL_ERROR("[HostCannBridge] dlopen %s failed: %s", libName, dlerror());
            return s;
        }
        s.selectorCtor = reinterpret_cast<SelectorCtorFn>(dlsym(s.handle, kSymSelectorCtor));
        s.selectorRun = reinterpret_cast<SelectorRunFn>(dlsym(s.handle, kSymSelectorRun));
        if (s.selectorCtor == nullptr || s.selectorRun == nullptr) {
            HCCL_ERROR(
                "[HostCannBridge] dlsym selector failed: SelectorCtor=%p SelectorRun=%p err=%s",
                reinterpret_cast<void*>(s.selectorCtor), reinterpret_cast<void*>(s.selectorRun), dlerror());
        } else {
            s.selectorOk = true;
        }
        s.regInstance = reinterpret_cast<RegInstanceFn>(dlsym(s.handle, kSymRegistryInstance));
        s.regGetAlgExec = reinterpret_cast<RegGetAlgExecFn>(dlsym(s.handle, kSymRegistryGetAlgExec));
        if (s.regInstance == nullptr || s.regGetAlgExec == nullptr) {
            HCCL_ERROR(
                "[HostCannBridge] dlsym registry failed: Instance=%p GetAlgExec=%p err=%s",
                reinterpret_cast<void*>(s.regInstance), reinterpret_cast<void*>(s.regGetAlgExec), dlerror());
        } else {
            s.getAlgExecOk = true;
        }
        s.getAlgRes = reinterpret_cast<HcclGetAlgResFn>(dlsym(s.handle, kSymHcclGetAlgRes));
        if (s.getAlgRes == nullptr) {
            HCCL_ERROR("[HostCannBridge] dlsym HcclGetAlgRes failed: %s", dlerror());
        } else {
            s.getAlgResOk = true;
        }
        HCCL_RUN_INFO(
            "[HostCannBridge] loaded %s handle=%p selectorOk=%d getAlgExecOk=%d getAlgResOk=%d", libName, s.handle,
            static_cast<int>(s.selectorOk), static_cast<int>(s.getAlgExecOk), static_cast<int>(s.getAlgResOk));
        return s;
    }();
    return syms;
}

} // namespace
} // namespace cann_abi

bool UseCannBridge(const OpParam& param)
{
    return (param.engine == COMM_ENGINE_AICPU_TS || param.engine == COMM_ENGINE_AICPU) &&
           (param.opType == HcclCMDType::HCCL_CMD_ALLTOALLV || param.opType == HcclCMDType::HCCL_CMD_ALLREDUCE);
}

HcclResult SelectViaCann(OpParam& ascParam, TopoInfoWithNetLayerDetails* ascTopo, std::string& algName)
{
    const cann_abi::CannSyms& syms = cann_abi::LoadCannSyms();
    if (!syms.selectorOk) {
        HCCL_ERROR(
            "[HostCannBridge][Select] symbols not ready, fallback to local selector. opType[%u]",
            static_cast<u32>(ascParam.opType));
        return HCCL_E_NOT_SUPPORT;
    }
    if (ascTopo == nullptr) {
        HCCL_ERROR("[HostCannBridge][Select] null arg: ascTopo=%p", ascTopo);
        return HCCL_E_PARA;
    }

    alignas(cann_abi::kSelectorBufAlign) unsigned char selBuf[cann_abi::kSelectorBufSize] = {0};
    syms.selectorCtor(static_cast<void*>(selBuf));
    const void* self = static_cast<const void*>(selBuf);

    HCCL_RUN_INFO(
        "[HostCannBridge][Select] before CANN ExecuteSelector::Run, opType=%u engine=%d "
        "opExecuteConfig=%d",
        static_cast<u32>(ascParam.opType), static_cast<int>(ascParam.engine),
        static_cast<int>(ascParam.opExecuteConfig));
    HcclResult ret = syms.selectorRun(self, ascParam, ascTopo, algName);
    if (ret != HCCL_SUCCESS) {
        HCCL_ERROR(
            "[HostCannBridge][Select] CANN  failed, ret=%d opType=%u", static_cast<int>(ret),
            static_cast<u32>(ascParam.opType));
        return ret;
    }
    if (algName.empty()) {
        HCCL_ERROR(
            "[HostCannBridge][Select] CANN selected empty algName, opType=%u", static_cast<u32>(ascParam.opType));
        return HCCL_E_NOT_SUPPORT;
    }

    HCCL_RUN_INFO(
        "[HostCannBridge][Select] CANN ExecuteSelector::Run success, opType=%u algName[%s] engine=%d "
        "opExecuteConfig=%d",
        static_cast<u32>(ascParam.opType), algName.c_str(), static_cast<int>(ascParam.engine),
        static_cast<int>(ascParam.opExecuteConfig));
    return HCCL_SUCCESS;
}

std::unique_ptr<InsCollAlgBase> GetAlgExecViaCann(HcclCMDType opType, const std::string& algName)
{
    const cann_abi::CannSyms& syms = cann_abi::LoadCannSyms();
    if (!syms.getAlgExecOk) {
        HCCL_ERROR(
            "[HostCannBridge][GetAlgExec] symbols not ready. opType[%u] algName[%s]", static_cast<u32>(opType),
            algName.c_str());
        return nullptr;
    }

    void* self = syms.regInstance();
    if (self == nullptr) {
        HCCL_ERROR(
            "[HostCannBridge][GetAlgExec] registry Instance() returned null. opType[%u]", static_cast<u32>(opType));
        return nullptr;
    }
    std::unique_ptr<InsCollAlgBase> executor = syms.regGetAlgExec(self, opType, algName);
    HCCL_RUN_INFO(
        "[HostCannBridge][GetAlgExec] opType[%u] algName[%s] executor[%p]", static_cast<u32>(opType), algName.c_str(),
        reinterpret_cast<void*>(executor.get()));
    return executor;
}
} // namespace mc2_ops_hccl
