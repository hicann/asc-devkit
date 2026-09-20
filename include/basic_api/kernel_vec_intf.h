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
 * \file kernel_vec_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_vec_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_VEC_INTF_H__
#endif

#ifndef ASCENDC_MODULE_VEC_INTERFACE_H
#define ASCENDC_MODULE_VEC_INTERFACE_H

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include "stub_def.h"
#include "stub_fun.h"
#endif // ASCENDC_CPU_DEBUG

#include "kernel_type.h"
#include "kernel_tpipe.h"
#include "kernel_tensor.h"
#include "kernel_prof_trace_intf.h"
#include "kernel_operator_data_copy_intf.h"
#include "kernel_operator_dump_tensor_intf.h"
#include "kernel_operator_common_intf.h"
#include "kernel_operator_vec_binary_intf.h"
#include "kernel_operator_vec_binary_scalar_intf.h"
#include "kernel_operator_vec_duplicate_intf.h"
#include "kernel_operator_vec_gather_mask_intf.h"
#include "kernel_operator_vec_vconv_intf.h"
#include "kernel_operator_scalar_intf.h"
#include "kernel_operator_vec_reduce_intf.h"
#include "kernel_operator_proposal_intf.h"
#include "kernel_operator_determine_compute_sync_intf.h"
#include "kernel_operator_vec_transpose_intf.h"
#include "kernel_operator_vec_gather_intf.h"
#include "kernel_operator_vec_scatter_intf.h"
#include "kernel_operator_vec_brcb_intf.h"
#include "kernel_operator_vec_cmpsel_intf.h"
#include "kernel_operator_vec_mulcast_intf.h"
#include "kernel_operator_vec_bilinearinterpolation_intf.h"
#include "kernel_operator_vec_createvecindex_intf.h"
#include "kernel_operator_vec_ternary_scalar_intf.h"
#include "kernel_operator_vec_unary_intf.h"
#include "kernel_operator_vec_vpadding_intf.h"
#include "kernel_operator_limits_intf.h"
#include "kernel_operator_sys_var_intf.h"
#include "kernel_operator_atomic_intf.h"
#include "kernel_operator_set_atomic_intf.h"
#include "kernel_operator_cache_intf.h"
#include "kernel_operator_utils_intf.h"

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_VEC_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_VEC_INTF_H__
#endif
