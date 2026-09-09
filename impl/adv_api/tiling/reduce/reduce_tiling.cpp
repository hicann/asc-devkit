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
#include "adv_api/reduce/reduce_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"

#include <string>
#include <cstdint>
#include <algorithm>

#include "../../detail/host_log.h"
namespace AscendC {
namespace {
constexpr uint32_t ONE_BLK_SIZE = 32;
constexpr uint32_t ONE_REPEAT_BYTE_SIZE = 256;
constexpr uint32_t HALF_TYPE_SIZE = 2;
constexpr uint32_t FLOAT_TYPE_SIZE = 4;
constexpr uint32_t INT32_TYPE_SIZE = 4;
constexpr uint32_t ALLOWED_SHAPE_DIM = 2;
constexpr uint32_t B32_ELEM_NUM_PER_REPEAT = 64;

uint32_t GetTypeSize(const AscendC::TensorDataType dataType)
{
    if (dataType == ge::DT_FLOAT) {
        return FLOAT_TYPE_SIZE;
    } else if (dataType == ge::DT_FLOAT16) {
        return HALF_TYPE_SIZE;
    } else if (dataType == ge::DT_INT32) {
        return INT32_TYPE_SIZE;
    }
    return 1;
}
// Find the most closest power of two results.
uint32_t FindK(uint32_t n)
{
    uint32_t ret = 1U;
    while (n > 1U) {
        ret <<= 1U;
        n >>= 1U;
    }
    return ret;
}

inline void CheckParams(
    std::vector<int64_t> shapeDims, bool isSrcInnerPad, ReducePattern pattern, uint32_t first, uint32_t last,
    std::string apiName, std::string funcName)
{
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();

    ASCENDC_HOST_ASSERT(
        shapeDims.size() == ALLOWED_SHAPE_DIM, return, "[%s][%s] srcShape dims must be 2.", apiName.c_str(),
        funcName.c_str());
    if (!((apiName == "ReduceMin" || apiName == "ReduceMax" || apiName == "ReduceSum") &&
          (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201))) {
        ASCENDC_HOST_ASSERT(
            isSrcInnerPad, return, "[%s][%s] isSrcInnerPad must be true on this platform.", apiName.c_str(),
            funcName.c_str());
    }
    ASCENDC_HOST_ASSERT(
        pattern == ReducePattern::AR || pattern == ReducePattern::RA, return,
        "[%s][%s] Currently only support AR and RA pattern.", apiName.c_str(), funcName.c_str());
    if (!(npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201)) {
        ASCENDC_HOST_ASSERT(
            first > 0 && last > 0, return, "[%s][%s] both first and last axis must be greater than 0.", apiName.c_str(),
            funcName.c_str());
    }
}
} // namespace

inline void GetReduceCommonMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue, bool isBinaryAdd,
    std::string apiName, std::string funcName)
{
    std::vector<int64_t> shapeDims = srcShape.GetDims();

    const uint32_t first = static_cast<uint32_t>(shapeDims[0]);
    const uint32_t last = static_cast<uint32_t>(shapeDims[1]);
    CheckParams(shapeDims, isSrcInnerPad, pattern, first, last, apiName, funcName);
    if (isReuseSource) {
        maxValue = minValue = 0U;
        return;
    }
    if (pattern == ReducePattern::AR) {
        if (isBinaryAdd) {
            uint32_t k = FindK(last);
            if (k == last && k > 1u) {
                k >>= 1;
            }
            maxValue = minValue = (k * GetTypeSize(dataType) + ONE_BLK_SIZE - 1u) / ONE_BLK_SIZE * ONE_BLK_SIZE;
        } else {
            uint32_t elePerBlk = ONE_BLK_SIZE / GetTypeSize(dataType);
            uint32_t elePerRep = ONE_REPEAT_BYTE_SIZE / GetTypeSize(dataType);
            if (last <= elePerBlk) {
                maxValue = minValue = 0u;
            } else if (last > elePerBlk && last < elePerRep) {
                maxValue = minValue = first * elePerBlk * GetTypeSize(dataType);
            } else {
                maxValue = minValue = first * elePerRep * GetTypeSize(dataType);
            }
        }
        return;
    }
    uint32_t k = FindK(first);
    if (k == first && first > 1U) {
        k >>= 1U;
    }
    maxValue = minValue = k * ((last * GetTypeSize(dataType) + ONE_BLK_SIZE - 1u) / ONE_BLK_SIZE * ONE_BLK_SIZE);
}

