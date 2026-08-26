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
 * \file context_builder_impl.cpp
 * \brief
 */

#include "context_builder_impl.h"
#include <map>
#include <fstream>
#include <limits.h>
#include <stdlib.h>
#include "exe_graph/runtime/shape.h"
#include "exe_graph/runtime/tiling_context.h"
#include "exe_graph/runtime/kernel_context.h"
#include "context_ascendc_log.h"

namespace context_ascendc {
KernelRunContextHolder::KernelRunContextHolder(
    gert::ContextHolder<gert::KernelContext>&& ctxHolder, gert::KernelContext* kernelContextPtr,
    KernelRunContext* contextPtr)
    : valueHolder(std::make_unique<ValueHolderImpl>(std::move(ctxHolder))),
      context(contextPtr),
      kernelContext(kernelContextPtr)
{}

KernelRunContextHolder::KernelRunContextHolder(
    gert::ContextHolder<gert::TilingContext>&& ctxHolder, std::vector<std::unique_ptr<uint8_t[]>>&& inputTensorHolder,
    std::vector<std::unique_ptr<uint8_t[]>>&& outputTensorHolder, gert::KernelContext* kernelContextPtr)
    : valueHolder(std::make_unique<ValueHolderImpl>(
          std::move(ctxHolder), std::move(inputTensorHolder), std::move(outputTensorHolder))),
      kernelContext(kernelContextPtr)
{}
namespace {
using SetConstDataFunc = std::function<bool(void*, int64_t, int64_t, std::unique_ptr<uint8_t[]>&)>;
const std::map<ge::DataType, SetConstDataFunc> SET_CONST_DATA_FUNC_MAP = {
    {ge::DT_INT8, context_ascendc::DataUtils::SetConstData<int8_t>},
    {ge::DT_UINT8, context_ascendc::DataUtils::SetConstData<uint8_t>},
    {ge::DT_INT16, context_ascendc::DataUtils::SetConstData<int16_t>},
    {ge::DT_UINT16, context_ascendc::DataUtils::SetConstData<uint16_t>},
    {ge::DT_INT32, context_ascendc::DataUtils::SetConstData<int32_t>},
    {ge::DT_UINT32, context_ascendc::DataUtils::SetConstData<uint32_t>},
    {ge::DT_INT64, context_ascendc::DataUtils::SetConstData<int64_t>},
    {ge::DT_UINT64, context_ascendc::DataUtils::SetConstData<uint64_t>},
    {ge::DT_FLOAT, context_ascendc::DataUtils::SetConstData<float>},
    {ge::DT_DOUBLE, context_ascendc::DataUtils::SetConstData<double>},
    {ge::DT_FLOAT16, context_ascendc::DataUtils::SetConstDataWithFloat16},
    {ge::DT_BF16, context_ascendc::DataUtils::SetConstDataWithBF16}};
} // namespace
ContextBuilderImpl::ContextBuilderImpl()
{
    kernelCtxBuilder_ = std::make_unique<gert::OpKernelContextBuilder>();
    tilingCtxBuilder_ = std::make_unique<gert::OpTilingContextBuilder>();

    tilingCtxBuilder_->OpName("default_name").OpType("default_type").IONum(1, 1);
    tilingCtxBuilder_->CompileInfo("default_compileinfo").PlatformInfo("default_platforminfo");
    kernelCtxBuilder_->OpName("default_name").OpType("default_type").IONum(1, 1);
}
void ContextBuilderImpl::NodeIoNum(size_t inputNum, size_t outputNum)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->IONum(inputNum, outputNum);
    inputNum_ = inputNum;
    outputNum_ = outputNum;
}

void ContextBuilderImpl::IrInstanceNum(std::vector<uint32_t> instanceNum)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->IOInstanceNum(instanceNum, std::vector<uint32_t>(outputNum_, 1));
    uint32_t inputIstNum = 0;
    for (const auto& num : instanceNum) {
        inputIstNum += num;
    }
    inputNum_ = inputIstNum;
}

