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
 * \file logical_ors.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "logical_ors.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGICAL_ORS_H__
#endif

#ifndef LIB_MATH_LOGICAL_ORS_H
#define LIB_MATH_LOGICAL_ORS_H

#include "kernel_tensor.h"
#include "include/adv_api/math/logical_ors_utils.h"

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/logical_ors/logical_ors_common_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \ingroup LogicalOrs
 * \param [out] dst, output LocalTensor
 * \param [in] src0, input LocalTensor
 * \param [in] src1, input LocalTensor
 * \param [in] count, amount of data to be calculated
 */
template <const LogicalOrsConfig& config = DEFAULT_LOGICAL_ORS_CONFIG, typename T, typename U, typename S>
__aicore__ inline void LogicalOrs(const LocalTensor<T>& dst, const U& src0, const S& src1, const uint32_t count)
{
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LogicalOrsImpl<config, T, U, S>(dst, src0, src1, count);
#endif
}

#pragma end_pipe
} // namespace AscendC
#endif // LIB_MATH_LOGICAL_ORS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGICAL_ORS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LOGICAL_ORS_H__
#endif
