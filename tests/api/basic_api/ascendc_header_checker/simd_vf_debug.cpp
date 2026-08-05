/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef VERIFY_SINGLE_HEADER
#include "kernel_operator.h"
#endif

#if __NPU_ARCH__ == 3510
#include "utils/debug/asc_dump.h"
#include "utils/debug/asc_printf.h"

__simd_vf__ inline void TestSimdVfDebugApis(__ubuf__ uint32_t* ubInput)
{
    vector_u32 regInput;
    __asc_simd_vf::printf("SIMD VF debug %u", 1U);
    asc_dump_ubuf<uint32_t>(ubInput, 0, 1);
    asc_dump_reg<uint32_t>(regInput, 0, 1);
    asc_dump<uint32_t>(ubInput, 0, 1);
    asc_dump<uint32_t>(regInput, 0, 1);
    __asc_simd_vf::asc_dump_ubuf<uint32_t>(ubInput, 0, 1);
    __asc_simd_vf::asc_dump_reg<uint32_t>(regInput, 0, 1);
    __asc_simd_vf::asc_dump<uint32_t>(ubInput, 0, 1);
    __asc_simd_vf::asc_dump<uint32_t>(regInput, 0, 1);
}
#endif
