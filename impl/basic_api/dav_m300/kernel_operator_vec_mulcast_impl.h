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
 * \file kernel_operator_vec_mulcast_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m300/kernel_operator_vec_mulcast_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_MULCAST_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_MULCAST_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_MULCAST_IMPL_H
#include "../../../include/basic_api/kernel_tensor.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../../../include/basic_api/kernel_struct_binary.h"
#if ASCENDC_CPU_DEBUG
#include "../kernel_check.h"

#endif

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_M300_STAGE_MULCAST_LOAD_AND_COMPUTE(                                                          \
    repeatParams, repeatTime, vreg0, src0, preg, vreg1, src1, vreg2, vreg3, DataType, dst, pregLower)     \
    uint32_t strideConfig0 = (static_cast<uint32_t>(repeatParams.src0BlkStride) << 16);                   \
    uint32_t strideConfig1 = (static_cast<uint32_t>(repeatParams.src1BlkStride) << 16);                   \
    uint32_t strideConfig2 = (static_cast<uint32_t>(repeatParams.dstBlkStride) << 16);                    \
    uint32_t strideOffset0 = static_cast<uint32_t>(repeatParams.src0RepStride * 256 / 16);                \
    uint32_t strideOffset1 = static_cast<uint32_t>(repeatParams.src1RepStride * 256 / 16);                \
    uint32_t strideOffset2 = static_cast<uint32_t>(repeatParams.dstRepStride * 256 / 8);                  \
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                                    \
        mem_bar(VST_VLD);                                                                                 \
        vsldb(vreg0, (__ubuf__ half*)src0.GetPhyAddr() + i * strideOffset0, strideConfig0, preg);         \
        vsldb(vreg1, (__ubuf__ half*)src1.GetPhyAddr() + i * strideOffset1, strideConfig1, preg);         \
        vmul(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                    \
        vcvt(vreg3, vreg2, preg, ROUND_R, RS_ENABLE, PART_EVEN);                                          \
        vpack((vector_u8&)vreg3, (vector_u16&)vreg3, LOWER, MODE_ZEROING);                                \
        vsstb(vreg3, (__ubuf__ DataType*)dst.GetPhyAddr() + i * strideOffset2, strideConfig2, pregLower); \
    }

