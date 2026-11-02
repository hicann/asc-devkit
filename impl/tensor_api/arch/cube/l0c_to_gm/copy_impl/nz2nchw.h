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
#warning \
    "impl/tensor_api/arch/cube/l0c_to_gm/copy_impl/nz2nchw.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NCHW_H
#endif

/*!
 * \file nz2nchw.h
 * \brief L0C(NZ) -> GM(NCHW) copy via the nz2dn fixpipe path (no quant).
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NCHW_H
#define IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NCHW_H

#include "impl/tensor_api/arch/cube/l0c_to_gm/copy_impl/nz2nc_common.h"

namespace asc {
namespace te {

class data_copy_l0c_to_gm_nz2nchw : public data_copy_l0c_to_gm_nz2nc_base<false> {};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NCHW_H

#if defined( \
    UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NCHW_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NCHW_H
#endif
