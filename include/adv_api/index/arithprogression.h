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
 * \file arithprogression.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "arithprogression.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_H__
#endif

#ifndef LIB_ARITHPROGRESSION_H
#define LIB_ARITHPROGRESSION_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/index/arithprogression/arithprogression_3510_impl.h"
#else
#include "../../../impl/adv_api/detail/index/arithprogression/arithprogression_common_impl.h"
#endif

namespace AscendC {
/* !
 * \brief This function realizes the arithmetic sequence function. The formula is as follows:
 * dst[i+1] = dst[i] + diffValue
 *
 * \note support data type: half, float, int16_t, int32_t
 * \note Ascend950 support data type: half, float, int16_t, int32_t, int64_t
 *
 * \param [out] dst, output LocalTensor
 * \param [in] firstValue, first value of arithmetic sequence
 * \param [in] diffValue, diff value of arithmetic sequence
 * \param [in] count, length of the sequence
 */
template <typename T>
__aicore__ inline __in_pipe__(S) __out_pipe__(V, S) void Arange(
    const LocalTensor<T>& dst, const T firstValue, const T diffValue, const int32_t count)
{
    ArithProgressionImpl(dst, firstValue, diffValue, count);
}
} // namespace AscendC

#endif // LIB_ARITHPROGRESSION_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ARITHPROGRESSION_H__
#endif
