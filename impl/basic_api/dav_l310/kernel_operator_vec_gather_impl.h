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
 * \file kernel_operator_vec_gather_impl.h
 * \brief AscendC l310 support vector gather api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l310/kernel_operator_vec_gather_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_GATHER_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_GATHER_IMPL_H
#include "kernel_operator_common_impl.h"
namespace AscendC {

/* **************************************************************************************************
 * Gather                                                                                           *
 * **************************************************************************************************/
// gatherb::Level 0
template <typename T>
__aicore__ inline void GatherImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ uint32_t* offset, const uint32_t srcLength, uint8_t repeatTimes,
    const GatherRepeatParams& repeatParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "GatherImpl is not supported on current device!"); });
}

// Gather::Level 0 Normal mode
template <typename T>
__aicore__ inline void GatherImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcLength,
    const uint32_t srcBaseAddr, const uint64_t mask, const uint8_t repeatTime, const uint16_t& dstRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "GatherImpl is not supported on current device!"); });
}

/* **************************************************************************************************
 * Gather                                             *
 * ************************************************************************************************* */
// Gather::Level 0 Bit-wise mode
template <typename T>
__aicore__ inline void GatherImpl(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ uint32_t* srcOffsetLocal, const uint32_t srcLength,
    const uint32_t srcBaseAddr, const uint64_t mask[], const uint8_t repeatTime, const uint16_t& dstRepStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "GatherImpl is not supported on current device!"); });
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_GATHER_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_GATHER_IMPL_H__
#endif
