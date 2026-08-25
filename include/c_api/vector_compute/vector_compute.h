/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "vector_compute.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_VECTOR_COMPUTE_H
#define INCLUDE_C_API_VECTOR_COMPUTE_VECTOR_COMPUTE_H

#include "c_api/vector_compute/compute/vector_arith.h"
#include "c_api/vector_compute/compute/vector_logic.h"
#include "c_api/vector_compute/compute/vector_reduce.h"
#include "c_api/vector_compute/compute/vector_broadcast.h"
#include "c_api/vector_compute/compute/vector_fused.h"
#include "c_api/vector_compute/compute/vector_compare.h"
#include "c_api/vector_compute/compute/vector_permute_sel.h"
#include "c_api/vector_compute/compute/vector_sort.h"
#include "c_api/vector_compute/compute/vector_gather.h"
#include "c_api/vector_compute/compute/vector_convert.h"
#include "c_api/vector_compute/compute/vector_mask_config.h"
#include "c_api/composite/vector_compute_composite.h"

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
