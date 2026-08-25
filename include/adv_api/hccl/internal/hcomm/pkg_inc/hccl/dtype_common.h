/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef DTYPE_COMMON_H
#define DTYPE_COMMON_H

#include <unordered_map>
#include "hccl/base.h"

// 2 is sizeof(float16), 8 is sizeof(float64), 2 is sizeof(bfloat16)..
constexpr u32 SIZE_TABLE[HCCL_DATA_TYPE_RESERVED] = {
    sizeof(s8),
    sizeof(s16),
    sizeof(s32),
    2,
    sizeof(float),
    sizeof(s64),
    sizeof(u64),
    sizeof(u8),
    sizeof(u16),
    sizeof(u32),
    8,
    2,
    16,
    2,
    1,
    1,
    1,
    1,
    1};

static_assert(SIZE_TABLE[HCCL_DATA_TYPE_MXFP8] == 1U, "MXFP8 elements occupy one byte");

// 对内芯片类型
#define MACRO_DEV_TYPE_NEW // 兼容性处理，后续删除
enum class DevType {
    DEV_TYPE_910 = 0,
    DEV_TYPE_310P3 = 1, // PG
    DEV_TYPE_910B = 2,
    DEV_TYPE_310P1 = 3, // AG
    DEV_TYPE_910_93 = 4,
    DEV_TYPE_NOSOC = 5,
    DEV_TYPE_950 = 6,
    DEV_TYPE_COUNT = 7
};

#ifdef __cplusplus
extern "C" {
#endif
HcclResult hrtGetDeviceType(DevType& devType);
#ifdef __cplusplus
} // extern "C"
#endif

#endif // DTYPE_COMMON_H