void ContextBuilderImpl::SetOpNameType(const std::string& opName, const std::string& opType)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->OpName(opName.c_str());
    tilingCtxBuilder_->OpType(opType.c_str());
}

std::shared_ptr<KernelRunContextHolder> ContextBuilderImpl::BuildKernelRunContext()
{
    ASCENDC_ASSERT(kernelCtxBuilder_ != nullptr, return nullptr, CXT_ASCENDC_LOGE("kernelCtxBuilder_ is nullptr!"));
    gert::ContextHolder<gert::KernelContext> ctxHolder = kernelCtxBuilder_->Build();
    if (ctxHolder.GetContext() == nullptr) {
        return nullptr;
    }
    return std::shared_ptr<KernelRunContextHolder>(new KernelRunContextHolder(
        std::move(ctxHolder), ctxHolder.GetContext(), reinterpret_cast<KernelRunContext*>(ctxHolder.GetContext())));
}

std::shared_ptr<KernelRunContextHolder> ContextBuilderImpl::BuildTilingContext()
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return nullptr, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    if (errFlag_) {
        CXT_ASCENDC_LOGE("Build Context Error!");
        return nullptr;
    }
    std::vector<std::unique_ptr<uint8_t[]>> tensorValueVec(inputNum_);
    std::vector<std::unique_ptr<uint8_t[]>> tensorValueVecOut(outputNum_);
    std::vector<gert::Tensor*> tensorVec(inputNum_, nullptr);
    for (size_t i = 0; i < inputNum_; ++i) {
        auto iter = dependTensorsData_.find(static_cast<int32_t>(i));
        if (iter != dependTensorsData_.end()) {
            tensorValueVec[i] = std::move(iter->second);
            tensorVec[i] = reinterpret_cast<gert::Tensor*>(tensorValueVec[i].get());
        }
    }
    for (size_t i = 0; i < outputNum_; ++i) {
        auto iter = dependOutputTensorsData_.find(static_cast<int32_t>(i));
        if (iter != dependOutputTensorsData_.end()) {
            tensorValueVecOut[i] = std::move(iter->second);
        }
    }
    tilingCtxBuilder_->InputTensors(tensorVec);
    gert::ContextHolder<gert::TilingContext> ctxHolder = tilingCtxBuilder_->Build();
    if (ctxHolder.GetContext() == nullptr) {
        return nullptr;
    }
    gert::TilingContext* tilingCtx = ctxHolder.GetContext();
    KernelRunContext* kernelRunCtx = reinterpret_cast<gert::KernelContext*>(tilingCtx)->GetContext();
    gert::KernelContext* kernelCtx = reinterpret_cast<gert::KernelContext*>(kernelRunCtx);
    return std::shared_ptr<KernelRunContextHolder>(new KernelRunContextHolder(
        std::move(ctxHolder), std::move(tensorValueVec), std::move(tensorValueVecOut), kernelCtx));
}

void ContextBuilderImpl::AddInputTd(
    int32_t index, ge::DataType dtype, ge::Format originFormat, ge::Format storageFormat,
    gert::StorageShape storageShape)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    int64_t tensorSize = context_ascendc::DataUtils::GetTensorSizeByStorageShape(storageShape, dtype);
    if (tensorSize < 0) {
        CXT_ASCENDC_LOGE("tensor idx: %d size is below 0", index);
        errFlag_ = true;
        return;
    }
    size_t totalSize = 0UL;
    auto dataPtr = gert::Tensor::CreateFollowing(dtype, static_cast<size_t>(tensorSize), totalSize);
    auto tensor = reinterpret_cast<gert::Tensor*>(dataPtr.get());
    tensor->MutableOriginShape() = storageShape.GetOriginShape();
    tensor->MutableStorageShape() = storageShape.GetStorageShape();
    tensor->SetDataType(dtype);
    tensor->SetStorageFormat(storageFormat);
    tensor->SetOriginFormat(originFormat);
    dependTensorsData_.insert({index, std::move(dataPtr)});
}

void ContextBuilderImpl::AddInputTd(
    int32_t index, ge::DataType dtype, ge::Format originFormat, ge::Format storageFormat,
    gert::StorageShape storageShape, void* constValues)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));

    int64_t tensorSize = context_ascendc::DataUtils::GetTensorSizeByStorageShape(storageShape, dtype);
    if (tensorSize < 0) {
        CXT_ASCENDC_LOGE("tensor idx: %d size is below 0", index);
        errFlag_ = true;
        return;
    }
    size_t totalSize = 0UL;
    auto tensorPtr = gert::Tensor::CreateFollowing(dtype, static_cast<size_t>(tensorSize), totalSize);
    auto iter = SET_CONST_DATA_FUNC_MAP.find(dtype);
    if (iter != SET_CONST_DATA_FUNC_MAP.cend()) {
        auto res = iter->second(constValues, static_cast<size_t>(tensorSize), totalSize, tensorPtr);
        if (!res) {
            CXT_ASCENDC_LOGE("Set const data failed, input idx : %d", index);
            errFlag_ = true;
            return;
        }
    } else {
        CXT_ASCENDC_LOGE("Cannot find set cosnt data function for Input idx :%d, please check dtype.", index);
        errFlag_ = true;
        return;
    }
    auto tensor = reinterpret_cast<gert::Tensor*>(tensorPtr.get());
    tensor->MutableOriginShape() = storageShape.GetOriginShape();
    tensor->MutableStorageShape() = storageShape.GetStorageShape();
    tensor->SetDataType(dtype);
    tensor->SetStorageFormat(storageFormat);
    tensor->SetOriginFormat(originFormat);
    dependTensorsData_.insert({index, std::move(tensorPtr)});
}

void ContextBuilderImpl::AddInputTd(
    int32_t index, ge::DataType dtype, ge::Format originFormat, ge::Format storageFormat,
    gert::StorageShape storageShape, const std::string& filePath)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    int64_t tensorSize = context_ascendc::DataUtils::GetTensorSizeByStorageShape(storageShape, dtype);
    if (tensorSize < 0) {
        CXT_ASCENDC_LOGE("tensor idx: %d size is below 0", index);
        errFlag_ = true;
        return;
    }
    void* buffer = malloc(static_cast<size_t>(tensorSize));
    if (buffer == nullptr) {
        CXT_ASCENDC_LOGE("AddInputTd %d failed, alloc device memory failed.", index);
        errFlag_ = true;
        return;
    }
    if (!context_ascendc::DataUtils::ReadBinFile(filePath, buffer, static_cast<size_t>(tensorSize))) {
        CXT_ASCENDC_LOGE("Add Input Idx :%d error, file path is invalid", index);
        free(buffer);
        buffer = nullptr;
        errFlag_ = true;
        return;
    }
    size_t totalSize = 0UL;
    auto tensorPtr = gert::Tensor::CreateFollowing(dtype, static_cast<size_t>(tensorSize), totalSize);
    auto tensor = reinterpret_cast<gert::Tensor*>(tensorPtr.get());
    errno_t err = memcpy_s(tensor->GetData<uint8_t>(), totalSize - sizeof(gert::Tensor), buffer, tensorSize);
    if (err != EOK) {
        CXT_ASCENDC_LOGE("Set Const Data Failed!");
        free(buffer);
        buffer = nullptr;
        errFlag_ = true;
        return;
    }

    tensor->MutableOriginShape() = storageShape.GetOriginShape();
    tensor->MutableStorageShape() = storageShape.GetStorageShape();
    tensor->SetDataType(dtype);
    tensor->SetStorageFormat(storageFormat);
    tensor->SetOriginFormat(originFormat);
    free(buffer);
    dependTensorsData_.insert({index, std::move(tensorPtr)});
    buffer = nullptr;
}

