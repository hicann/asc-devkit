/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_operator_vec_scatter_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l310/kernel_operator_vec_scatter_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_SCATTER_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_SCATTER_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_SCATTER_IMPL_H
#include "kernel_operator_common_impl.h"

namespace AscendC {
/* **************************************************************************************************
 * scatter                                             *
 * ************************************************************************************************* */
constexpr uint32_t mulsScalar = 2;
constexpr uint32_t addsScalar = 1;
constexpr int16_t b32ShiftVal = 2;
constexpr int16_t b64ShiftVal = 3;
constexpr int16_t b16ShiftVal = 1;
constexpr uint32_t b32BlkElems = 8;
constexpr uint32_t b16BlkElems = 16;
constexpr uint32_t b8BlkElems = 32;
constexpr uint32_t indexRepElems = 64;
constexpr uint32_t srcRepElems = 64;
constexpr uint32_t srcRep128 = 128;
constexpr uint32_t b64RepElems = 32;
constexpr Reg::CastTrait castTraitEven = {Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING};

constexpr Reg::CastTrait castTraitOdd = {Reg::RegLayout::ONE, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING};

template <typename T>
__aicore__ inline void ScatterImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ uint32_t* dstOffsetLocal, const uint32_t dstLength,
    const uint32_t dstBaseAddr, const uint64_t mask, const uint8_t repeatTime, const uint8_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ScatterImpl is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void ScatterImpl(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ uint32_t* dstOffsetLocal, const uint32_t dstLength,
    const uint32_t dstBaseAddr, const uint64_t mask[], const uint8_t repeatTime, const uint8_t srcRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "ScatterImpl is not supported on current device!"); });
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_SCATTER_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_SCATTER_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_SCATTER_IMPL_H__
#endif
