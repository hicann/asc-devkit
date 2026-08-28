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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE
#endif

/**
 * \file vector_compute.h
 * \brief Aggregate header for Tensor API experimental vector interfaces.
 */
#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_VECTOR_COMPUTE_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_VECTOR_COMPUTE_H

#ifndef CANN_ASC_USE_EXPERIMENTAL
#error "this is experimental feature, please set CANN_ASC_USE_EXPERIMENTAL = ON to use it."
#endif

#include "tensor_api/tensor.h"
#include "tensor_api/experimental/utils/reg_enum.h"
#include "tensor_api/experimental/arch/vector/reg_tensor.h"
#include "tensor_api/experimental/arch/vector/type_conversion.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"
#include "tensor_api/experimental/arch/vector/logical_compute.h"
#include "tensor_api/experimental/arch/vector/composite_compute.h"
#include "tensor_api/experimental/arch/vector/reduction_compute.h"
#include "tensor_api/experimental/arch/vector/histogram_compute.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"
#include "tensor_api/experimental/arch/vector/data_reorder.h"
#include "tensor_api/experimental/arch/vector/data_padding.h"
#include "tensor_api/experimental/arch/vector/compare_and_select.h"
#include "tensor_api/experimental/arch/vector/reg_data_load.h"
#include "tensor_api/experimental/arch/vector/reg_data_store.h"

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_VECTOR_COMPUTE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE
#endif
