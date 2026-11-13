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
    "impl/basic_api/dav_m310/kernel_operator_vec_binary_continuous_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"

namespace AscendC {
/* **************************************************************************************************
 * Add                                             *
 * ************************************************************************************************* */
// Add::Level 2
template <typename T>
__aicore__ inline void AddImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M310_DEFINE_BINARY_COUNT(                                                                   \
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
            uint32_t sregLower = ElementCount;                                                          \
            uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, ElementCount));             \
            for (uint16_t i = 0; i < repeatTime; ++i) {                                                 \
                preg = MaskIntrinsic(sreg, POST_UPDATE);                                                \
                vlds(vreg0, src0, i* sregLower, NORM);                                                  \
                vlds(vreg1, src1, i* sregLower, NORM);                                                  \
                VectorIntrinsic(vreg2, vreg0, vreg1, preg, MODE_ZEROING);                               \
                vsts(vreg2, dst, i* sregLower, NORM_B16Arg, preg);                                      \
            }                                                                                           \
        }                                                                                               \
    }

ASC_M310_DEFINE_BINARY_COUNT(AddImpl, half, vector_f16, 128, plt_b16, vadd, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(AddImpl, int16_t, vector_s16, 128, plt_b16, vadd, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(AddImpl, int32_t, vector_s32, 64, plt_b32, vadd, NORM_B32)
ASC_M310_DEFINE_BINARY_COUNT(AddImpl, float, vector_f32, 64, plt_b32, vadd, NORM_B32)

/* **************************************************************************************************
 * Sub                                             *
 * ************************************************************************************************* */
// Sub::Level 2
template <typename T>
__aicore__ inline void SubImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_COUNT(SubImpl, half, vector_f16, 128, plt_b16, vsub, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(SubImpl, int16_t, vector_s16, 128, plt_b16, vsub, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(SubImpl, int32_t, vector_s32, 64, plt_b32, vsub, NORM_B32)
ASC_M310_DEFINE_BINARY_COUNT(SubImpl, float, vector_f32, 64, plt_b32, vsub, NORM_B32)

/* **************************************************************************************************
 * Mul                                             *
 * ************************************************************************************************* */
// Mul::Level 2
template <typename T>
__aicore__ inline void MulImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_COUNT(MulImpl, half, vector_f16, 128, plt_b16, vmul, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(MulImpl, int16_t, vector_s16, 128, plt_b16, vmul, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(MulImpl, int32_t, vector_s32, 64, plt_b32, vmul, NORM_B32)
ASC_M310_DEFINE_BINARY_COUNT(MulImpl, float, vector_f32, 64, plt_b32, vmul, NORM_B32)

/* **************************************************************************************************
 * Div                                             *
 * ************************************************************************************************* */
// Div::Level 2
template <typename T>
__aicore__ inline void DivImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_COUNT(DivImpl, half, vector_f16, 128, plt_b16, vdiv, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(DivImpl, float, vector_f32, 64, plt_b32, vdiv, NORM_B32)

/* **************************************************************************************************
 * Max                                             *
 * ************************************************************************************************* */
// Max::Level 2
template <typename T>
__aicore__ inline void MaxImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_COUNT(MaxImpl, half, vector_f16, 128, plt_b16, vmax, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(MaxImpl, int16_t, vector_s16, 128, plt_b16, vmax, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(MaxImpl, int32_t, vector_s32, 64, plt_b32, vmax, NORM_B32)
ASC_M310_DEFINE_BINARY_COUNT(MaxImpl, float, vector_f32, 64, plt_b32, vmax, NORM_B32)

/* **************************************************************************************************
 * Min                                             *
 * ************************************************************************************************* */
// Min::Level 2
template <typename T>
__aicore__ inline void MinImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_COUNT(MinImpl, half, vector_f16, 128, plt_b16, vmin, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(MinImpl, int16_t, vector_s16, 128, plt_b16, vmin, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(MinImpl, int32_t, vector_s32, 64, plt_b32, vmin, NORM_B32)
ASC_M310_DEFINE_BINARY_COUNT(MinImpl, float, vector_f32, 64, plt_b32, vmin, NORM_B32)

/* **************************************************************************************************
 * And                                             *
 * ************************************************************************************************* */
// And::Level 2
template <typename T>
__aicore__ inline void AndImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_COUNT(AndImpl, int16_t, vector_s16, 128, plt_b16, vand, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(AndImpl, uint16_t, vector_u16, 128, plt_b16, vand, NORM_B16)

/* **************************************************************************************************
 * Or                                             *
 * ************************************************************************************************* */
// Or::Level 2
template <typename T>
__aicore__ inline void OrImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported on current device"); });
}

ASC_M310_DEFINE_BINARY_COUNT(OrImpl, int16_t, vector_s16, 128, plt_b16, vor, NORM_B16)
ASC_M310_DEFINE_BINARY_COUNT(OrImpl, uint16_t, vector_u16, 128, plt_b16, vor, NORM_B16)

#undef ASC_M310_DEFINE_BINARY_COUNT
/* **************************************************************************************************
 * FusedMulAdd                                             *
 * ************************************************************************************************* */
// FusedMulAdd::Level 2
template <typename T>
__aicore__ inline void FusedMulAddImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "FusedMulAdd is not supported on current device!"); });
}

/* **************************************************************************************************
 * FusedMulAddRelu                                             *
 * ************************************************************************************************* */
// FusedMulAddRelu::Level 2
template <typename T>
__aicore__ inline void FusedMulAddReluImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "FusedMulAddRelu is not supported on current device!"); });
}
/* **************************************************************************************************
 * MulAddDst                                             *
 * ************************************************************************************************* */
// MulAddDst::Level 2
template <typename T, typename U>
__aicore__ inline void MulAddDstImpl(__ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const int32_t& count)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "MulAddDst is not supported on current device!"); });
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__
#endif
