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
#else
#include "kernel_operator_sys_var_intf.h"
#endif

// __aicore__ inline void GetArchVersion(uint32_t& coreVersion);
extern "C" __global__ __aicore__ void KernelTestGetArchVersion1()
{
    uint32_t coreVersion = 0;
    AscendC::GetArchVersion(coreVersion);
}

// __aicore__ inline void Trap();
extern "C" __global__ __aicore__ void KernelTestTrap1() { AscendC::Trap(); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
// template <SpecialPurposeReg spr>
// __aicore__ inline void ClearSpr();
extern "C" __global__ __aicore__ void KernelTestClearSpr1() { AscendC::ClearSpr<AscendC::SpecialPurposeReg::AR>(); }
#endif
