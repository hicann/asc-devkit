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
#include "kernel_operator_vec_binary_intf.h"
#endif

// __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAdd1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Add(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAdd2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Add(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const
// int32_t& count);
extern "C" __global__ __aicore__ void KernelTestAdd3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::Add(dst, src0, src1, count);
}

// __aicore__ inline void Sub(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSub1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Sub(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Sub(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSub2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Sub(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Sub(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const
// int32_t& count);
extern "C" __global__ __aicore__ void KernelTestSub3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::Sub(dst, src0, src1, count);
}

// __aicore__ inline void Mul(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestMul1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Mul(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Mul(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestMul2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Mul(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Mul(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const
// int32_t& count);
extern "C" __global__ __aicore__ void KernelTestMul3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::Mul(dst, src0, src1, count);
}

// __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestDiv1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Div(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestDiv2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Div(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestDiv3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Div(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestDiv4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Div(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const
// int32_t& count);
extern "C" __global__ __aicore__ void KernelTestDiv5()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::Div(dst, src0, src1, count);
}

// __aicore__ inline void Div(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const
// int32_t& count);
extern "C" __global__ __aicore__ void KernelTestDiv6()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::Div(dst, src0, src1, count);
}

// __aicore__ inline void MulAddDst(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// const uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestMulAddDst1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::MulAddDst(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void MulAddDst(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestMulAddDst2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::MulAddDst(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void MulAddDst(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestMulAddDst3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::MulAddDst(dst, src0, src1, count);
}

// __aicore__ inline void Max(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestMax1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Max(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Max(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestMax2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Max(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Max(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const
// int32_t& count);
extern "C" __global__ __aicore__ void KernelTestMax3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::Max(dst, src0, src1, count);
}

// __aicore__ inline void Min(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestMin1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Min(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Min(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestMin2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Min(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Min(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const
// int32_t& count);
extern "C" __global__ __aicore__ void KernelTestMin3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::Min(dst, src0, src1, count);
}

