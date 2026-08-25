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
 * \file kernel_operator_vec_cmpsel_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l311/kernel_operator_vec_cmpsel_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_IMPL_H

#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "../../../include/basic_api/kernel_struct_unary.h"
namespace AscendC {
namespace CmpSelInternal {
constexpr uint32_t maskBitToByte = 8;
}
/* ***************************************************************************************
 * ************************************** Compare ****************************************
 * ************************************************************************************** */
#define COUNTER_MODE_B8_VCMPV_VF(cmpMode)                                                    \
    __VEC_SCOPE__                                                                            \
    {                                                                                        \
        RegTensor<T> vSrc0;                                                                  \
        RegTensor<T> vSrc1;                                                                  \
        uint32_t sreg = (uint32_t)count;                                                     \
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));                       \
        uint16_t repeatTime = CeilDivision(count, sregLower);                                \
        MaskReg preg;                                                                        \
        MaskReg dstReg;                                                                      \
        AddrReg dstOffset;                                                                   \
        for (uint16_t i = 0; i < repeatTime; ++i) {                                          \
            preg = CreatePredicate<T>(sreg);                                                 \
            dstReg = CreatePredicate<T>();                                                   \
            dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);         \
            DataCopy(vSrc0, src0, i* sregLower);                                             \
            DataCopy(vSrc1, src1, i* sregLower);                                             \
            Compare<T, cmpMode>(dstReg, vSrc0, vSrc1, preg);                                 \
            DataCopy<uint32_t, Dist::DIST_NORM>((__ubuf__ uint32_t*)dst, dstReg, dstOffset); \
        }                                                                                    \
    }

#define COUNTER_MODE_B16_VCMPV_VF(cmpMode)                                                 \
    __VEC_SCOPE__                                                                          \
    {                                                                                      \
        RegTensor<T> vSrc0;                                                                \
        RegTensor<T> vSrc1;                                                                \
        uint32_t sreg = (uint32_t)count;                                                   \
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));                     \
        uint16_t repeatTime = CeilDivision(count, sregLower);                              \
        MaskReg preg;                                                                      \
        MaskReg dstReg;                                                                    \
        AddrReg dstOffset;                                                                 \
        for (uint16_t i = 0; i < repeatTime; ++i) {                                        \
            preg = CreatePredicate<T>(sreg);                                               \
            dstReg = CreatePredicate<T>();                                                 \
            dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);       \
            DataCopy(vSrc0, src0, i* sregLower);                                           \
            DataCopy(vSrc1, src1, i* sregLower);                                           \
            Compare<T, cmpMode>(dstReg, vSrc0, vSrc1, preg);                               \
            DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg, dstOffset); \
        }                                                                                  \
    }

#define COUNTER_MODE_B32_VCMPV_VF(cmpMode)                                                                        \
    uint32_t sreg = (uint32_t)count;                                                                              \
    uint32_t sregLower = VECTOR_REG_WIDTH / sizeof(T);                                                            \
    uint16_t repeatTime = CeilDivision(count, sregLower);                                                         \
    uint16_t halfRepeatTimes = repeatTime / 2;                                                                    \
    if (halfRepeatTimes > 0) {                                                                                    \
        __VEC_SCOPE__                                                                                             \
        {                                                                                                         \
            for (uint16_t i = 0; i < halfRepeatTimes; ++i) {                                                      \
                RegTensor<T> vSrc00, vSrc01;                                                                      \
                RegTensor<T> vSrc10, vSrc11;                                                                      \
                MaskReg dstReg0 = CreatePredicate<T>();                                                           \
                MaskReg dstReg1 = CreatePredicate<T>();                                                           \
                MaskReg dstReg2 = CreatePredicate<T>();                                                           \
                MaskReg dstReg3 = CreatePredicate<T>();                                                           \
                MaskReg preg = CreatePredicate<T>(sreg);                                                          \
                AddrReg dstOffset = CreateAddrReg<U>(2 * sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);              \
                DataCopy(vSrc00, src0, 2 * i * sregLower);                                                        \
                DataCopy(vSrc10, src1, 2 * i * sregLower);                                                        \
                DataCopy(vSrc01, src0 + sregLower, 2 * i * sregLower);                                            \
                DataCopy(vSrc11, src1 + sregLower, 2 * i * sregLower);                                            \
                Compare<T, cmpMode>(dstReg0, vSrc00, vSrc10, preg);                                               \
                Compare<T, cmpMode>(dstReg1, vSrc01, vSrc11, preg);                                               \
                PredicateDeInterleave<uint8_t>(dstReg2, dstReg3, dstReg0, dstReg1);                               \
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg2, dstOffset);                   \
            }                                                                                                     \
        }                                                                                                         \
    }                                                                                                             \
    uint16_t tailTimes = repeatTime - halfRepeatTimes * 2;                                                        \
    if (tailTimes > 0) {                                                                                          \
        __ubuf__ T* src0Tail = src0 + sregLower * halfRepeatTimes * 2;                                            \
        __ubuf__ T* src1Tail = src1 + sregLower * halfRepeatTimes * 2;                                            \
        __ubuf__ U* dstTail = (__ubuf__ U*)dst + halfRepeatTimes * sregLower * 2 / sizeof(U) / ONE_BYTE_BIT_SIZE; \
        __VEC_SCOPE__                                                                                             \
        {                                                                                                         \
            for (uint16_t i = 0; i < tailTimes; ++i) {                                                            \
                RegTensor<T> vSrc0, vSrc1;                                                                        \
                MaskReg dstReg0 = CreatePredicate<T>();                                                           \
                MaskReg dstReg1 = CreatePredicate<T>();                                                           \
                AddrReg dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);                  \
                MaskReg preg = CreatePredicate<T>(sreg);                                                          \
                DataCopy(vSrc0, src0Tail, i* sregLower);                                                          \
                DataCopy(vSrc1, src1Tail, i* sregLower);                                                          \
                Compare<T, cmpMode>(dstReg0, vSrc0, vSrc1, preg);                                                 \
                PredicatePack(dstReg1, dstReg0);                                                                  \
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dstTail, dstReg1, dstOffset);               \
            }                                                                                                     \
        }                                                                                                         \
    }

// level 0, mask count mode
#define CONTINUOUS_MODE_B8_VCMPV_VF(cmpMode)                                                        \
    __VEC_SCOPE__                                                                                   \
    {                                                                                               \
        RegTensor<T> vSrc0;                                                                         \
        RegTensor<T> vSrc1;                                                                         \
        uint32_t sreg = (uint32_t)mask;                                                             \
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));                              \
        uint32_t dstCalcElm = sregLower / ONE_BYTE_BIT_SIZE;                                        \
        MaskReg preg = CreatePredicate<T>(sreg);                                                    \
        MaskReg dstReg = CreatePredicate<T>();                                                      \
        for (uint16_t i = 0; i < repeatTime; ++i) {                                                 \
            uint32_t dstOffsetUint32 = i * dstCalcElm;                                              \
            DataCopy(vSrc0, src0, repeatParams.src0BlkStride, i* repeatParams.src0RepStride, preg); \
            DataCopy(vSrc1, src1, repeatParams.src1BlkStride, i* repeatParams.src1RepStride, preg); \
            Compare<T, cmpMode>(dstReg, vSrc0, vSrc1, preg);                                        \
            DataCopy<uint32_t, Dist::DIST_NORM>((__ubuf__ uint32_t*)dst, dstReg, dstOffsetUint32);  \
        }                                                                                           \
    }

#define CONTINUOUS_MODE_B16_VCMPV_VF(cmpMode)                                                       \
    __VEC_SCOPE__                                                                                   \
    {                                                                                               \
        RegTensor<T> vSrc0;                                                                         \
        RegTensor<T> vSrc1;                                                                         \
        uint32_t sreg = (uint32_t)mask;                                                             \
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));                              \
        uint32_t dstCalcElm = sregLower / ONE_BYTE_BIT_SIZE;                                        \
        MaskReg preg = CreatePredicate<T>(sreg);                                                    \
        MaskReg dstReg = CreatePredicate<T>();                                                      \
        for (uint16_t i = 0; i < repeatTime; ++i) {                                                 \
            uint32_t dstOffsetUint32 = i * dstCalcElm;                                              \
            DataCopy(vSrc0, src0, repeatParams.src0BlkStride, i* repeatParams.src0RepStride, preg); \
            DataCopy(vSrc1, src1, repeatParams.src1BlkStride, i* repeatParams.src1RepStride, preg); \
            Compare<T, cmpMode>(dstReg, vSrc0, vSrc1, preg);                                        \
            DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg, dstOffsetUint32);    \
        }                                                                                           \
    }

