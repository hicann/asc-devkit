/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file context_builder.cpp
 * \brief
 */
#include "context_builder.h"
#include <climits>
#include <dlfcn.h>
#include <fstream>
#include <csignal>
#include "base/registry/op_impl_space_registry_v2.h"
#include "securec.h"
#include "platform/platform_info.h"
#include "platform/platform_infos_def.h"
#include "context_builder_impl.h"
#include "context_ascendc_log.h"
#include "exe_graph/runtime/extended_kernel_context.h"
#include "graph/ascend_string.h"

namespace context_ascendc {

gert::ComputeNodeInfo* KernelRunContextHolder::MutableComputeNodeInfo()
{
    return valueHolder->MutableComputeNodeInfo();
}

enum class SocVersion {
    ASCEND910 = 0, // Ascend910A, Ascend910B
    ASCEND910B,    // Ascend910B1~4, Ascend910B2C, Ascend910_93 Serials
    ASCEND310P,    // Ascend310P1, Ascend310P3
    ASCEND310B,    // Ascend310B1, Ascend310B2, Ascend310B3, Ascend310B4
    ASCEND950,     // ASCEND950, __DAV_C310__
    ASCEND350,     // ASCEND350, __DAV_C310__
    ASCEND960DT,   // Ascend960DT, 920R1
    ASCEND960PR,   // Ascend960PR, 920R2
    ASCEND5162,    // Ascend5162, __DAV_516R2__
    RESERVED_VERSION = 99999
};

TilingFunc OpTilingRegistry::GetTilingFunc(const char* opType) const
{
    auto registry = gert::DefaultOpImplSpaceRegistryV2::GetInstance().GetSpaceRegistry();
    const gert::OpImplKernelRegistry::OpImplFunctionsV2* funcs;
    if (registry == nullptr) {
        CXT_ASCENDC_LOGD("failed to find implfuncs in 2.0 way, registery is null. op type is %s.", opType);
        return nullptr;
    }
    funcs = registry->GetOpImpl(opType);
    if (funcs == nullptr || funcs->tiling == nullptr || funcs->tiling_parse == nullptr) {
        return nullptr;
    }
    CXT_ASCENDC_LOGD("find implfuncs in 2.0 way, op type is %s.", opType);
    return funcs->tiling;
}

bool OpTilingRegistry::LoadTilingLibrary(const char* tilingSoPath) const
{
    if (tilingSoPath == nullptr) {
        CXT_ASCENDC_LOGE("tilingSoPath is null.");
        return false;
    }
    if (strlen(tilingSoPath) >= PATH_MAX) {
        CXT_ASCENDC_LOGE("OpTilingRegistry tiling so path [%s] is too long.", tilingSoPath);
        return false;
    }
    char resovedPath[PATH_MAX] = {0};
    if (realpath(tilingSoPath, resovedPath) == nullptr) {
        CXT_ASCENDC_LOGE("OpTilingRegistry Tiling so path [%s] is not exist.", tilingSoPath);
        return false;
    }
    std::string absFilePath(resovedPath);
    void* handle = nullptr;
    void (*registryFunc)(const char*);

    handle = dlopen(absFilePath.c_str(), RTLD_LAZY);
    if (handle == nullptr) {
        CXT_ASCENDC_LOGE("OpTilingRegistry dlopen so path [%s] failed.", absFilePath.c_str());
        return false;
    }

    registryFunc = reinterpret_cast<void (*)(const char*)>(dlsym(handle, "TbeLoadSoAndSaveToRegistry"));
    if (registryFunc == nullptr) {
        dlclose(handle);
        CXT_ASCENDC_LOGE("OpTilingRegistry get TbeLoadSoAndSaveToRegistry func failed.");
        return false;
    }

    registryFunc(absFilePath.c_str());
    dlclose(handle);
    return true;
}

KernelRunContextHolder::KernelRunContextHolder() = default;
KernelRunContextHolder::~KernelRunContextHolder() { (void)valueHolder.reset(); }

ContextBuilder::ContextBuilder() { impl_.reset(new ContextBuilderImpl); }

ContextBuilder::~ContextBuilder() = default;

// kernel context builder
ContextBuilder& ContextBuilder::Inputs(std::vector<void*> inputs)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->Inputs(inputs);
    return *this;
}