// __aicore__ inline void And(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAnd1()
{
    AscendC::LocalTensor<uint16_t> dst;
    AscendC::LocalTensor<uint16_t> src0;
    AscendC::LocalTensor<uint16_t> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::And(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void And(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAnd2()
{
    AscendC::LocalTensor<uint16_t> dst;
    AscendC::LocalTensor<uint16_t> src0;
    AscendC::LocalTensor<uint16_t> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::And(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void And(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const
// int32_t& count);
extern "C" __global__ __aicore__ void KernelTestAnd3()
{
    AscendC::LocalTensor<uint16_t> dst;
    AscendC::LocalTensor<uint16_t> src0;
    AscendC::LocalTensor<uint16_t> src1;
    int32_t count = 0;
    AscendC::And(dst, src0, src1, count);
}

// __aicore__ inline void Or(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t
// mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestOr1()
{
    AscendC::LocalTensor<uint16_t> dst;
    AscendC::LocalTensor<uint16_t> src0;
    AscendC::LocalTensor<uint16_t> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Or(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Or(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t
// mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestOr2()
{
    AscendC::LocalTensor<uint16_t> dst;
    AscendC::LocalTensor<uint16_t> src0;
    AscendC::LocalTensor<uint16_t> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::Or(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Or(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const
// int32_t& count);
extern "C" __global__ __aicore__ void KernelTestOr3()
{
    AscendC::LocalTensor<uint16_t> dst;
    AscendC::LocalTensor<uint16_t> src0;
    AscendC::LocalTensor<uint16_t> src1;
    int32_t count = 0;
    AscendC::Or(dst, src0, src1, count);
}

#if (__NPU_ARCH__ == 3510)
// __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1,
// const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestShiftLeft1()
{
    AscendC::LocalTensor<int32_t> dst;
    AscendC::LocalTensor<int32_t> src0;
    AscendC::LocalTensor<int32_t> src1;
    int32_t count = 0;
    AscendC::ShiftLeft(dst, src0, src1, count);
}

// __aicore__ inline void ShiftRight(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1,
// const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestShiftRight1()
{
    AscendC::LocalTensor<int32_t> dst;
    AscendC::LocalTensor<int32_t> src0;
    AscendC::LocalTensor<int32_t> src1;
    int32_t count = 0;
    AscendC::ShiftRight(dst, src0, src1, count);
}
#endif

// __aicore__ inline void AddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAddRelu1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::AddRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void AddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAddRelu2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::AddRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

#if (__NPU_ARCH__ != 1001) && (__NPU_ARCH__ != 3102)
// __aicore__ inline void AddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestAddRelu3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::AddRelu(dst, src0, src1, count);
}
#endif

// __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const
// LocalTensor<int32_t>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAddDeqRelu1()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<int32_t> src0;
    AscendC::LocalTensor<int32_t> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::AddDeqRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAddDeqRelu2()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<int32_t> src0;
    AscendC::LocalTensor<int32_t> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::AddDeqRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const
// LocalTensor<int32_t>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAddDeqRelu3()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<int32_t> src0;
    AscendC::LocalTensor<int32_t> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::AddDeqRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAddDeqRelu4()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<int32_t> src0;
    AscendC::LocalTensor<int32_t> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::AddDeqRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const
// LocalTensor<int32_t>& src1, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestAddDeqRelu5()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<int32_t> src0;
    AscendC::LocalTensor<int32_t> src1;
    int32_t count = 0;
    AscendC::AddDeqRelu(dst, src0, src1, count);
}

// __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestAddDeqRelu6()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<int32_t> src0;
    AscendC::LocalTensor<int32_t> src1;
    int32_t count = 0;
    AscendC::AddDeqRelu(dst, src0, src1, count);
}

// __aicore__ inline void FusedMulAdd(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestFusedMulAdd1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::FusedMulAdd(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void FusedMulAdd(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestFusedMulAdd2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::FusedMulAdd(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void FusedMulAdd(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestFusedMulAdd3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::FusedMulAdd(dst, src0, src1, count);
}

// __aicore__ inline void FusedMulAddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>&
// src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestFusedMulAddRelu1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::FusedMulAddRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void FusedMulAddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>&
// src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestFusedMulAddRelu2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::FusedMulAddRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void FusedMulAddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>&
// src1, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestFusedMulAddRelu3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::FusedMulAddRelu(dst, src0, src1, count);
}

// __aicore__ inline void SubRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSubRelu1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::SubRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void SubRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSubRelu2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::SubRelu(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void SubRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
// const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestSubRelu3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    int32_t count = 0;
    AscendC::SubRelu(dst, src0, src1, count);
}

#if (__NPU_ARCH__ == 3510)
// __aicore__ inline void Prelu(const LocalTensor<T>& dst, const LocalTensor<T> &src0,  const LocalTensor<T> &src1,
// const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestPrelu1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint32_t count = 0;
    AscendC::Prelu(dst, src0, src1, count);
}

// __aicore__ inline void Mull(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const
// LocalTensor<T>& src1, const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestMull1()
{
    AscendC::LocalTensor<int32_t> dst0;
    AscendC::LocalTensor<int32_t> dst1;
    AscendC::LocalTensor<int32_t> src0;
    AscendC::LocalTensor<int32_t> src1;
    uint32_t count = 0;
    AscendC::Mull(dst0, dst1, src0, src1, count);
}

// __aicore__ inline void FusedAbsSub(const LocalTensor<T> &dst, const LocalTensor<T> &src0,  const LocalTensor<T>
// &src1, const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestFusedAbsSub1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint32_t count = 0;
    AscendC::FusedAbsSub(dst, src0, src1, count);
}

// __aicore__ inline void FusedExpSub(const LocalTensor<T> &dst, const LocalTensor<U> &src0,  const LocalTensor<U>
// &src1, const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestFusedExpSub1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint32_t count = 0;
    AscendC::FusedExpSub(dst, src0, src1, count);
}

#endif
