/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "ccu_runtime_checker.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <sstream>

namespace HcclSim {
namespace CcuSt {

namespace {

void ApplyCopy(
    std::vector<uint8_t>& dst, uint64_t dstOffset, const std::vector<uint8_t>& src, uint64_t srcOffset, uint64_t size)
{
    std::copy_n(
        src.begin() + static_cast<size_t>(srcOffset), static_cast<size_t>(size),
        dst.begin() + static_cast<size_t>(dstOffset));
}

uint32_t DataTypeSize(HcclDataType dataType)
{
    switch (dataType) {
        case HCCL_DATA_TYPE_INT8:
            return 1;
        case HCCL_DATA_TYPE_FP16:
        case HCCL_DATA_TYPE_BFP16:
            return 2;
        case HCCL_DATA_TYPE_INT32:
        case HCCL_DATA_TYPE_FP32:
            return 4;
        case HCCL_DATA_TYPE_INT64:
        case HCCL_DATA_TYPE_UINT64:
        case HCCL_DATA_TYPE_FP64:
            return 8;
        default:
            return 0;
    }
}

float Float16ToFloat(uint16_t bits)
{
    uint32_t sign = (bits >> 15) & 0x1;
    uint32_t exponent = (bits >> 10) & 0x1F;
    uint32_t mantissa = bits & 0x3FF;
    uint32_t floatBits = sign << 31;
    if (exponent == 0) {
        if (mantissa != 0) {
            exponent = 1;
            while ((mantissa & 0x400) == 0) {
                mantissa <<= 1;
                exponent--;
            }
            mantissa &= 0x3FF;
            floatBits |= (127 + exponent - 14) << 23;
            floatBits |= mantissa << 13;
        }
    } else if (exponent == 0x1F) {
        floatBits |= 0x7F800000 | (mantissa << 13);
    } else {
        floatBits |= (exponent + 127 - 15) << 23;
        floatBits |= mantissa << 13;
    }
    float result;
    std::memcpy(&result, &floatBits, sizeof(float));
    return result;
}

uint16_t FloatToFloat16(float value)
{
    uint32_t floatBits;
    std::memcpy(&floatBits, &value, sizeof(float));
    uint16_t sign = (floatBits >> 16) & 0x8000;
    int32_t exponent = ((floatBits >> 23) & 0xFF) - 127 + 15;
    uint32_t mantissa = floatBits & 0x7FFFFF;
    if (exponent <= 0) {
        if (exponent < -10) {
            return sign;
        }
        mantissa |= 0x800000;
        uint16_t rounded = static_cast<uint16_t>(mantissa >> (14 - exponent));
        return sign | rounded;
    } else if (exponent >= 0x1F) {
        return sign | 0x7C00 | (mantissa ? 0x200 : 0);
    } else {
        return sign | (exponent << 10) | static_cast<uint16_t>(mantissa >> 13);
    }
}

float Bfloat16ToFloat(uint16_t bits)
{
    uint32_t floatBits = static_cast<uint32_t>(bits) << 16;
    float result;
    std::memcpy(&result, &floatBits, sizeof(float));
    return result;
}

uint16_t FloatToBfloat16(float value)
{
    uint32_t floatBits;
    std::memcpy(&floatBits, &value, sizeof(float));
    uint32_t rounded = floatBits + 0x7FFF + ((floatBits >> 16) & 1);
    return static_cast<uint16_t>(rounded >> 16);
}

template <typename T>
void ReduceElement(T& dst, T src, HcclReduceOp op)
{
    switch (op) {
        case HCCL_REDUCE_SUM:
            dst += src;
            break;
        case HCCL_REDUCE_PROD:
            dst *= src;
            break;
        case HCCL_REDUCE_MAX:
            dst = std::max(dst, src);
            break;
        case HCCL_REDUCE_MIN:
            dst = std::min(dst, src);
            break;
        default:
            break;
    }
}

void ApplyReduce(
    std::vector<uint8_t>& dst, uint64_t dstOffset, const std::vector<uint8_t>& src, uint64_t srcOffset, uint64_t size,
    HcclDataType dataType, HcclReduceOp reduceOp)
{
    const uint32_t elemSize = DataTypeSize(dataType);
    if (elemSize == 0 || size % elemSize != 0) {
        ApplyCopy(dst, dstOffset, src, srcOffset, size);
        return;
    }
    const uint64_t elemCount = size / elemSize;
    for (uint64_t i = 0; i < elemCount; ++i) {
        const size_t dstPos = static_cast<size_t>(dstOffset + i * elemSize);
        const size_t srcPos = static_cast<size_t>(srcOffset + i * elemSize);
        switch (dataType) {
            case HCCL_DATA_TYPE_INT8:
                ReduceElement(
                    reinterpret_cast<int8_t&>(dst[dstPos]), reinterpret_cast<const int8_t&>(src[srcPos]), reduceOp);
                break;
            case HCCL_DATA_TYPE_FP16: {
                uint16_t dstBits;
                uint16_t srcBits;
                std::memcpy(&dstBits, &dst[dstPos], sizeof(uint16_t));
                std::memcpy(&srcBits, &src[srcPos], sizeof(uint16_t));
                float dstFloat = Float16ToFloat(dstBits);
                float srcFloat = Float16ToFloat(srcBits);
                ReduceElement(dstFloat, srcFloat, reduceOp);
                dstBits = FloatToFloat16(dstFloat);
                std::memcpy(&dst[dstPos], &dstBits, sizeof(uint16_t));
                break;
            }
            case HCCL_DATA_TYPE_BFP16: {
                uint16_t dstBits;
                uint16_t srcBits;
                std::memcpy(&dstBits, &dst[dstPos], sizeof(uint16_t));
                std::memcpy(&srcBits, &src[srcPos], sizeof(uint16_t));
                float dstFloat = Bfloat16ToFloat(dstBits);
                float srcFloat = Bfloat16ToFloat(srcBits);
                ReduceElement(dstFloat, srcFloat, reduceOp);
                dstBits = FloatToBfloat16(dstFloat);
                std::memcpy(&dst[dstPos], &dstBits, sizeof(uint16_t));
                break;
            }
            case HCCL_DATA_TYPE_INT32:
                ReduceElement(
                    reinterpret_cast<int32_t&>(dst[dstPos]), reinterpret_cast<const int32_t&>(src[srcPos]), reduceOp);
                break;
            case HCCL_DATA_TYPE_FP32:
                ReduceElement(
                    reinterpret_cast<float&>(dst[dstPos]), reinterpret_cast<const float&>(src[srcPos]), reduceOp);
                break;
            default:
                break;
        }
    }
}

} // namespace

CheckResult CaptureRuntimeExpectation(
    const ScenarioData& data, const CcuStScenario& scenario, RuntimeExpectation& expectation)
{
    const size_t rankSize = data.memories.size();
    if (rankSize == 0) {
        return {Result::PARAM_ERROR, "no memories in scenario data"};
    }
    expectation.outputs.resize(rankSize);
    for (size_t rank = 0; rank < rankSize; ++rank) {
        expectation.outputs[rank] = data.memories[rank].output;
    }
    for (size_t srcRank = 0; srcRank < rankSize; ++srcRank) {
        for (size_t dstRank = 0; dstRank < rankSize; ++dstRank) {
            if (srcRank >= data.srcOffsets.size() || dstRank >= data.srcOffsets[srcRank].size()) {
                continue;
            }
            const uint64_t srcOffset = data.srcOffsets[srcRank][dstRank];
            const uint64_t dstOffset = data.dstOffsets[srcRank][dstRank];
            const uint64_t size = scenario.sizes[srcRank][dstRank];
            const auto& src = data.memories[srcRank].input;
            auto& dst = expectation.outputs[dstRank];
            if (srcOffset + size > src.size() || dstOffset + size > dst.size()) {
                return {Result::OUT_OF_RANGE, "expected transfer is out of range"};
            }
            if (data.reduceOp == HCCL_REDUCE_RESERVED || srcRank == 0) {
                ApplyCopy(dst, dstOffset, src, srcOffset, size);
            } else {
                ApplyReduce(dst, dstOffset, src, srcOffset, size, scenario.dataType, data.reduceOp);
            }
        }
    }
    return {};
}

CheckResult CheckRuntimeOutput(
    const std::vector<RankMemory>& memories, const RuntimeExpectation& expectation, const RuntimeWorld& world)
{
    if (expectation.outputs.size() != memories.size()) {
        return {Result::CONTRACT_ERROR, "expected output rank count does not match memories"};
    }
    for (size_t rank = 0; rank < memories.size(); ++rank) {
        if (memories[rank].output != expectation.outputs[rank]) {
            const auto mismatch = std::mismatch(
                memories[rank].output.begin(), memories[rank].output.end(), expectation.outputs[rank].begin());
            std::ostringstream message;
            message << "output mismatch at rank " << rank;
            if (mismatch.first != memories[rank].output.end()) {
                size_t pos = std::distance(memories[rank].output.begin(), mismatch.first);
                message << ", byte " << pos;
                message << " actual[0x" << std::hex << static_cast<int>(*mismatch.first) << std::dec;
                message << " expected[0x" << std::hex << static_cast<int>(*mismatch.second) << std::dec;
                if (pos + 1 < memories[rank].output.size()) {
                    message << " actual+1[0x" << std::hex << static_cast<int>(memories[rank].output[pos + 1])
                            << std::dec;
                    message << " expected+1[0x" << std::hex << static_cast<int>(expectation.outputs[rank][pos + 1])
                            << std::dec;
                }
            }
            return {Result::CONTRACT_ERROR, message.str()};
        }
    }
    return {};
}

} // namespace CcuSt
} // namespace HcclSim
