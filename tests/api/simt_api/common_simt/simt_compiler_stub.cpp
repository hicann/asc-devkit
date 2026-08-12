/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "simt_compiler_stub.h"
#include "basic_api/kernel_common.h"
#include "basic_api/kernel_operator_utils_intf.h"
#include "kernel_simt_cpu.h"
#include "stub_def.h"
#include <cmath>
int64_t block_idx = 0;
int64_t block_num = 8;

uint32_t g_threadDimX = 1024;
uint32_t g_threadDimY = 1;

thread_local uint32_t g_threadIdxX = 1;
thread_local uint32_t g_threadIdxY = 1;
thread_local uint32_t g_threadIdxZ = 1;

int32_t asc_get_block_idx() { return 0; }

int32_t asc_get_block_num() { return 8; }

int64_t clz(uint64_t x)
{
    int32_t count = 0;
    uint64_t one = 1;
    uint8_t bitLen = 64;
    for (int i = 0; i < bitLen; i++) {
        uint64_t tmp = one << (bitLen - 1 - i);
        if (((tmp & x) >> (bitLen - 1 - i)) == 0) {
            count += 1;
        } else {
            break;
        }
    }
    return count;
}
int64_t bcnt1(uint64_t x) { return 0; }
namespace AscendC {
namespace check {
bool CheckFunVecBinaryScalarImplForMaskArray(
    VecBinaryScalarApiParams& chkParams, const uint64_t mask[], const char* intriName)
{
    return true;
}
bool CheckFunVecBinaryScalarImpl(VecBinaryScalarApiParams& chkParams, const uint64_t mask, const char* intriName)
{
    return true;
}
bool CheckFunVecBinaryScalarImpl(VecBinaryScalarApiParams& chkParams, const char* intriName) { return true; }
} // namespace check
} // namespace AscendC

namespace {
static uint64_t g_gridSyncArgs[5] = {0U, 0U, 0U, 0U, 0U};
} // namespace

uint64_t __cce_simt_get_para_base_imp() { return reinterpret_cast<uint64_t>(&g_gridSyncArgs[4]); }
