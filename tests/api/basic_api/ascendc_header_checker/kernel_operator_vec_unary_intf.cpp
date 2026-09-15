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
#include "kernel_operator_vec_unary_intf.h"
#endif

// __aicore__ inline void Relu(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestRelu1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Relu(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Relu(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestRelu2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Relu(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Relu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestRelu3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Relu(dst, src, count);
}

// __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestExp1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Exp(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestExp2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Exp(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestExp3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Exp(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestExp4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Exp(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestExp5()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Exp(dst, src, count);
}

// __aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestExp6()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Exp(dst, src, count);
}

// __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestLn1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Ln(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestLn2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Ln(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestLn3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Ln(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestLn4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Ln(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestLn5()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Ln(dst, src, count);
}

// __aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestLn6()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Ln(dst, src, count);
}

// __aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAbs1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Abs(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAbs2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Abs(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestAbs3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Abs(dst, src, count);
}

// __aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestAbs4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Abs(dst, src, count);
}

// __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const
// uint8_t repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestReciprocal1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Reciprocal(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestReciprocal2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Reciprocal(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const
// uint8_t repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestReciprocal3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Reciprocal(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestReciprocal4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Reciprocal(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestReciprocal5()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Reciprocal(dst, src, count);
}

// __aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestReciprocal6()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Reciprocal(dst, src, count);
}

// __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestRsqrt1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Rsqrt(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestRsqrt2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Rsqrt(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestRsqrt3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Rsqrt(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestRsqrt4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Rsqrt(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestRsqrt5()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Rsqrt(dst, src, count);
}

// __aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestRsqrt6()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Rsqrt(dst, src, count);
}

// __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSqrt1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Sqrt(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSqrt2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Sqrt(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSqrt3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Sqrt(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSqrt4()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Sqrt(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestSqrt5()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Sqrt(dst, src, count);
}

// __aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestSqrt6()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    int32_t count = 0;
    AscendC::Sqrt(dst, src, count);
}

// __aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestNot1()
{
    AscendC::LocalTensor<int16_t> dst;
    AscendC::LocalTensor<int16_t> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Not(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestNot2()
{
    AscendC::LocalTensor<int16_t> dst;
    AscendC::LocalTensor<int16_t> src;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Not(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
extern "C" __global__ __aicore__ void KernelTestNot3()
{
    AscendC::LocalTensor<int16_t> dst;
    AscendC::LocalTensor<int16_t> src;
    int32_t count = 0;
    AscendC::Not(dst, src, count);
}

#if (__NPU_ARCH__ == 3510)
// __aicore__ inline void Neg(const LocalTensor<T> &dst, const LocalTensor<T> &src, const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestNeg1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint32_t count = 0;
    AscendC::Neg(dst, src, count);
}
#endif
