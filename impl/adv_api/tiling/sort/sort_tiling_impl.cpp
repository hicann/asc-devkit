/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "adv_api/utils/types.h"
#include <cstdint>
#include <set>
#include <map>

#include "adv_api/sort/sort_tiling_intf.h"
#include "../../detail/host_log.h"
#include "utils/tiling/platform/platform_ascendc.h"

namespace AscendC {
namespace {
constexpr uint32_t SORT_HALF_SIZE = 2;
constexpr uint32_t SORT_FLOAT_SIZE = 4;

void CheckSortHostCommon(
    const char* apiName, const char* hostFuncName, const platform_ascendc::PlatformAscendC& ascendcPlatform,
    const uint32_t elemCount, const uint32_t dataTypeSize)
{
    ASCENDC_HOST_ASSERT(elemCount > 0, return, "[%s][%s] The elemCount must be greater than 0.", apiName, hostFuncName);
    ASCENDC_HOST_ASSERT(
        dataTypeSize == SORT_HALF_SIZE || dataTypeSize == SORT_FLOAT_SIZE, return,
        "[%s][%s] Type size %u is unsupported!", apiName, hostFuncName, dataTypeSize);
    uint64_t ubSize = 0;
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    ASCENDC_HOST_ASSERT(
        static_cast<uint64_t>(elemCount * dataTypeSize) < ubSize, continue,
        "[%s][%s] The size of srcShape is %luB, should be less than UB size.", apiName, hostFuncName,
        static_cast<uint64_t>(elemCount * dataTypeSize));
    return;
}
} // namespace

void GetSortMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, AscendC::TensorDataType valueType, AscendC::TensorDataType indexType,
    bool isReuseSource, const SortConfig& config, uint32_t& maxValue, uint32_t& minValue)
{
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT(platform != nullptr, return, "Failed to get PlatformAscendC");
    auto npuArch = platform->GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102, return,
        "Unsupported NpuArch for Sort API.");
    std::set<AscendC::TensorDataType> supportValueType = {ge::DT_INT8,   ge::DT_UINT8,  ge::DT_FLOAT, ge::DT_FLOAT16,
                                                          ge::DT_BF16,   ge::DT_INT16,  ge::DT_INT32, ge::DT_UINT16,
                                                          ge::DT_UINT32, ge::DT_UINT64, ge::DT_INT64};
    std::set<AscendC::TensorDataType> supportIndexType = {ge::DT_INT32, ge::DT_UINT32, ge::DT_INT64, ge::DT_UINT64};
    ASCENDC_HOST_ASSERT(
        (!config.hasSrcIndex || config.hasDstIndex), return, "Sort API cannot only have source input index.");
    ASCENDC_HOST_ASSERT(
        supportValueType.find(valueType) != supportValueType.end(), return, "Unsupported valueType for Sort API.");
    if (config.hasSrcIndex) {
        ASCENDC_HOST_ASSERT(
            supportIndexType.find(indexType) != supportIndexType.end(), return, "Unsupported indexType for Sort API.");
    } else if (config.hasDstIndex) {
        ASCENDC_HOST_ASSERT(indexType == ge::DT_UINT32, return, "Unsupported indexType for Sort API.");
    }
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    uint32_t inputSize = 1;
    for (const auto dim : shapeDims) {
        inputSize *= dim;
    }
    if (config.type == SortType::MERGE_SORT) {
        constexpr uint32_t MERGE_SORT_ONE_ELM_SIZE = 8u;
        maxValue = MERGE_SORT_ONE_ELM_SIZE * inputSize;
        minValue = MERGE_SORT_ONE_ELM_SIZE * inputSize;
        return;
    }
    std::map<AscendC::TensorDataType, uint32_t> typeToSize = {
        {ge::DT_INT8, 1},  {ge::DT_UINT8, 1},   {ge::DT_INT16, 2}, {ge::DT_UINT16, 2},
        {ge::DT_INT32, 4}, {ge::DT_UINT32, 4},  {ge::DT_INT64, 8}, {ge::DT_UINT64, 8},
        {ge::DT_FLOAT, 4}, {ge::DT_FLOAT16, 2}, {ge::DT_BF16, 2},
    };
    auto alignUp = [](const uint32_t a, const uint32_t b) { return (a + b - 1) / b * b; };
    constexpr uint32_t alignedBlock = 32;
    inputSize = alignUp(inputSize, alignedBlock);
    uint32_t extraTypeSize = typeToSize[valueType];
    uint32_t extraIndexTypeSize = typeToSize[indexType];
    if (isReuseSource) {
        extraTypeSize = 0;
        extraIndexTypeSize = 0;
    }
    // 4 times for u32 sorted local and next round, 2 times for u16 tmp index, 1 time src u8.
    constexpr uint32_t fixBufferRate = 7;
    constexpr uint32_t fixedBucketBuffer = 512;

    int32_t mode = 2; // has extra both src and dst index.
    if (!config.hasSrcIndex && !config.hasDstIndex) {
        mode = 1; // only has src/dst value.
    } else if (!config.hasSrcIndex && config.hasDstIndex) {
        mode = 0; // has extra dst Index.
    }

    uint32_t requiredBuffer = inputSize * fixBufferRate + fixedBucketBuffer;
    if (typeToSize[valueType] != 1) {
        if (mode == 1) {
            requiredBuffer += inputSize * extraTypeSize;
        } else if (mode == 0) {
            requiredBuffer += inputSize * (sizeof(uint32_t) + extraTypeSize);
        } else {
            requiredBuffer += inputSize * (extraTypeSize + extraIndexTypeSize);
        }
    } else if (mode == 0) {
        // for B8 type with dstIndex on mode1, it doesn't need extra space for localIndex.
        constexpr uint32_t specialSavedType = 2;
        requiredBuffer -= specialSavedType * inputSize;
    }
    maxValue = requiredBuffer;
    minValue = requiredBuffer;
}

uint32_t GetConcatTmpSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t elemCount, const uint32_t dataTypeSize)
{
    CheckSortHostCommon("Sort", "GetConcatTmpSize", ascendcPlatform, elemCount, dataTypeSize);
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    if (npuArch == NpuArch::DAV_2201 || npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 ||
        npuArch == NpuArch::DAV_3003) {
        return 0;
    } else {
        return elemCount * REGION_PROPOSAL_DATA_SIZE_V200 * dataTypeSize;
    }
}

uint32_t GetSortTmpSize(
    const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t elemCount, const uint32_t dataTypeSize)
{
    CheckSortHostCommon("Sort", "GetSortTmpSize", ascendcPlatform, elemCount, dataTypeSize);
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    if (npuArch == NpuArch::DAV_2201 || npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 ||
        npuArch == NpuArch::DAV_3003) {
        if (dataTypeSize == sizeof(float)) {
            return elemCount * REGION_PROPOSAL_DATA_SIZE_FLOAT_V220 * dataTypeSize;
        } else {
            return elemCount * REGION_PROPOSAL_DATA_SIZE_HALF_V220 * dataTypeSize;
        }
    } else {
        return elemCount * REGION_PROPOSAL_DATA_SIZE_V200 * dataTypeSize;
    }
}
} // namespace AscendC
