/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
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
    "impl/basic_api/dav_5161/kernel_operator_fixpipe_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H
#include "kernel_operator_set_spr_impl.h"
#include "../../../include/basic_api/kernel_struct_fixpipe.h"

namespace AscendC {
__aicore__ inline void SetFixPipeClipReluImpl(uint64_t config) { set_m_clip_relu(config); }

template <typename T>
__aicore__ inline void SetFixPipeAddrImpl(const LocalTensor<T>& eleWiseTensor, uint16_t c0ChStride)
{
    (void)(eleWiseTensor);
    (void)(c0ChStride);
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeAddr");
}

template <typename T>
__aicore__ inline void SetFixPipeConfigImpl(
    const LocalTensor<T>& reluPre, const LocalTensor<T>& quantPre, bool isUnitFlag = false)
{
    ASCENDC_REPORT_NOT_SUPPORT(!isUnitFlag, "SetFixPipeConfig isUnitFlag");
    uint64_t config = 0;
    config = config | ((uint64_t)reluPre.GetPhyAddr() >> 6);         // align with 64bit, FPC[7:0], ReluPreAddr
    config = config | (((uint64_t)quantPre.GetPhyAddr() >> 7) << 8); // align with 128bit, FPC[15:8], QuantPreAddr.
    set_m_fpc(config);
}

template <typename T, bool setRelu = false>
__aicore__ inline void SetFixPipeConfigImpl(const LocalTensor<T>& preTensor, bool isUnitFlag = false)
{
    ASCENDC_REPORT_NOT_SUPPORT(!isUnitFlag, "SetFixPipeConfig isUnitFlag");
    uint64_t config = 0;
    if constexpr (setRelu) {
        config = config | ((uint64_t)preTensor.GetPhyAddr() >> 6); // align with 64bit, FPC[7:0], ReluPreAddr.
    } else {
        config = config | (((uint64_t)preTensor.GetPhyAddr() >> 7) << 8); // align with 128bit, FPC[15:8], QuantPreAddr.
    }
    set_m_fpc(config);
}

__aicore__ inline void SetFixpipeNz2ndFlagImpl(uint16_t ndNum, uint16_t srcNdStride, uint32_t dstNdStride)
{
    (void)(ndNum);
    (void)(srcNdStride);
    (void)(dstNdStride);
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixpipeNz2ndFlag");
}

__aicore__ inline void SetFixpipePreQuantFlagImpl(uint64_t config) { set_m_quant_pre(config); }
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__
#endif
