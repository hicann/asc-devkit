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
 * \file kernel_reg_compute_impl_intf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_impl_intf.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_IMPL_INTF__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_IMPL_INTF_H
#define ASCENDC_MODULE_REG_COMPUTE_IMPL_INTF_H

#include "kernel_reg_compute_addrreg_impl.h"
#include "kernel_reg_compute_common_impl.h"
#include "kernel_reg_compute_copy_impl.h"
#include "kernel_reg_compute_datacopy_impl.h"
#include "kernel_reg_compute_gather_mask_impl.h"
#include "kernel_reg_compute_maskreg_impl.h"
#include "kernel_reg_compute_pack_impl.h"
#include "kernel_reg_compute_vec_binary_impl.h"
#include "kernel_reg_compute_vec_binary_scalar_impl.h"
#include "kernel_reg_compute_vec_cmpsel_impl.h"
#include "kernel_reg_compute_vec_arange_impl.h"
#include "kernel_reg_compute_vec_duplicate_impl.h"
#include "kernel_reg_compute_vec_reduce_impl.h"
#include "kernel_reg_compute_vec_ternary_scalar_impl.h"
#include "kernel_reg_compute_vec_unary_impl.h"
#include "kernel_reg_compute_vec_vconv_impl.h"
#include "kernel_reg_compute_vec_fused_impl.h"
#include "kernel_reg_compute_histograms_impl.h"
#endif // ASCENDC_MODULE_REG_COMPUTE_IMPL_INTF_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_IMPL_INTF__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_IMPL_INTF__
#endif
