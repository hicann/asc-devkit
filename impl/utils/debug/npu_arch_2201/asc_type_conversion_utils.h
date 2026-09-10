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
 * \file asc_type_conversion_utils.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_NPU_ARCH_2201_ASC_TYPE_CONVERSION_UTILS_H
#define IMPL_UTILS_DEBUG_NPU_ARCH_2201_ASC_TYPE_CONVERSION_UTILS_H

#include "impl/utils/sys_macros.h"
namespace __asc_aicore {

__aicore__ inline float cast_type(const bfloat16_t& bVal)
{
    constexpr uint32_t bf16ToFp32ManLen = 16;
    bfloat16_t bNum = bVal;
    union ToFloatUnion {
        __aicore__ ToFloatUnion() {}
        uint32_t val;
        float fNum;
    } floatUnion;
    union ToUint16Union {
        __aicore__ ToUint16Union() {}
        bfloat16_t uret;
        uint16_t num;
    } u16Union;
    u16Union.uret = bNum;
    floatUnion.val = u16Union.num << bf16ToFp32ManLen;
    return floatUnion.fNum;
}

__aicore__ inline float to_float(const bfloat16_t& bVal) { return cast_type(bVal); }
} // namespace __asc_aicore
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_TYPE_CONVERSION_UTILS__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_TYPE_CONVERSION_UTILS__
#endif

#endif // IMPL_UTILS_DEBUG_NPU_ARCH_2201_ASC_TYPE_CONVERSION_UTILS_H
