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
 * \file pad.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "pad.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_H__
#endif

#ifndef LIB_PAD_PAD_H
#define LIB_PAD_PAD_H

#include "kernel_basic_intf.h"
#include "kernel_tensor.h"
#include "../../../impl/basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/pad/pad/pad_common_impl.h"
#endif
namespace AscendC {
/* **************************************************************************************************
 * Pad                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Pad
 * @brief pad from src to dst, applicable to vector data
 * @param [out] dstTensor output LocalTensor
 * @param [in] srcTensor input LocalTensor
 * @param [in] sharedTmpBuffer tmp buffer LocalTensor
 * @param [in] PadParams.leftPad number of left pad
 * @param [in] PadParams.rightPad number of right pad
 * @param [in] PadParams.padValue value of pad
 */
#pragma begin_pipe(V)
template <typename T>
__aicore__ inline void Pad(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams,
    const LocalTensor<uint8_t>& sharedTmpBuffer, PadTiling& tiling)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::Pad);
    PadImpl<T>(dstTensor, srcTensor, padParams, sharedTmpBuffer, tiling);
    TRACE_STOP(TraceId::Pad);
#endif
}

/* **************************************************************************************************
 * Pad                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Pad
 * @brief pad from src to dst, applicable to vector data
 * @param [out] dstTensor output LocalTensor
 * @param [in] srcTensor input LocalTensor
 * @param [in] PadParams.leftPad number of left pad
 * @param [in] PadParams.rightPad number of right pad
 * @param [in] PadParams.padValue value of pad
 */
template <typename T>
__aicore__ inline void Pad(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams, PadTiling& tiling)
{
    LocalTensor<uint8_t> tmpBuffer;
    bool res = PopStackBuffer<uint8_t, TPosition::LCM>(tmpBuffer);
    ASCENDC_ASSERT(res, { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    PadImpl<T>(dstTensor, srcTensor, padParams, tmpBuffer, tiling);
#endif
}

/* **************************************************************************************************
 * UnPad                                             *
 * ************************************************************************************************* */
/*
 * @ingroup UnPad
 * @brief unpad from src to dst, applicable to vector data
 * @param [out] dstTensor output LocalTensor
 * @param [in] srcTensor input LocalTensor
 * @param [in] sharedTmpBuffer tmp buffer LocalTensor
 * @param [in] unPadParams.leftPad number of left unpad
 * @param [in] unPadParams.rightPad number of right unpad
 */
template <typename T>
__aicore__ inline void UnPad(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, UnPadParams& unPadParams,
    LocalTensor<uint8_t>& sharedTmpBuffer, UnPadTiling& tiling)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    UnPadImpl<T>(dstTensor, srcTensor, unPadParams, sharedTmpBuffer, tiling);
#endif
}

/* **************************************************************************************************
 * UnPad                                             *
 * ************************************************************************************************* */
/*
 * @ingroup UnPad
 * @brief unpad from src to dst, applicable to vector data
 * @param [out] dstTensor output LocalTensor
 * @param [in] srcTensor input LocalTensor
 * @param [in] unPadParams.leftPad number of left unpad
 * @param [in] unPadParams.rightPad number of right unpad
 */
template <typename T>
__aicore__ inline void UnPad(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, UnPadParams& unPadParams, UnPadTiling& tiling)
{
    LocalTensor<uint8_t> tmpBuffer;
    bool res = PopStackBuffer<uint8_t, TPosition::LCM>(tmpBuffer);
    ASCENDC_ASSERT(res, { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    UnPadImpl<T>(dstTensor, srcTensor, unPadParams, tmpBuffer, tiling);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_PAD_PAD_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_H__
#endif