#pragma begin_pipe(V)
namespace AscendC {
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulCastCalc(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M300_DEFINE_MULCAST_1(DataType, VectorType)                                                                \
    template <typename T = DataType, typename U = half, bool isSetMask = true>                                         \
    __aicore__ inline void MulCastCalc(                                                                                \
        const LocalTensor<DataType>& dst, const LocalTensor<half>& src0, const LocalTensor<half>& src1, uint64_t mask, \
        const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                                              \
    {                                                                                                                  \
        static_assert(SupportType<PrimT<U>, half>(), "MulCast level-0 api only support half on current device");       \
        static_assert(                                                                                                 \
            SupportType<PrimT<T>, DataType>(), "MulCast level-0 api only support int8_t/uint8_t on current device");   \
        __VEC_SCOPE__                                                                                                  \
        {                                                                                                              \
            vector_f16 vreg0;                                                                                          \
            vector_f16 vreg1;                                                                                          \
            vector_f16 vreg2;                                                                                          \
            VectorType vreg3;                                                                                          \
            uint32_t sregLower = static_cast<uint32_t>(mask);                                                          \
            uint32_t sregUpper = static_cast<uint32_t>(mask);                                                          \
            vector_bool pregLower = plt_b8(sregLower, POST_UPDATE);                                                    \
            vector_bool preg = plt_b16(sregUpper, POST_UPDATE);                                                        \
            ASC_M300_STAGE_MULCAST_LOAD_AND_COMPUTE(                                                                   \
                repeatParams, repeatTime, vreg0, src0, preg, vreg1, src1, vreg2, vreg3, DataType, dst, pregLower)      \
        }                                                                                                              \
    }

ASC_M300_DEFINE_MULCAST_1(int8_t, vector_s8)
ASC_M300_DEFINE_MULCAST_1(uint8_t, vector_u8)

#undef ASC_M300_DEFINE_MULCAST_1

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulCastCalc(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_MULCAST_2(DataType, VectorType)                                                              \
    template <typename T = DataType, typename U = half, bool isSetMask = true>                                       \
    __aicore__ inline void MulCastCalc(                                                                              \
        const LocalTensor<DataType>& dst, const LocalTensor<half>& src0, const LocalTensor<half>& src1,              \
        uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)                           \
    {                                                                                                                \
        static_assert(SupportType<PrimT<U>, half>(), "MulCast level-0 api only support half on current device");     \
        static_assert(                                                                                               \
            SupportType<PrimT<T>, DataType>(), "MulCast level-0 api only support int8_t/uint8_t on current device"); \
        if constexpr (isSetMask) {                                                                                   \
            SetVectorMask<U>(mask[1], mask[0]);                                                                      \
        }                                                                                                            \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            vector_f16 vreg0;                                                                                        \
            vector_f16 vreg1;                                                                                        \
            vector_f16 vreg2;                                                                                        \
            VectorType vreg3;                                                                                        \
            vector_bool pregLower;                                                                                   \
            vector_bool preg;                                                                                        \
            preg = movp_b16();                                                                                       \
            ppack(pregLower, preg, LOWER);                                                                           \
            ASC_M300_STAGE_MULCAST_LOAD_AND_COMPUTE(                                                                 \
                repeatParams, repeatTime, vreg0, src0, preg, vreg1, src1, vreg2, vreg3, DataType, dst, pregLower)    \
        }                                                                                                            \
    }

ASC_M300_DEFINE_MULCAST_2(int8_t, vector_s8)
ASC_M300_DEFINE_MULCAST_2(uint8_t, vector_u8)

#undef ASC_M300_DEFINE_MULCAST_2

template <typename T, typename U>
__aicore__ inline void MulCastCalc(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint32_t count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_MULCAST_3(DataType, VectorType)                                                              \
    template <typename T = DataType, typename U = half>                                                              \
    __aicore__ inline void MulCastCalc(                                                                              \
        const LocalTensor<DataType>& dst, const LocalTensor<half>& src0, const LocalTensor<half>& src1,              \
        uint32_t count)                                                                                              \
    {                                                                                                                \
        static_assert(                                                                                               \
            SupportType<PrimT<T>, DataType>(), "MulCast level-2 api only support int8_t/uint8_t on current device"); \
        static_assert(SupportType<PrimT<U>, half>(), "MulCast level-2 api only support half on current device");     \
        __VEC_SCOPE__                                                                                                \
        {                                                                                                            \
            vector_f16 vreg0;                                                                                        \
            vector_f16 vreg1;                                                                                        \
            vector_f16 vreg2;                                                                                        \
            VectorType vreg3;                                                                                        \
            uint32_t sreg = static_cast<uint32_t>(count);                                                            \
            vector_bool preg;                                                                                        \
            uint32_t sregLower = static_cast<uint32_t>(128);                                                         \
            uint16_t repeatTime = CeilDivision(count, 128);                                                          \
            for (uint16_t i = 0; i < repeatTime; ++i) {                                                              \
                preg = plt_b16(sreg, POST_UPDATE);                                                                   \
                vlds(vreg0, (__ubuf__ half*)src0.GetPhyAddr(), i* sregLower, NORM);                                  \
                vlds(vreg1, (__ubuf__ half*)src1.GetPhyAddr(), i* sregLower, NORM);                                  \
                vmul(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                       \
                vcvt(vreg3, vreg2, preg, ROUND_R, RS_ENABLE, PART_EVEN);                                             \
                vsts(vreg3, (__ubuf__ DataType*)dst.GetPhyAddr(), i* sregLower, PK_B16, preg);                       \
            }                                                                                                        \
        }                                                                                                            \
    }

ASC_M300_DEFINE_MULCAST_3(int8_t, vector_s8)
ASC_M300_DEFINE_MULCAST_3(uint8_t, vector_u8)

#undef ASC_M300_DEFINE_MULCAST_3
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_MULCAST_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_MULCAST_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_MULCAST_IMPL_H__
#endif

#undef ASC_M300_STAGE_MULCAST_LOAD_AND_COMPUTE
