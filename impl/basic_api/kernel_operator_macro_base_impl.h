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
 * \file kernel_operator_macro_base_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_macro_base_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_macro_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BASE_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MACRO_BASE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MACRO_BASE_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_npu_debug.h"

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "dav_5161/kernel_operator_macro_impl.h"
#endif
#include "kernel_operator_macro_check.h"
#include "../../include/basic_api/kernel_struct_macro.h"

namespace AscendC {
__aicore__ inline void SetConvKernelImpl(const ConvKernelBitModeParams& params) { SetConvKernelCal(params); }

__aicore__ inline void SetConvKernelImpl(const ConvKernelParams& params) { SetConvKernelCal(params); }

__aicore__ inline void SetConvFMatrixImpl(const ConvFMatrixBitModeParams& params) { SetConvFMatrixCal(params); }

__aicore__ inline void SetConvFMatrixImpl(const ConvFMatrixParams& params) { SetConvFMatrixCal(params); }

__aicore__ inline void SetConvFMatrixStepPosImpl(const ConvFMatrixStepPosBitModeParams& params)
{
    SetConvFMatrixStepPosCal(params);
}

__aicore__ inline void SetConvFMatrixStepPosImpl(const ConvFMatrixStepPosParams& params)
{
    SetConvFMatrixStepPosCal(params);
}

__aicore__ inline void SetConvFMatrixDual0Impl(const ConvFMatrixDual0BitModeParams& params)
{
    SetConvFMatrixDual0Cal(params);
}

__aicore__ inline void SetConvFMatrixDual0Impl(const ConvFMatrixDual0Params& params) { SetConvFMatrixDual0Cal(params); }

__aicore__ inline void SetConvFMatrixDual1Impl(const ConvFMatrixDual1BitModeParams& params)
{
    SetConvFMatrixDual1Cal(params);
}

__aicore__ inline void SetConvFMatrixDual1Impl(const ConvFMatrixDual1Params& params) { SetConvFMatrixDual1Cal(params); }

template <typename T>
__aicore__ inline void SetConvPaddingImpl(const ConvPaddingBitModeParams<T>& params)
{
    SetConvPaddingCal(params);
}

template <typename T>
__aicore__ inline void SetConvPaddingImpl(const ConvPaddingParams<T>& params)
{
    SetConvPaddingCal(params);
}

__aicore__ inline void SetFixPipeConfigImpl(const FixPipeConfigBitModeParams& params) { SetFixPipeConfigCal(params); }

__aicore__ inline void SetFixPipeConfigImpl(const FixPipeConfigParams& params) { SetFixPipeConfigCal(params); }

__aicore__ inline void SetFixPipeReluAlphaImpl(const FixPipeReluAlphaBitModeParams& params)
{
    SetFixPipeReluAlphaCal(params);
}

__aicore__ inline void SetFixPipeReluAlphaImpl(const FixPipeReluAlphaParams& params) { SetFixPipeReluAlphaCal(params); }

__aicore__ inline void SetFixpipePostQuantFlagImpl(uint64_t config) { SetFixpipePostQuantFlagCal(config); }

__aicore__ inline void SetFixpipeEltAntiqImpl(uint64_t config) { SetFixpipeEltAntiqCal(config); }

__aicore__ inline void SetFixPipeAddrImpl(const FixPipeAddrBitModeParams& params) { SetFixPipeAddrCal(params); }

__aicore__ inline void SetFixPipeAddrImpl(const FixPipeAddrParams& params) { SetFixPipeAddrCal(params); }

__aicore__ inline void SetConvStrideParaImpl(const ConvStrideBitModeParams& params) { SetConvStrideParaCal(params); }

__aicore__ inline void SetConvStrideParaImpl(const ConvStrideParams& params) { SetConvStrideParaCal(params); }

__aicore__ inline void SetMatrixParaImpl(const MatrixParaBitModeParams& params) { SetMatrixParaCal(params); }

__aicore__ inline void SetMatrixParaImpl(const MatrixParaParams& params) { SetMatrixParaCal(params); }

__aicore__ inline void SetMmadStrideParaImpl(const MmadStrideParaBitModeParams& params)
{
    SetMmadStrideParaCal(params);
}

__aicore__ inline void SetMmadStrideParaImpl(const MmadStrideParaParams& params) { SetMmadStrideParaCal(params); }

__aicore__ inline void SetItaMaxAddrImpl(const ItaMaxAddrBitModeParams& params) { SetItaMaxAddrCal(params); }

__aicore__ inline void SetItaMaxAddrImpl(const ItaMaxAddrParaParams& params) { SetItaMaxAddrCal(params); }

__aicore__ inline void SetMatrixMxParaImpl(const MatrixMxParaBitModeParams& params) { SetMatrixMxParaCal(params); }

__aicore__ inline void SetMatrixMxParaImpl(const MatrixMxParaParams& params) { SetMatrixMxParaCal(params); }

template <typename T, typename U, typename S>
__aicore__ inline void ConvFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter,
    const ConvFixBitModeParams& convFixParams)
{
    ConvFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)fm.GetPhyAddr(),
        (__cb__ PrimT<S>*)filter.GetPhyAddr(), convFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void ConvFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter,
    const ConvFixParams& convFixParams)
{
    ConvFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)fm.GetPhyAddr(),
        (__cb__ PrimT<S>*)filter.GetPhyAddr(), convFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void ConvFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const ConvFixBitModeParams& convFixParams)
{
    ConvFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)fm.GetPhyAddr(),
        (__cb__ PrimT<S>*)filter.GetPhyAddr(), (uint64_t)bias.GetPhyAddr(), convFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void ConvFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const ConvFixParams& convFixParams)
{
    ConvFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)fm.GetPhyAddr(),
        (__cb__ PrimT<S>*)filter.GetPhyAddr(), (uint64_t)bias.GetPhyAddr(), convFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixBitModeParams& mmadFixParams)
{
    MmadFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)left.GetPhyAddr(),
        (__cb__ PrimT<S>*)right.GetPhyAddr(), mmadFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixParams& mmadFixParams)
{
    MmadFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)left.GetPhyAddr(),
        (__cb__ PrimT<S>*)right.GetPhyAddr(), mmadFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixBitModeParams& mmadFixParams)
{
    MmadFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)left.GetPhyAddr(),
        (__cb__ PrimT<S>*)right.GetPhyAddr(), (uint64_t)bias.GetPhyAddr(), mmadFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixParams& mmadFixParams)
{
    MmadFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)left.GetPhyAddr(),
        (__cb__ PrimT<S>*)right.GetPhyAddr(), (uint64_t)bias.GetPhyAddr(), mmadFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadMxFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixBitModeParams& mmadFixParams)
{
    MmadMxFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)left.GetPhyAddr(),
        (__cb__ PrimT<S>*)right.GetPhyAddr(), mmadFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadMxFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixParams& mmadFixParams)
{
    MmadMxFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)left.GetPhyAddr(),
        (__cb__ PrimT<S>*)right.GetPhyAddr(), mmadFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadMxFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixBitModeParams& mmadFixParams)
{
    MmadMxFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)left.GetPhyAddr(),
        (__cb__ PrimT<S>*)right.GetPhyAddr(), (uint64_t)bias.GetPhyAddr(), mmadFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadMxFixImpl(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixParams& mmadFixParams)
{
    MmadMxFixCal(
        (__cbuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<U>*)left.GetPhyAddr(),
        (__cb__ PrimT<S>*)right.GetPhyAddr(), (uint64_t)bias.GetPhyAddr(), mmadFixParams);
}
} // namespace AscendC

#endif // __NPU_ARCH__
#endif // ASCENDC_MODULE_OPERATOR_MACRO_BASE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BASE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BASE_IMPL_H__
#endif
