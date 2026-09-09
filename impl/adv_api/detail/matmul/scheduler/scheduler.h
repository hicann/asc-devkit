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
 * \file scheduler.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/scheduler/scheduler.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_SCHEDULER_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_SCHEDULER_H
#define IMPL_MATMUL_SCHEDULER_SCHEDULER_H

#include "batch/batch_scheduler_single.h"
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "batch/batch_scheduler_v200.h"
#else
#include "batch/batch_scheduler.h"
#endif
#include "base/scheduler_intrablock.h"
#include "base/scheduler_mdl.h"
#include "base/scheduler_mdl_fullload.h"
#include "base/scheduler_norm.h"
#include "base/scheduler_norm_outer_product.h"
#include "base/scheduler_special_mdl.h"
#include "base/scheduler_n_buffer.h"
#include "base/scheduler_mdl_partial_output.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "base/scheduler_mdl_mx.h"
#include "base/scheduler_norm_mx.h"
#endif

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_SCHEDULER_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_SCHEDULER_H__
#endif