#define CONTINUOUS_MODE_B32_VCMPV_VF(cmpMode)                                                                       \
    uint32_t sreg = (uint32_t)mask;                                                                                 \
    uint32_t sregLower = VECTOR_REG_WIDTH / sizeof(T);                                                              \
    uint16_t halfRepeatTimes = repeatTime / 2;                                                                      \
    uint32_t dstCalcElm = 2 * sregLower / ONE_BYTE_BIT_SIZE;                                                        \
    if (halfRepeatTimes > 0) {                                                                                      \
        __VEC_SCOPE__                                                                                               \
        {                                                                                                           \
            MaskReg preg = CreatePredicate<T>(sreg);                                                                \
            for (uint16_t i = 0; i < halfRepeatTimes; ++i) {                                                        \
                RegTensor<T> vSrc00, vSrc01;                                                                        \
                RegTensor<T> vSrc10, vSrc11;                                                                        \
                MaskReg dstReg0 = CreatePredicate<T>();                                                             \
                MaskReg dstReg1 = CreatePredicate<T>();                                                             \
                MaskReg dstReg2 = CreatePredicate<T>();                                                             \
                MaskReg dstReg3 = CreatePredicate<T>();                                                             \
                uint32_t dstOffsetUint32 = i * dstCalcElm;                                                          \
                DataCopy(vSrc00, src0, repeatParams.src0BlkStride, 2 * i * repeatParams.src0RepStride, preg);       \
                DataCopy(vSrc10, src1, repeatParams.src1BlkStride, 2 * i * repeatParams.src1RepStride, preg);       \
                DataCopy(vSrc01, src0, repeatParams.src0BlkStride, (2 * i + 1) * repeatParams.src0RepStride, preg); \
                DataCopy(vSrc11, src1, repeatParams.src1BlkStride, (2 * i + 1) * repeatParams.src1RepStride, preg); \
                Compare<T, cmpMode>(dstReg0, vSrc00, vSrc10, preg);                                                 \
                Compare<T, cmpMode>(dstReg1, vSrc01, vSrc11, preg);                                                 \
                PredicateDeInterleave<uint8_t>(dstReg2, dstReg3, dstReg0, dstReg1);                                 \
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg2, dstOffsetUint32);               \
            }                                                                                                       \
        }                                                                                                           \
    }                                                                                                               \
    uint16_t tailTimes = repeatTime - halfRepeatTimes * 2;                                                          \
    if (tailTimes > 0) {                                                                                            \
        __ubuf__ T* src0Tail = src0 + sregLower * halfRepeatTimes * 2 * repeatParams.src0BlkStride;                 \
        __ubuf__ T* src1Tail = src1 + sregLower * halfRepeatTimes * 2 * repeatParams.src1BlkStride;                 \
        __ubuf__ U* dstTail = (__ubuf__ U*)dst + halfRepeatTimes * dstCalcElm;                                      \
        __VEC_SCOPE__                                                                                               \
        {                                                                                                           \
            MaskReg preg = CreatePredicate<T>(sreg);                                                                \
            for (uint16_t i = 0; i < tailTimes; ++i) {                                                              \
                RegTensor<T> vSrc0, vSrc1;                                                                          \
                MaskReg dstReg0 = CreatePredicate<T>();                                                             \
                MaskReg dstReg1 = CreatePredicate<T>();                                                             \
                AddrReg dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);                    \
                DataCopy(vSrc0, src0Tail, repeatParams.src0BlkStride, 0, preg);                                     \
                DataCopy(vSrc1, src1Tail, repeatParams.src1BlkStride, 0, preg);                                     \
                Compare<T, cmpMode>(dstReg0, vSrc0, vSrc1, preg);                                                   \
                PredicatePack(dstReg1, dstReg0);                                                                    \
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dstTail, dstReg1, dstOffset);                 \
            }                                                                                                       \
        }                                                                                                           \
    }

// level 0, mask bit mode
#define BITS_MODE_B16_VCMPV_VF(cmpMode)                                                             \
    __VEC_SCOPE__                                                                                   \
    {                                                                                               \
        RegTensor<T> vSrc0;                                                                         \
        RegTensor<T> vSrc1;                                                                         \
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));                              \
        uint32_t dstCalcElm = sregLower / ONE_BYTE_BIT_SIZE;                                        \
        MaskReg preg;                                                                               \
        DataCopy<uint32_t, Dist::DIST_US>(preg, ((__ubuf__ uint32_t*)tempBuf), 0);                  \
        for (uint16_t i = 0; i < repeatTime; ++i) {                                                 \
            MaskReg dstReg = CreatePredicate<T>();                                                  \
            uint32_t dstOffsetUint32 = i * dstCalcElm;                                              \
            DataCopy(vSrc0, src0, repeatParams.src0BlkStride, i* repeatParams.src0RepStride, preg); \
            DataCopy(vSrc1, src1, repeatParams.src1BlkStride, i* repeatParams.src1RepStride, preg); \
            Compare<T, cmpMode>(dstReg, vSrc0, vSrc1, preg);                                        \
            DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg, dstOffsetUint32);    \
        }                                                                                           \
    }

#define BITS_MODE_B32_VCMPV_VF(cmpMode)                                                                              \
    uint32_t sregLower = VECTOR_REG_WIDTH / sizeof(T);                                                               \
    uint16_t halfRepeatTimes = repeatTime / 2;                                                                       \
    uint32_t dstCalcElm = 2 * sregLower / ONE_BYTE_BIT_SIZE;                                                         \
    if (halfRepeatTimes > 0) {                                                                                       \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            MaskReg preg;                                                                                            \
            MaskReg preg1;                                                                                           \
            DataCopy<uint32_t, Dist::DIST_US>(preg, ((__ubuf__ uint32_t*)tempBuf), 0);                               \
            PredicateUnPack(preg1, preg);                                                                            \
            for (uint16_t i = 0; i < halfRepeatTimes; ++i) {                                                         \
                RegTensor<T> vSrc00, vSrc01;                                                                         \
                RegTensor<T> vSrc10, vSrc11;                                                                         \
                MaskReg dstReg0 = CreatePredicate<T>();                                                              \
                MaskReg dstReg1 = CreatePredicate<T>();                                                              \
                MaskReg dstReg2 = CreatePredicate<T>();                                                              \
                MaskReg dstReg3 = CreatePredicate<T>();                                                              \
                uint32_t dstOffsetUint32 = i * dstCalcElm;                                                           \
                DataCopy(vSrc00, src0, repeatParams.src0BlkStride, 2 * i * repeatParams.src0RepStride, preg1);       \
                DataCopy(vSrc10, src1, repeatParams.src1BlkStride, 2 * i * repeatParams.src1RepStride, preg1);       \
                DataCopy(vSrc01, src0, repeatParams.src0BlkStride, (2 * i + 1) * repeatParams.src0RepStride, preg1); \
                DataCopy(vSrc11, src1, repeatParams.src1BlkStride, (2 * i + 1) * repeatParams.src1RepStride, preg1); \
                Compare<T, cmpMode>(dstReg0, vSrc00, vSrc10, preg1);                                                 \
                Compare<T, cmpMode>(dstReg1, vSrc01, vSrc11, preg1);                                                 \
                PredicateDeInterleave<uint8_t>(dstReg2, dstReg3, dstReg0, dstReg1);                                  \
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg2, dstOffsetUint32);                \
            }                                                                                                        \
        }                                                                                                            \
    }                                                                                                                \
    uint16_t tailTimes = repeatTime - halfRepeatTimes * 2;                                                           \
    if (tailTimes > 0) {                                                                                             \
        __ubuf__ T* src0Tail = src0 + sregLower * halfRepeatTimes * 2 * repeatParams.src0BlkStride;                  \
        __ubuf__ T* src1Tail = src1 + sregLower * halfRepeatTimes * 2 * repeatParams.src1BlkStride;                  \
        __ubuf__ U* dstTail = (__ubuf__ U*)dst + halfRepeatTimes * dstCalcElm;                                       \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            MaskReg preg;                                                                                            \
            MaskReg preg1;                                                                                           \
            DataCopy<uint32_t, Dist::DIST_US>(preg, ((__ubuf__ uint32_t*)tempBuf), 0);                               \
            PredicateUnPack(preg1, preg);                                                                            \
            for (uint16_t i = 0; i < tailTimes; ++i) {                                                               \
                RegTensor<T> vSrc0, vSrc1;                                                                           \
                MaskReg dstReg0 = CreatePredicate<T>();                                                              \
                MaskReg dstReg1 = CreatePredicate<T>();                                                              \
                AddrReg dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);                     \
                DataCopy(vSrc0, src0Tail, repeatParams.src0BlkStride, 0, preg1);                                     \
                DataCopy(vSrc1, src1Tail, repeatParams.src1BlkStride, 0, preg1);                                     \
                Compare<T, cmpMode>(dstReg0, vSrc0, vSrc1, preg1);                                                   \
                PredicatePack(dstReg1, dstReg0);                                                                     \
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dstTail, dstReg1, dstOffset);                  \
            }                                                                                                        \
        }                                                                                                            \
    }

// Compare::Level 2
template <typename U, typename T>
typename std::enable_if_t<
    !std::is_same<T, uint8_t>::value && !std::is_same<T, int8_t>::value && !std::is_same<T, uint16_t>::value &&
    !std::is_same<T, int16_t>::value && !std::is_same<T, half>::value && !std::is_same<T, uint32_t>::value &&
    !std::is_same<T, int32_t>::value &&
    !std::is_same<T, float>::
        value> __aicore__ inline VcmpvImpl(__ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

template <typename U, typename T>
typename std::
    enable_if_t<std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value> __aicore__ inline VcmpvImpl(
        __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t count)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            COUNTER_MODE_B8_VCMPV_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            COUNTER_MODE_B8_VCMPV_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            COUNTER_MODE_B8_VCMPV_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            COUNTER_MODE_B8_VCMPV_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            COUNTER_MODE_B8_VCMPV_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            COUNTER_MODE_B8_VCMPV_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
}

template <typename U, typename T>
typename std::enable_if_t<
    std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value ||
    std::is_same<T, half>::
        value> __aicore__ inline VcmpvImpl(__ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t count)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            COUNTER_MODE_B16_VCMPV_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            COUNTER_MODE_B16_VCMPV_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            COUNTER_MODE_B16_VCMPV_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            COUNTER_MODE_B16_VCMPV_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            COUNTER_MODE_B16_VCMPV_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            COUNTER_MODE_B16_VCMPV_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
}

template <typename U, typename T>
typename std::enable_if_t<
    std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value ||
    std::is_same<T, float>::
        value> __aicore__ inline VcmpvImpl(__ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t count)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            COUNTER_MODE_B32_VCMPV_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            COUNTER_MODE_B32_VCMPV_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            COUNTER_MODE_B32_VCMPV_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            COUNTER_MODE_B32_VCMPV_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            COUNTER_MODE_B32_VCMPV_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            COUNTER_MODE_B32_VCMPV_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
}

// Compare::Level 0 - mask count mode
template <typename T, typename U, bool isSetMask = true>
typename std::enable_if_t<
    !std::is_same<T, uint8_t>::value && !std::is_same<T, int8_t>::value && !std::is_same<T, uint16_t>::value &&
    !std::is_same<T, int16_t>::value && !std::is_same<T, half>::value && !std::is_same<T, uint32_t>::value &&
    !std::is_same<T, int32_t>::value &&
    !std::is_same<T, float>::
        value> __aicore__ inline VcmpvImpl(__ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, uint16_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

template <typename T, typename U, bool isSetMask = true>
typename std::
    enable_if_t<std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value> __aicore__ inline VcmpvImpl(
        __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, uint16_t repeatTime,
        const BinaryRepeatParams& repeatParams)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            CONTINUOUS_MODE_B8_VCMPV_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            CONTINUOUS_MODE_B8_VCMPV_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            CONTINUOUS_MODE_B8_VCMPV_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            CONTINUOUS_MODE_B8_VCMPV_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            CONTINUOUS_MODE_B8_VCMPV_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            CONTINUOUS_MODE_B8_VCMPV_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
}

