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
 * \file kernel_operator_vec_duplicate_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m310/kernel_operator_vec_duplicate_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_IMPL_H
#include <type_traits>
#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_common.h"
#include "kernel_operator_common_impl.h"

namespace AscendC {
template <typename T>
constexpr __aicore__ inline void CheckDuplicateSupportedType()
{
    static_assert(
        std::is_same<T, half>::value || std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value ||
            std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, float>::value,
        "Duplicate instr only support half/int16_t/uint16_t/int32_t/uint32_t/float type on current device");
}

template <typename T, bool isSetMask = true>
__aicore__ inline void DuplicateImpl(
    __ubuf__ T* dstLocal, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const uint16_t dstBlockStride,
    const uint8_t dstRepeatStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M310_DEFINE_DUPLICATE_CONTINUOUS_MASK(DataType, VectorType, MaskIntrinsic, ElementCount)        \
    template <>                                                                                             \
    __aicore__ inline void DuplicateImpl<DataType, true>(                                                   \
        __ubuf__ DataType * dstLocal, const DataType& scalarValue, uint64_t mask, const uint8_t repeatTime, \
        const uint16_t dstBlockStride, const uint8_t dstRepeatStride)                                       \
    {                                                                                                       \
        __VEC_SCOPE__                                                                                       \
        {                                                                                                   \
            VectorType vreg;                                                                                \
            uint32_t sreg = static_cast<uint32_t>(mask);                                                    \
            vector_bool preg = MaskIntrinsic(sreg, POST_UPDATE);                                            \
            uint32_t strideConfig = ((static_cast<uint32_t>(dstBlockStride)) << 16);                        \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                              \
                vdup(vreg, scalarValue, preg, MODE_ZEROING);                                                \
                vsstb(vreg, dstLocal + i * dstRepeatStride * ElementCount, strideConfig, preg);             \
            }                                                                                               \
        }                                                                                                   \
    }

ASC_M310_DEFINE_DUPLICATE_CONTINUOUS_MASK(half, vector_f16, plt_b16, 16)
ASC_M310_DEFINE_DUPLICATE_CONTINUOUS_MASK(float, vector_f32, plt_b32, 8)
ASC_M310_DEFINE_DUPLICATE_CONTINUOUS_MASK(int16_t, vector_s16, plt_b16, 16)
ASC_M310_DEFINE_DUPLICATE_CONTINUOUS_MASK(uint16_t, vector_u16, plt_b16, 16)
ASC_M310_DEFINE_DUPLICATE_CONTINUOUS_MASK(int32_t, vector_s32, plt_b32, 8)
ASC_M310_DEFINE_DUPLICATE_CONTINUOUS_MASK(uint32_t, vector_u32, plt_b32, 8)

#undef ASC_M310_DEFINE_DUPLICATE_CONTINUOUS_MASK

template <typename T, bool isSetMask = true>
__aicore__ inline void DuplicateImpl(
    __ubuf__ T* dstLocal, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime,
    const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_DUPLICATE_BITMASK_B16(DataType, VectorType)                                           \
    template <>                                                                                               \
    __aicore__ inline void DuplicateImpl<DataType, true>(                                                     \
        __ubuf__ DataType * dstLocal, const DataType& scalarValue, uint64_t mask[], const uint8_t repeatTime, \
        const uint16_t dstBlockStride, const uint8_t dstRepeatStride)                                         \
    {                                                                                                         \
        __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);          \
        *((__ubuf__ uint64_t*)tmpBuf) = mask[0];                                                              \
        *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];                                                          \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));              \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                                 \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                                \
        __VEC_SCOPE__                                                                                         \
        {                                                                                                     \
            VectorType vreg;                                                                                  \
            vector_bool preg;                                                                                 \
            plds(preg, ((__ubuf__ uint32_t*)tmpBuf), 0, US);                                                  \
            uint32_t strideConfig = ((static_cast<uint32_t>(dstBlockStride)) << 16);                          \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                \
                vdup(vreg, scalarValue, preg, MODE_ZEROING);                                                  \
                vsstb(vreg, dstLocal + i * dstRepeatStride * 16, strideConfig, preg);                         \
            }                                                                                                 \
        }                                                                                                     \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);                                                   \
    }

ASC_M310_DEFINE_DUPLICATE_BITMASK_B16(half, vector_f16)

