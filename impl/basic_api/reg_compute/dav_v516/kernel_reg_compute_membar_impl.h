/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_membar_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_membar_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_membar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MEMBAR_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_MEMBAR_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_MEMBAR_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_utils.h"

namespace AscendC {
namespace Reg {

#ifndef __ASC_NPU_HOST__
template <MemType src, MemType dst>
__simd_callee__ inline void LocalMemBarImpl()
{
    static_assert(
        (SupportEnum<src, MemType::VEC_STORE>() && SupportEnum<dst, MemType::VEC_LOAD>()) ||
            (SupportEnum<src, MemType::VEC_LOAD>() && SupportEnum<dst, MemType::VEC_STORE>()) ||
            (SupportEnum<src, MemType::VEC_STORE>() && SupportEnum<dst, MemType::VEC_STORE>()) ||
            (SupportEnum<src, MemType::VEC_STORE>() && SupportEnum<dst, MemType::SCALAR_LOAD>()) ||
            (SupportEnum<src, MemType::VEC_STORE>() && SupportEnum<dst, MemType::SCALAR_STORE>()) ||
            (SupportEnum<src, MemType::VEC_LOAD>() && SupportEnum<dst, MemType::SCALAR_STORE>()) ||
            (SupportEnum<src, MemType::SCALAR_STORE>() && SupportEnum<dst, MemType::VEC_LOAD>()) ||
            (SupportEnum<src, MemType::SCALAR_STORE>() && SupportEnum<dst, MemType::VEC_STORE>()) ||
            (SupportEnum<src, MemType::SCALAR_LOAD>() && SupportEnum<dst, MemType::VEC_STORE>()) ||
            (SupportEnum<src, MemType::VEC_ALL>() && SupportEnum<dst, MemType::VEC_ALL>()) ||
            (SupportEnum<src, MemType::VEC_ALL>() && SupportEnum<dst, MemType::SCALAR_ALL>()) ||
            (SupportEnum<src, MemType::SCALAR_ALL>() && SupportEnum<dst, MemType::VEC_ALL>()),
        "LocalMemBar does support current MemType Combination on current device!");
    if constexpr (src == MemType::VEC_STORE && dst == MemType::VEC_LOAD) {
        mem_bar(VST_VLD);
    } else if constexpr (src == MemType::VEC_LOAD && dst == MemType::VEC_STORE) {
        mem_bar(VLD_VST);
    } else if constexpr (src == MemType::VEC_STORE && dst == MemType::VEC_STORE) {
        mem_bar(VST_VST);
    } else if constexpr (src == MemType::VEC_STORE && dst == MemType::SCALAR_LOAD) {
        mem_bar(VST_LD);
    } else if constexpr (src == MemType::VEC_STORE && dst == MemType::SCALAR_STORE) {
        mem_bar(VST_ST);
    } else if constexpr (src == MemType::VEC_LOAD && dst == MemType::SCALAR_STORE) {
        mem_bar(VLD_ST);
    } else if constexpr (src == MemType::SCALAR_STORE && dst == MemType::VEC_LOAD) {
        mem_bar(ST_VLD);
    } else if constexpr (src == MemType::SCALAR_STORE && dst == MemType::VEC_STORE) {
        mem_bar(ST_VST);
    } else if constexpr (src == MemType::SCALAR_LOAD && dst == MemType::VEC_STORE) {
        mem_bar(LD_VST);
    } else if constexpr (src == MemType::VEC_ALL && dst == MemType::VEC_ALL) {
        mem_bar(VV_ALL);
    } else if constexpr (src == MemType::VEC_ALL && dst == MemType::SCALAR_ALL) {
        mem_bar(VS_ALL);
    } else if constexpr (src == MemType::SCALAR_ALL && dst == MemType::VEC_ALL) {
        mem_bar(SV_ALL);
    }
}
#endif

} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_MEMBAR_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MEMBAR_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MEMBAR_IMPL__
#endif
