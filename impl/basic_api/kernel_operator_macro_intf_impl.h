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
 * \file kernel_operator_macro_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_macro_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_macro_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_INTF_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MACRO_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MACRO_INTERFACE_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_check.h"
#include "kernel_reg.h"
#include "kernel_npu_debug.h"
#include "kernel_operator_macro_base_impl.h"
#include "../../include/basic_api/kernel_struct_macro.h"

namespace AscendC {
__aicore__ inline void SetConvKernel(const ConvKernelBitModeParams& params) { SetConvKernelImpl(params); }

__aicore__ inline void SetConvKernel(const ConvKernelParams& params) { SetConvKernelImpl(params); }

__aicore__ inline void SetConvFMatrix(const ConvFMatrixBitModeParams& params) { SetConvFMatrixImpl(params); }

__aicore__ inline void SetConvFMatrix(const ConvFMatrixParams& params) { SetConvFMatrixImpl(params); }

__aicore__ inline void SetConvFMatrixStepPos(const ConvFMatrixStepPosBitModeParams& params)
{
    SetConvFMatrixStepPosImpl(params);
}

__aicore__ inline void SetConvFMatrixStepPos(const ConvFMatrixStepPosParams& params)
{
    SetConvFMatrixStepPosImpl(params);
}

__aicore__ inline void SetConvFMatrixDual0(const ConvFMatrixDual0BitModeParams& params)
{
    SetConvFMatrixDual0Impl(params);
}

__aicore__ inline void SetConvFMatrixDual0(const ConvFMatrixDual0Params& params) { SetConvFMatrixDual0Impl(params); }

__aicore__ inline void SetConvFMatrixDual1(const ConvFMatrixDual1BitModeParams& params)
{
    SetConvFMatrixDual1Impl(params);
}

__aicore__ inline void SetConvFMatrixDual1(const ConvFMatrixDual1Params& params) { SetConvFMatrixDual1Impl(params); }

template <typename T>
__aicore__ inline void SetConvPadding(const ConvPaddingBitModeParams<T>& params)
{
    SetConvPaddingImpl(params);
}

template <typename T>
__aicore__ inline void SetConvPadding(const ConvPaddingParams<T>& params)
{
    SetConvPaddingImpl(params);
}

__aicore__ inline void SetFixPipeConfig(const FixPipeConfigBitModeParams& params) { SetFixPipeConfigImpl(params); }

__aicore__ inline void SetFixPipeConfig(const FixPipeConfigParams& params) { SetFixPipeConfigImpl(params); }

__aicore__ inline void SetFixPipeReluAlpha(const FixPipeReluAlphaBitModeParams& params)
{
    SetFixPipeReluAlphaImpl(params);
}

__aicore__ inline void SetFixPipeReluAlpha(const FixPipeReluAlphaParams& params) { SetFixPipeReluAlphaImpl(params); }

__aicore__ inline void SetFixpipePostQuantFlag(uint64_t config) { SetFixpipePostQuantFlagImpl(config); }

__aicore__ inline void SetFixpipeEltAntiq(uint64_t config) { SetFixpipeEltAntiqImpl(config); }

__aicore__ inline void SetFixPipeAddr(const FixPipeAddrBitModeParams& params) { SetFixPipeAddrImpl(params); }

__aicore__ inline void SetFixPipeAddr(const FixPipeAddrParams& params) { SetFixPipeAddrImpl(params); }

__aicore__ inline void SetConvStridePara(const ConvStrideBitModeParams& params) { SetConvStrideParaImpl(params); }

__aicore__ inline void SetConvStridePara(const ConvStrideParams& params) { SetConvStrideParaImpl(params); }

__aicore__ inline void SetMatrixPara(const MatrixParaBitModeParams& params) { SetMatrixParaImpl(params); }

__aicore__ inline void SetMatrixPara(const MatrixParaParams& params) { SetMatrixParaImpl(params); }

__aicore__ inline void SetMmadStridePara(const MmadStrideParaBitModeParams& params) { SetMmadStrideParaImpl(params); }

__aicore__ inline void SetMmadStridePara(const MmadStrideParaParams& params) { SetMmadStrideParaImpl(params); }

__aicore__ inline void SetItaMaxAddr(const ItaMaxAddrBitModeParams& params) { SetItaMaxAddrImpl(params); }

__aicore__ inline void SetItaMaxAddr(const ItaMaxAddrParaParams& params) { SetItaMaxAddrImpl(params); }

__aicore__ inline void SetMatrixMxPara(const MatrixMxParaBitModeParams& params) { SetMatrixMxParaImpl(params); }

__aicore__ inline void SetMatrixMxPara(const MatrixMxParaParams& params) { SetMatrixMxParaImpl(params); }

template <typename T, typename U, typename S>
__aicore__ inline void ConvFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter,
    const ConvFixBitModeParams& convFixParams)
{
    ConvFixImpl(dst, fm, filter, convFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void ConvFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const ConvFixBitModeParams& convFixParams)
{
    ConvFixImpl(dst, fm, filter, bias, convFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void ConvFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter,
    const ConvFixParams& convFixParams)
{
    ConvFixImpl(dst, fm, filter, convFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void ConvFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const ConvFixParams& convFixParams)
{
    ConvFixImpl(dst, fm, filter, bias, convFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixBitModeParams& mmadFixParams)
{
    MmadFixImpl(dst, left, right, mmadFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixBitModeParams& mmadFixParams)
{
    MmadFixImpl(dst, left, right, bias, mmadFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixParams& mmadFixParams)
{
    MmadFixImpl(dst, left, right, mmadFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixParams& mmadFixParams)
{
    MmadFixImpl(dst, left, right, bias, mmadFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadMxFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixBitModeParams& mmadFixParams)
{
    MmadMxFixImpl(dst, left, right, mmadFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadMxFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixBitModeParams& mmadFixParams)
{
    MmadMxFixImpl(dst, left, right, bias, mmadFixParams);
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadMxFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixParams& mmadFixParams)
{
    MmadMxFixImpl(dst, left, right, mmadFixParams);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadMxFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixParams& mmadFixParams)
{
    MmadMxFixImpl(dst, left, right, bias, mmadFixParams);
}
} // namespace AscendC

#endif // __NPU_ARCH__
#endif // ASCENDC_MODULE_OPERATOR_MACRO_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_INTF_IMPL_H__
#endif
