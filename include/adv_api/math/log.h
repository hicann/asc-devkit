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
 * \file log.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "log.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOG_H__
#endif

#ifndef LIB_MATH_LOG_H
#define LIB_MATH_LOG_H
#include "kernel_tensor.h"
#include "../../../impl/adv_api/detail/math/log/log_common_impl.h"

namespace AscendC {

#pragma begin_pipe(V)
/*
 * @brief dst = log(src)
 * @ingroup Log
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 * @param [in] calCount, amount of data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Log(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
{
    // Only for AI Vector Core.
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102)
    if ASCEND_IS_AIC {
        return;
    }
#endif
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LogImpl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

/*
 * @brief dst = log(src), calcalate
 * @ingroup Log
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Log(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Log<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

/*
 * @brief dst = log2(src)
 * @ingroup Log
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 * @param [in] sharedTmpBuffer, input local temporary Tensor
 * @param [in] calCount, amount of data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Log2(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    uint32_t calCount)
{
    // Only for AI Vector Core.
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102)
    if ASCEND_IS_AIC {
        return;
    }
#endif
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Log2Impl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
#endif
}

/*
 * @brief dst = log2(src)
 * @ingroup Log
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 * @param [in] sharedTmpBuffer, input local temporary Tensor
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Log2(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    Log2<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, srcTensor.GetSize());
}

/*
 * @brief dst = log2(src)
 * @ingroup Log
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 * @param [in] calCount, amount of data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
{
    LocalTensor<uint8_t> stackTensor;
    // Only half requires tmpbuf.
    if constexpr (std::is_same<T, half>::value) {
        bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
        ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    }

    Log2<T, isReuseSource>(dstTensor, srcTensor, stackTensor, calCount);
}

/*
 * @brief dst = log2(src)
 * @ingroup Log
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Log2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Log2<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

/*
 * @brief dst = log10(src)
 * @ingroup Log
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 * @param [in] calCount, amount of data to be calculated
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Log10(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
{
    // Only for AI Vector Core.
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102)
    if ASCEND_IS_AIC {
        return;
    }
#endif
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    Log10Impl<T, isReuseSource>(dstTensor, srcTensor, calCount);
#endif
}

/*
 * @brief dst = log10(src)
 * @ingroup Log
 * @param [out] dstTensor, output LocalTensor
 * @param [in] srcTensor, input LocalTensor
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void Log10(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    Log10<T, isReuseSource>(dstTensor, srcTensor, srcTensor.GetSize());
}

#pragma end_pipe
} // namespace AscendC

#endif // LIB_MATH_LOG_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOG_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOG_H__
#endif