template <typename T, typename U, bool isSetMask = true>
typename std::
    enable_if_t<std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, half>::value> __aicore__ inline VcmpvImpl(
        __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, uint16_t repeatTime,
        const BinaryRepeatParams& repeatParams)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            CONTINUOUS_MODE_B16_VCMPV_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            CONTINUOUS_MODE_B16_VCMPV_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            CONTINUOUS_MODE_B16_VCMPV_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            CONTINUOUS_MODE_B16_VCMPV_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            CONTINUOUS_MODE_B16_VCMPV_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            CONTINUOUS_MODE_B16_VCMPV_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
}

template <typename T, typename U, bool isSetMask = true>
typename std::
    enable_if_t<std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value || std::is_same<T, float>::value> __aicore__ inline VcmpvImpl(
        __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, uint16_t repeatTime,
        const BinaryRepeatParams& repeatParams)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            CONTINUOUS_MODE_B32_VCMPV_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            CONTINUOUS_MODE_B32_VCMPV_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            CONTINUOUS_MODE_B32_VCMPV_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            CONTINUOUS_MODE_B32_VCMPV_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            CONTINUOUS_MODE_B32_VCMPV_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            CONTINUOUS_MODE_B32_VCMPV_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
}

// Compare::Level 0 - mask bit mode
template <typename T, typename U, bool isSetMask = true>
typename std::enable_if_t<
    !std::is_same<T, uint16_t>::value && !std::is_same<T, int16_t>::value && !std::is_same<T, half>::value &&
    !std::is_same<T, uint32_t>::value && !std::is_same<T, int32_t>::value &&
    !std::is_same<T, float>::
        value> __aicore__ inline VcmpvImpl(__ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask[2], uint16_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

template <typename T, typename U, bool isSetMask = true>
typename std::
    enable_if_t<std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, half>::value> __aicore__ inline VcmpvImpl(
        __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask[2],
        uint16_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);
    (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf)) = ((uint64_t)mask[0]);
    (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf + 1)) = ((uint64_t)mask[1]);

    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);

    switch (cmpMode) {
        case CMPMODE::LT: {
            BITS_MODE_B16_VCMPV_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            BITS_MODE_B16_VCMPV_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            BITS_MODE_B16_VCMPV_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            BITS_MODE_B16_VCMPV_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            BITS_MODE_B16_VCMPV_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            BITS_MODE_B16_VCMPV_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
    AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);
}

template <typename T, typename U, bool isSetMask = true>
typename std::
    enable_if_t<std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value || std::is_same<T, float>::value> __aicore__ inline VcmpvImpl(
        __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask[2],
        uint16_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    __ubuf__ uint8_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint8_t>(TMP_UB_OFFSET, 16);
    (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf)) = ((uint64_t)mask[0]);
    (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf + 1)) = ((uint64_t)mask[1]);

    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);

    switch (cmpMode) {
        case CMPMODE::LT: {
            BITS_MODE_B32_VCMPV_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            BITS_MODE_B32_VCMPV_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            BITS_MODE_B32_VCMPV_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            BITS_MODE_B32_VCMPV_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            BITS_MODE_B32_VCMPV_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            BITS_MODE_B32_VCMPV_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
    AscendCUtils::FreeTemporaryBuffer<uint8_t>(tempBuf);
}

/* ***************************************************************************************
 * *********************************** CompareScalar *************************************
 * ************************************************************************************** */
// CompareScalar::Level 0 - bit mode / continious mode
template <typename T, typename U, CMPMODE cmpMode, bool isSetMask>
__simd_vf__ inline void CompareScalarLevel0CounterMode(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, const uint64_t mask, __ubuf__ uint64_t* tempBuf,
    const UnaryRepeatParams repeatParams)
{
    Reg::MaskReg maskReg;
    Reg::RegTensor<T> src0Reg;
    Reg::MaskReg dstReg;
    uint32_t sreg = static_cast<uint32_t>(mask);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    if constexpr (!isSetMask) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::DataCopy<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
    }

    uint16_t newRepeatTimes = CeilDivision(sreg, oneRepSize);
    uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));

    if constexpr (sizeof(T) == 2) {
        uint32_t dstOffsetUint32 = sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE;
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                src0Reg, src0, static_cast<uint32_t>(repeatParams.srcBlkStride),
                static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
            Reg::CompareScalar<T, cmpMode>(dstReg, src0Reg, src1, maskReg);
            Reg::DataCopy<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE, Reg::MaskDist::DIST_PACK>(
                (__ubuf__ uint32_t*&)dst, dstReg, dstOffsetUint32);
        }
    } else {
        uint16_t halfRepeatTimes = newRepeatTimes / 2;
        uint32_t dstCalcElm = 2 * sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE;

        if (halfRepeatTimes > 0) {
            for (uint16_t i = 0; i < halfRepeatTimes; ++i) {
                maskReg = Reg::UpdateMask<T>(sreg);
                RegTensor<T> vSrc00, vSrc01;
                MaskReg dstReg0 = CreatePredicate<T>();
                MaskReg dstReg1 = CreatePredicate<T>();
                MaskReg dstReg2 = CreatePredicate<T>();
                MaskReg dstReg3 = CreatePredicate<T>();
                uint32_t dstOffsetUint32 = i * dstCalcElm;
                DataCopy(vSrc00, src0, repeatParams.srcBlkStride, 2 * i * repeatParams.srcRepStride, maskReg);
                DataCopy(vSrc01, src0, repeatParams.srcBlkStride, (2 * i + 1) * repeatParams.srcRepStride, maskReg);
                CompareScalar<T, cmpMode>(dstReg0, vSrc00, src1, maskReg);
                CompareScalar<T, cmpMode>(dstReg1, vSrc01, src1, maskReg);
                PredicateDeInterleave<uint8_t>(dstReg2, dstReg3, dstReg0, dstReg1);
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg2, dstOffsetUint32);
            }
        }
        uint16_t tailTimes = newRepeatTimes - halfRepeatTimes * 2;
        if (tailTimes > 0) {
            __ubuf__ T* src0Tail = src0 + sregLower * halfRepeatTimes * 2 * repeatParams.srcBlkStride;
            __ubuf__ U* dstTail = (__ubuf__ U*)dst + halfRepeatTimes * dstCalcElm;
            for (uint16_t i = 0; i < tailTimes; ++i) {
                maskReg = Reg::UpdateMask<T>(sreg);
                RegTensor<T> vSrc0;
                MaskReg dstReg0 = CreatePredicate<T>();
                MaskReg dstReg1 = CreatePredicate<T>();
                AddrReg dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);
                DataCopy(vSrc0, src0Tail, repeatParams.srcBlkStride, 0, maskReg);
                CompareScalar<T, cmpMode>(dstReg0, vSrc0, src1, maskReg);
                PredicatePack(dstReg1, dstReg0);
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dstTail, dstReg1, dstOffset);
            }
        }
    }
}

template <typename T, typename U, CMPMODE cmpMode, bool isBitMapMode, bool isSetMask>
__simd_vf__ inline void CompareScalarLevel0NormalMode(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams)
{
    Reg::MaskReg maskReg;
    Reg::RegTensor<T> src0Reg;
    Reg::MaskReg dstReg;
    if constexpr (isBitMapMode) {
        maskReg = Reg::MoveMask<T>();
    } else {
        if constexpr (isSetMask) {
            uint32_t sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        } else {
            maskReg = Reg::MoveMask<T>();
        }
    }

    uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
    if constexpr (sizeof(T) == 2) {
        uint32_t dstOffsetUint32 = sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE;
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                src0Reg, src0, static_cast<uint32_t>(repeatParams.srcBlkStride),
                static_cast<uint32_t>(repeatParams.srcRepStride), maskReg);
            Reg::CompareScalar<T, cmpMode>(dstReg, src0Reg, src1, maskReg);
            Reg::DataCopy<uint32_t, Reg::PostLiteral::POST_MODE_UPDATE, Reg::MaskDist::DIST_PACK>(
                (__ubuf__ uint32_t*&)dst, dstReg, dstOffsetUint32);
        }
    } else {
        uint16_t halfRepeatTimes = repeatTime / 2;
        uint32_t dstCalcElm = 2 * sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE;

        if (halfRepeatTimes > 0) {
            for (uint16_t i = 0; i < halfRepeatTimes; ++i) {
                RegTensor<T> vSrc00, vSrc01;
                MaskReg dstReg0 = CreatePredicate<T>();
                MaskReg dstReg1 = CreatePredicate<T>();
                MaskReg dstReg2 = CreatePredicate<T>();
                MaskReg dstReg3 = CreatePredicate<T>();
                uint32_t dstOffsetUint32 = i * dstCalcElm;
                DataCopy(vSrc00, src0, repeatParams.srcBlkStride, 2 * i * repeatParams.srcRepStride, maskReg);
                DataCopy(vSrc01, src0, repeatParams.srcBlkStride, (2 * i + 1) * repeatParams.srcRepStride, maskReg);
                CompareScalar<T, cmpMode>(dstReg0, vSrc00, src1, maskReg);
                CompareScalar<T, cmpMode>(dstReg1, vSrc01, src1, maskReg);
                PredicateDeInterleave<uint8_t>(dstReg2, dstReg3, dstReg0, dstReg1);
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg2, dstOffsetUint32);
            }
        }
        uint16_t tailTimes = repeatTime - halfRepeatTimes * 2;
        if (tailTimes > 0) {
            __ubuf__ T* src0Tail = src0 + sregLower * halfRepeatTimes * 2 * repeatParams.srcBlkStride;
            __ubuf__ U* dstTail = (__ubuf__ U*)dst + halfRepeatTimes * dstCalcElm;
            for (uint16_t i = 0; i < tailTimes; ++i) {
                RegTensor<T> vSrc0;
                MaskReg dstReg0 = CreatePredicate<T>();
                MaskReg dstReg1 = CreatePredicate<T>();
                AddrReg dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);
                DataCopy(vSrc0, src0Tail, repeatParams.srcBlkStride, 0, maskReg);
                CompareScalar<T, cmpMode>(dstReg0, vSrc0, src1, maskReg);
                PredicatePack(dstReg1, dstReg0);
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dstTail, dstReg1, dstOffset);
            }
        }
    }
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2);
        switch (cmpMode) {
            case CMPMODE::LT: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>(
                    dst, src0, src1, mask[0], tempBuf, repeatParams);
                break;
            }
            default:
                break;
        }
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
    } else {
        if constexpr (isSetMask) {
            SetVectorMask<T>(mask[1], mask[0]);
        }
        switch (cmpMode) {
            case CMPMODE::LT: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::LT, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::GT, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::EQ, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::LE, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::GE, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::NE, true, isSetMask>(
                    dst, src0, src1, 0, repeatTime, repeatParams);
                break;
            }
            default:
                break;
        }
    }
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvsImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, const T src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2);
        switch (cmpMode) {
            case CMPMODE::LT: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::LT, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::GT, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::EQ, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::LE, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::GE, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarLevel0CounterMode<T, U, CMPMODE::NE, isSetMask>(
                    dst, src0, src1, mask, tempBuf, repeatParams);
                break;
            }
            default:
                break;
        }
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
    } else {
        switch (cmpMode) {
            case CMPMODE::LT: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::LT, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GT: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::GT, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::EQ: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::EQ, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::LE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::LE, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::GE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::GE, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            case CMPMODE::NE: {
                CompareScalarLevel0NormalMode<T, U, CMPMODE::NE, false, isSetMask>(
                    dst, src0, src1, mask, repeatTime, repeatParams);
                break;
            }
            default:
                break;
        }
    }
}

