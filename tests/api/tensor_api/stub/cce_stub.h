/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TESTS_API_TENSOR_API_STUB_CCE_STUB_H
#define TESTS_API_TENSOR_API_STUB_CCE_STUB_H

#include "c_api/stub/cce_stub.h"

using float8_e8m0_t = fp8_e8m0_t;

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ != 3510)
using vector_uint8_t = uint8_t;
using vector_uint16_t = uint8_t;
using vector_uint32_t = uint8_t;
using vector_uint64_t = uint8_t;
using vector_int8_t = uint8_t;
using vector_int16_t = uint8_t;
using vector_int32_t = uint8_t;
using vector_int64_t = uint8_t;
using vector_bfloat16_t = uint8_t;
using vector_half = uint8_t;
using vector_float = uint8_t;
using vector_hifloat8_t = uint8_t;
using vector_fp8_e4m3fn_t = uint8_t;
using vector_fp8_e5m2_t = uint8_t;
using vector_fp8_e8m0_t = uint8_t;
using vector_int4x2_t = uint8_t;
using vector_fp4x2_e2m1_t = uint8_t;
using vector_fp4x2_e1m2_t = uint8_t;
#endif

#endif
