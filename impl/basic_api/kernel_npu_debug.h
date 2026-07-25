/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_npu_debug.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_npu_debug.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_mm_intf_impl.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_NPU_DEBUG_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_KERNEL_NPU_DEBUG_H
#define ASCENDC_MODULE_OPERATOR_KERNEL_NPU_DEBUG_H

#include "kernel_log.h"
#include "utils/kernel_utils_ceil_oom_que.h"
#include "utils/kernel_utils_constants.h"
#include "../../include/basic_api/kernel_tensor.h"

namespace AscendC {

namespace Internal {

template <typename T>
__aicore__ inline bool CheckIsZero(T param)
{
    // 910B does not support scalar comparison for half / bf16 / double etc. Convert to uint for comparison.
    if constexpr (sizeof(T) == 1) {
        return *reinterpret_cast<const uint8_t*>(&param) == 0;
    } else if constexpr (sizeof(T) == 2) {
        return *reinterpret_cast<const uint16_t*>(&param) == 0;
    } else if constexpr (sizeof(T) == 4) {
        return *reinterpret_cast<const uint32_t*>(&param) == 0;
    } else if constexpr (sizeof(T) == 8) {
        return *reinterpret_cast<const uint64_t*>(&param) == 0;
    }
    return true;
}

} // namespace Internal

template <typename T>
struct NamedTensor {
    __aicore__ NamedTensor(const LocalTensor<T>& tensorIn, const __gm__ char* tensorNameIn)
        : tensor(tensorIn), tensorName(tensorNameIn)
    {}

    LocalTensor<T> tensor;
    const __gm__ char* tensorName;
};

__aicore__ inline __gm__ const char* GetTPositionSuffix(TPosition pos)
{
    switch (pos) {
        case TPosition::GM:
            return "";
        case TPosition::A1:
            return "(A1)";
        case TPosition::B1:
            return "(B1)";
        case TPosition::C1:
            return "(C1)";
        case TPosition::A2:
            return "(A2)";
        case TPosition::B2:
            return "(B2)";
        case TPosition::C2:
            return "(C2)";
        case TPosition::CO1:
            return "(CO1)";
        case TPosition::CO2:
            return "(CO2)";
        case TPosition::VECIN:
            return "(VECIN)";
        case TPosition::VECOUT:
            return "(VECOUT)";
        case TPosition::VECCALC:
            return "(VECCALC/LCM)";
        case TPosition::SPM:
            return "(SPM/SHM)";
        case TPosition::TSCM:
            return "(TSCM)";
        case TPosition::C2PIPE2GM:
            return "(C2PIPE2GM)";
        case TPosition::C2PIPE2LOCAL:
            return "(C2PIPE2LOCAL)";
        case TPosition::MAX:
            return "(MAX)";
        default:
            return "(UNKNOWN)";
    }
}

__aicore__ inline __gm__ const char* GetHardwareName(Hardware pos)
{
    switch (pos) {
        case Hardware::GM:
            return "GM";
        case Hardware::UB:
            return "UB";
        case Hardware::L1:
            return "L1 Buffer";
        case Hardware::L0A:
            return "L0A Buffer";
        case Hardware::L0B:
            return "L0B Buffer";
        case Hardware::L0C:
            return "L0C Buffer";
        case Hardware::BIAS:
            return "BiasTable Buffer";
        case Hardware::FIXBUF:
            return "Fixpipe Buffer";
        default:
            return "UNKNOWN";
    }
}

template <typename T>
__aicore__ inline void ReportNopWarning(T param, const __gm__ char* paramName, const __gm__ char* apiName)
{
    ASCENDC_DEBUG_WARNING(
        (param != 0), KERNEL_LOG_INTERNAL(
                          KERNEL_WARN, "The value of %s in %s equals 0, which makes %s equivalent to a NOP.\n",
                          paramName, apiName, apiName));
}

__aicore__ inline void ReportNotSupport(bool isSupported, const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        (isSupported), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "%s is not supported on current device.\n", apiName));
}

template <Hardware... expectedPos, typename T>
__aicore__ inline void CheckTensorPhyPosition(
    const LocalTensor<T>& inputTensor, const __gm__ char* tensorName, const __gm__ char* posName,
    const __gm__ char* apiName)
{
    const TPosition tpos = static_cast<TPosition>(inputTensor.GetPosition());
    const Hardware currentPos = GetPhyType(tpos);
    bool posValid = ((currentPos == expectedPos) || ...);
    ASCENDC_DEBUG_ASSERT(
        (posValid), KERNEL_LOG_INTERNAL(
                        KERNEL_ERROR,
                        "Failed to check %s tensor position in %s, "
                        "supported positions are %s, current position is %s%s.\n",
                        tensorName, apiName, posName, GetHardwareName(currentPos), GetTPositionSuffix(tpos)));
}

