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
 * \file kernel_operator_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_INTERFACE_IMPL_H

#include "kernel_operator_data_copy_intf_impl.h"
#include "kernel_operator_dump_tensor_intf_impl.h"
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
#include "kernel_operator_mm_bitmode_intf_impl.h"
#endif
#include "kernel_operator_mm_intf_impl.h"
#include "kernel_operator_gemm_intf_impl.h"
#include "kernel_operator_fixpipe_intf_impl.h"
#include "kernel_operator_conv2d_intf_impl.h"
#include "kernel_operator_common_intf_impl.h"
#include "kernel_operator_proposal_intf_impl.h"
#include "kernel_operator_vec_bilinearinterpolation_intf_impl.h"
#include "kernel_operator_vec_createvecindex_intf_impl.h"
#include "kernel_operator_vec_mulcast_intf_impl.h"
#include "kernel_operator_determine_compute_sync_intf_impl.h"
#include "kernel_operator_vec_transpose_intf_impl.h"
#include "kernel_operator_vec_gather_intf_impl.h"
#include "kernel_operator_vec_scatter_intf_impl.h"
#include "kernel_operator_vec_brcb_intf_impl.h"
#include "kernel_operator_vec_binary_intf_impl.h"
#include "kernel_operator_vec_binary_scalar_intf_impl.h"
#include "kernel_operator_vec_cmpsel_intf_impl.h"
#include "kernel_operator_vec_duplicate_intf_impl.h"
#include "kernel_operator_vec_reduce_intf_impl.h"
#include "kernel_operator_vec_gather_mask_intf_impl.h"
#include "kernel_operator_vec_ternary_scalar_intf_impl.h"
#include "kernel_operator_vec_unary_intf_impl.h"
#include "kernel_operator_vec_vconv_intf_impl.h"
#include "kernel_operator_vec_vpadding_intf_impl.h"
#include "kernel_operator_scalar_intf_impl.h"
#include "kernel_operator_sys_var_intf_impl.h"
#include "kernel_operator_set_atomic_intf_impl.h"
#include "kernel_prof_trace_intf_impl.h"
#include "kernel_operator_atomic_intf_impl.h"

#endif // ASCENDC_MODULE_OPERATOR_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_INTF_IMPL_H__
#endif
