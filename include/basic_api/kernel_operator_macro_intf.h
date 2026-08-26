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
 * \file kernel_operator_macro_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_macro_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MACRO_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_MACRO_INTERFACE_

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/common_types.h"
#include "../../impl/basic_api/kernel_operator_macro_base_impl.h"
#include "kernel_struct_macro.h"
#include "kernel_tensor.h"
#include "../../impl/basic_api/utils/kernel_utils_constants.h"
#include "../../impl/basic_api/utils/kernel_utils_macros.h"
#include "kernel_operator_macro_bitmode_intf.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

namespace AscendC {
/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetConvKernel(const ConvKernelBitModeParams& params);
__aicore__ inline void SetConvKernel(const ConvKernelParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetConvFMatrix(const ConvFMatrixBitModeParams& params);
__aicore__ inline void SetConvFMatrix(const ConvFMatrixParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetConvFMatrixStepPos(const ConvFMatrixStepPosBitModeParams& params);
__aicore__ inline void SetConvFMatrixStepPos(const ConvFMatrixStepPosParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetConvFMatrixDual0(const ConvFMatrixDual0BitModeParams& params);
__aicore__ inline void SetConvFMatrixDual0(const ConvFMatrixDual0Params& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetConvFMatrixDual1(const ConvFMatrixDual1BitModeParams& params);
__aicore__ inline void SetConvFMatrixDual1(const ConvFMatrixDual1Params& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
template <typename T>
__aicore__ inline void SetConvPadding(const ConvPaddingBitModeParams<T>& params);
template <typename T>
__aicore__ inline void SetConvPadding(const ConvPaddingParams<T>& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetFixPipeConfig(const FixPipeConfigBitModeParams& params);
__aicore__ inline void SetFixPipeConfig(const FixPipeConfigParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetFixPipeReluAlpha(const FixPipeReluAlphaBitModeParams& params);
__aicore__ inline void SetFixPipeReluAlpha(const FixPipeReluAlphaParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetFixpipePostQuantFlag(uint64_t config);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetFixpipeEltAntiq(uint64_t config);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetFixPipeAddr(const FixPipeAddrBitModeParams& params);
__aicore__ inline void SetFixPipeAddr(const FixPipeAddrParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetConvStridePara(const ConvStrideBitModeParams& params);
__aicore__ inline void SetConvStridePara(const ConvStrideParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetMatrixPara(const MatrixParaBitModeParams& params);
__aicore__ inline void SetMatrixPara(const MatrixParaParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetMmadStridePara(const MmadStrideParaBitModeParams& params);
__aicore__ inline void SetMmadStridePara(const MmadStrideParaParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetItaMaxAddr(const ItaMaxAddrBitModeParams& params);
__aicore__ inline void SetItaMaxAddr(const ItaMaxAddrParaParams& params);

/*
 * @ingroup MacroCube
 * @brief setting the register
 * @param [in] params register
 */
__aicore__ inline void SetMatrixMxPara(const MatrixMxParaBitModeParams& params);
__aicore__ inline void SetMatrixMxPara(const MatrixMxParaParams& params);

/*
 * @ingroup MacroCube
 * @brief performs normal convolution
 * @param [out] dst
 * @param [in] fm
 * @param [in] filter
 * @param [in] bias
 * @param [in] convFixParams
 */
template <typename T, typename U, typename S>
__aicore__ inline void ConvFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter,
    const ConvFixBitModeParams& convFixParams);

template <typename T, typename U, typename S, typename V>
__aicore__ inline void ConvFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const ConvFixBitModeParams& convFixParams);

template <typename T, typename U, typename S>
__aicore__ inline void ConvFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter,
    const ConvFixParams& convFixParams);

template <typename T, typename U, typename S, typename V>
__aicore__ inline void ConvFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const ConvFixParams& convFixParams);

/*
 * @ingroup MacroCube
 * @brief performs normal matmul
 * @param [out] dst
 * @param [in] left
 * @param [in] right
 * @param [in] bias
 * @param [in] mmadFixParams
 */
template <typename T, typename U, typename S>
__aicore__ inline void MmadFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixBitModeParams& mmadFixParams);

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixBitModeParams& mmadFixParams);

template <typename T, typename U, typename S>
__aicore__ inline void MmadFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixParams& mmadFixParams);

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixParams& mmadFixParams);

/*
 * @ingroup MacroCube
 * @brief performs normal matmul
 * @param [out] dst
 * @param [in] left
 * @param [in] right
 * @param [in] bias
 * @param [in] mmadFixParams
 */
template <typename T, typename U, typename S>
__aicore__ inline void MmadMxFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixBitModeParams& mmadFixParams);

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadMxFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixBitModeParams& mmadFixParams);

template <typename T, typename U, typename S>
__aicore__ inline void MmadMxFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right,
    const MmadFixParams& mmadFixParams);

template <typename T, typename U, typename S, typename V>
__aicore__ inline void MmadMxFix(
    const LocalTensor<T>& dst, const LocalTensor<U>& left, const LocalTensor<S>& right, const LocalTensor<V>& bias,
    const MmadFixParams& mmadFixParams);
} // namespace AscendC

#if defined(__NPU_ARCH__)
#include "../../impl/basic_api/kernel_operator_macro_intf_impl.h"
#endif

#endif // __NPU_ARCH__
#endif // ASCENDC_MODULE_OPERATOR_MACRO_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_INTF_H__
#endif