void ContextBuilderImpl::Inputs(std::vector<void*> inputs)
{
    ASCENDC_ASSERT(kernelCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("kernelCtxBuilder_ is nullptr!"));
    kernelCtxBuilder_->Inputs(inputs);
}

void ContextBuilderImpl::Outputs(std::vector<void*> outputs)
{
    ASCENDC_ASSERT(kernelCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("kernelCtxBuilder_ is nullptr!"));
    kernelCtxBuilder_->Outputs(outputs);
}

void ContextBuilderImpl::AddAttr(const std::string& attrName, int64_t attrValue)
{
    (void)attrName;
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->AppendAttr(attrValue);
}

void ContextBuilderImpl::AddAttr(const std::string& attrName, bool attrValue)
{
    (void)attrName;
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->AppendAttr(attrValue);
}

void ContextBuilderImpl::AddAttr(const std::string& attrName, const std::string& attrValue)
{
    (void)attrName;
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->AppendAttr(ge::AscendString(attrValue.c_str()));
}

void ContextBuilderImpl::AddAttr(const std::string& attrName, float attrValue)
{
    (void)attrName;
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->AppendAttr(attrValue);
}

void ContextBuilderImpl::AddAttr(const std::string& attrName, const std::vector<float>& attrValue)
{
    (void)attrName;
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->AppendAttr(attrValue);
}

void ContextBuilderImpl::AddAttr(const std::string& attrName, const std::vector<bool>& attrValue)
{
    (void)attrName;
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->AppendAttr(attrValue);
}

void ContextBuilderImpl::AddAttr(const std::string& attrName, const std::vector<int64_t>& attrValue)
{
    (void)attrName;
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->AppendAttr(attrValue);
}

void ContextBuilderImpl::AddAttr(const std::string& attrName, const std::vector<std::string>& attrValue)
{
    (void)attrName;
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    std::vector<ge::AscendString> attrValues;
    for (const auto& str : attrValue) {
        attrValues.emplace_back(str.c_str());
    }
    tilingCtxBuilder_->AppendAttr(std::vector<ge::AscendString>(attrValues));
}

void ContextBuilderImpl::AddAttr(const std::string& attrName, const std::vector<std::vector<int64_t>>& attrValue)
{
    (void)attrName;
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->AppendAttr(attrValue);
}

void ContextBuilderImpl::AddOutputTd(
    int32_t index, ge::DataType dtype, ge::Format originFormat, ge::Format storageFormat,
    gert::StorageShape storageShape)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("tilingCtxBuilder_ is nullptr!"));
    int64_t tensorSize = context_ascendc::DataUtils::GetTensorSizeByStorageShape(storageShape, dtype);
    if (tensorSize < 0) {
        CXT_ASCENDC_LOGE("tensor idx: %d size is below 0", index);
        errFlag_ = true;
        return;
    }
    size_t totalSize = 0UL;
    auto dataPtr = gert::Tensor::CreateFollowing(dtype, static_cast<size_t>(tensorSize), totalSize);
    auto tensor = reinterpret_cast<gert::Tensor*>(dataPtr.get());
    tensor->MutableOriginShape() = storageShape.GetOriginShape();
    tensor->MutableStorageShape() = storageShape.GetStorageShape();
    tensor->SetDataType(dtype);
    tensor->SetStorageFormat(storageFormat);
    tensor->SetOriginFormat(originFormat);
    dependOutputTensorsData_.insert({index, std::move(dataPtr)});
    std::vector<gert::Tensor*> tensorVector = {tensor};
    tilingCtxBuilder_->OutputTensors(tensorVector);
}

void ContextBuilderImpl::CompileInfo(void* compileInfo)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("kernelCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->CompileInfo(compileInfo);
}

