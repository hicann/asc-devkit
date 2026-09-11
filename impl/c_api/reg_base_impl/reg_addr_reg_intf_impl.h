/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/reg_base_impl/reg_addr_reg_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_ADDR_REG_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_ADDR_REG_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/reg_addr_reg_intf_impl.h"
#endif

#define ASC_DEFINE_UPDATE_ADDR_REG_API(bxx)                                                                         \
    __simd_callee__ inline addr_reg asc_update_addr_reg_##bxx(uint32_t offset)                                      \
    {                                                                                                               \
        if ASC_IS_AIV {                                                                                             \
            return vag_##bxx(offset);                                                                               \
        }                                                                                                           \
        return addr_reg{};                                                                                          \
    }                                                                                                               \
    __simd_callee__ inline addr_reg asc_update_addr_reg_##bxx(uint32_t offset0, uint32_t offset1)                   \
    {                                                                                                               \
        if ASC_IS_AIV {                                                                                             \
            return vag_##bxx(offset0, offset1);                                                                     \
        }                                                                                                           \
        return addr_reg{};                                                                                          \
    }                                                                                                               \
    __simd_callee__ inline addr_reg asc_update_addr_reg_##bxx(uint32_t offset0, uint32_t offset1, uint32_t offset2) \
    {                                                                                                               \
        if ASC_IS_AIV {                                                                                             \
            return vag_##bxx(offset0, offset1, offset2);                                                            \
        }                                                                                                           \
        return addr_reg{};                                                                                          \
    }                                                                                                               \
    __simd_callee__ inline addr_reg asc_update_addr_reg_##bxx(                                                      \
        uint32_t offset0, uint32_t offset1, uint32_t offset2, uint32_t offset3)                                     \
    {                                                                                                               \
        if ASC_IS_AIV {                                                                                             \
            return vag_##bxx(offset0, offset1, offset2, offset3);                                                   \
        }                                                                                                           \
        return addr_reg{};                                                                                          \
    }

ASC_DEFINE_UPDATE_ADDR_REG_API(b32)
ASC_DEFINE_UPDATE_ADDR_REG_API(b16)
ASC_DEFINE_UPDATE_ADDR_REG_API(b8)

#undef ASC_DEFINE_UPDATE_ADDR_REG_API

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
