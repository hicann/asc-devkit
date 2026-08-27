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
 * \file kernel_utils_struct_norm_sort.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_utils_struct_norm_sort.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_NORM_SORT_H__
#endif
#ifndef ASCENDC_MODULE_UTILS_STRUCT_NORM_SORT_H
#define ASCENDC_MODULE_UTILS_STRUCT_NORM_SORT_H
#include "kernel_utils_mode.h"

namespace AscendC {
struct MatMulInfo {
    const uint16_t m{0};
    const uint16_t n{0};
    const uint16_t k{0};
    const bool isInitOut{false};
    const bool isBias{false};
};

struct DropOutShapeInfo {
    __aicore__ DropOutShapeInfo() {};
    uint32_t firstAxis = 0;
    uint32_t srcLastAxis = 0;
    uint32_t maskLastAxis = 0;
};

struct SelectWithBytesMaskShapeInfo {
    __aicore__ SelectWithBytesMaskShapeInfo() {};
    uint32_t firstAxis = 0;
    uint32_t srcLastAxis = 0;
    uint32_t maskLastAxis = 0;
};

template <typename T>
class LocalTensor;
template <typename T>
class GlobalTensor;

template <typename T>
struct LayerNormParams {
    __aicore__ LayerNormParams() {};
    LocalTensor<T> tempTensorA;
    LocalTensor<T> tempTensorB;
    LocalTensor<T> tempTensorC;
    LocalTensor<T> meanTmpTensor;
    LocalTensor<T> varianceTmpTensor;
};

template <typename T>
struct BatchNormParams {
    __aicore__ BatchNormParams() {};
    float firstDimValueBack = 1.0;
    uint8_t srcRepeatStride = 1;
    uint32_t srcOffset = 1;
    uint32_t basicLoop = 0;
    uint32_t brcRepeatTimes = 0;
    uint32_t oriBloop = 0;
    uint32_t oriBTail = 0;
    uint32_t oriBTmpLoopOffset = 0;
    uint32_t oriBTmpTailOffset = 0;
    uint32_t oriBOutLoopOffset = 0;
    uint32_t oriBOutTailOffset = 0;
    uint32_t reduceAddLoop = 0;
    uint32_t reduceAddTail = 0;
    uint32_t reduceAddTailOffset = 0;

    LocalTensor<T> tempTensorA;
    LocalTensor<T> tempTensorB;
    LocalTensor<T> tempTensorC;
    LocalTensor<T> meanTmpTensor;
    LocalTensor<T> varianceTmpTensor;
};

template <typename T>
struct DeepNormParams {
    __aicore__ DeepNormParams() {};
    float lastDimValueBack = 1.0;

    LocalTensor<T> tempTensorA;
    LocalTensor<T> tempTensorB;
    LocalTensor<T> tempTensorC;
    LocalTensor<T> meanTmpTensor;
    LocalTensor<T> varianceTmpTensor;
};

template <typename T>
struct ExpParams {
    __aicore__ ExpParams() {};
    uint32_t inputSize = 0;         // total data num
    uint32_t oneTmpSize = 0;        // data num in one tmp buffer
    uint32_t firstTmpStartPos = 0;  // first tmp buffer start position
    uint32_t secondTmpStartPos = 0; // second tmp buffer start position
    uint32_t thirdTmpStartPos = 0;  // third tmp buffer start position
    uint32_t fourthTmpStartPos = 0; // fourth tmp buffer start position
    uint32_t loopNum = 0;           // how many loop for main block calculation
    uint32_t tailSize = 0;          // tail block
    uint32_t tailPos = 0;           // tail block start pos
    uint32_t curDataLength = 0;     // current data num for calculation
    uint32_t expandLevel = 0;       // taylor param expand level