// CompareScalar::Level 2
#define COUNTER_MODE_B8_VCMPVS_VF(cmpMode)                                                   \
    __VEC_SCOPE__                                                                            \
    {                                                                                        \
        RegTensor<T> vSrc0;                                                                  \
        uint32_t sreg = (uint32_t)count;                                                     \
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));                       \
        uint16_t repeatTime = CeilDivision(count, sregLower);                                \
        MaskReg preg;                                                                        \
        MaskReg dstReg;                                                                      \
        AddrReg dstOffset;                                                                   \
        for (uint16_t i = 0; i < repeatTime; ++i) {                                          \
            preg = CreatePredicate<T>(sreg);                                                 \
            dstReg = CreatePredicate<T>();                                                   \
            dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);         \
            DataCopy(vSrc0, src0, i* sregLower);                                             \
            CompareScalar<T, cmpMode>(dstReg, vSrc0, src1Scalar, preg);                      \
            DataCopy<uint32_t, Dist::DIST_NORM>((__ubuf__ uint32_t*)dst, dstReg, dstOffset); \
        }                                                                                    \
    }

#define COUNTER_MODE_B16_VCMPVS_VF(cmpMode)                                                \
    __VEC_SCOPE__                                                                          \
    {                                                                                      \
        RegTensor<T> vSrc0;                                                                \
        uint32_t sreg = (uint32_t)count;                                                   \
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));                     \
        uint16_t repeatTime = CeilDivision(count, sregLower);                              \
        MaskReg preg;                                                                      \
        MaskReg dstReg;                                                                    \
        AddrReg dstOffset;                                                                 \
        for (uint16_t i = 0; i < repeatTime; ++i) {                                        \
            preg = CreatePredicate<T>(sreg);                                               \
            dstReg = CreatePredicate<T>();                                                 \
            dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);       \
            DataCopy(vSrc0, src0, i* sregLower);                                           \
            CompareScalar<T, cmpMode>(dstReg, vSrc0, src1Scalar, preg);                    \
            DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg, dstOffset); \
        }                                                                                  \
    }

#define COUNTER_MODE_B32_VCMPVS_VF(cmpMode)                                                                       \
    uint32_t sreg = (uint32_t)count;                                                                              \
    uint32_t sregLower = VECTOR_REG_WIDTH / sizeof(T);                                                            \
    uint16_t repeatTime = CeilDivision(count, sregLower);                                                         \
    uint16_t halfRepeatTimes = repeatTime / 2;                                                                    \
    if (halfRepeatTimes > 0) {                                                                                    \
        __VEC_SCOPE__                                                                                             \
        {                                                                                                         \
            for (uint16_t i = 0; i < halfRepeatTimes; ++i) {                                                      \
                RegTensor<T> vSrc00, vSrc01;                                                                      \
                MaskReg dstReg0 = CreatePredicate<T>();                                                           \
                MaskReg dstReg1 = CreatePredicate<T>();                                                           \
                MaskReg dstReg2 = CreatePredicate<T>();                                                           \
                MaskReg dstReg3 = CreatePredicate<T>();                                                           \
                MaskReg preg = CreatePredicate<T>(sreg);                                                          \
                AddrReg dstOffset = CreateAddrReg<U>(2 * sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);              \
                DataCopy(vSrc00, src0, 2 * i * sregLower);                                                        \
                DataCopy(vSrc01, src0 + sregLower, 2 * i * sregLower);                                            \
                CompareScalar<T, cmpMode>(dstReg0, vSrc00, src1Scalar, preg);                                     \
                CompareScalar<T, cmpMode>(dstReg1, vSrc01, src1Scalar, preg);                                     \
                PredicateDeInterleave<uint8_t>(dstReg2, dstReg3, dstReg0, dstReg1);                               \
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dst, dstReg2, dstOffset);                   \
            }                                                                                                     \
        }                                                                                                         \
    }                                                                                                             \
    uint16_t tailTimes = repeatTime - halfRepeatTimes * 2;                                                        \
    if (tailTimes > 0) {                                                                                          \
        __ubuf__ T* src0Tail = src0 + sregLower * halfRepeatTimes * 2;                                            \
        __ubuf__ U* dstTail = (__ubuf__ U*)dst + halfRepeatTimes * sregLower * 2 / sizeof(U) / ONE_BYTE_BIT_SIZE; \
        __VEC_SCOPE__                                                                                             \
        {                                                                                                         \
            for (uint16_t i = 0; i < tailTimes; ++i) {                                                            \
                RegTensor<T> vSrc0, vSrc1;                                                                        \
                MaskReg dstReg0 = CreatePredicate<T>();                                                           \
                MaskReg dstReg1 = CreatePredicate<T>();                                                           \
                AddrReg dstOffset = CreateAddrReg<U>(sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE);                  \
                MaskReg preg = CreatePredicate<T>(sreg);                                                          \
                DataCopy(vSrc0, src0Tail, i* sregLower);                                                          \
                CompareScalar<T, cmpMode>(dstReg0, vSrc0, src1Scalar, preg);                                      \
                PredicatePack(dstReg1, dstReg0);                                                                  \
                DataCopy<uint32_t, Dist::DIST_PK>((__ubuf__ uint32_t*)dstTail, dstReg1, dstOffset);               \
            }                                                                                                     \
        }                                                                                                         \
    }

// CompareScalar::Level 2
template <typename U, typename T>
typename std::enable_if_t<
    !std::is_same<T, uint8_t>::value && !std::is_same<T, int8_t>::value && !std::is_same<T, uint16_t>::value &&
    !std::is_same<T, int16_t>::value && !std::is_same<T, half>::value && !std::is_same<T, uint32_t>::value &&
    !std::is_same<T, int32_t>::value &&
    !std::is_same<T, float>::
        value> __aicore__ inline VcmpvsImpl(__ubuf__ U* dst, __ubuf__ T* src0, const T src1Scalar, CMPMODE cmpMode, const uint32_t count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

template <typename U, typename T>
typename std::
    enable_if_t<std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value> __aicore__ inline VcmpvsImpl(
        __ubuf__ U* dst, __ubuf__ T* src0, const T src1Scalar, CMPMODE cmpMode, const uint32_t count)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            COUNTER_MODE_B8_VCMPVS_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            COUNTER_MODE_B8_VCMPVS_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            COUNTER_MODE_B8_VCMPVS_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            COUNTER_MODE_B8_VCMPVS_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            COUNTER_MODE_B8_VCMPVS_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            COUNTER_MODE_B8_VCMPVS_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
}

template <typename U, typename T>
typename std::enable_if_t<
    std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value ||
    std::is_same<T, half>::
        value> __aicore__ inline VcmpvsImpl(__ubuf__ U* dst, __ubuf__ T* src0, const T src1Scalar, CMPMODE cmpMode, const uint32_t count)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            COUNTER_MODE_B16_VCMPVS_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            COUNTER_MODE_B16_VCMPVS_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            COUNTER_MODE_B16_VCMPVS_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            COUNTER_MODE_B16_VCMPVS_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            COUNTER_MODE_B16_VCMPVS_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            COUNTER_MODE_B16_VCMPVS_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
}

template <typename U, typename T>
typename std::enable_if_t<
    std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value ||
    std::is_same<T, float>::
        value> __aicore__ inline VcmpvsImpl(__ubuf__ U* dst, __ubuf__ T* src0, const T src1Scalar, CMPMODE cmpMode, const uint32_t count)
{
    switch (cmpMode) {
        case CMPMODE::LT: {
            COUNTER_MODE_B32_VCMPVS_VF(CMPMODE::LT);
            break;
        }
        case CMPMODE::GT: {
            COUNTER_MODE_B32_VCMPVS_VF(CMPMODE::GT);
            break;
        }
        case CMPMODE::EQ: {
            COUNTER_MODE_B32_VCMPVS_VF(CMPMODE::EQ);
            break;
        }
        case CMPMODE::LE: {
            COUNTER_MODE_B32_VCMPVS_VF(CMPMODE::LE);
            break;
        }
        case CMPMODE::GE: {
            COUNTER_MODE_B32_VCMPVS_VF(CMPMODE::GE);
            break;
        }
        case CMPMODE::NE: {
            COUNTER_MODE_B32_VCMPVS_VF(CMPMODE::NE);
            break;
        }
        default:
            break;
    }
}

