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
#error "asc_memory_vector.h cannot be used with compile flag --enable-simt enabled."
#endif

#ifndef INCLUDE_C_API_ASC_MEMORY_VECTOR_H
#define INCLUDE_C_API_ASC_MEMORY_VECTOR_H

#include "utils/debug/asc_assert.h"
#include "utils/debug/asc_dump.h"
#include "utils/debug/asc_printf.h"
#include "utils/debug/asc_time.h"
#include "c_api/defs/defs.h"
#include "c_api/atomic/atomic.h"
#include "c_api/cache_ctrl/cache_ctrl.h"
#include "c_api/scalar_compute/scalar_compute.h"
#include "c_api/sync/sync.h"
#include "c_api/utils/utils.h"
#include "c_api/vector_compute/vector_compute.h"
#include "c_api/vector_datamove/vector_datamove.h"

#endif
