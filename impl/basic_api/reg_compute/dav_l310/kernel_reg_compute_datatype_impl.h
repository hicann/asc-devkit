/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_reg_compute_datatype_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_l310/kernel_reg_compute_datatype_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_struct_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATATYPE_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_DATATYPE_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_DATATYPE_IMPL_H

#include "../../kernel_utils.h"

namespace AscendC {
namespace Reg {
template <typename T>
struct TypeGet;
template <>
struct TypeGet<uint32_t> {
    using T = vector_u32;
};
template <>
struct TypeGet<int32_t> {
    using T = vector_s32;
};
template <>
struct TypeGet<float> {
    using T = vector_f32;
};
template <>
struct TypeGet<uint16_t> {
    using T = vector_u16;
};
template <>
struct TypeGet<half> {
    using T = vector_f16;
};
template <>
struct TypeGet<uint64_t> {
    using T = vector_u64;
};
template <>
struct TypeGet<int64_t> {
    using T = vector_s64;
};
template <>
struct TypeGet<int16_t> {
    using T = vector_s16;
};
template <>
struct TypeGet<uint8_t> {
    using T = vector_u8;
};
template <>
struct TypeGet<int8_t> {
    using T = vector_s8;
};
template <>
struct TypeGet<bool> {
    using T = vector_s8;
};
template <>
struct TypeGet<int4x2_t> {
    using T = vector_s4x2;
};
template <>
struct TypeGet<bfloat16_t> {
    using T = vector_bf16;
};
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_DATATYPE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATATYPE_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATATYPE_IMPL__
#endif