// /* ***************************************************************************************
//  * *************************************** Select ****************************************
//  * ************************************************************************************** */
// Level 2, select mode: 0/2
template <typename T, typename U>
typename std::enable_if_t<
    !std::is_same<T, uint8_t>::value && !std::is_same<T, int8_t>::value && !std::is_same<T, uint16_t>::value &&
    !std::is_same<T, int16_t>::value && !std::is_same<T, half>::value && !std::is_same<T, uint32_t>::value &&
    !std::is_same<T, int32_t>::value &&
    !std::is_same<T, float>::
        value> __aicore__ inline VselImpl(__ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, uint32_t count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

template <typename T, typename U>
typename std::enable_if_t<std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value> __aicore__ inline VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, uint32_t count)
{
    __VEC_SCOPE__
    {
        RegTensor<T> vSrc0, vSrc1;
        RegTensor<T> vDst;
        uint32_t sreg = (uint32_t)count;
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
        uint16_t repeatTime = CeilDivision(count, sregLower);
        uint32_t selMaskOffset;
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            selMaskOffset = 0;
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            selMaskOffset = sregLower / ONE_BYTE_BIT_SIZE;
        }
        for (uint16_t i = 0; i < repeatTime; ++i) {
            MaskReg preg;
            DataCopy<uint32_t, Dist::DIST_NORM>(preg, ((__ubuf__ uint32_t*)sel), i * selMaskOffset);
            MaskReg dstReg = CreatePredicate<T>(sreg);
            DataCopy(vSrc0, src0, i * sregLower);
            DataCopy(vSrc1, src1, i * sregLower);
            Select<T>(vDst, vSrc0, vSrc1, preg);
            DataCopy(dst, vDst, i * sregLower, dstReg);
        }
    }
}

template <typename T, typename U>
typename std::
    enable_if_t<std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value || std::is_same<T, half>::value> __aicore__ inline VselImpl(
        __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, uint32_t count)
{
    __VEC_SCOPE__
    {
        RegTensor<T> vSrc0, vSrc1;
        RegTensor<T> vDst;
        uint32_t sreg = (uint32_t)count;
        uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(T));
        uint16_t repeatTime = CeilDivision(count, sregLower);
        uint32_t selMaskOffset;
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            selMaskOffset = 0;
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            selMaskOffset = sregLower / ONE_BYTE_BIT_SIZE;
        }
        for (uint16_t i = 0; i < repeatTime; ++i) {
            MaskReg preg;
            DataCopy<uint32_t, Dist::DIST_US>(preg, ((__ubuf__ uint32_t*)sel), i * selMaskOffset);
            MaskReg dstReg = CreatePredicate<T>(sreg);
            DataCopy(vSrc0, src0, i * sregLower);
            DataCopy(vSrc1, src1, i * sregLower);
            Select<T>(vDst, vSrc0, vSrc1, preg);
            DataCopy(dst, vDst, i * sregLower, dstReg);
        }
    }
}

template <typename T, typename U>
typename std::
    enable_if_t<std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value || std::is_same<T, float>::value> __aicore__ inline VselImpl(
        __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, uint32_t count)
{
    uint32_t sreg = (uint32_t)count;
    uint32_t sregLower = VECTOR_REG_WIDTH / sizeof(T);
    uint16_t repeatTime = CeilDivision(count, sregLower);
    uint16_t halfRepeatTimes = repeatTime / 2;
    if (halfRepeatTimes > 0) {
        uint32_t selMaskOffset;
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            selMaskOffset = 0;
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            selMaskOffset = 2 * sregLower / ONE_BYTE_BIT_SIZE;
        }
        __VEC_SCOPE__
        {
            for (uint16_t i = 0; i < halfRepeatTimes; ++i) {
                RegTensor<T> vSrc00, vSrc01;
                RegTensor<T> vSrc10, vSrc11;
                RegTensor<T> vDst0, vDst1;
                MaskReg preg0;
                MaskReg preg1 = CreatePredicate<T, Pat::ALLF>();
                MaskReg preg2 = CreatePredicate<T>();
                MaskReg preg3 = CreatePredicate<T>();
                DataCopy<uint32_t, Dist::DIST_US>(preg0, ((__ubuf__ uint32_t*)sel), i * selMaskOffset);
                MaskReg dstReg = CreatePredicate<T>(sreg);
                DataCopy(vSrc00, src0, 2 * i * sregLower);
                DataCopy(vSrc10, src1, 2 * i * sregLower);
                DataCopy(vSrc01, src0 + sregLower, 2 * i * sregLower);
                DataCopy(vSrc11, src1 + sregLower, 2 * i * sregLower);
                PredicateInterleave<uint16_t>(preg2, preg3, preg0, preg1); // u8,u16均可以
                Select<T>(vDst0, vSrc00, vSrc10, preg2);
                if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
                    Select<T>(vDst1, vSrc01, vSrc11, preg2); // SELMODE::VSEL_CMPMASK_SPR使用preg2,固定使用前64bit
                } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
                    Select<T>(vDst1, vSrc01, vSrc11, preg3); // SELMODE::VSEL_TENSOR_TENSOR_MODE使用preg3,连续消耗
                }
                DataCopy(dst, vDst0, 2 * i * sregLower, dstReg);
                DataCopy(dst + sregLower, vDst1, 2 * i * sregLower, dstReg);
            }
        }
    }

    uint16_t tailTimes = repeatTime - halfRepeatTimes * 2;
    if (tailTimes > 0) {
        __ubuf__ T* src0Tail = src0 + halfRepeatTimes * sregLower * 2;
        __ubuf__ T* src1Tail = src1 + halfRepeatTimes * sregLower * 2;
        uint16_t selMaskOffset;
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            selMaskOffset = 0;
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            selMaskOffset = halfRepeatTimes * 2 * sregLower / sizeof(U) / ONE_BYTE_BIT_SIZE; // 单位为元素个数
        }
        __ubuf__ U* selTail = (__ubuf__ U*)sel + selMaskOffset;
        __ubuf__ T* dstTail = dst + halfRepeatTimes * sregLower * 2;
        __VEC_SCOPE__
        {
            for (uint16_t i = 0; i < tailTimes; ++i) {
                RegTensor<T> vSrc0;
                RegTensor<T> vSrc1;
                RegTensor<T> vDst;
                MaskReg preg0;
                MaskReg preg1 = CreatePredicate<T>();
                DataCopy<uint32_t, Dist::DIST_US>(preg0, ((__ubuf__ uint32_t*)selTail), 0);
                MaskReg dstReg = CreatePredicate<T>(sreg);
                DataCopy(vSrc0, src0Tail, 0);
                DataCopy(vSrc1, src1Tail, 0);
                PredicateUnPack(preg1, preg0);
                Select<T>(vDst, vSrc0, vSrc1, preg1);
                DataCopy(dstTail, vDst, 0, dstReg);
            }
        }
    }
}

template <typename T, bool isCounterMode>
__aicore__ inline void SelectWithoutMaskMode0ImplVF(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, __ubuf__ uint64_t* tempBuf, int32_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    Reg::RegTensor<T> srcReg0, srcReg1, dstReg;
    Reg::MaskReg maskReg, selMask;
    Reg::RegTensor<uint32_t> selReg;
    Reg::UnalignReg ureg;
    uint16_t newRepeatTimes = repeatTime;
    uint32_t sreg;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    if constexpr (sizeof(T) == 2) {
        Reg::DataCopy<uint32_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint32_t*)tempBuf);
    } else if constexpr (sizeof(T) == 4) {
        Reg::DataCopyUnAlignPre(ureg, (__ubuf__ uint32_t*)tempBuf);
        Reg::DataCopyUnAlign(selReg, ureg, (__ubuf__ uint32_t*)tempBuf);
        Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, selReg);
    }
    if constexpr (isCounterMode) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::DataCopy<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        maskReg = Reg::MoveMask<T>();
    }
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            maskReg = Reg::UpdateMask<T>(sreg);
        }
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg0, src0 + i * blockElm * repeatParams.src0RepStride,
            static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg1, src1 + i * blockElm * repeatParams.src1RepStride,
            static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
        Reg::Select(dstReg, srcReg0, srcReg1, selMask);
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * blockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            maskReg);
    }
}

template <typename T, bool isCounterMode>
__aicore__ inline void SelectWithoutMaskMode2ImplVF(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, __ubuf__ uint64_t* tempBuf, uint64_t selAddr,
    int32_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    Reg::RegTensor<T> srcReg0, srcReg1, dstReg;
    Reg::MaskReg maskReg, selMask;
    Reg::RegTensor<uint8_t> selReg;
    Reg::UnalignReg ureg;
    uint16_t newRepeatTimes = repeatTime;
    constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    uint32_t sreg;
    if constexpr (isCounterMode) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::DataCopy<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        maskReg = Reg::MoveMask<T>();
    }
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            maskReg = Reg::UpdateMask<T>(sreg);
        }
        if constexpr (sizeof(T) == 2) {
            Reg::DataCopy<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)selAddr + i * selOffset);
        } else if constexpr (sizeof(T) == 4) {
            Reg::DataCopyUnAlignPre(ureg, (__ubuf__ uint8_t*)selAddr + i * selOffset);
            Reg::DataCopyUnAlign(selReg, ureg, (__ubuf__ uint8_t*)selAddr + i * selOffset);
            Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, (Reg::RegTensor<uint32_t>&)selReg);
        }
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg0, src0 + i * blockElm * repeatParams.src0RepStride,
            static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg1, src1 + i * blockElm * repeatParams.src1RepStride,
            static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
        Reg::Select(dstReg, srcReg0, srcReg1, selMask);
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * blockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            maskReg);
    }
}

template <typename T, SELMODE selMode>
__aicore__ inline void SelectCal(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, int32_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2);
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    if constexpr (selMode == SELMODE::VSEL_CMPMASK_SPR) {
        if constexpr (sizeof(T) == 2) {
            (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf)) = Internal::g_cmpMaskLow;
            (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf + 1)) = Internal::g_cmpMaskHigh;
        } else {
            (*(__ubuf__ uint64_t*)((__ubuf__ uint64_t*)tempBuf)) = Internal::g_cmpMaskLow;
        }
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        if (isCounterMode) {
            VF_CALL<SelectWithoutMaskMode0ImplVF<T, true>>(dst, src0, src1, tempBuf, repeatTime, repeatParams);
        } else {
            VF_CALL<SelectWithoutMaskMode0ImplVF<T, false>>(dst, src0, src1, tempBuf, repeatTime, repeatParams);
        }
    } else if constexpr (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
        uint64_t selAddr = Internal::g_cmpMaskLow;
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        if (isCounterMode) {
            VF_CALL<SelectWithoutMaskMode2ImplVF<T, true>>(dst, src0, src1, tempBuf, selAddr, repeatTime, repeatParams);
        } else {
            VF_CALL<SelectWithoutMaskMode2ImplVF<T, false>>(
                dst, src0, src1, tempBuf, selAddr, repeatTime, repeatParams);
        }
    }
    AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
}

template <typename T, typename U, bool isCounterMode>
__simd_callee__ inline void SelectWithoutMaskMode1ImplVF(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T scalar, __ubuf__ uint64_t* tempBuf, int32_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    Reg::RegTensor<T> srcReg0, srcReg1, dstReg;
    Reg::MaskReg maskReg, selMask;
    Reg::RegTensor<uint8_t> selReg;
    Reg::UnalignReg ureg;
    uint16_t newRepeatTimes = repeatTime;
    uint32_t sreg;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
    if constexpr (isCounterMode) {
        maskReg = Reg::MoveMask<uint16_t>();
        Reg::DataCopy<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        sreg = static_cast<uint32_t>(tempBuf[0]);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        maskReg = Reg::MoveMask<T>();
    }
    Reg::Duplicate(srcReg1, scalar);
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (isCounterMode) {
            maskReg = Reg::UpdateMask<T>(sreg);
        }
        if constexpr (sizeof(T) == 2) {
            Reg::DataCopy<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
        } else if constexpr (sizeof(T) == 4) {
            Reg::DataCopyUnAlignPre(ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::DataCopyUnAlign(selReg, ureg, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskGenWithRegTensor<uint32_t, 0>(selMask, (Reg::RegTensor<uint32_t>&)selReg);
        }
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg0, src0 + i * blockElm * repeatParams.src0RepStride,
            static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
        Reg::Select(dstReg, srcReg0, srcReg1, selMask);
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * blockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            maskReg);
    }
}

template <typename T, typename U>
__aicore__ inline void SelectCal(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, int32_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    T scalar = *reinterpret_cast<T*>(&Internal::g_cmpMaskLow);
    event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
    SetFlag<HardEvent::S_V>(eventIdSToV);
    WaitFlag<HardEvent::S_V>(eventIdSToV);
    if (isCounterMode) {
        __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2);
        VF_CALL<SelectWithoutMaskMode1ImplVF<T, U, true>>(dst, sel, src0, scalar, tempBuf, repeatTime, repeatParams);
        AscendCUtils::FreeTemporaryBuffer<uint64_t>(tempBuf);
    } else {
        VF_CALL<SelectWithoutMaskMode1ImplVF<T, U, false>>(dst, sel, src0, scalar, nullptr, repeatTime, repeatParams);
    }
}

// ============ select mode: 0/2 ============
// ================Level2====================
template <typename T, typename U, bool isBitMap, bool isCounterMode>
__simd_callee__ inline void SelectMode0Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
    Reg::MaskReg maskReg;
    uint32_t sreg;
    uint16_t newRepeatTimes = repeatTime;
    if constexpr (isCounterMode) {
        sreg = static_cast<uint32_t>(mask);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        if constexpr (isBitMap) {
            maskReg = Reg::MoveMask<T>();
        } else {
            sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        }
    }
    Reg::MaskReg selMask;
    Reg::DataCopy<U, Reg::MaskDist::DIST_US>(selMask, sel);
    if constexpr (sizeof(T) == 4) {
        Reg::MaskUnPack(selMask, selMask);
    }
    for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
        if constexpr (isCounterMode) {
            maskReg = Reg::UpdateMask<T>(sreg);
        }
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            src0Reg, src0 + i * blockElm * repeatParams.src0RepStride,
            static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            src1Reg, src1 + i * blockElm * repeatParams.src1RepStride,
            static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
        Reg::Select(dstReg, src0Reg, src1Reg, selMask);
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * blockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            maskReg);
    }
}

template <typename T, typename U, bool isBitMap, bool isCounterMode>
__simd_callee__ inline void SelectMode2Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t newRepeatTimes = repeatTime;
    uint32_t sreg;
    if constexpr (sizeof(T) == 4) {
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        Reg::RegTensor<T> src0Reg, src1Reg, src2Reg, src3Reg, dst0Reg, dst1Reg;
        Reg::MaskReg maskReg;
        if constexpr (isCounterMode) {
            sreg = static_cast<uint32_t>(mask);
            newRepeatTimes = CeilDivision(sreg, oneRepSize);
        } else {
            if constexpr (isBitMap) {
                maskReg = Reg::MoveMask<T>();
            } else {
                sreg = static_cast<uint32_t>(mask);
                maskReg = Reg::UpdateMask<T>(sreg);
            }
        }

        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = newRepeatTimes % unRollConstant;
        newRepeatTimes = newRepeatTimes / unRollConstant;
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::DataCopy<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src0 + i * unRollConstant * blockElm * repeatParams.src0RepStride,
                static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src1 + i * unRollConstant * blockElm * repeatParams.src1RepStride,
                static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
            Reg::Select(dst0Reg, src0Reg, src1Reg, selMask0);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src2Reg, src0 + (i * unRollConstant + 1) * blockElm * repeatParams.src0RepStride,
                static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src3Reg, src1 + (i * unRollConstant + 1) * blockElm * repeatParams.src1RepStride,
                static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
            Reg::Select(dst1Reg, src2Reg, src3Reg, selMask1);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
        }
        Reg::RegTensor<T> src4Reg, src5Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src0RepStride * blockElm;
        uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.src1RepStride * blockElm;
        uint32_t offset2 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
        for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(tailSreg);
            }
            Reg::DataCopy<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src4Reg, src0 + offset0, static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src5Reg, src1 + offset1, static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
            Reg::Select(dst2Reg, src4Reg, src5Reg, selMask2);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + offset2, dst2Reg, static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
        }
    } else {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::MaskReg maskReg;
        if constexpr (isCounterMode) {
            sreg = static_cast<uint32_t>(mask);
            newRepeatTimes = CeilDivision(sreg, oneRepSize);
        } else {
            if constexpr (isBitMap) {
                maskReg = Reg::MoveMask<T>();
            } else {
                sreg = static_cast<uint32_t>(mask);
                maskReg = Reg::UpdateMask<T>(sreg);
            }
        }
        Reg::MaskReg selMask;
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::DataCopy<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src0 + i * blockElm * repeatParams.src0RepStride,
                static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src1 + i * blockElm * repeatParams.src1RepStride,
                static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * blockElm * repeatParams.dstRepStride, dstReg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            VF_CALL<SelectMode0Level0<T, U, false, true>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            VF_CALL<SelectMode2Level0<T, U, false, true>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
        }
    } else {
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            VF_CALL<SelectMode0Level0<T, U, false, false>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            VF_CALL<SelectMode2Level0<T, U, false, false>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SetVectorMask<T>(mask[1], mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            VF_CALL<SelectMode0Level0<T, U, true, true>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            VF_CALL<SelectMode2Level0<T, U, true, true>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        }
    } else {
        if (selMode == SELMODE::VSEL_CMPMASK_SPR) {
            VF_CALL<SelectMode0Level0<T, U, true, false>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        } else if (selMode == SELMODE::VSEL_TENSOR_TENSOR_MODE) {
            VF_CALL<SelectMode2Level0<T, U, true, false>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        }
    }
}
// ============ select mode: 1 ============
// ================Level0====================

template <typename T, typename U, bool isBitMap, bool isCounterMode>
__simd_callee__ inline void SelectMode1Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t newRepeatTimes = repeatTime;
    uint32_t sreg;
    if constexpr (sizeof(T) == 2) {
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::Duplicate(src1Reg, (const T&)src1);
        Reg::MaskReg maskReg;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        if constexpr (isCounterMode) {
            sreg = static_cast<uint32_t>(mask);
            newRepeatTimes = CeilDivision(sreg, oneRepSize);
        } else {
            if constexpr (isBitMap) {
                maskReg = Reg::MoveMask<T>();
            } else {
                sreg = static_cast<uint32_t>(mask);
                maskReg = Reg::UpdateMask<T>(sreg);
            }
        }
        Reg::MaskReg selMask;
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::DataCopy<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src0 + i * blockElm * repeatParams.src0RepStride,
                static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * blockElm * repeatParams.dstRepStride, dstReg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
        }
    } else {
        Reg::RegTensor<T> scalarReg, src0Reg, src1Reg, dst0Reg, dst1Reg;
        Reg::MaskReg maskReg;
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        if constexpr (isCounterMode) {
            sreg = static_cast<uint32_t>(mask);
            newRepeatTimes = CeilDivision(sreg, oneRepSize);
        } else {
            if constexpr (isBitMap) {
                maskReg = Reg::MoveMask<T>();
            } else {
                sreg = static_cast<uint32_t>(mask);
                maskReg = Reg::UpdateMask<T>(sreg);
            }
        }
        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = newRepeatTimes % unRollConstant;
        newRepeatTimes = newRepeatTimes / unRollConstant;
        Reg::Duplicate(scalarReg, (const T&)src1);
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::DataCopy<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src0 + i * unRollConstant * blockElm * repeatParams.src0RepStride,
                static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
            Reg::Select(dst0Reg, src0Reg, scalarReg, selMask0);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src0 + (i * unRollConstant + 1) * blockElm * repeatParams.src0RepStride,
                static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
            Reg::Select(dst1Reg, src1Reg, scalarReg, selMask1);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
        }
        Reg::RegTensor<T> src2Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src0RepStride * blockElm;
        uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
        for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(tailSreg);
            }
            Reg::DataCopy<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src2Reg, src0 + offset0, static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
            Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + offset1, dst2Reg, static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, SELMODE selMode, const uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        VF_CALL<SelectMode1Level0<T, U, false, true>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
    } else {
        VF_CALL<SelectMode1Level0<T, U, false, false>>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, SELMODE selMode, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SetVectorMask<T>(mask[1], mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        VF_CALL<SelectMode1Level0<T, U, true, true>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
    } else {
        VF_CALL<SelectMode1Level0<T, U, true, false>>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
    }
}

