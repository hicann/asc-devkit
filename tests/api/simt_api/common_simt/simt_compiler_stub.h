/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef SIMT_COMPILER_STUB_H
#define SIMT_COMPILER_STUB_H

#include "kernel_simt_cpu.h"
#include "kernel_vectorized.h"
#include "simt_stub.h"
#include "stub_def.h"
#include "simt_stub_fun.h"

#include <cstdint>

int32_t asc_get_block_idx();

int32_t asc_get_block_num();

static int32_t laneid() { return 0; }

static int32_t lanemask_lt() { return 0xFFFFFFFF; }

uint64_t __cce_simt_get_para_base_imp();

#endif