template <typename T>
__aicore__ inline void CheckTensorAlignment(
    const LocalTensor<T>& inputTensor, uint32_t alignBytes, const __gm__ char* tensorName, const __gm__ char* apiName)
{
    uint64_t tensorAddr = reinterpret_cast<uint64_t>(inputTensor.GetPhyAddr());
#ifdef ASCENDC_CPU_DEBUG
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    TPosition tensorPos = static_cast<TPosition>(inputTensor.GetPosition());
    tensorAddr = tensorAddr - reinterpret_cast<uint64_t>(
                                  ConstDefiner::Instance().hardwareCpuBufferMap.at(positionHardMap.at(tensorPos)));
#endif
    ASCENDC_DEBUG_ASSERT(
        (tensorAddr % alignBytes == 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check %s tensor "
            "start address alignment in %s, its start address must align with %u bytes, current start address is "
            "%llu.\n",
            tensorName, apiName, alignBytes, static_cast<unsigned long long>(tensorAddr)));
}

__aicore__ inline void CheckAddrAlignment(
    uint64_t addr, Hardware phyPos, uint32_t alignBytes, const __gm__ char* addrName, const __gm__ char* apiName)
{
    uint64_t checkAddr = addr;
#ifdef ASCENDC_CPU_DEBUG
    checkAddr = addr - reinterpret_cast<uint64_t>(ConstDefiner::Instance().hardwareCpuBufferMap.at(phyPos));
#endif
    ASCENDC_DEBUG_ASSERT(
        (checkAddr % alignBytes == 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check %s "
            "address alignment in %s, its address must align with %u bytes, current address is %llu.\n",
            addrName, apiName, alignBytes, static_cast<unsigned long long>(checkAddr)));
}

template <typename T>
__aicore__ inline void CheckValueRange(
    T value, T valueLow, T valueHigh, const __gm__ char* paramName, const __gm__ char* apiName)
{
    if constexpr (SupportType<T, int8_t, int16_t, int32_t, int64_t>()) {
        int64_t castValue = static_cast<int64_t>(value);
        int64_t castLow = static_cast<int64_t>(valueLow);
        int64_t castHigh = static_cast<int64_t>(valueHigh);
        ASCENDC_DEBUG_ASSERT(
            (value <= valueHigh && value >= valueLow),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to "
                "check %s value in %s, its valid range is [%lld, %lld], current value is %lld.\n",
                paramName, apiName, static_cast<long long>(castLow), static_cast<long long>(castHigh),
                static_cast<long long>(castValue)));
    } else if constexpr (SupportType<T, uint8_t, uint16_t, uint32_t, uint64_t>()) {
        uint64_t castValue = static_cast<uint64_t>(value);
        uint64_t castLow = static_cast<uint64_t>(valueLow);
        uint64_t castHigh = static_cast<uint64_t>(valueHigh);
        ASCENDC_DEBUG_ASSERT(
            (value <= valueHigh && value >= valueLow),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to "
                "check %s value in %s, its valid range is [%llu, %llu], current value is %llu.\n",
                paramName, apiName, static_cast<unsigned long long>(castLow), static_cast<unsigned long long>(castHigh),
                static_cast<unsigned long long>(castValue)));
    } else {
        ASCENDC_DEBUG_ASSERT(
            (false), KERNEL_LOG_INTERNAL(
                         KERNEL_ERROR, "Current dtype is not supported for check "
                                       "function CheckValueRange.\n"));
    }
}

__aicore__ inline bool IsMaskNorm()
{
    int64_t ctrlValue = get_ctrl();
    int64_t modeValue = (ctrlValue >> 56) & 1ULL;
    return (modeValue == 0); // 0: norm, 1: counter
}

// mask check:
// m200: when norm mode, not support both mask 0. When calculation are executed, it will trigger error
// m200 + c220: when counter mode, mask should be in range [0, UINT32_MAX]
template <typename T, bool isSetMask = true>
__aicore__ inline void CheckMaskArray(const uint64_t mask[], const __gm__ char* apiName)
{
    if ASCEND_IS_AIC {
        return; // AIC does not need to check mask
    }
    ASCENDC_DEBUG_ASSERT(
        (mask != nullptr), KERNEL_LOG_INTERNAL(
                               KERNEL_ERROR,
                               "Failed to check mask[] in %s, it is "
                               "nullptr.\n",
                               apiName));
    if constexpr (!isSetMask) {
        return; // when isSetMask = false, use previous mask. Thus no need for check.
    }
#if __NPU_ARCH__ == 2002
    ASCENDC_DEBUG_ASSERT(
        !(IsMaskNorm() && mask[0] == 0 && mask[1] == 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to "
            "check mask[] value in %s, when in norm mode, mask cannot be both 0 on current platform.\n",
            apiName));
#endif
    ASCENDC_DEBUG_WARNING(
        (!(mask[0] == 0 && mask[1] == 0)),
        KERNEL_LOG_INTERNAL(
            KERNEL_WARN, "The value of mask[] in %s equals [0, 0], which makes %s equivalent to a NOP.\n", apiName,
            apiName));
    if constexpr (sizeof(T) == 8) {
        // norm and counter both need mask[0] in range [0, UINT32_MAX]
        ASCENDC_DEBUG_ASSERT(
            (mask[0] <= UINT32_MAX),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check mask[] value "
                "in %s, when sizeof(T) == 8, mask[0] should be in range [0, UINT32_MAX], current value is %llu.\n",
                apiName, mask[0]));
    }
}