    LocalTensor<T> tempTensorFloorX;    // FP32 (x - floor(x)) for Taylor calculation
    LocalTensor<T> tempTensorFloorXPow; // FP32 to calculate (x - floor(x)) ^ n
    LocalTensor<T> tempTensorRes;       // FP32 result to store sum
    LocalTensor<T> tempTensorIntPart;   // FP32 exp(floor(x)) result
};

template <typename T>
struct AntiquantParams {
    __aicore__ AntiquantParams() {};
    LocalTensor<T> tempTensorOffset; // FP32 offset after brcb           8 * N
    LocalTensor<T> tempTensorScale;  // FP32 scale after brcb            8 * N
    LocalTensor<T> tempTensorInput;  // partial FP32 input after cast
};

template <typename T, typename U>
struct DropOutParams {
    __aicore__ DropOutParams() {};
    uint32_t dataSize = 0;
    uint32_t stackBufferSize = 0;
    uint32_t repeatTimes = 1;

    uint32_t maxRepeatSize = 0;
    uint32_t oneRepeatSize = 0;

    uint32_t currentSize = 0;
    uint32_t repeatRounding = 0;
    uint32_t repeatRemaining = 0;
    uint32_t repeatTail = 0;

    LocalTensor<T> firstLocal;
    LocalTensor<U> secondLocal;
};

template <typename T, typename U>
struct PowerFParams {
    __aicore__ PowerFParams() {};
    LocalTensor<T> tmpTensor1;
    LocalTensor<T> tmpTensor2;
    LocalTensor<T> tmpTensor3;
    LocalTensor<U> tmpMask1;
    LocalTensor<U> tmpMask2;
    LocalTensor<U> tmpMask3;
    LocalTensor<U> finiteIntegerYMask;
};

template <typename T, typename U>
struct PowerIParams {
    __aicore__ PowerIParams() {};
    float expIterateSum;

    LocalTensor<T> expUBIterate;
    LocalTensor<T> oriAbsExp;
    LocalTensor<T> recordExpNode;
    LocalTensor<T> tmpTensor1;
    LocalTensor<T> tmpTensor2;
    LocalTensor<U> negMask;
    LocalTensor<U> mask;
    LocalTensor<U> tmpScalar;
};

template <typename T>
struct GeluParams {
    __aicore__ GeluParams() {};
    uint32_t repeatTimes = 1;

    uint32_t currentSize = 0;
    uint32_t repeatRounding = 0;
    uint32_t repeatRemaining = 0;
    uint32_t tail = 0;

    uint32_t maxRepeatSize = 0;
    uint32_t oneRepeatSize = 0;

    uint32_t dataSize = 0;
    uint32_t stackSize = 0;
    uint32_t tmpBufferSize = 0;
    LocalTensor<T> sharedTmpBuffer;

    LocalTensor<T> tempTensorConv;
    LocalTensor<T> tempTensorA;
    LocalTensor<T> tempTensorB;
    LocalTensor<T> tempTensorC;
};

template <typename T>
struct TanhParams {
    __aicore__ TanhParams() {};
    uint32_t repeatTimes = 1;
    uint32_t calCount = 0;
    uint32_t stackSize = 0;
    uint32_t tmpBufferSize = 0;
    LocalTensor<T> sharedTmpBuffer;

    LocalTensor<T> tempTensorConv;
    LocalTensor<T> tmpClip;
};

template <typename T>
struct AscendDequantParams {
    __aicore__ AscendDequantParams() {};
    uint64_t tmpSize;

    LocalTensor<T> tmpAddrA;
    LocalTensor<T> tmpAddrB;
};

template <typename T>
constexpr __aicore__ inline uint64_t GetScalarBitcodeValue(T scalarValue)
{
    union ScalarBitcode {
        __callee__ ScalarBitcode() {}
        T input;
        uint64_t output;
    } data;

    data.input = scalarValue;
    return data.output;
}

template <typename T, typename U>
constexpr __aicore__ inline U GetScalarBitcodeValue(T scalarValue)
{
    union ScalarBitcode {
        __callee__ ScalarBitcode() {}
        T input;
        U output;
    } data;

    data.input = scalarValue;
    return static_cast<U>(data.output);
}

template <typename T>
constexpr __aicore__ inline half GetScalarBitcodeToHalf(T scalarValue)
{
    union ScalarBitcode {
        __callee__ ScalarBitcode() {}
        T input;
        half output;
    } data;

    data.input = scalarValue;
    return data.output;
}
} // namespace AscendC
#endif // ASCENDC_MODULE_UTILS_STRUCT_NORM_SORT_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_NORM_SORT_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_UTILS_STRUCT_NORM_SORT_H__
#endif
