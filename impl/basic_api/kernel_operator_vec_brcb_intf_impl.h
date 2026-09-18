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
 * \file kernel_operator_vec_brcb_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_vec_brcb_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_vec_brcb_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BRCB_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BRCB_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BRCB_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "../../include/basic_api/kernel_struct_brcb.h"
#include "mstx_local_tensor_info.h"
#include "kernel_npu_debug.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_vec_brcb_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_vec_brcb_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_vec_brcb_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_vec_brcb_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_vec_brcb_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_vec_brcb_impl.h"
#elif (__NPU_ARCH__ == 5162)
#include "dav_v516/kernel_operator_vec_brcb_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_vec_brcb_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_vec_brcb_impl.h"
#endif
#pragma begin_pipe(V)
namespace AscendC {
/*
 * @ingroup brcb Level 0
 * @brief this function fetches 8 b16/b32 data from src0, broadcast each data into one 32B block,
 * @brief then finally writes these 8 blocks into dst continuously.
 * @brief gather element in the uint of block
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 */
template <typename T>
__aicore__ inline void Brcb(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const uint8_t repeatTime,
    const BrcbRepeatParams& repeatParams)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckVectorTensor("Brcb", NamedTensor(dst, "dst"), NamedTensor(src0, "src0"));
    ASCENDC_DEBUG_ASSERT(
        (src0.GetSize() >= 8 * repeatTime), KERNEL_LOG_INTERNAL(
                                                KERNEL_ERROR,
                                                "Failed to check src0 "
                                                "tensor size in Brcb, it must be src0.GetSize() >= 8 * repeatTime, "
                                                "current src0.GetSize() is %u, repeatTime is "
                                                "%u.\n",
                                                src0.GetSize(), repeatTime));
    ASCENDC_DEBUG_ASSERT(
        (src0.GetPhyAddr() != dst.GetPhyAddr()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Brcb does not "
                          "support src0 tensor and dst tensor having same addr.\n"));
    ASCENDC_DEBUG_WARNING(
        (repeatParams.dstBlkStride != 0), KERNEL_LOG_INTERNAL(
                                              KERNEL_WARN, "dstBlkStride = 0 in Brcb "
                                                           "is equivalent to dstBlkStride = 1.\n"));
#endif
#ifdef __MSTX_DFX_REPORT__
    MstxTensor::GetMstxVecBrcbInfo(dst, src0, repeatTime, repeatParams, "Brcb");
#endif
    using PrimType = PrimT<T>;
#if ASCENDC_CPU_DEBUG
    if (!CheckFunBcB(dst, src0, repeatTime, repeatParams, "Brcb")) {
        ASCENDC_REPORT_CHECK_ERROR("Brcb", KernelFuncType::NONE_MODE);
    }
#endif
    BrcbImpl((__ubuf__ PrimType*)dst.GetPhyAddr(), (__ubuf__ PrimType*)src0.GetPhyAddr(), repeatTime, repeatParams);
}
} // namespace AscendC
#pragma end_pipe
#endif // ASCENDC_MODULE_OPERATOR_VEC_BRCB_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BRCB_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BRCB_INTF_IMPL_H__
#endif
