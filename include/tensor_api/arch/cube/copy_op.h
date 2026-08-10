/**
* Copyright (c) 2026 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/


#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif

/*!
* \file copy_op.h
* \brief
*/
#ifndef INCLUDE_TENSOR_API_ARCH_CUBE_COPY_OP_H
#define INCLUDE_TENSOR_API_ARCH_CUBE_COPY_OP_H

namespace asc {
namespace te {

struct copy_gm_to_l1;
struct copy_l1_to_ub;
struct copy_l1_to_l0b;
struct copy_l1_to_l0a;
struct copy_l1_to_l0scalea;
struct copy_l1_to_l0scaleb;
struct copy_l1_to_fixbuf;
struct copy_l1_to_biastable;
struct copy_l0c_to_gm;
struct copy_l0c_to_ub;
struct copy_l0c_to_l1;

struct copy_gm_to_l1_trait;
struct copy_gm_to_l1_trait_default;
struct copy_l1_to_ub_trait;
struct copy_l1_to_ub_trait_default;
struct copy_l1_to_l0a_trait;
struct copy_l1_to_l0a_trait_default;
struct copy_l1_to_l0b_trait;
struct copy_l1_to_l0b_trait_default;
struct copy_l1_to_l0scalea_trait;
struct copy_l1_to_l0scalea_trait_default;
struct copy_l1_to_l0scaleb_trait;
struct copy_l1_to_l0scaleb_trait_default;
struct copy_l1_to_fixbuf_trait;
struct copy_l1_to_fixbuf_trait_default;
struct copy_l1_to_biastable_trait;
struct copy_l1_to_biastable_trait_default;
struct copy_l0c_to_gm_trait;
struct copy_l0c_to_gm_trait_default;
struct copy_l0c_to_ub_trait;
struct copy_l0c_to_ub_trait_default;
struct copy_l0c_to_l1_trait;
struct copy_l0c_to_l1_trait_default;

} // namespace te
} // namespace asc

#endif // INCLUDE_TENSOR_API_ARCH_CUBE_COPY_OP_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif
