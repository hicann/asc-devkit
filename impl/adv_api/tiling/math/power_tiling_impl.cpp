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
 * \file power_tiling_impl.cpp
 * \brief
 */
#include "adv_api/utils/types.h"
#include "adv_api/math/power_tiling.h"
#include "utils/tiling/platform/platform_ascendc.h"

#include <cstdint>
#include "../../detail/host_log.h"

namespace AscendC {
namespace {
constexpr uint32_t POWER_MIN_TMP_SIZE = 256;
constexpr uint32_t REPEAT_BYTE_SIZE = 256;

// Power(tensor, tensor)
// If the data type is half, twice the half space is required.
constexpr uint32_t POW_TENSOR_TENSOR_HALF_CALC_PROC = 14;
constexpr uint32_t POW_TENSOR_TENSOR_INT_CALC_PROC = 6;
constexpr uint32_t POW_TENSOR_TENSOR_FLOAT_CALC_PROC = 4;
constexpr uint32_t POW_TENSOR_TENSOR_HALF_CALC_PROC_V200 = 16;
constexpr uint32_t POW_TENSOR_TENSOR_INT_CALC_PROC_V200 = 7;
constexpr uint32_t POW_TENSOR_TENSOR_FLOAT_CALC_PROC_V200 = 5;

// Power(tensor, scalar) or Power(scalar, tensor)
// If the data type is half, twice the half space is required.
constexpr uint32_t POW_TENSOR_SCALAR_HALF_CALC_PROC = 14;
constexpr uint32_t POW_TENSOR_SCALAR_INT_CALC_PROC = 7;
constexpr uint32_t POW_TENSOR_SCALAR_FLOAT_CALC_PROC = 5;
constexpr uint32_t POW_TENSOR_SCALAR_HALF_CALC_PROC_V200 = 16;
constexpr uint32_t POW_TENSOR_SCALAR_INT_CALC_PROC_V200 = 8;
constexpr uint32_t POW_TENSOR_SCALAR_FLOAT_CALC_PROC_V200 = 6;

constexpr uint32_t POW_HALF_CALC_PROC = 2;
constexpr uint32_t POW_DOUBLE = 2;
constexpr uint32_t POW_TRIPLE = 3;
constexpr uint32_t FLOAT_SIZE = 4;

struct PowTiling {
    // TT means Power(Tensor, Tensor)
    uint32_t halfTT = 0;
    uint32_t intTT = 0;
    uint32_t floatTT = 0;
    // TS means Power(Tensor, Scalar), Power(Scalar, Tensor)
    uint32_t halfTS = 0;
    uint32_t intTS = 0;
    uint32_t floatTS = 0;

