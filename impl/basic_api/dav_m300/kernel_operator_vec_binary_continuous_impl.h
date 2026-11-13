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
 * \file kernel_operator_vec_binary_continuous_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m300/kernel_operator_vec_binary_continuous_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#include "../kernel_utils.h"

// Expand shared register stages in place to preserve the compiler input and instruction order.
#define ASC_M300_STAGE_CONTIGUOUS_INIT_COUNT_MASK(VectorType, vreg1, vreg2, vreg3, count, ElementCount) \
    VectorType vreg1;                                                                                   \
    VectorType vreg2;                                                                                   \
    VectorType vreg3;                                                                                   \
    uint32_t sreg = static_cast<uint32_t>(count);                                                       \
    vector_bool preg;                                                                                   \
    uint32_t sregLower = static_cast<uint32_t>(ElementCount);                                           \
    uint16_t repeatTime = CeilDivision(count, ElementCount)

namespace AscendC {
/* **************************************************************************************************
 * Add                                             *
 * ************************************************************************************************* */
// Add::Level 2
template <typename T>
__aicore__ inline void AddImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M300_DEFINE_CONTIGUOUS_1(                                                                   \
    Function, DataType, VectorType, ElementCount, MaskIntrinsic, VectorIntrinsic, NORM_B16Arg)          \
    __aicore__ inline void Function(                                                                    \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const int32_t& count) \
    {                                                                                                   \
        __VEC_SCOPE__                                                                                   \
        {                                                                                               \
            VectorType vreg0;                                                                           \
            VectorType vreg1;                                                                           \
            VectorType vreg2;                                                                           \
            uint32_t sreg = static_cast<uint32_t>(count);                                               \
            vector_bool preg;                                                                           \
            uint32_t sregLower = static_cast<uint32_t>(ElementCount);                                   \
            uint16_t repeatTime = CeilDivision(count, ElementCount);                                    \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                          \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                \
                vlds(vreg0, src0, i* sregLower, NORM);                                                  \
                vlds(vreg1, src1, i* sregLower, NORM);                                                  \
                VectorIntrinsic(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                               \
                vsts(vreg2, dst, i* sregLower, NORM_B16Arg, preg);                                      \
            }                                                                                           \
        }                                                                                               \
    }

ASC_M300_DEFINE_CONTIGUOUS_1(AddImpl, half, vector_f16, 128, plt_b16, vadd, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(AddImpl, int16_t, vector_s16, 128, plt_b16, vadd, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(AddImpl, int32_t, vector_s32, 64, plt_b32, vadd, NORM_B32)
ASC_M300_DEFINE_CONTIGUOUS_1(AddImpl, float, vector_f32, 64, plt_b32, vadd, NORM_B32)

/* **************************************************************************************************
 * Sub                                             *
 * ************************************************************************************************* */
// Sub::Level 2
template <typename T>
__aicore__ inline void SubImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_CONTIGUOUS_1(SubImpl, half, vector_f16, 128, plt_b16, vsub, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(SubImpl, int16_t, vector_s16, 128, plt_b16, vsub, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(SubImpl, int32_t, vector_s32, 64, plt_b32, vsub, NORM_B32)
ASC_M300_DEFINE_CONTIGUOUS_1(SubImpl, float, vector_f32, 64, plt_b32, vsub, NORM_B32)

/* **************************************************************************************************
 * Mul                                             *
 * ************************************************************************************************* */
// Mul::Level 2
template <typename T>
__aicore__ inline void MulImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_CONTIGUOUS_1(MulImpl, half, vector_f16, 128, plt_b16, vmul, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(MulImpl, int16_t, vector_s16, 128, plt_b16, vmul, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(MulImpl, int32_t, vector_s32, 64, plt_b32, vmul, NORM_B32)
ASC_M300_DEFINE_CONTIGUOUS_1(MulImpl, float, vector_f32, 64, plt_b32, vmul, NORM_B32)

/* **************************************************************************************************
 * Div                                             *
 * ************************************************************************************************* */
// Div::Level 2
template <typename T>
__aicore__ inline void DivImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_CONTIGUOUS_1(DivImpl, half, vector_f16, 128, plt_b16, vdiv, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(DivImpl, float, vector_f32, 64, plt_b32, vdiv, NORM_B32)

/* **************************************************************************************************
 * Max                                             *
 * ************************************************************************************************* */
// Max::Level 2
template <typename T>
__aicore__ inline void MaxImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_CONTIGUOUS_1(MaxImpl, half, vector_f16, 128, plt_b16, vmax, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(MaxImpl, int16_t, vector_s16, 128, plt_b16, vmax, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(MaxImpl, int32_t, vector_s32, 64, plt_b32, vmax, NORM_B32)
ASC_M300_DEFINE_CONTIGUOUS_1(MaxImpl, float, vector_f32, 64, plt_b32, vmax, NORM_B32)

/* **************************************************************************************************
 * Min                                             *
 * ************************************************************************************************* */
// Min::Level 2
template <typename T>
__aicore__ inline void MinImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_CONTIGUOUS_1(MinImpl, half, vector_f16, 128, plt_b16, vmin, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(MinImpl, int16_t, vector_s16, 128, plt_b16, vmin, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(MinImpl, int32_t, vector_s32, 64, plt_b32, vmin, NORM_B32)
ASC_M300_DEFINE_CONTIGUOUS_1(MinImpl, float, vector_f32, 64, plt_b32, vmin, NORM_B32)

/* **************************************************************************************************
 * And                                             *
 * ************************************************************************************************* */
// And::Level 2
template <typename T>
__aicore__ inline void AndImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_CONTIGUOUS_1(AndImpl, int16_t, vector_s16, 128, plt_b16, vand, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(AndImpl, uint16_t, vector_u16, 128, plt_b16, vand, NORM_B16)

/* **************************************************************************************************
 * Or                                             *
 * ************************************************************************************************* */
// Or::Level 2
template <typename T>
__aicore__ inline void OrImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

ASC_M300_DEFINE_CONTIGUOUS_1(OrImpl, int16_t, vector_s16, 128, plt_b16, vor, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_1(OrImpl, uint16_t, vector_u16, 128, plt_b16, vor, NORM_B16)

#undef ASC_M300_DEFINE_CONTIGUOUS_1

/* **************************************************************************************************
 * AddRelu                                             *
 * ************************************************************************************************* */
// AddRelu::Level 2
template <typename T>
__aicore__ inline void AddReluImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_CONTIGUOUS_2(DataType, VectorType, ElementCount, MaskIntrinsic, NORM_B16Arg)                \
    template <typename T = DataType>                                                                                \
    __aicore__ inline void AddReluImpl(                                                                             \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const int32_t& count)             \
    {                                                                                                               \
        static_assert(SupportType<T, DataType>(), "AddRelu level-2 api only support half/float on current device"); \
        __VEC_SCOPE__                                                                                               \
        {                                                                                                           \
            VectorType vreg0;                                                                                       \
            ASC_M300_STAGE_CONTIGUOUS_INIT_COUNT_MASK(VectorType, vreg1, vreg2, vreg3, count, ElementCount);        \
            for (uint16_t i = 0; i < repeatTime; ++i) {                                                             \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                            \
                vlds(vreg0, src0, i* sregLower, NORM);                                                              \
                vlds(vreg1, src1, i* sregLower, NORM);                                                              \
                vadd(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                      \
                vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                            \
                vsts(vreg3, dst, i* sregLower, NORM_B16Arg, preg);                                                  \
            }                                                                                                       \
        }                                                                                                           \
    }

ASC_M300_DEFINE_CONTIGUOUS_2(half, vector_f16, 128, plt_b16, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_2(float, vector_f32, 64, plt_b32, NORM_B32)

#undef ASC_M300_DEFINE_CONTIGUOUS_2

/* **************************************************************************************************
 * FusedMulAdd                                             *
 * ************************************************************************************************* */
// FusedMulAdd::Level 2
template <typename T>
__aicore__ inline void FusedMulAddImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_CONTIGUOUS_3(DataType, VectorType, ElementCount, MaskIntrinsic, NORM_B16Arg)          \
    template <typename T = DataType>                                                                          \
    __aicore__ inline void FusedMulAddImpl(                                                                   \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const int32_t& count)       \
    {                                                                                                         \
        static_assert(                                                                                        \
            SupportType<T, DataType>(), "FusedMulAdd level-2 api only support half/float on current device"); \
        __VEC_SCOPE__                                                                                         \
        {                                                                                                     \
            ASC_M300_STAGE_CONTIGUOUS_INIT_COUNT_MASK(VectorType, vreg0, vreg1, vreg2, count, ElementCount);  \
            for (uint16_t i = 0; i < repeatTime; ++i) {                                                       \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                      \
                vlds(vreg0, src0, i* sregLower, NORM);                                                        \
                vlds(vreg1, src1, i* sregLower, NORM);                                                        \
                vlds(vreg2, dst, i* sregLower, NORM);                                                         \
                vmadd(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                               \
                vsts(vreg2, dst, i* sregLower, NORM_B16Arg, preg);                                            \
            }                                                                                                 \
        }                                                                                                     \
    }

ASC_M300_DEFINE_CONTIGUOUS_3(half, vector_f16, 128, plt_b16, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_3(float, vector_f32, 64, plt_b32, NORM_B32)

#undef ASC_M300_DEFINE_CONTIGUOUS_3

/* **************************************************************************************************
 * FusedMulAddRelu                                             *
 * ************************************************************************************************* */
// FusedMulAddRelu::Level 2
template <typename T>
__aicore__ inline void FusedMulAddReluImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_CONTIGUOUS_4(DataType, VectorType, ElementCount, MaskIntrinsic, NORM_B16Arg)              \
    template <typename T = DataType>                                                                              \
    __aicore__ inline void FusedMulAddReluImpl(                                                                   \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const int32_t& count)           \
    {                                                                                                             \
        static_assert(                                                                                            \
            SupportType<T, DataType>(), "FusedMulAddRelu level-2 api only support half/float on current device"); \
        __VEC_SCOPE__                                                                                             \
        {                                                                                                         \
            VectorType vreg0;                                                                                     \
            VectorType vreg1;                                                                                     \
            VectorType vreg2;                                                                                     \
            VectorType vreg3;                                                                                     \
            uint32_t sreg = static_cast<uint32_t>(count);                                                         \
            vector_bool preg;                                                                                     \
            uint32_t sregLower = static_cast<uint32_t>(ElementCount);                                             \
            uint16_t repeatTime = CeilDivision(count, ElementCount);                                              \
            for (uint16_t i = 0; i < repeatTime; ++i) {                                                           \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                          \
                vlds(vreg0, src0, i* sregLower, NORM);                                                            \
                vlds(vreg1, src1, i* sregLower, NORM);                                                            \
                vlds(vreg2, dst, i* sregLower, NORM);                                                             \
                vmadd(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                   \
                vrelu(vreg3, vreg2, preg, MODE_ZEROING);                                                          \
                vsts(vreg3, dst, i* sregLower, NORM_B16Arg, preg);                                                \
            }                                                                                                     \
        }                                                                                                         \
    }

ASC_M300_DEFINE_CONTIGUOUS_4(half, vector_f16, 128, plt_b16, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_4(float, vector_f32, 64, plt_b32, NORM_B32)

#undef ASC_M300_DEFINE_CONTIGUOUS_4
/* **************************************************************************************************
 * MulAddDst                                             *
 * ************************************************************************************************* */
// MulAddDst::Level 2
template <typename T, typename U>
__aicore__ inline void MulAddDstImpl(__ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

#define ASC_M300_DEFINE_CONTIGUOUS_5(DataType, VectorType, ElementCount, MaskIntrinsic, NORM_B16Arg)               \
    template <typename T = DataType, typename U = DataType>                                                        \
    __aicore__ inline void MulAddDstImpl(                                                                          \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const int32_t& count)            \
    {                                                                                                              \
        static_assert(SupportType<U, DataType>(), "MulAddDst level-2 api only support b16/b32 on current device"); \
        static_assert(SupportType<T, DataType>(), "MulAddDst level-2 api only support b16/b32 on current device"); \
        __VEC_SCOPE__                                                                                              \
        {                                                                                                          \
            ASC_M300_STAGE_CONTIGUOUS_INIT_COUNT_MASK(VectorType, vreg0, vreg1, vreg2, count, ElementCount);       \
            for (uint16_t i = 0; i < repeatTime; ++i) {                                                            \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                           \
                vlds(vreg0, src0, i* sregLower, NORM);                                                             \
                vlds(vreg1, src1, i* sregLower, NORM);                                                             \
                vlds(vreg2, dst, i* sregLower, NORM);                                                              \
                vmula(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                                                    \
                vsts(vreg2, dst, i* sregLower, NORM_B16Arg, preg);                                                 \
            }                                                                                                      \
        }                                                                                                          \
    }

ASC_M300_DEFINE_CONTIGUOUS_5(half, vector_f16, 128, plt_b16, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_5(int16_t, vector_s16, 128, plt_b16, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_5(uint16_t, vector_u16, 128, plt_b16, NORM_B16)
ASC_M300_DEFINE_CONTIGUOUS_5(float, vector_f32, 64, plt_b32, NORM_B32)
ASC_M300_DEFINE_CONTIGUOUS_5(int32_t, vector_s32, 64, plt_b32, NORM_B32)
ASC_M300_DEFINE_CONTIGUOUS_5(uint32_t, vector_u32, 64, plt_b32, NORM_B32)

#undef ASC_M300_DEFINE_CONTIGUOUS_5
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__
#endif

#undef ASC_M300_STAGE_CONTIGUOUS_INIT_COUNT_MASK