#define ASC_M310_DEFINE_DUPLICATE_BITMASK_B32(DataType, VectorType)                                           \
    template <>                                                                                               \
    __aicore__ inline void DuplicateImpl<DataType, true>(                                                     \
        __ubuf__ DataType * dstLocal, const DataType& scalarValue, uint64_t mask[], const uint8_t repeatTime, \
        const uint16_t dstBlockStride, const uint8_t dstRepeatStride)                                         \
    {                                                                                                         \
        __ubuf__ uint8_t* tmpBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);          \
        *((__ubuf__ uint64_t*)tmpBuf) = mask[0];                                                              \
        *((__ubuf__ uint64_t*)tmpBuf + 1) = mask[1];                                                          \
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));              \
        SetFlag<HardEvent::S_V>(eventIdSToV);                                                                 \
        WaitFlag<HardEvent::S_V>(eventIdSToV);                                                                \
        __VEC_SCOPE__                                                                                         \
        {                                                                                                     \
            VectorType vreg;                                                                                  \
            vector_bool preg0;                                                                                \
            vector_bool preg1;                                                                                \
            plds(preg0, ((__ubuf__ uint32_t*)tmpBuf), 0, US);                                                 \
            punpack(preg1, preg0, LOWER);                                                                     \
            uint32_t strideConfig = ((static_cast<uint32_t>(dstBlockStride)) << 16);                          \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                \
                vdup(vreg, scalarValue, preg1, MODE_ZEROING);                                                 \
                vsstb(vreg, dstLocal + i * dstRepeatStride * 8, strideConfig, preg1);                         \
            }                                                                                                 \
        }                                                                                                     \
        AscendCUtils::FreeTemporaryBuffer<uint8_t>(tmpBuf);                                                   \
    }

ASC_M310_DEFINE_DUPLICATE_BITMASK_B32(float, vector_f32)
ASC_M310_DEFINE_DUPLICATE_BITMASK_B16(int16_t, vector_s16)
ASC_M310_DEFINE_DUPLICATE_BITMASK_B16(uint16_t, vector_u16)

#undef ASC_M310_DEFINE_DUPLICATE_BITMASK_B16

ASC_M310_DEFINE_DUPLICATE_BITMASK_B32(int32_t, vector_s32)
ASC_M310_DEFINE_DUPLICATE_BITMASK_B32(uint32_t, vector_u32)

#undef ASC_M310_DEFINE_DUPLICATE_BITMASK_B32

template <typename T>
__aicore__ inline void DuplicateImpl(__ubuf__ T* dstLocal, const T& scalarValue, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

#define ASC_M310_DEFINE_DUPLICATE_COUNT(DataType, VectorType, ElementCount, MaskIntrinsic, NORM_B16Arg) \
    template <>                                                                                         \
    __aicore__ inline void DuplicateImpl(                                                               \
        __ubuf__ DataType* dstLocal, const DataType& scalarValue, const int32_t& count)                 \
    {                                                                                                   \
        __VEC_SCOPE__                                                                                   \
        {                                                                                               \
            VectorType vreg;                                                                            \
            uint32_t sreg = static_cast<uint32_t>(count);                                               \
            vector_bool preg;                                                                           \
            uint16_t repeatTime = CeilDivision(count, ElementCount);                                    \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                          \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                \
                vdup(vreg, scalarValue, preg, MODE_ZEROING);                                            \
                vsts(vreg, dstLocal, i* ElementCount, NORM_B16Arg, preg);                               \
            }                                                                                           \
        }                                                                                               \
    }

ASC_M310_DEFINE_DUPLICATE_COUNT(half, vector_f16, 128, plt_b16, NORM_B16)
ASC_M310_DEFINE_DUPLICATE_COUNT(float, vector_f32, 64, plt_b32, NORM_B32)
ASC_M310_DEFINE_DUPLICATE_COUNT(int16_t, vector_s16, 128, plt_b16, NORM_B16)
ASC_M310_DEFINE_DUPLICATE_COUNT(uint16_t, vector_u16, 128, plt_b16, NORM_B16)
ASC_M310_DEFINE_DUPLICATE_COUNT(int32_t, vector_s32, 64, plt_b32, NORM_B32)
ASC_M310_DEFINE_DUPLICATE_COUNT(uint32_t, vector_u32, 64, plt_b32, NORM_B32)

#undef ASC_M310_DEFINE_DUPLICATE_COUNT
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_IMPL_H__
#endif
