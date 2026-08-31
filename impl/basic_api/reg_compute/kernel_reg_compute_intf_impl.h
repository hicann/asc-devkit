/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file kernel_reg_compute_intf_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/kernel_reg_compute_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_INTF_IMPL__
#endif

#ifndef ASCENDC_KERNEL_REG_COMPUTE_INTF_IMPL_H
#define ASCENDC_KERNEL_REG_COMPUTE_INTF_IMPL_H

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 3003) || ((__NPU_ARCH__ == 3113)) || defined(__ASC_NPU_HOST__) || \
    (__NPU_ARCH__ == 5102)
#include "kernel_reg_compute_maskreg_intf_impl.h"
#include "kernel_reg_compute_addrreg_intf_impl.h"
#include "kernel_reg_compute_common_intf_impl.h"
#include "kernel_reg_compute_copy_intf_impl.h"
#include "kernel_reg_compute_vec_duplicate_intf_impl.h"
#include "kernel_reg_compute_vec_data_reorder_intf_impl.h"
#include "kernel_reg_compute_datacopy_intf_impl.h"
#include "kernel_reg_compute_gather_mask_intf_impl.h"
#include "kernel_reg_compute_pack_intf_impl.h"
#include "kernel_reg_compute_vec_binary_scalar_intf_impl.h"
#include "kernel_reg_compute_vec_binary_intf_impl.h"
#include "kernel_reg_compute_vec_cmpsel_intf_impl.h"
#include "kernel_reg_compute_vec_arange_intf_impl.h"
#include "kernel_reg_compute_vec_reduce_intf_impl.h"
#include "kernel_reg_compute_vec_ternary_scalar_intf_impl.h"
#include "kernel_reg_compute_vec_unary_intf_impl.h"
#include "kernel_reg_compute_vec_vconv_intf_impl.h"
#include "kernel_reg_compute_vec_fused_intf_impl.h"
#include "kernel_reg_compute_histograms_intf_impl.h"
#endif

#endif // ASCENDC_KERNEL_REG_COMPUTE_INTF_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_INTF_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_INTF_IMPL__
#endif
