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
 * \file pad_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/pad/pad/pad_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/pad.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_PAD_PAD_COMMON_IMPL_H__
#endif

#ifndef IMPL_PAD_PAD_PAD_COMMON_IMPL_H
#define IMPL_PAD_PAD_PAD_COMMON_IMPL_H

#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/pad/pad/pad_check.h"
#include "../../api_check/kernel_check/pad/pad/unpad_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
#include "pad_v200_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "pad_v220_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "pad_3510_impl.h"
#endif

namespace AscendC {
template <typename T>
__aicore__ inline void PadImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams,
    const LocalTensor<uint8_t>& sharedTmpBuffer, PadTiling& tiling)
{
    CHECK_FUNC_HIGHLEVEL_API(Pad, (T), (dstTensor, srcTensor, padParams, sharedTmpBuffer, tiling));
    PadCompute<T>(dstTensor, srcTensor, padParams, sharedTmpBuffer, tiling);
}

template <typename T>
__aicore__ inline void UnPadImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, UnPadParams& unPadParams,
    LocalTensor<uint8_t>& sharedTmpBuffer, UnPadTiling& tiling)
{
    CHECK_FUNC_HIGHLEVEL_API(UnPad, (T), (dstTensor, srcTensor, unPadParams, sharedTmpBuffer, tiling));

    UnPadCompute<T>(dstTensor, srcTensor, unPadParams, sharedTmpBuffer, tiling);
}
} // namespace AscendC
#endif // IMPL_PAD_PAD_PAD_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_PAD_PAD_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_PAD_PAD_COMMON_IMPL_H__
#endif