inline void GetReduceSumMeanCommonTmpSize(
    const AscendC::TensorShape& srcShape, ReducePattern pattern, bool isSrcInnerPad, bool isReuseSource,
    uint32_t& maxValue, uint32_t& minValue, std::string apiName, std::string funcName)
{
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    const uint32_t first = static_cast<uint32_t>(shapeDims[0]);
    const uint32_t last = static_cast<uint32_t>(shapeDims[1]);
    CheckParams(shapeDims, isSrcInnerPad, pattern, first, last, apiName, funcName);
    if (isReuseSource) {
        maxValue = minValue = 0U;
        return;
    }
    uint32_t elePerBlk = ONE_BLK_SIZE / FLOAT_TYPE_SIZE;
    if (pattern == ReducePattern::AR) {
        uint32_t k = FindK(last);
        if (k == last && first > 1U) {
            k >>= 1U;
        }
        if (last <= B32_ELEM_NUM_PER_REPEAT) {
            maxValue = minValue = 0U;
        } else {
            maxValue = minValue = (first * k) * FLOAT_TYPE_SIZE;
        }
    } else {
        uint32_t k = FindK(first);
        uint32_t padLast = (last + elePerBlk - 1U) / elePerBlk * elePerBlk;
        if (first == k && first > 1U) {
            k >>= 1U;
        }
        maxValue = minValue = (k * padLast) * FLOAT_TYPE_SIZE;
    }
    return;
}

inline void GetReduceAnyAllCommonTmpSize(
    const AscendC::TensorShape& srcShape, ReducePattern pattern, bool isSrcInnerPad, bool isReuseSource,
    uint32_t& maxValue, uint32_t& minValue, std::string apiName, std::string funcName)
{
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    const uint32_t first = static_cast<uint32_t>(shapeDims[0]);
    const uint32_t last = static_cast<uint32_t>(shapeDims[1]);
    CheckParams(shapeDims, isSrcInnerPad, pattern, first, last, apiName, funcName);
    if (pattern == ReducePattern::AR) {
        uint32_t elePerBlk = static_cast<uint32_t>(ONE_BLK_SIZE / sizeof(uint8_t));
        uint32_t padLast = (last + elePerBlk - 1U) / elePerBlk * elePerBlk;
        minValue = maxValue = static_cast<uint32_t>(padLast * sizeof(uint16_t)) + (first * elePerBlk);
    } else {
        if (isReuseSource) {
            maxValue = minValue = 0U;
            return;
        }
        uint32_t k = FindK(first);
        if (k == first && first > 1U) {
            k >>= 1U;
        }
        maxValue = minValue = k * ((last + ONE_BLK_SIZE - 1U) / ONE_BLK_SIZE * ONE_BLK_SIZE);
    }
    return;
}

void GetReduceProdMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    ASCENDC_HOST_ASSERT(
        dataType == ge::DT_FLOAT, return,
        "[ReduceProd][GetReduceProdMaxMinTmpSize] it only supports float type on this platform.");
    std::vector<int64_t> shapeDims = srcShape.GetDims();
    const uint32_t first = static_cast<uint32_t>(shapeDims[0]);
    const uint32_t last = static_cast<uint32_t>(shapeDims[1]);
    CheckParams(shapeDims, isSrcInnerPad, pattern, first, last, "ReduceProd", "GetReduceProdMaxMinTmpSize");
    if (isReuseSource) {
        minValue = pattern == ReducePattern::AR ? ONE_REPEAT_BYTE_SIZE : 0U;
        maxValue = minValue;
        return;
    }
    uint32_t elePerBlk = ONE_BLK_SIZE / FLOAT_TYPE_SIZE;
    if (pattern == ReducePattern::AR) {
        uint32_t k = FindK(last);
        if (k == last && first > 1U) {
            k >>= 1U;
        }
        uint32_t blkAlignK = elePerBlk > k ? elePerBlk : k;
        maxValue = minValue = (blkAlignK + first * elePerBlk) * FLOAT_TYPE_SIZE + ONE_REPEAT_BYTE_SIZE;
        return;
    }
    uint32_t k = FindK(first);
    if (k == first && first > 1U) {
        k >>= 1U;
    }
    maxValue = minValue = k * ((last * GetTypeSize(dataType) + ONE_BLK_SIZE - 1u) / ONE_BLK_SIZE * ONE_BLK_SIZE);
}

void GetReduceMaxMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201) {
        ASCENDC_HOST_ASSERT(
            dataType == ge::DT_INT8 || dataType == ge::DT_UINT8 || dataType == ge::DT_INT16 ||
                dataType == ge::DT_UINT16 || dataType == ge::DT_FLOAT16 || dataType == ge::DT_BF16 ||
                dataType == ge::DT_INT32 || dataType == ge::DT_UINT32 || dataType == ge::DT_FLOAT ||
                dataType == ge::DT_INT64 || dataType == ge::DT_UINT64,
            return, "[ReduceMax][GetReduceMaxMaxMinTmpSize] it only supports \
                int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t \
                type on this platform.");
    } else {
        ASCENDC_HOST_ASSERT(
            dataType == ge::DT_FLOAT || dataType == ge::DT_FLOAT16, return,
            "[ReduceMax][GetReduceMaxMaxMinTmpSize] it only supports float and half type on this platform.");
    }
    GetReduceCommonMaxMinTmpSize(
        srcShape, dataType, pattern, isSrcInnerPad, isReuseSource, maxValue, minValue, false, "ReduceMax",
        "GetReduceMaxMaxMinTmpSize");
}

void GetReduceMinMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201) {
        ASCENDC_HOST_ASSERT(
            dataType == ge::DT_INT8 || dataType == ge::DT_UINT8 || dataType == ge::DT_INT16 ||
                dataType == ge::DT_UINT16 || dataType == ge::DT_FLOAT16 || dataType == ge::DT_BF16 ||
                dataType == ge::DT_INT32 || dataType == ge::DT_UINT32 || dataType == ge::DT_FLOAT ||
                dataType == ge::DT_INT64 || dataType == ge::DT_UINT64,
            return, "[ReduceMin][GetReduceMinMaxMinTmpSize] it only supports \
                int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t \
                type on this platform.");
    } else {
        ASCENDC_HOST_ASSERT(
            dataType == ge::DT_FLOAT || dataType == ge::DT_FLOAT16, return,
            "[ReduceMin][GetReduceMinMaxMinTmpSize] it only supports float and half type on this platform.");
    }
    GetReduceCommonMaxMinTmpSize(
        srcShape, dataType, pattern, isSrcInnerPad, isReuseSource, maxValue, minValue, false, "ReduceMin",
        "GetReduceMinMaxMinTmpSize");
}

void GetReduceAnyMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    ASCENDC_HOST_ASSERT(
        dataType == ge::DT_FLOAT || dataType == ge::DT_UINT8, return,
        "[ReduceAny][GetReduceAnyMaxMinTmpSize] it only supports float and uint8_t type on this platform.");
    if (dataType == ge::DT_UINT8) {
        GetReduceAnyAllCommonTmpSize(
            srcShape, pattern, isSrcInnerPad, isReuseSource, maxValue, minValue, "ReduceAny",
            "GetReduceAnyMaxMinTmpSize");
    } else {
        GetReduceCommonMaxMinTmpSize(
            srcShape, dataType, pattern, isSrcInnerPad, isReuseSource, maxValue, minValue, false, "ReduceAny",
            "GetReduceAnyMaxMinTmpSize");
    }
}

void GetReduceAllMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    ASCENDC_HOST_ASSERT(
        (dataType == ge::DT_FLOAT || dataType == ge::DT_UINT8), return,
        "[ReduceAll][GetReduceAllMaxMinTmpSize] it only supports float and uint8 type on this platform.");
    if (dataType == ge::DT_UINT8) {
        GetReduceAnyAllCommonTmpSize(
            srcShape, pattern, isSrcInnerPad, isReuseSource, maxValue, minValue, "ReduceAll",
            "GetReduceAllMaxMinTmpSize");
    } else {
        GetReduceCommonMaxMinTmpSize(
            srcShape, dataType, pattern, isSrcInnerPad, isReuseSource, maxValue, minValue, false, "ReduceAll",
            "GetReduceAllMaxMinTmpSize");
    }
}

void GetReduceSumMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
    ASCENDC_HOST_ASSERT((platform != nullptr), return, "Failed to get PlatformAscendC.");
    const auto npuArch = platform->GetCurNpuArch();
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201) {
        ASCENDC_HOST_ASSERT(
            dataType == ge::DT_INT32 || dataType == ge::DT_UINT32 || dataType == ge::DT_FLOAT ||
                dataType == ge::DT_INT64 || dataType == ge::DT_UINT64,
            return,
            "[ReduceSum][GetReduceSumMaxMinTmpSize] it only supports int32_t/uint32_t/float/int64_t/uint64_t type on "
            "this platform.");
    } else {
        ASCENDC_HOST_ASSERT(
            dataType == ge::DT_FLOAT, return,
            "[ReduceSum][GetReduceSumMaxMinTmpSize] it only supports float type on this platform.");
    }
    GetReduceSumMeanCommonTmpSize(
        srcShape, pattern, isSrcInnerPad, isReuseSource, maxValue, minValue, "ReduceSum", "GetReduceSumMaxMinTmpSize");
}

void GetReduceMeanMaxMinTmpSize(
    const AscendC::TensorShape& srcShape, const AscendC::TensorDataType dataType, ReducePattern pattern,
    bool isSrcInnerPad, bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    ASCENDC_HOST_ASSERT(
        dataType == ge::DT_FLOAT, return,
        "[ReduceMean][GetReduceMeanMaxMinTmpSize] it only supports float type on this platform.");
    GetReduceSumMeanCommonTmpSize(
        srcShape, pattern, isSrcInnerPad, isReuseSource, maxValue, minValue, "ReduceMean",
        "GetReduceMeanMaxMinTmpSize");
}
} // namespace AscendC