// ===============  Src0 Scalar =====================
template <typename T, typename U, bool isBitMap, bool isCounterMode>
__simd_vf__ inline void SelectSrc0ScalarMode1Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    uint32_t sreg;
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t newRepeatTimes = repeatTime;
    Reg::MaskReg maskReg;
    if constexpr (isCounterMode) {
        sreg = static_cast<uint32_t>(mask);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        if constexpr (isBitMap) {
            maskReg = Reg::MoveMask<T>();
        } else {
            sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        }
    }
    if constexpr (sizeof(T) == 2) {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
        Reg::Duplicate(src0Reg, (const T&)src0);
        Reg::MaskReg selMask;
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src1 + i * blockElm * repeatParams.src1RepStride,
                static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * blockElm * repeatParams.dstRepStride, dstReg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
        }
    } else {
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        Reg::RegTensor<T> scalarReg, src0Reg, src1Reg, dst0Reg, dst1Reg;
        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = newRepeatTimes % unRollConstant;
        newRepeatTimes = newRepeatTimes / unRollConstant;
        Reg::Duplicate(scalarReg, (const T&)src0);
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src0Reg, src1 + i * unRollConstant * blockElm * repeatParams.src1RepStride,
                static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
            Reg::Select(dst0Reg, scalarReg, src0Reg, selMask0);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(sreg);
            }
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src1Reg, src1 + (i * unRollConstant + 1) * blockElm * repeatParams.src1RepStride,
                static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
            Reg::Select(dst1Reg, scalarReg, src1Reg, selMask1);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
        }
        Reg::RegTensor<T> src2Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src1RepStride * blockElm;
        uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
        for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
            if constexpr (isCounterMode) {
                maskReg = Reg::UpdateMask<T>(tailSreg);
            }
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                src2Reg, src1 + offset0, static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
            Reg::Select(dst2Reg, scalarReg, src2Reg, selMask2);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + offset1, dst2Reg, static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        SelectSrc0ScalarMode1Level0<T, U, false, true>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
    } else {
        SelectSrc0ScalarMode1Level0<T, U, false, false>(dst, sel, src0, src1, mask, repeatTime, repeatParams);
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SetVectorMask<T>(mask[1], mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        SelectSrc0ScalarMode1Level0<T, U, true, true>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
    } else {
        SelectSrc0ScalarMode1Level0<T, U, true, false>(dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
    }
}

// both src0 / src1 are tensor
template <typename T, typename U, bool isBitMap, uint8_t scalarIdx, Reg::LoadDist pattern, bool isCounterMode>
__simd_vf__ inline void SelectBothTensorMode1Level0(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams repeatParams)
{
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    uint16_t newRepeatTimes = repeatTime;
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(T);
    uint32_t sreg;
    Reg::MaskReg maskReg, selMask;
    if constexpr (isCounterMode) {
        sreg = static_cast<uint32_t>(mask);
        newRepeatTimes = CeilDivision(sreg, oneRepSize);
    } else {
        if constexpr (isBitMap) {
            maskReg = Reg::MoveMask<T>();
        } else {
            sreg = static_cast<uint32_t>(mask);
            maskReg = Reg::UpdateMask<T>(sreg);
        }
    }
    if constexpr (scalarIdx == 0) {
        if constexpr (sizeof(T) == 2) {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
            Reg::LoadAlign<T, pattern>(src0Reg, src0);
            for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src1Reg, src1 + i * blockElm * repeatParams.src1RepStride,
                    static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + i * blockElm * repeatParams.dstRepStride, dstReg,
                    static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
            }
        } else {
            constexpr uint32_t unRollConstant = 2;
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
            Reg::RegTensor<T> scalarReg, src0Reg, src1Reg, dst0Reg, dst1Reg;
            Reg::MaskReg selMask0, selMask1, tmpMask0;
            Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
            uint16_t tail = newRepeatTimes % unRollConstant;
            newRepeatTimes = newRepeatTimes / unRollConstant;
            Reg::LoadAlign<T, pattern>(scalarReg, src0);
            for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src0Reg, src1 + i * unRollConstant * blockElm * repeatParams.src1RepStride,
                    static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
                Reg::Select(dst0Reg, scalarReg, src0Reg, selMask0);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                    static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src1Reg, src1 + (i * unRollConstant + 1) * blockElm * repeatParams.src1RepStride,
                    static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
                Reg::Select(dst1Reg, scalarReg, src1Reg, selMask1);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                    static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
            }
            Reg::RegTensor<T> src2Reg, dst2Reg;
            Reg::MaskReg selMask2;
            uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src0RepStride * blockElm;
            uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
            uint32_t newSelOffset = newRepeatTimes * selOffset;
            uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
            for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(tailSreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
                Reg::MaskUnPack(selMask2, selMask2);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src2Reg, src1 + offset0, static_cast<uint32_t>(repeatParams.src1BlkStride), maskReg);
                Reg::Select(dst2Reg, scalarReg, src2Reg, selMask2);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + offset1, dst2Reg, static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
            }
        }
    } else if constexpr (scalarIdx == 1) {
        if constexpr (sizeof(T) == 2) {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            Reg::RegTensor<T> src0Reg, src1Reg, dstReg;
            Reg::LoadAlign<T, pattern>(src1Reg, src1);
            for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src0Reg, src0 + i * blockElm * repeatParams.src0RepStride,
                    static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + i * blockElm * repeatParams.dstRepStride, dstReg,
                    static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
            }
        } else {
            constexpr uint32_t unRollConstant = 2;
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
            Reg::RegTensor<T> scalarReg, src0Reg, src1Reg, dst0Reg, dst1Reg;
            Reg::MaskReg selMask0, selMask1, tmpMask0;
            Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
            uint16_t tail = newRepeatTimes % unRollConstant;
            newRepeatTimes = newRepeatTimes / unRollConstant;
            Reg::LoadAlign<T, pattern>(scalarReg, src1);
            for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src0Reg, src0 + i * unRollConstant * blockElm * repeatParams.src0RepStride,
                    static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
                Reg::Select(dst0Reg, src0Reg, scalarReg, selMask0);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + i * unRollConstant * blockElm * repeatParams.dstRepStride, dst0Reg,
                    static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(sreg);
                }
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src1Reg, src0 + (i * unRollConstant + 1) * blockElm * repeatParams.src0RepStride,
                    static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
                Reg::Select(dst1Reg, src1Reg, scalarReg, selMask1);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + (i * unRollConstant + 1) * blockElm * repeatParams.dstRepStride, dst1Reg,
                    static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
            }
            Reg::RegTensor<T> src2Reg, dst2Reg;
            Reg::MaskReg selMask2;
            uint32_t offset0 = newRepeatTimes * unRollConstant * repeatParams.src0RepStride * blockElm;
            uint32_t offset1 = newRepeatTimes * unRollConstant * repeatParams.dstRepStride * blockElm;
            uint32_t newSelOffset = newRepeatTimes * selOffset;
            uint32_t tailSreg = sreg - unRollConstant * newRepeatTimes * oneRepSize;
            for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
                if constexpr (isCounterMode) {
                    maskReg = Reg::UpdateMask<T>(tailSreg);
                }
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
                Reg::MaskUnPack(selMask2, selMask2);
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    src2Reg, src0 + offset0, static_cast<uint32_t>(repeatParams.src0BlkStride), maskReg);
                Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                    dst + offset1, dst2Reg, static_cast<uint32_t>(repeatParams.dstBlkStride), maskReg);
            }
        }
    }
}

template <typename T, typename U, uint8_t scalarIdx>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if constexpr (sizeof(T) == 2) {
            SelectBothTensorMode1Level0<T, U, false, scalarIdx, Reg::LoadDist::DIST_BRC_B16, true>(
                dst, sel, src0, src1, mask, repeatTime, repeatParams);
        } else if constexpr (sizeof(T) == 4) {
            SelectBothTensorMode1Level0<T, U, false, scalarIdx, Reg::LoadDist::DIST_BRC_B32, true>(
                dst, sel, src0, src1, mask, repeatTime, repeatParams);
        }
    } else {
        if constexpr (sizeof(T) == 2) {
            SelectBothTensorMode1Level0<T, U, false, scalarIdx, Reg::LoadDist::DIST_BRC_B16, false>(
                dst, sel, src0, src1, mask, repeatTime, repeatParams);
        } else if constexpr (sizeof(T) == 4) {
            SelectBothTensorMode1Level0<T, U, false, scalarIdx, Reg::LoadDist::DIST_BRC_B32, false>(
                dst, sel, src0, src1, mask, repeatTime, repeatParams);
        }
    }
}