void ContextBuilderImpl::TilingData(void* tilingData)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("kernelCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->TilingData(reinterpret_cast<gert::TilingData*>(tilingData));
}

void ContextBuilderImpl::Workspace(gert::ContinuousVector* workspace)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("kernelCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->Workspace(workspace);
}

void ContextBuilderImpl::PlatformInfo(void* platformInfo)
{
    ASCENDC_ASSERT(tilingCtxBuilder_ != nullptr, return, CXT_ASCENDC_LOGE("kernelCtxBuilder_ is nullptr!"));
    tilingCtxBuilder_->PlatformInfo(platformInfo);
}

namespace DataUtils {
constexpr int64_t OFFSET = 2;

uint16_t FloatToBF16(const ge::float32_t value)
{
    union Fp32 {
        uint32_t u;
        float f;
    };
    Fp32 temp;
    temp.f = value;
    constexpr uint32_t kInt16BitsNum = 16U;
    return uint16_t(temp.u >> kInt16BitsNum);
}

uint16_t FloatToUint16(const float value)
{
    union Fp32 {
        uint32_t u;
        float f;
    };
    constexpr Fp32 f32Infty = {static_cast<uint32_t>(255) << static_cast<uint32_t>(23)};
    constexpr uint32_t signMask = 0x80000000U;
    constexpr uint32_t rightShift16 = 16U;
    Fp32 temp;
    uint16_t out;
    temp.f = value;
    const uint32_t sign = temp.u & signMask;
    temp.u ^= sign;

    if (temp.u >= f32Infty.u) {
        constexpr uint32_t roundMax = 0x7FFFU;
        constexpr uint32_t dstAddr = 0x7C00U;
        out = (temp.u > f32Infty.u) ? roundMax : dstAddr;
    } else {
        constexpr uint32_t rightShift13 = 13U;
        constexpr Fp32 f16Infty = {static_cast<uint32_t>(31) << static_cast<uint32_t>(23)};
        constexpr Fp32 magic = {static_cast<uint32_t>(15) << static_cast<uint32_t>(23)};
        constexpr uint32_t roundMask = static_cast<uint32_t>(~0xFFFU);

        temp.u &= roundMask;
        temp.f *= magic.f;
        temp.u -= roundMask;
        if (temp.u > f16Infty.u) {
            temp.u = f16Infty.u;
        }
        out = uint16_t(temp.u >> rightShift13);
    }
    out = uint16_t(out | (sign >> rightShift16));
    return out;
}

bool ReadBinFile(const std::string& fileName, void* buf, std::size_t bufferLen)
{
    try {
        if (fileName.empty()) {
            CXT_ASCENDC_LOGE("Path is NULL.");
            return false;
        }
        if (fileName.size() >= PATH_MAX) {
            CXT_ASCENDC_LOGE("Path[%s] is too long.", fileName.c_str());
            return false;
        }
        char resovedPath[PATH_MAX] = {0};
        std::string absFilePath = "";
        if (realpath(fileName.c_str(), resovedPath) == nullptr) {
            CXT_ASCENDC_LOGE("Path[%s] is not exist.", fileName.c_str());
            return false;
        }
        absFilePath = resovedPath;
        std::ifstream inFile{absFilePath};
        if (!inFile.is_open()) {
            CXT_ASCENDC_LOGE("open file: %s failed.", fileName.c_str());
            return false;
        }
        inFile.read(reinterpret_cast<char*>(buf), bufferLen);
        inFile.close();
    } catch (std::exception& e) {
        CXT_ASCENDC_LOGE("read file %s failed, err = %s", fileName.c_str(), e.what());
        return false;
    }
    return true;
}

int64_t GetTensorSizeByStorageShape(const gert::StorageShape& storageShape, const ge::DataType& dtype)
{
    int64_t dataSize = storageShape.GetStorageShape().GetShapeSize();
    if (dataSize <= 0) {
        CXT_ASCENDC_LOGE("Add Input Idx Invalid: shape size overflow.");
        return -1;
    }
    int64_t tensorSize = ge::GetSizeInBytes(dataSize, dtype);
    if (tensorSize <= 0) {
        CXT_ASCENDC_LOGE("Add Input Idx Invalid: tensorSize overflow.");
        return -1;
    }
    return tensorSize;
}

bool SetConstDataWithFloat16(void* rawData, int64_t bufferLen, int64_t holderSize, std::unique_ptr<uint8_t[]>& dstData)
{
    auto tensor = reinterpret_cast<gert::Tensor*>(dstData.get());
    auto floatValues = std::vector<float>(
        reinterpret_cast<float*>(rawData), reinterpret_cast<float*>(rawData) + bufferLen * OFFSET / sizeof(float));
    std::vector<uint16_t> float16Values;
    for (size_t i = 0UL; i < floatValues.size(); ++i) {
        uint16_t float16Value = context_ascendc::DataUtils::FloatToUint16(floatValues[i]);
        float16Values.emplace_back(float16Value);
    }
    if (static_cast<size_t>(holderSize) < sizeof(gert::Tensor)) {
        CXT_ASCENDC_LOGE("Set Const Data Failed! Tensor Holder Size is smaller than size of tensor");
        return false;
    }
    errno_t err = memcpy_s(
        tensor->GetData<uint8_t>(), holderSize - sizeof(gert::Tensor), float16Values.data(),
        float16Values.size() * sizeof(uint16_t));
    if (err != EOK) {
        CXT_ASCENDC_LOGE("Set Const Data Failed!");
        return false;
    }
    return true;
}

bool SetConstDataWithBF16(void* rawData, int64_t bufferLen, int64_t holderSize, std::unique_ptr<uint8_t[]>& dstData)
{
    auto tensor = reinterpret_cast<gert::Tensor*>(dstData.get());
    auto floatValues = std::vector<float>(
        reinterpret_cast<float*>(rawData), reinterpret_cast<float*>(rawData) + bufferLen * OFFSET / sizeof(float));
    std::vector<uint16_t> bf16Values;
    for (size_t i = 0UL; i < floatValues.size(); ++i) {
        uint16_t bf16Value = FloatToBF16(floatValues[i]);
        bf16Values.emplace_back(bf16Value);
    }
    if (static_cast<size_t>(holderSize) < sizeof(gert::Tensor)) {
        CXT_ASCENDC_LOGE("Set Const Data Failed! Tensor Holder Size is smaller than size of tensor");
        return false;
    }
    errno_t err = memcpy_s(
        tensor->GetData<uint8_t>(), holderSize - sizeof(gert::Tensor), bf16Values.data(),
        bf16Values.size() * sizeof(uint16_t));
    if (err != EOK) {
        CXT_ASCENDC_LOGE("Set Const Data Failed!");
        return false;
    }
    return true;
}

template <typename T>
bool SetConstData(void* rawData, int64_t bufferLen, int64_t holderSize, std::unique_ptr<uint8_t[]>& dstData)
{
    auto tensor = reinterpret_cast<gert::Tensor*>(dstData.get());
    std::vector<uint8_t> constValues =
        std::vector<uint8_t>(reinterpret_cast<uint8_t*>(rawData), reinterpret_cast<uint8_t*>(rawData) + bufferLen);
    if (static_cast<size_t>(holderSize) < sizeof(gert::Tensor)) {
        CXT_ASCENDC_LOGE("Set Const Data Failed! Tensor Holder Size is smaller than size of tensor");
        return false;
    }
    errno_t err =
        memcpy_s(tensor->GetData<uint8_t>(), holderSize - sizeof(gert::Tensor), constValues.data(), bufferLen);
    if (err != EOK) {
        CXT_ASCENDC_LOGE("Set Const Data Failed!");
        return false;
    }
    return true;
}
} // namespace DataUtils
} // namespace context_ascendc