template <typename T, bool isSetMask = true, typename MaskType = uint64_t>
__aicore__ inline void CheckMaskValue(const MaskType mask, const __gm__ char* apiName)
{
    if ASCEND_IS_AIC {
        return; // AIC does not need to check mask
    }
    if constexpr (!isSetMask) {
        return; // when isSetMask = false, use previous mask. Thus no need for check.
    }

    ReportNopWarning<MaskType>(mask, "mask", apiName);

    if (!IsMaskNorm()) {
        if constexpr (SupportType<MaskType, int32_t>()) {
            CheckValueRange<int32_t>(mask, 0, INT32_MAX, "mask when counter mode", apiName);
        } else {
            ASCENDC_DEBUG_ASSERT(
                (mask <= UINT32_MAX),
                KERNEL_LOG_INTERNAL(
                    KERNEL_ERROR,
                    "Failed to check mask value when "
                    "counter mode in %s, mask should be in range [0, UINT32_MAX], current value is %llu.\n",
                    apiName, mask));
        }
        return;
    }

#if __NPU_ARCH__ == 2002
    ASCENDC_DEBUG_ASSERT(
        (mask != 0), KERNEL_LOG_INTERNAL(
                         KERNEL_ERROR,
                         "Failed to check mask value in %s, when in"
                         "norm mode, mask cannot be 0 on current platform.\n",
                         apiName));
#endif

    if constexpr (sizeof(T) == 2) {
        CheckValueRange<MaskType>(mask, 0, ONE_REPEAT_BYTE_SIZE / sizeof(T), "mask when sizeof(T) == 2", apiName);
    } else if constexpr (sizeof(T) == 4) {
        CheckValueRange<MaskType>(mask, 0, ONE_REPEAT_BYTE_SIZE / sizeof(T), "mask when sizeof(T) == 4", apiName);
    } else if constexpr (sizeof(T) == 8) {
        CheckValueRange<MaskType>(mask, 0, ONE_REPEAT_BYTE_SIZE / sizeof(T), "mask when sizeof(T) == 8", apiName);
    }
}

__aicore__ inline void CheckRepeatValue(const uint8_t repeatTime, const __gm__ char* apiName)
{
    if ASCEND_IS_AIC {
        return; // AIC does not need to check mask
    }
    ReportNopWarning<uint8_t>(repeatTime, "repeatTime", apiName);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void CheckMaskRepeat(const uint64_t mask[], const uint8_t repeatTime, const __gm__ char* apiName)
{
    CheckMaskArray<T, isSetMask>(mask, apiName);
    CheckRepeatValue(repeatTime, apiName);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void CheckMaskRepeat(const uint64_t mask, const uint8_t repeatTime, const __gm__ char* apiName)
{
    CheckMaskValue<T, isSetMask, uint64_t>(mask, apiName);
    CheckRepeatValue(repeatTime, apiName);
}

__aicore__ inline void CheckCalcount(const int32_t calcount, const __gm__ char* paramName, const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        (calcount >= 0), KERNEL_LOG_INTERNAL(
                             KERNEL_ERROR,
                             "Failed to check %s value in %s, %s should "
                             "be >= 0, current value is %d.\n",
                             paramName, apiName, paramName, calcount));
}

template <typename... Ts>
__aicore__ inline void CheckVectorTensor(const __gm__ char* apiName, const Ts&... tensors)
{
    (CheckTensorPhyPosition<Hardware::UB>(tensors.tensor, tensors.tensorName, "UB(VECIN/VECCALC/VECOUT)", apiName),
     ...);
    (CheckTensorAlignment(tensors.tensor, ONE_BLK_SIZE, tensors.tensorName, apiName), ...);
}

} // namespace AscendC

#endif // ASCENDC_MODULE_OPERATOR_KERNEL_NPU_DEBUG_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_NPU_DEBUG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_NPU_DEBUG_H__
#endif