    constexpr PowTiling(
        uint32_t halfTTIn, uint32_t intTTIn, uint32_t floatTTIn, uint32_t halfTSIn, uint32_t intTSIn,
        uint32_t floatTSIn)
        : halfTT(halfTTIn), intTT(intTTIn), floatTT(floatTTIn), halfTS(halfTSIn), intTS(intTSIn), floatTS(floatTSIn)
    {}
};

constexpr PowTiling POW_TILING_PARAM_V220(
    POW_TENSOR_TENSOR_HALF_CALC_PROC, POW_TENSOR_TENSOR_INT_CALC_PROC, POW_TENSOR_TENSOR_FLOAT_CALC_PROC,
    POW_TENSOR_SCALAR_HALF_CALC_PROC, POW_TENSOR_SCALAR_INT_CALC_PROC, POW_TENSOR_SCALAR_FLOAT_CALC_PROC);

constexpr PowTiling POW_TILING_PARAM_V200(
    POW_TENSOR_TENSOR_HALF_CALC_PROC_V200, POW_TENSOR_TENSOR_INT_CALC_PROC_V200, POW_TENSOR_TENSOR_FLOAT_CALC_PROC_V200,
    POW_TENSOR_SCALAR_HALF_CALC_PROC_V200, POW_TENSOR_SCALAR_INT_CALC_PROC_V200,
    POW_TENSOR_SCALAR_FLOAT_CALC_PROC_V200);

/*
Determine whether the input and output shapes are tensors.
*/
inline bool ShapeIsTensor(const AscendC::TensorShape& shape)
{
    // The shape is one dimension and the length of the dimension is one, which is a scalar.
    if (shape.GetDimNum() == 1 && shape.GetDim(0) == 1) {
        return false;
    }
    return true;
}

inline uint32_t GetPowerMaxTmpSizeArch3510(
    const AscendC::TensorShape& srcShape1, const AscendC::TensorShape& srcShape2, const bool typeIsInt,
    const bool isReuseSource)
{
    (void)(isReuseSource);
    std::vector<int64_t> shapeDims1 = srcShape1.GetDims();
    std::vector<int64_t> shapeDims2 = srcShape2.GetDims();
    const bool src1IsTensor = ShapeIsTensor(srcShape1);
    const bool src2IsTensor = ShapeIsTensor(srcShape2);
    uint32_t maxTmpSize = 0;
    if (!typeIsInt) {
        uint32_t inputSize = 1;
        if (src1IsTensor) {
            for (const auto dim : shapeDims1) {
                inputSize *= dim;
            }
        } else if (src2IsTensor) {
            for (const auto dim : shapeDims2) {
                inputSize *= dim;
            }
        }

        maxTmpSize = FLOAT_SIZE * inputSize * POW_TRIPLE;
    }

    return maxTmpSize;
}

inline uint32_t GetPowerMinTmpSize(
    const NpuArch npuArch, const AscendC::TensorShape& srcShape1, const AscendC::TensorShape& srcShape2,
    const bool typeIsInt, const uint32_t typeSize, const bool isReuseSource)
{
    (void)(isReuseSource);
    struct PowTiling param(0, 0, 0, 0, 0, 0);
    if (npuArch == NpuArch::DAV_2201) {
        param = POW_TILING_PARAM_V220;
    } else if (npuArch == NpuArch::DAV_2002) {
        param = POW_TILING_PARAM_V200;
    }
    const bool src1IsTensor = ShapeIsTensor(srcShape1);
    const bool src2IsTensor = ShapeIsTensor(srcShape2);
    const bool isPowTensorTensor = src1IsTensor && src2IsTensor;
    uint32_t minTmpSize = 0;

    if (typeIsInt) {
        minTmpSize = isPowTensorTensor ? POWER_MIN_TMP_SIZE * param.intTT : POWER_MIN_TMP_SIZE * param.intTS;
    } else if (typeSize == sizeof(float)) {
        minTmpSize = (isPowTensorTensor ? POWER_MIN_TMP_SIZE * param.floatTT : POWER_MIN_TMP_SIZE * param.floatTS) +
                     REPEAT_BYTE_SIZE;
    } else {
        minTmpSize = isPowTensorTensor ? POWER_MIN_TMP_SIZE * param.halfTT : POWER_MIN_TMP_SIZE * param.halfTS;
        minTmpSize = minTmpSize / POW_HALF_CALC_PROC + REPEAT_BYTE_SIZE;
    }
    return minTmpSize;
}

inline uint32_t GetPowerMaxTmpSize(
    const NpuArch npuArch, const AscendC::TensorShape& srcShape1, const AscendC::TensorShape& srcShape2,
    const bool typeIsInt, const uint32_t typeSize, const bool isReuseSource)
{
    (void)(isReuseSource);
    struct PowTiling param(0, 0, 0, 0, 0, 0);
    if (npuArch == NpuArch::DAV_2201) {
        param = POW_TILING_PARAM_V220;
    } else if (npuArch == NpuArch::DAV_2002) {
        param = POW_TILING_PARAM_V200;
    }
    std::vector<int64_t> shapeDims1 = srcShape1.GetDims();
    std::vector<int64_t> shapeDims2 = srcShape2.GetDims();
    const bool src1IsTensor = ShapeIsTensor(srcShape1);
    const bool src2IsTensor = ShapeIsTensor(srcShape2);
    const bool isPowTensorTensor = src1IsTensor && src2IsTensor;

    uint32_t inputSize = 1;
    if (src1IsTensor) { // include two interface: Power(tensor, tensor) and Power(tensor, scalar)
        for (const auto dim : shapeDims1) {
            inputSize *= dim;
        }
    } else { // include one interface: Power(scalar, tensor)
        for (const auto dim : shapeDims2) {
            inputSize *= dim;
        }
    }

    uint32_t maxTmpSize = 0;
    uint32_t calcProc = 0;
    if (typeSize == sizeof(float)) {
        calcProc = inputSize * typeSize > POWER_MIN_TMP_SIZE ? inputSize * typeSize : POWER_MIN_TMP_SIZE;
    } else {
        calcProc = std::max(inputSize * typeSize, POWER_MIN_TMP_SIZE / POW_HALF_CALC_PROC);
    }
    if (typeIsInt) {
        maxTmpSize = isPowTensorTensor ? calcProc * param.intTT : calcProc * param.intTS;
    } else if (typeSize == sizeof(float)) {
        maxTmpSize = (isPowTensorTensor ? calcProc * param.floatTT : calcProc * param.floatTS) + REPEAT_BYTE_SIZE;
    } else {
        maxTmpSize = (isPowTensorTensor ? calcProc * param.halfTT : calcProc * param.halfTS) + REPEAT_BYTE_SIZE;
    }
    return maxTmpSize;
}

/*
Determine the power interface type based on the two input bool values of the interface.
Return the number of required nodes or buffer size based on the interface type and typeSize.
*/
void GetPowerTmpBufferFactorSize(
    const NpuArch npuArch, const bool baseIsTensor, const bool expIsTensor, const bool typeIsInt,
    const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
{
    struct PowTiling param(0, 0, 0, 0, 0, 0);
    if (npuArch == NpuArch::DAV_2201) {
        param = POW_TILING_PARAM_V220;
    } else if (npuArch == NpuArch::DAV_2002) {
        param = POW_TILING_PARAM_V200;
    }
    bool isPowTensorTensor = baseIsTensor && expIsTensor;
    extraBuffer = REPEAT_BYTE_SIZE;
    if (typeIsInt) {
        extraBuffer = 0;
        maxLiveNodeCount = isPowTensorTensor ? param.intTT : param.intTS;
    } else if (typeSize == sizeof(float)) {
        maxLiveNodeCount = isPowTensorTensor ? param.floatTT : param.floatTS;
    } else {
        maxLiveNodeCount = isPowTensorTensor ? param.halfTT : param.halfTS;
    }
}

void GetPowerTmpBufferFactorSizeArch3510(
    const bool typeIsInt, const uint32_t typeSize, uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
{
    extraBuffer = 0u;
    if (typeIsInt) {
        maxLiveNodeCount = 0u;
    } else if (typeSize == sizeof(float)) {
        maxLiveNodeCount = POW_TRIPLE;
    } else {
        maxLiveNodeCount = POW_DOUBLE * POW_TRIPLE;
    }
}
} // namespace

void GetPowerMaxMinTmpSize(
    const AscendC::TensorShape& srcShape1, const AscendC::TensorShape& srcShape2, const bool typeIsInt,
    const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
{
    ASCENDC_HOST_ASSERT(
        platform_ascendc::PlatformAscendCManager::GetInstance() != nullptr, return,
        "PlatformAscendCManager gets instance failed!");
    auto npuArch = platform_ascendc::PlatformAscendCManager::GetInstance()->GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_2201 || npuArch == NpuArch::DAV_2002 || npuArch == NpuArch::DAV_3510 ||
         npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102 || npuArch == NpuArch::DAV_3003 ||
         npuArch == NpuArch::DAV_3113),
        return, "GetPowerMaxMinTmpSize is not supported on current device!");
    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102 ||
        npuArch == NpuArch::DAV_3003 || npuArch == NpuArch::DAV_3113) {
        maxValue = GetPowerMaxTmpSizeArch3510(srcShape1, srcShape2, typeIsInt, isReuseSource);
        minValue = maxValue;
    } else {
        maxValue = GetPowerMaxTmpSize(npuArch, srcShape1, srcShape2, typeIsInt, typeSize, isReuseSource);
        minValue = GetPowerMinTmpSize(npuArch, srcShape1, srcShape2, typeIsInt, typeSize, isReuseSource);
    }
}

void GetPowerTmpBufferFactorSize(
    const bool baseIsTensor, const bool expIsTensor, const bool typeIsInt, const uint32_t typeSize,
    uint32_t& maxLiveNodeCount, uint32_t& extraBuffer)
{
    ASCENDC_HOST_ASSERT(
        platform_ascendc::PlatformAscendCManager::GetInstance() != nullptr, return,
        "PlatformAscendCManager gets instance failed!");
    const auto npuArch = platform_ascendc::PlatformAscendCManager::GetInstance()->GetCurNpuArch();
    ASCENDC_HOST_ASSERT(
        (npuArch == NpuArch::DAV_2201 || npuArch == NpuArch::DAV_2002 || npuArch == NpuArch::DAV_3510 ||
         npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102 || npuArch == NpuArch::DAV_3003 ||
         npuArch == NpuArch::DAV_3113),
        return, "GetPowerTmpBufferFactorSize is not supported on current device!");

    if (npuArch == NpuArch::DAV_3510 || npuArch == NpuArch::DAV_9201 || npuArch == NpuArch::DAV_5102 ||
        npuArch == NpuArch::DAV_3003 || npuArch == NpuArch::DAV_3113) {
        GetPowerTmpBufferFactorSizeArch3510(typeIsInt, typeSize, maxLiveNodeCount, extraBuffer);
    } else {
        GetPowerTmpBufferFactorSize(
            npuArch, baseIsTensor, expIsTensor, typeIsInt, typeSize, maxLiveNodeCount, extraBuffer);
    }
}
} // namespace AscendC
