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
#include "kernel_operator_scalar_intf.h"
#endif

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
// __aicore__ inline void GetUintDivMagicAndShift(T& magic, T& shift, T divisor);
extern "C" __global__ __aicore__ void KernelTestGetUintDivMagicAndShift1()
{
    uint32_t magic = 0;
    uint32_t shift = 0;
    uint32_t divisor = 0;
    AscendC::GetUintDivMagicAndShift(magic, shift, divisor);
}
#endif

#if __NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
// __aicore__ inline void WriteGmBypassDCache(__gm__ T* addr, T value);
extern "C" __global__ __aicore__ void KernelTestWriteGmBypassDCache1()
{
    __gm__ int32_t* addr;
    int32_t value = 0;
    AscendC::WriteGmBypassDCache(addr, value);
}
#endif
