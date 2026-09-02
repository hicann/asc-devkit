/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_REG_COMPUTE_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_REG_COMPUTE_H
#define INCLUDE_C_API_REG_COMPUTE_REG_COMPUTE_H

#include "c_api/reg_compute/gather/ub_gather.h"
#include "c_api/reg_compute/load/loadalign.h"
#include "c_api/reg_compute/load/loadunalign.h"
#include "c_api/composite/loadunalign_composite.h"
#include "c_api/reg_compute/scatter/ub_scatter.h"
#include "c_api/reg_compute/store/storealign.h"
#include "c_api/reg_compute/store/storeunalign.h"
#include "c_api/composite/storeunalign_composite.h"
#include "c_api/reg_compute/reg_copy.h"
#include "c_api/reg_compute/compute/reg_arith.h"
#include "c_api/reg_compute/compute/reg_logic.h"
#include "c_api/reg_compute/compute/reg_reduce.h"
#include "c_api/reg_compute/compute/reg_broadcast.h"
#include "c_api/reg_compute/compute/reg_fused.h"
#include "c_api/reg_compute/compute/reg_compare.h"
#include "c_api/reg_compute/compute/reg_permute_sel.h"
#include "c_api/reg_compute/compute/reg_index.h"
#include "c_api/reg_compute/compute/reg_histogram.h"
#include "c_api/reg_compute/compute/reg_gather.h"
#include "c_api/reg_compute/compute/reg_mask.h"
#include "c_api/reg_compute/compute/reg_addr_reg.h"
#include "c_api/reg_compute/reg_convert.h"
#include "c_api/reg_compute/reg_sync.h"

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_REG_COMPUTE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_REG_COMPUTE_H
#endif
