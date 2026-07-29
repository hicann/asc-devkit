/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef __TILINGDATA_HEADER__
#define __TILINGDATA_HEADER__
#include "kernel_tiling/kernel_tiling.h"
#ifdef ASCENDC_CPU_DEBUG
#include "kernel_log.h"
#else
#ifndef __aicore__
#define __aicore__ [aicore]
#endif
#endif
class TilingData_A {
public:
    uint32_t A_xxx1 = 0;
    uint32_t A_xxx2 = 0;
    uint32_t totalLength = 0;
    uint32_t tileNum = 0;
};

class TilingData_B {
public:
    uint32_t B_xxx1 = 0;
    uint32_t B_xxx2 = 0;
    TilingData_A A_in_B;
};

class TilingData_C {
public:
    uint32_t C_xxx1 = 0;
    uint32_t C_xxx2 = 0;
    TilingData_B B_in_C;
};

class TilingData_E {
public:
    uint32_t E_xxx1 = 0;
    uint32_t E_xxx2 = 0;
    TilingData_B B_in_E;
};

class TilingData_D {
public:
    uint32_t D_xxx1 = 0;
    uint8_t C_in_DPH[4] = {};
    TilingData_C C_in_D;
    uint32_t D_xxx2 = 0;
    uint8_t E_in_DPH[4] = {};
    TilingData_E E_in_D;
};

class TilingData_D2 {
public:
    __aicore__ __inline__ __attribute__((always_inline)) TilingData_D2() {}
    uint32_t totalLength;
    uint8_t D_in_tilingPH[4];
    TilingData_D D_in_tiling;
    uint32_t tileNum;
    uint8_t TilingData_D2PH[4];
} __attribute__((__may_alias__));

class TilingData {
public:
    uint32_t totalLength = 0;
    uint8_t D_in_tilingPH[4] = {0, 0, 0, 0};
    TilingData_D D_in_tiling = {
        0,
        {0, 0, 0, 0},
        {
            0,
            0,
            {
                0,
                0,
                {
                    0,
                    0,
                    16384,
                    8,
                },
            },
        },
        0,
        {0, 0, 0, 0},
        {
            0,
            0,
            {
                0,
                0,
                {
                    0,
                    0,
                    0,
                    0,
                },
            },
        },
    };
    uint32_t tileNum = 0;
    uint8_t TilingDataPH[4] = {0, 0, 0, 0};
};

#ifdef ASCENDC_TIME_STAMP_ON
#define GET_TILING_DATA(tiling_data, tiling_arg) const TilingData tiling_data;

#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg) const tiling_struct tiling_data;

#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling) \
    const tiling_type point##var;                                \
    auto& var = point##var.member;

#else
#define GET_TILING_DATA(tiling_data, tiling_arg) const TilingData tiling_data;

#define GET_TILING_DATA_WITH_STRUCT(tiling_struct, tiling_data, tiling_arg) const tiling_struct tiling_data;

#define GET_TILING_DATA_MEMBER(tiling_type, member, var, tiling) \
    const tiling_type point##var;                                \
    auto& var = point##var.member;

#endif
#endif // __TILINGDATA_HEADER__
