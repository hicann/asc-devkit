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
 * \file kernel_operator_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_INTERFACE_H
#if !(                       \
    defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163))
// MICRO API
#include "reg_compute/kernel_reg_compute_intf.h"
#endif
#include "kernel_prof_trace_intf.h"
#include "kernel_operator_data_copy_intf.h"
#include "kernel_operator_fixpipe_intf.h"
#include "kernel_operator_dump_tensor_intf.h"
#include "kernel_operator_mm_intf.h"
#include "kernel_operator_gemm_intf.h"
#include "kernel_operator_conv2d_intf.h"
#include "kernel_operator_common_intf.h"
#if !(                       \
    defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163))
#include "kernel_operator_vec_binary_intf.h"
#include "kernel_operator_vec_binary_scalar_intf.h"
#include "kernel_operator_vec_duplicate_intf.h"
#include "kernel_operator_vec_gather_mask_intf.h"
#include "kernel_operator_vec_vconv_intf.h"
#endif
#include "kernel_operator_scalar_intf.h"
#if !(                       \
    defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163))
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
#endif
#include "kernel_operator_limits_intf.h"
#include "kernel_operator_sys_var_intf.h"
#include "kernel_operator_atomic_intf.h"
#include "kernel_operator_set_atomic_intf.h"
#include "kernel_operator_cache_intf.h"
#include "kernel_operator_utils_intf.h"

#if !(                       \
    defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163))
#include "include/adv_api/kernel_api.h"
#endif

#include "core_mng/roc/kernel_operator_cube_group_intf.h"
#include "core_mng/roc/kernel_operator_group_barrier_intf.h"

#if ((__NPU_ARCH__ != 3102) && (__NPU_ARCH__ != 3510)) && (__NPU_ARCH__ != 5102)
#include "include/adv_api/filter/dropout.h"
#include "include/adv_api/activation/sigmoid.h"
#include "include/adv_api/activation/softmax.h"
#include "include/adv_api/activation/simplesoftmax.h"
#include "include/adv_api/activation/softmaxflashv2.h"
#include "include/adv_api/activation/softmaxgrad.h"
#endif
#if (__NPU_ARCH__ <= 1001) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3003) || \
    (__NPU_ARCH__ == 3113)
#include "include/adv_api/index/arithprogression.h"
#include "include/adv_api/normalization/layernormgrad.h"
#include "include/adv_api/normalization/layernormgradbeta.h"
#include "include/adv_api/pad/pad.h"
#include "include/adv_api/math/frac.h"
#include "include/adv_api/math/power.h"
#include "include/adv_api/math/log.h"
#include "include/adv_api/math/sin.h"
#include "include/adv_api/math/cos.h"
#include "include/adv_api/math/asin.h"
#include "include/adv_api/math/acos.h"
#include "include/adv_api/math/asinh.h"
#include "include/adv_api/math/acosh.h"
#include "include/adv_api/math/atan.h"
#include "include/adv_api/math/cosh.h"
#include "include/adv_api/math/erf.h"
#include "include/adv_api/math/erfc.h"
#include "include/adv_api/math/clamp.h"
#include "include/adv_api/normalization/rmsnorm.h"
#include "include/adv_api/normalization/batchnorm.h"
#include "include/adv_api/math/tanh.h"
#include "include/adv_api/math/atanh.h"
#include "include/adv_api/normalization/deepnorm.h"
#include "include/adv_api/math/exp.h"
#include "include/adv_api/normalization/layernorm.h"
#include "include/adv_api/reduce/sum.h"
#include "include/adv_api/activation/silu.h"
#include "include/adv_api/activation/gelu.h"
#include "include/adv_api/quantization/ascend_quant.h"
#include "include/adv_api/quantization/ascend_dequant.h"
#include "include/adv_api/quantization/ascend_antiquant.h"
#include "include/adv_api/activation/logsoftmax.h"
#include "include/adv_api/activation/softmaxflash.h"
#include "include/adv_api/transpose/confusion_transpose.h"
#include "include/adv_api/select/selectwithbytesmask.h"
#include "include/adv_api/math/sinh.h"
#include "include/adv_api/activation/swiglu.h"
#include "include/adv_api/activation/reglu.h"
#include "include/adv_api/math/tan.h"
#include "include/adv_api/math/round.h"
#include "include/adv_api/math/trunc.h"
#include "include/adv_api/activation/swish.h"
#include "include/adv_api/sort/topk.h"
#include "include/adv_api/activation/geglu.h"
#include "include/adv_api/math/lgamma.h"
#include "include/adv_api/math/digamma.h"
#include "include/adv_api/math/xor.h"
#include "include/adv_api/math/sign.h"
#include "include/adv_api/reduce/mean.h"
#include "include/adv_api/math/axpy.h"
#include "include/adv_api/math/ceil.h"
#include "include/adv_api/math/floor.h"
#include "include/adv_api/pad/broadcast.h"
#include "include/adv_api/reduce/reduce_xor_sum.h"
#include "include/adv_api/math/cumsum.h"
#endif
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
// SIMT API
#include "simt_api/cpp/kernel_simt_intf.h"
#endif
#endif // ASCENDC_MODULE_OPERATOR_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_INTF_H__
#endif
