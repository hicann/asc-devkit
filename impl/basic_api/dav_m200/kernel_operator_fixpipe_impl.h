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
 * \file kernel_operator_fixpipe_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m200/kernel_operator_fixpipe_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H
#include "../../../include/basic_api/kernel_struct_fixpipe.h"

namespace AscendC {

template <typename T>
__aicore__ inline void SetFixPipeConfigImpl(
    const LocalTensor<T>& reluPre, const LocalTensor<T>& quantPre, bool isUnitFlag = false)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeConfig");
}

template <typename T, bool setRelu = false>
__aicore__ inline void SetFixPipeConfigImpl(const LocalTensor<T>& pre, bool isUnitFlag = false)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeConfig");
}

__aicore__ inline void SetFixpipeNz2ndFlagImpl(uint16_t ndNum, uint16_t srcNdStride, uint16_t dstNdStride)
{
    (void)(ndNum);
    (void)(srcNdStride);
    (void)(dstNdStride);
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixpipeNz2ndFlag");
}

__aicore__ inline void SetFixpipePreQuantFlagImpl(uint64_t config)
{
    (void)(config);
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixpipePreQuantFlag");
}

__aicore__ inline void SetFixPipeClipReluImpl(uint64_t config)
{
    (void)(config);
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeClipRelu");
}

template <typename T>
__aicore__ inline void SetFixPipeAddrImpl(const LocalTensor<T>& eleWise, uint16_t c0ChStride)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeAddr");
}
/* **************************************************************************************************
 * Fixpipe                                             *
 * ************************************************************************************************* */
template <typename T, typename U>
[[deprecated("NOTICE: Fixpipe is not deprecated. Currently, Fixpipe is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
FixpipeL0C2GMImpl(__gm__ T* dst, __cc__ U* src, const FixpipeParams<U>& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "Fixpipe from L0C Buffer(CO1) to GM");
}

template <typename T, typename U>
[[deprecated("NOTICE: Fixpipe is not deprecated. Currently, Fixpipe is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
FixpipeL0C2L1Impl(__cbuf__ T* dst, __cc__ U* src, const FixpipeParams<U>& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "Fixpipe from L0C Buffer(CO1) to UB(C1)");
}

// L0C->L1
template <typename T, typename U, typename S>
[[deprecated("NOTICE: Fixpipe is not deprecated. Currently, Fixpipe is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParams<S>& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "Fixpipe from L0C Buffer(CO1) to UB(C1)");
}
// L0C->L1 deq tensor quant
template <typename T, typename U, typename S, typename V>
[[deprecated("NOTICE: Fixpipe is not deprecated. Currently, Fixpipe is an unsupported API on current device."
             "Please check your code!")]] __aicore__ inline void
Fixpipe(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace,
    const FixpipeParams<V>& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "Fixpipe from L0C Buffer(CO1) to UB(C1)");
}

// L0C->GM
template <typename T, typename U, typename S>
[[deprecated("NOTICE: Fixpipe is not deprecated. Currently, Fixpipe is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParams<S>& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "Fixpipe from L0C Buffer(CO1) to GM");
}

// L0C->GM deq tensor quant
template <typename T, typename U, typename S, typename V>
[[deprecated("NOTICE: Fixpipe is not deprecated. Currently, Fixpipe is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
Fixpipe(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace,
    const FixpipeParams<V>& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "Fixpipe from L0C Buffer(CO1) to GM");
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(__ubuf__ T* dst, __cc__ U* src, const FixpipeParamsV220& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        false, KERNEL_LOG_INTERNAL(KERNEL_ERROR, "Fixpipe doesn't support L0C to UB on current device\n"));
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(
    __ubuf__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        false, KERNEL_LOG_INTERNAL(KERNEL_ERROR, "Fixpipe doesn't support L0C to UB on current device\n"));
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__
#endif