ContextBuilder& ContextBuilder::Outputs(std::vector<void*> outputs)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->Outputs(outputs);
    return *this;
}

std::shared_ptr<KernelRunContextHolder> ContextBuilder::BuildKernelRunContext()
{
    ASCENDC_ASSERT(impl_ != nullptr, return nullptr, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    return impl_->BuildKernelRunContext();
}

// OpInfo
ContextBuilder& ContextBuilder::NodeIoNum(size_t inputNum, size_t outputNum)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->NodeIoNum(inputNum, outputNum);
    return *this;
}

ContextBuilder& ContextBuilder::SetOpNameType(const std::string& opName, const std::string& opType)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->SetOpNameType(opName, opType);
    return *this;
}

ContextBuilder& ContextBuilder::IrInstanceNum(std::vector<uint32_t> instanceNum)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->IrInstanceNum(instanceNum);
    return *this;
}

ContextBuilder& ContextBuilder::AddInputTd(
    int32_t index, ge::DataType dtype, ge::Format originFormat, ge::Format storageFormat,
    gert::StorageShape storageShape)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddInputTd(index, dtype, originFormat, storageFormat, storageShape);
    return *this;
}

ContextBuilder& ContextBuilder::AddInputTd(
    int32_t index, ge::DataType dtype, ge::Format originFormat, ge::Format storageFormat,
    gert::StorageShape storageShape, void* constValues)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddInputTd(index, dtype, originFormat, storageFormat, storageShape, constValues);
    return *this;
}

ContextBuilder& ContextBuilder::AddInputTd(
    int32_t index, ge::DataType dtype, ge::Format originFormat, ge::Format storageFormat,
    gert::StorageShape storageShape, const std::string& filePath)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddInputTd(index, dtype, originFormat, storageFormat, storageShape, filePath);
    return *this;
}

ContextBuilder& ContextBuilder::AddOutputTd(
    int32_t index, ge::DataType dtype, ge::Format originFormat, ge::Format storageFormat,
    gert::StorageShape storageShape)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddOutputTd(index, dtype, originFormat, storageFormat, storageShape);
    return *this;
}

ContextBuilder& ContextBuilder::AddAttr(const std::string& attrName, int64_t attrValue)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddAttr(attrName, attrValue);
    return *this;
}

ContextBuilder& ContextBuilder::AddAttr(const std::string& attrName, bool attrValue)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddAttr(attrName, attrValue);
    return *this;
}

ContextBuilder& ContextBuilder::AddAttr(const std::string& attrName, const std::string& attrValue)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddAttr(attrName, attrValue);
    return *this;
}

ContextBuilder& ContextBuilder::AddAttr(const std::string& attrName, float attrValue)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddAttr(attrName, attrValue);
    return *this;
}

ContextBuilder& ContextBuilder::AddAttr(const std::string& attrName, const std::vector<float>& attrValue)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddAttr(attrName, attrValue);
    return *this;
}

ContextBuilder& ContextBuilder::AddAttr(const std::string& attrName, const std::vector<bool>& attrValue)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddAttr(attrName, attrValue);
    return *this;
}

ContextBuilder& ContextBuilder::AddAttr(const std::string& attrName, const std::vector<int64_t>& attrValue)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddAttr(attrName, attrValue);
    return *this;
}

ContextBuilder& ContextBuilder::AddAttr(const std::string& attrName, const std::vector<std::string>& attrValue)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddAttr(attrName, attrValue);
    return *this;
}

ContextBuilder& ContextBuilder::AddAttr(const std::string& attrName, const std::vector<std::vector<int64_t>>& attrValue)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->AddAttr(attrName, attrValue);
    return *this;
}

ContextBuilder& ContextBuilder::CompileInfo(void* compileInfo)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->CompileInfo(compileInfo);
    return *this;
}
ContextBuilder& ContextBuilder::PlatformInfo(void* platformInfo)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->PlatformInfo(platformInfo);
    return *this;
}