template <typename T, typename U, uint8_t scalarIdx>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, int16_t, uint16_t, int32_t, uint32_t, float>(), "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SetVectorMask<T>(mask[1], mask[0]);
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        if constexpr (sizeof(T) == 2) {
            SelectBothTensorMode1Level0<T, U, true, scalarIdx, Reg::LoadDist::DIST_BRC_B16, true>(
                dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        } else if constexpr (sizeof(T) == 4) {
            SelectBothTensorMode1Level0<T, U, true, scalarIdx, Reg::LoadDist::DIST_BRC_B32, true>(
                dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        }
    } else {
        if constexpr (sizeof(T) == 2) {
            SelectBothTensorMode1Level0<T, U, true, scalarIdx, Reg::LoadDist::DIST_BRC_B16, false>(
                dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        } else if constexpr (sizeof(T) == 4) {
            SelectBothTensorMode1Level0<T, U, true, scalarIdx, Reg::LoadDist::DIST_BRC_B32, false>(
                dst, sel, src0, src1, mask[0], repeatTime, repeatParams);
        }
    }
}

// ============ select mode: 1 ============
// =============== LEVEL2 ===================
template <typename T, typename U, typename RegT>
__simd_vf__ inline void SelectMode1Level2(__ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T) * RegT::trait.REG_NUM;
    uint32_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 4) {
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        RegT src0Reg, src1Reg, scalarReg, dst0Reg, dst1Reg;
        Reg::MaskReg maskReg;
        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = repeatTime % unRollConstant;
        uint16_t newRepeatTimes = repeatTime / unRollConstant;
        Reg::Duplicate(scalarReg, (const T&)src1);
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * unRollConstant * repeatElm);
            Reg::Select(dst0Reg, src0Reg, scalarReg, selMask0);
            Reg::StoreAlign<T>(dst + i * unRollConstant * repeatElm, dst0Reg, maskReg);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src1Reg, src0 + (i * unRollConstant + 1) * repeatElm);
            Reg::Select(dst1Reg, src1Reg, scalarReg, selMask1);
            Reg::StoreAlign<T>(dst + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
        }
        RegT src2Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset = newRepeatTimes * unRollConstant * repeatElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src2Reg, src0 + offset);
            Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
            Reg::StoreAlign<T>(dst + offset, dst2Reg, maskReg);
        }
    } else {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        RegT src0Reg, src1Reg, dstReg;
        uint32_t sreg = static_cast<uint32_t>(calCount);
        Reg::MaskReg maskReg, selMask;
        Reg::Duplicate(src1Reg, (const T&)src1);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            if constexpr (sizeof(T) == 2) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            } else {
                Reg::LoadAlign<uint8_t>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            }
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, T src1, SELMODE selMode, uint32_t calCount)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, half, int16_t, uint16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SelectMode1Level2<T, U, Reg::RegTensor<T>>(dst, sel, src0, src1, calCount);
}
// Src0Scalar
template <typename T, typename U, typename RegT>
__simd_vf__ inline void SelectSrc0ScalarMode1Level2(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T) * RegT::trait.REG_NUM;
    uint32_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 4) {
        constexpr uint32_t unRollConstant = 2;
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
        RegT src0Reg, src1Reg, scalarReg, dst0Reg, dst1Reg;
        Reg::MaskReg maskReg;
        Reg::MaskReg selMask0, selMask1, tmpMask0;
        Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
        uint16_t tail = repeatTime % unRollConstant;
        uint16_t newRepeatTimes = repeatTime / unRollConstant;
        Reg::Duplicate(scalarReg, (const T&)src0);
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
            Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src1 + i * unRollConstant * repeatElm);
            Reg::Select(dst0Reg, scalarReg, src0Reg, selMask0);
            Reg::StoreAlign<T>(dst + i * unRollConstant * repeatElm, dst0Reg, maskReg);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src1Reg, src1 + (i * unRollConstant + 1) * repeatElm);
            Reg::Select(dst1Reg, scalarReg, src1Reg, selMask1);
            Reg::StoreAlign<T>(dst + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
        }
        RegT src2Reg, dst2Reg;
        Reg::MaskReg selMask2;
        uint32_t offset = newRepeatTimes * unRollConstant * repeatElm;
        uint32_t newSelOffset = newRepeatTimes * selOffset;
        for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
            Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src2Reg, src1 + offset);
            Reg::Select(dst2Reg, scalarReg, src2Reg, selMask2);
            Reg::StoreAlign<T>(dst + offset, dst2Reg, maskReg);
        }
    } else {
        constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
        RegT src0Reg, src1Reg, dstReg;
        Reg::MaskReg maskReg, selMask;
        Reg::Duplicate(src0Reg, (const T&)src0);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            if constexpr (sizeof(T) == 2) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            } else {
                Reg::LoadAlign<uint8_t>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
            }
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
        }
    }
}

template <typename T, typename U>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, T src0, __ubuf__ T* src1, SELMODE selMode, uint32_t calCount)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, half, int16_t, uint16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    SelectSrc0ScalarMode1Level2<T, U, Reg::RegTensor<T>>(dst, sel, src0, src1, calCount);
}
// both src0 / src1 Tensor
template <typename T, typename U, typename RegT, uint8_t scalarIdx, Reg::LoadDist pattern = Reg::LoadDist::DIST_BRC_B32>
__simd_vf__ inline void SelectBothTensorMode1Level2(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, uint32_t calCount)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T) * RegT::trait.REG_NUM;
    uint32_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (scalarIdx == 0) {
        if constexpr (sizeof(T) == 4) {
            constexpr uint32_t unRollConstant = 2;
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
            RegT src0Reg, src1Reg, scalarReg, dst0Reg, dst1Reg;
            Reg::MaskReg maskReg;
            Reg::MaskReg selMask0, selMask1, tmpMask0;
            Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
            uint16_t tail = repeatTime % unRollConstant;
            uint16_t newRepeatTimes = repeatTime / unRollConstant;
            Reg::LoadAlign<T, pattern>(scalarReg, src0);
            for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src0Reg, src1 + i * unRollConstant * repeatElm);
                Reg::Select(dst0Reg, scalarReg, src0Reg, selMask0);
                Reg::StoreAlign<T>(dst + i * unRollConstant * repeatElm, dst0Reg, maskReg);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src1Reg, src1 + (i * unRollConstant + 1) * repeatElm);
                Reg::Select(dst1Reg, scalarReg, src1Reg, selMask1);
                Reg::StoreAlign<T>(dst + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
            }
            RegT src2Reg, dst2Reg;
            Reg::MaskReg selMask2;
            uint32_t offset = newRepeatTimes * unRollConstant * repeatElm;
            uint32_t newSelOffset = newRepeatTimes * selOffset;
            for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
                Reg::MaskUnPack(selMask2, selMask2);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src2Reg, src1 + offset);
                Reg::Select(dst2Reg, scalarReg, src2Reg, selMask2);
                Reg::StoreAlign<T>(dst + offset, dst2Reg, maskReg);
            }
        } else {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            RegT src0Reg, src1Reg, dstReg;
            Reg::MaskReg maskReg, selMask;
            Reg::LoadAlign<T, pattern>(src0Reg, src0);
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
                if constexpr (sizeof(T) == 2) {
                    Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                } else {
                    Reg::LoadAlign<uint8_t>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                }
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src1Reg, src1 + i * repeatElm);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
            }
        }
    } else if constexpr (scalarIdx == 1) {
        if constexpr (sizeof(T) == 4) {
            constexpr uint32_t unRollConstant = 2;
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T) * unRollConstant;
            RegT src0Reg, src1Reg, scalarReg, dst0Reg, dst1Reg;
            Reg::MaskReg maskReg;
            Reg::MaskReg selMask0, selMask1, tmpMask0;
            Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
            uint16_t tail = repeatTime % unRollConstant;
            uint16_t newRepeatTimes = repeatTime / unRollConstant;
            Reg::LoadAlign<T, pattern>(scalarReg, src1);
            for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(tmpMask0, (__ubuf__ uint8_t*)sel + i * selOffset);
                Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src0Reg, src0 + i * unRollConstant * repeatElm);
                Reg::Select(dst0Reg, src0Reg, scalarReg, selMask0);
                Reg::StoreAlign<T>(dst + i * unRollConstant * repeatElm, dst0Reg, maskReg);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src1Reg, src0 + (i * unRollConstant + 1) * repeatElm);
                Reg::Select(dst1Reg, src1Reg, scalarReg, selMask1);
                Reg::StoreAlign<T>(dst + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
            }
            RegT src2Reg, dst2Reg;
            Reg::MaskReg selMask2;
            uint32_t offset = newRepeatTimes * unRollConstant * repeatElm;
            uint32_t newSelOffset = newRepeatTimes * selOffset;
            for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
                Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint8_t*)sel + newSelOffset);
                Reg::MaskUnPack(selMask2, selMask2);
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src2Reg, src0 + offset);
                Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
                Reg::StoreAlign<T>(dst + offset, dst2Reg, maskReg);
            }
        } else {
            constexpr uint32_t selOffset = GetVecLen() / CmpSelInternal::maskBitToByte / sizeof(T);
            RegT src0Reg, src1Reg, dstReg;
            Reg::MaskReg maskReg, selMask;
            Reg::LoadAlign<T, pattern>(src1Reg, src1);
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
                if constexpr (sizeof(T) == 2) {
                    Reg::LoadAlign<uint8_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                } else {
                    Reg::LoadAlign<uint8_t>(selMask, (__ubuf__ uint8_t*)sel + i * selOffset);
                }
                maskReg = Reg::UpdateMask<T>(sreg);
                Reg::LoadAlign<T>(src0Reg, src0 + i * repeatElm);
                Reg::Select(dstReg, src0Reg, src1Reg, selMask);
                Reg::StoreAlign<T>(dst + i * repeatElm, dstReg, maskReg);
            }
        }
    }
}

template <typename T, typename U, uint8_t scalarIdx>
__aicore__ inline void VselImpl(
    __ubuf__ T* dst, __ubuf__ U* sel, __ubuf__ T* src0, __ubuf__ T* src1, SELMODE selMode, uint32_t calCount)
{
    static_assert(
        SupportType<T, uint8_t, int8_t, half, int16_t, uint16_t, int32_t, uint32_t, float>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, uint16_t, uint32_t, uint64_t>(), "current data type is not supported!");
    if constexpr (sizeof(T) == 1) {
        SelectBothTensorMode1Level2<T, U, Reg::RegTensor<T>, scalarIdx, Reg::LoadDist::DIST_BRC_B8>(
            dst, sel, src0, src1, calCount);
    } else if constexpr (sizeof(T) == 2) {
        SelectBothTensorMode1Level2<T, U, Reg::RegTensor<T>, scalarIdx, Reg::LoadDist::DIST_BRC_B16>(
            dst, sel, src0, src1, calCount);
    } else if constexpr (sizeof(T) == 4) {
        SelectBothTensorMode1Level2<T, U, Reg::RegTensor<T>, scalarIdx, Reg::LoadDist::DIST_BRC_B32>(
            dst, sel, src0, src1, calCount);
    } else {
        SelectBothTensorMode1Level2<
            T, U, Reg::RegTensor<T, Reg::RegTraitNumTwo>, scalarIdx, Reg::LoadDist::DIST_BRC_B32>(
            dst, sel, src0, src1, calCount);
    }
}

template <typename T>
__aicore__ inline void GetCmpMaskImpl(__ubuf__ T* dst)
{
    pipe_barrier(PIPE_ALL);
    ((__ubuf__ uint64_t*)dst)[0] = Internal::g_cmpMaskLow;
    ((__ubuf__ uint64_t*)dst)[1] = Internal::g_cmpMaskHigh;
    pipe_barrier(PIPE_ALL);
}

template <typename T>
__aicore__ inline void SetCmpMaskImpl(__ubuf__ T* src)
{
    pipe_barrier(PIPE_ALL);
    Internal::g_cmpMaskLow = reinterpret_cast<uint64_t>(((__ubuf__ uint64_t*)src)[0]);
    Internal::g_cmpMaskHigh = reinterpret_cast<uint64_t>(((__ubuf__ uint64_t*)src)[1]);
    pipe_barrier(PIPE_ALL);
}

template <typename T, bool isSetMask>
__aicore__ inline void VcmpImpl(
    __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask[2], const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Vcmp is not supported!"); });
}

template <typename T, bool isSetMask>
__aicore__ inline void VcmpImpl(
    __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint64_t mask, const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Vcmp is not supported!"); });
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_IMPL_H__
#endif
