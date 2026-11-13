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
 * \file kernel_operator_vec_brcb_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m300/kernel_operator_vec_brcb_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BRCB_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BRCB_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BRCB_IMPL_H
#include "../../../include/basic_api/kernel_struct_brcb.h"

namespace AscendC {
/* **************************************************************************************************
 * Brcb                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void BrcbImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, const uint8_t repeatTime, const BrcbRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
}

// Emit the original overloads with their existing vector scope and intrinsic order.
#define ASC_M300_DEFINE_BRCB_1(                                                                            \
    DataType, VectorType, MaskIntrinsic, PAT_VL16Arg, ONE_BLK_HALF_NUMArg, BRC_B16Arg, NORM_B16Arg)        \
    __aicore__ inline void BrcbImpl(                                                                       \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, const uint8_t repeatTime,                         \
        const BrcbRepeatParams& repeatParams)                                                              \
    {                                                                                                      \
        __VEC_SCOPE__                                                                                      \
        {                                                                                                  \
            int32_t dstRepOffset = 0;                                                                      \
            VectorType vreg0;                                                                              \
            int32_t sreg0 = 0;                                                                             \
            int32_t sreg1 = 0;                                                                             \
            vector_bool preg0;                                                                             \
            preg0 = MaskIntrinsic(PAT_VL16Arg);                                                            \
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {                             \
                sreg0 = ((int32_t)repeatParams.dstRepStride * i * ONE_BLK_HALF_NUMArg);                    \
                sreg1 = (int32_t)BRCB_BROADCAST_NUMBER * i;                                                \
                for (uint16_t j = 0; j < static_cast<uint16_t>(BRCB_BROADCAST_NUMBER); ++j) {              \
                    dstRepOffset = (sreg0 + (int32_t)repeatParams.dstBlkStride * j * ONE_BLK_HALF_NUMArg); \
                    vlds(vreg0, src0, sreg1 + j, BRC_B16Arg);                                              \
                    vsts(vreg0, dst + dstRepOffset, 0, NORM_B16Arg, preg0);                                \
                }                                                                                          \
            }                                                                                              \
        }                                                                                                  \
    }

ASC_M300_DEFINE_BRCB_1(uint16_t, vector_u16, pset_b16, PAT_VL16, ONE_BLK_HALF_NUM, BRC_B16, NORM_B16)
ASC_M300_DEFINE_BRCB_1(uint32_t, vector_u32, pset_b32, PAT_VL8, ONE_BLK_FLOAT_NUM, BRC_B32, NORM_B32)
ASC_M300_DEFINE_BRCB_1(half, vector_f16, pset_b16, PAT_VL16, ONE_BLK_HALF_NUM, BRC_B16, NORM_B16)
ASC_M300_DEFINE_BRCB_1(float, vector_f32, pset_b32, PAT_VL8, ONE_BLK_FLOAT_NUM, BRC_B32, NORM_B32)

#undef ASC_M300_DEFINE_BRCB_1
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BRCB_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BRCB_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BRCB_IMPL_H__
#endif
