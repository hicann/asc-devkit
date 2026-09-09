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
 * \file k_loop.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/scheduler/iterator/k_loop/k_loop.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_H
#define IMPL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_H

#include "k_loop_mdl_reorder.h"
#include "k_loop_mdl.h"
#include "k_loop_norm.h"
#include "k_loop_intrablock.h"
#include "k_loop_mdl_partial_output.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "k_loop_mdl_mx.h"
#endif

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_ITERATOR_K_LOOP_K_LOOP_H__
#endif // _K_LOOP_H_
