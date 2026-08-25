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
 * \file philox.h
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "philox.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PHILOX_H__
#endif

#ifndef LIB_MATH_PHILOX_H
#define LIB_MATH_PHILOX_H

#include "kernel_tensor.h"
#include "include/adv_api/math/philox_utils.h"

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/math/philox/philox_3510_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)

template <uint16_t Rounds = 7, typename T>
__aicore__ inline void PhiloxRandom(
    const LocalTensor<T>& dstLocal, const PhiloxKey& philoxKey, const PhiloxCounter& philoxCounter, uint16_t count)
{
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    PhiloxRandomImpl<Rounds>(dstLocal, philoxKey, philoxCounter, count);
#endif
}

template <uint16_t Rounds = 7, typename T>
__aicore__ inline void PhiloxRandom(
    const LocalTensor<T>& dstLocal, const PhiloxKey& philoxKey, const PhiloxCounter& philoxCounter,
    const PhiloxRandomParams& params)
{
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    PhiloxRandomImpl<Rounds>(dstLocal, philoxKey, philoxCounter, params);
#endif
}

#pragma end_pipe
} // namespace AscendC

#endif // LIB_MATH_PHILOX_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PHILOX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PHILOX_H__
#endif