ContextBuilder& ContextBuilder::AddPlatformInfo(const char* customSocVersion)
{
    static std::map<std::string, SocVersion> convertMap = {
        {"Ascend310P", SocVersion::ASCEND310P},   {"Ascend910", SocVersion::ASCEND910},
        {"Ascend910B", SocVersion::ASCEND910B},   {"Ascend910_93", SocVersion::ASCEND910B},
        {"Ascend310B", SocVersion::ASCEND310B},   {"Ascend950", SocVersion::ASCEND950},
        {"Ascend350", SocVersion::ASCEND350},     {"Ascend960DT", SocVersion::ASCEND960DT},
        {"Ascend960PR", SocVersion::ASCEND960PR}, {"Ascend5162", SocVersion::ASCEND5162},
    };
    const static std::string labelVersion = "version";
    const static std::string labelShortSocVersion = "Short_SoC_version";

    constexpr uint32_t socVersionLen = 128;
    char socVersion[socVersionLen];

    if (customSocVersion == nullptr || strlen(customSocVersion) == 0) {
        void* handle = nullptr;
        void (*rtGetSocVersion)(char*, const uint32_t);
        handle = dlopen("libruntime.so", RTLD_LAZY);
        if (handle == nullptr) {
            CXT_ASCENDC_LOGE("ContextBuilder dlopen runtime so failed");
            return *this;
        }
        rtGetSocVersion =
            reinterpret_cast<void (*)(char* version, const uint32_t maxLen)>(dlsym(handle, "rtGetSocVersion"));
        if (rtGetSocVersion == nullptr) {
            dlclose(handle);
            CXT_ASCENDC_LOGE("ContextBuilder get rtGetSocVersion is null");
            return *this;
        }
        rtGetSocVersion(&(socVersion[0]), sizeof(socVersion));
        dlclose(handle);
    } else {
        errno_t r = strcpy_s(socVersion, sizeof(socVersion), customSocVersion);
        if (r != EOK) {
            CXT_ASCENDC_LOGE("strcpy socverison failed.");
            return *this;
        }
    }

    static fe::PlatFormInfos platformInfoIns;
    fe::PlatformInfoManager::Instance().InitRuntimePlatformInfos(std::string(socVersion));
    fe::PlatformInfoManager::Instance().GetRuntimePlatformInfosByDevice(0, platformInfoIns);
    std::string socVersionStr;
    const auto retCode = platformInfoIns.GetPlatformResWithLock(labelVersion, labelShortSocVersion, socVersionStr);
    if (!retCode) {
        ASCENDC_ASSERT(
            false, return *this, CXT_ASCENDC_LOGE("get platform short version failed, socVersion is %s", socVersion));
    }
    const auto& iter = convertMap.find(socVersionStr.c_str());
    if (iter != convertMap.end()) {
        SocVersion version = iter->second;
        if ((version == SocVersion::ASCEND310P) || (version == SocVersion::ASCEND910)) {
            platformInfoIns.SetCoreNumByCoreType("AiCore");
        } else {
            platformInfoIns.SetCoreNumByCoreType("VectorCore");
        }
    } else {
        ASCENDC_ASSERT(false, return *this, CXT_ASCENDC_LOGE("Invalid SocVersion."));
    }
    impl_->PlatformInfo(&platformInfoIns);
    return *this;
}

ContextBuilder& ContextBuilder::TilingData(void* tilingData)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->TilingData(tilingData);
    return *this;
}

ContextBuilder& ContextBuilder::Workspace(gert::ContinuousVector* workspace)
{
    ASCENDC_ASSERT(impl_ != nullptr, return *this, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    impl_->Workspace(workspace);
    return *this;
}

std::shared_ptr<KernelRunContextHolder> ContextBuilder::BuildTilingContext()
{
    ASCENDC_ASSERT(impl_ != nullptr, return nullptr, CXT_ASCENDC_LOGE("impl_ is nullptr!"));
    return impl_->BuildTilingContext();
}
} // namespace context_ascendc
