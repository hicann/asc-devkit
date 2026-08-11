/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/**
 * \file host_check_data_type.cpp
 * \brief 验证编译器对 Ascend C 常用数据类型（整型/浮点/半精度/FP8/FP4/复数）的支持。
 * 由 ascendc_host_header_checker 的 CMakeLists 对 ARCH_LIST(3510/2201/2002) 每个架构做
 * -fsyntax-only 编译检查：类型符号存在且字节宽度符合预期即通过。
 *
 * 说明：
 * - bool/int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t/int64_t/uint64_t/float/double
 *   为标准 C++ 内建/头文件类型，各架构均可用。
 * - half/bfloat16_t/hifloat8_t/int4x2_t 为编译器内建扩展类型，新版 bisheng 各架构均支持。
 * - fp8_e5m2_t/fp8_e4m3fn_t/fp8_e8m0_t/fp4x2_e2m1_t/fp4x2_e1m2_t 由头文件定义，
 *   新版 bisheng 各架构均支持真实的 float8/float4 类型（common_types.h 无条件定义）。
 * - complex32/complex64 由 kernel_utils_ceil_oom_que.h 无条件定义，各架构均可用。
 */

#include <cstdint>
#include "kernel_operator.h"
static void test_data_type_b8()
{
    static_assert(sizeof(bool) == 1, "bool must be 1 byte");
    static_assert(sizeof(int4x2_t) == 1, "int4x2_t must be 1 byte");
    static_assert(sizeof(int8_t) == 1, "int8_t must be 1 byte");
    static_assert(sizeof(uint8_t) == 1, "uint8_t must be 1 byte");
    static_assert(sizeof(hifloat8_t) == 1, "hifloat8_t must be 1 byte");
    static_assert(sizeof(fp8_e5m2_t) == 1, "fp8_e5m2_t must be 1 byte");
    static_assert(sizeof(fp8_e4m3fn_t) == 1, "fp8_e4m3fn_t must be 1 byte");
    static_assert(sizeof(fp8_e8m0_t) == 1, "fp8_e8m0_t must be 1 byte");
    static_assert(sizeof(fp4x2_e2m1_t) == 1, "fp4x2_e2m1_t must be 1 byte");
    static_assert(sizeof(fp4x2_e1m2_t) == 1, "fp4x2_e1m2_t must be 1 byte");
}

static void test_data_type_b16()
{
    static_assert(sizeof(int16_t) == 2, "int16_t must be 2 bytes");
    static_assert(sizeof(uint16_t) == 2, "uint16_t must be 2 bytes");
    static_assert(sizeof(half) == 2, "half must be 2 bytes");
    static_assert(sizeof(bfloat16_t) == 2, "bfloat16_t must be 2 bytes");
}

static void test_data_type_b32()
{
    static_assert(sizeof(int32_t) == 4, "int32_t must be 4 bytes");
    static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");
    static_assert(sizeof(float) == 4, "float must be 4 bytes");
    static_assert(sizeof(complex32) == 4, "complex32 must be 4 bytes");
}

static void test_data_type_b64()
{
    static_assert(sizeof(int64_t) == 8, "int64_t must be 8 bytes");
    static_assert(sizeof(uint64_t) == 8, "uint64_t must be 8 bytes");
    static_assert(sizeof(double) == 8, "double must be 8 bytes");
    static_assert(sizeof(complex64) == 8, "complex64 must be 8 bytes");
}
