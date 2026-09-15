/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if __NPU_ARCH__ != 3102
#ifndef VERIFY_SINGLE_HEADER
#include "kernel_operator.h"
#else
#include "kernel_operator_dump_tensor_intf.h"
#endif
// __aicore__ inline void DumpTensor(const LocalTensor<T> &tensor, uint32_t desc, uint32_t dumpSize);
extern "C" __global__ __aicore__ void KernelTestDumpTensor1()
{
    AscendC::LocalTensor<float> tensor;
    uint32_t desc = 0;
    uint32_t dumpSize = 0;
    AscendC::DumpTensor(tensor, desc, dumpSize);
}

// __aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize);
extern "C" __global__ __aicore__ void KernelTestDumpTensor2()
{
    AscendC::GlobalTensor<float> tensor;
    uint32_t desc = 0;
    uint32_t dumpSize = 0;
    AscendC::DumpTensor(tensor, desc, dumpSize);
}

// __aicore__ inline void DumpTensor(const LocalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize, const ShapeInfo&
// shapeInfo);
extern "C" __global__ __aicore__ void KernelTestDumpTensor3()
{
    AscendC::LocalTensor<float> tensor;
    uint32_t desc = 0;
    uint32_t dumpSize = 0;
    AscendC::ShapeInfo shapeInfo;
    AscendC::DumpTensor(tensor, desc, dumpSize, shapeInfo);
}

// __aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize, const ShapeInfo&
// shapeInfo);
extern "C" __global__ __aicore__ void KernelTestDumpTensor4()
{
    AscendC::GlobalTensor<float> tensor;
    uint32_t desc = 0;
    uint32_t dumpSize = 0;
    AscendC::ShapeInfo shapeInfo;
    AscendC::DumpTensor(tensor, desc, dumpSize, shapeInfo);
}

// __aicore__ inline void DumpAccChkPoint(const LocalTensor<T> &tensor, uint32_t index, uint32_t countOff, uint32_t
// dumpSize);
extern "C" __global__ __aicore__ void KernelTestDumpAccChkPoint1()
{
    AscendC::LocalTensor<float> tensor;
    uint32_t index = 0;
    uint32_t countOff = 0;
    uint32_t dumpSize = 0;
    AscendC::DumpAccChkPoint(tensor, index, countOff, dumpSize);
}

// __aicore__ inline void DumpAccChkPoint(const GlobalTensor<T> &tensor, uint32_t index, uint32_t countOff, uint32_t
// dumpSize);
extern "C" __global__ __aicore__ void KernelTestDumpAccChkPoint2()
{
    AscendC::GlobalTensor<float> tensor;
    uint32_t index = 0;
    uint32_t countOff = 0;
    uint32_t dumpSize = 0;
    AscendC::DumpAccChkPoint(tensor, index, countOff, dumpSize);
}
#endif

// // __aicore__ inline void PRINTF(__gm__ const char* fmt, Args&&... args);
// extern "C" __global__ __aicore__ void KernelTestPRINTF1() {
//     AscendC::PRINTF();
// }

// // __aicore__ inline void printf(__gm__ const char* fmt, Args&&... args);
// extern "C" __global__ __aicore__ void KernelTestprintf1() {
//     AscendC::printf();
// }
