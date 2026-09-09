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
 * \file quant_processor_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_out/quant/quant_processor_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_UTILS_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_UTILS_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_UTILS_H

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename CType, typename AType>
__aicore__ inline constexpr static bool IsQuantSenario()
{
    using L0cT = typename GetMmDstType<AType>::Type;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
    if constexpr (
        IsTypeOneOfV<CType, half, bfloat16_t> && !IsTypeOneOfV<AType, int8_t, hifloat8_t, fp8_e4m3fn_t, fp8_e5m2_t>) {
        return false;
    }
    if constexpr (
        IsTypeOneOfV<AType, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t> &&
        IsTypeOneOfV<CType, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, half, bfloat16_t, float>) {
        return true;
    }
    if constexpr (IsSameTypeV<L0cT, int32_t> && IsSameTypeV<CType, bfloat16_t>) {
        return true;
    }
#endif

    if constexpr (IsSameTypeV<L0cT, int32_t> && IsTypeOneOfV<CType, half, int8_t, uint8_t>) {
        return true;
    } else if constexpr (IsSameTypeV<L0cT, float> && IsTypeOneOfV<CType, int8_t, uint8_t>) {
        return true;
    }
    return false;
}

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_UTILS_H__
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_UTILS_H
