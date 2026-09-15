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
#include "kernel_operator_common_intf.h"
#endif

// template<pipe_t AIV_PIPE, pipe_t AIC_PIPE, bool FORCE>
// __aicore__ inline void SetNextTaskStart();
extern "C" __global__ __aicore__ void KernelTestSetNextTaskStart1()
{
    AscendC::SetNextTaskStart();
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ != 1001) && (__NPU_ARCH__ != 2002)
    AscendC::SetNextTaskStart<PIPE_MTE3, PIPE_FIX, true>();
#else
    AscendC::SetNextTaskStart<PIPE_MTE3, PIPE_MTE3, true>();
#endif
}

// template<bool FORCE>
// __aicore__ inline void WaitPreTaskEnd();
extern "C" __global__ __aicore__ void KernelTestWaitPreTaskEnd1()
{
    AscendC::WaitPreTaskEnd();
    AscendC::WaitPreTaskEnd<true>();
}

// __aicore__ inline void InitSocState();
extern "C" __global__ __aicore__ void KernelTestInitSocState1() { AscendC::InitSocState(); }

// template <AtomicDtype type, AtomicOp op>
// __aicore__ inline void SetStoreAtomicConfig();
extern "C" __global__ __aicore__ void KernelTestSetStoreAtomicConfig1()
{
    AscendC::SetStoreAtomicConfig<AscendC::AtomicDtype::ATOMIC_NONE, AscendC::AtomicOp::ATOMIC_SUM>();
}

// __aicore__ inline void GetStoreAtomicConfig(uint16_t& atomicType, uint16_t& atomicOp);
extern "C" __global__ __aicore__ void KernelTestGetStoreAtomicConfig1()
{
    uint16_t atomicType = 1;
    uint16_t atomicOp = 1;
    AscendC::GetStoreAtomicConfig(atomicType, atomicOp);
}

// __aicore__ inline void CheckLocalMemoryIA(const CheckLocalMemoryIAParam& checkParams);
extern "C" __global__ __aicore__ void KernelTestCheckLocalMemoryIA1()
{
    AscendC::CheckLocalMemoryIAParam checkParams;
    AscendC::CheckLocalMemoryIA(checkParams);
}

#if (__NPU_ARCH__ == 3510)
// template <SpecialPurposeReg spr>
// __aicore__ inline int64_t GetSpr();
extern "C" __global__ __aicore__ void KernelTestGetSpr1()
{
    int64_t result = AscendC::GetSpr<AscendC::SpecialPurposeReg::AR>();
}

// template <SpecialPurposeReg spr>
// __aicore__ inline void ClearSpr();
extern "C" __global__ __aicore__ void KernelTestClearSpr1() { AscendC::ClearSpr<AscendC::SpecialPurposeReg::AR>(); }

// template <int8_t startBit, int8_t endBit>
// __aicore__ static inline void SetCtrlSpr(int64_t value);
extern "C" __global__ __aicore__ void KernelTestSetCtrlSpr1()
{
    int64_t value = 0;
    AscendC::SetCtrlSpr<6, 7>(value);
}

// template <int8_t startBit, int8_t endBit>
// __aicore__ static inline int64_t GetCtrlSpr();
extern "C" __global__ __aicore__ void KernelTestGetCtrlSpr1() { int64_t result = AscendC::GetCtrlSpr<6, 7>(); }

// template <int8_t startBit, int8_t endBit>
// __aicore__ static inline void ResetCtrlSpr();
extern "C" __global__ __aicore__ void KernelTestResetCtrlSpr1() { AscendC::ResetCtrlSpr<6, 7>(); }
#endif
