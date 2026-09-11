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
 * \file kernel_reg_compute_datatype_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_datatype_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_struct_intf.h\"\" and use public functions or variables defined in interface headers files.")
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
struct TypeGet<complex32> {
    // only for passing compilation, T is actually ineffective here
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_u32;
#endif
};

template <>
struct TypeGet<double> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_u64;
#endif
};

template <>
struct TypeGet<complex64> {
    // only for passing compilation, T is actually ineffective here
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_u64;
#endif
};

template <>
struct TypeGet<uint64_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_u64;
#endif
};

template <>
struct TypeGet<int64_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_s64;
#endif
};

template <>
struct TypeGet<uint32_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_u32;
#endif
};
template <>
struct TypeGet<int32_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_s32;
#endif
};

template <>
struct TypeGet<float> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_f32;
#endif
};

template <>
struct TypeGet<uint16_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_u16;
#endif
};

template <>
struct TypeGet<half> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_f16;
#endif
};

template <>
struct TypeGet<bfloat16_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_bf16;
#endif
};

template <>
struct TypeGet<int16_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_s16;
#endif
};

template <>
struct TypeGet<uint8_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_u8;
#endif
};

template <>
struct TypeGet<int8_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_s8;
#endif
};

template <>
struct TypeGet<hifloat8_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_hif8;
#endif
};

template <>
struct TypeGet<fp8_e4m3fn_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_f8e4m3;
#endif
};

template <>
struct TypeGet<fp8_e5m2_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_f8e5m2;
#endif
};

#if !defined(__ASC_NPU_HOST__)
template <>
struct TypeGet<fp8_e8m0_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_f8e8m0;
#endif
};
#endif

template <>
struct TypeGet<fp4x2_e2m1_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_f4e2m1x2;
#endif
};

template <>
struct TypeGet<fp4x2_e1m2_t> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_f4e1m2x2;
#endif
};

#ifndef __ASC_NPU_HOST__
template <>
struct TypeGet<int4x2_t> {
    using T = vector_s4x2;
};
#endif

template <>
struct TypeGet<bool> {
#if defined(__ASC_NPU_HOST__)
    using T = int;
#else
    using T = vector_s8;
#endif
};
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_DATATYPE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATATYPE_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_DATATYPE_IMPL__
#endif
