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
#include "kernel_operator_vec_vconv_intf.h"
#endif

// __aicore__ inline void Cast(const LocalTensor<T>& dst, const LocalTensor<U>& src, const RoundMode& roundMode, const
// uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestCast1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::RoundMode roundMode{};
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Cast(dst, src, roundMode, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Cast(const LocalTensor<T>& dst, const LocalTensor<U>& src, const RoundMode& roundMode, const
// uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestCast2()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::RoundMode roundMode{};
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::Cast(dst, src, roundMode, mask, repeatTime, repeatParams);
}

// __aicore__ inline void Cast(const LocalTensor<T>& dst, const LocalTensor<U>& src, const RoundMode& roundMode, const
// uint32_t count);
extern "C" __global__ __aicore__ void KernelTestCast3()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    AscendC::RoundMode roundMode{};
    uint32_t count = 0;
    AscendC::Cast(dst, src, roundMode, count);
}

// __aicore__ inline void CastDeq(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestCastDeq1()
{
    AscendC::LocalTensor<int8_t> dst;
    AscendC::LocalTensor<int16_t> src;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::CastDeq(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void CastDeq(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, uint8_t
// repeatTime, const UnaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestCastDeq2()
{
    AscendC::LocalTensor<int8_t> dst;
    AscendC::LocalTensor<int16_t> src;
    int32_t mask = 0;
    uint8_t repeatTime = 1;
    AscendC::UnaryRepeatParams repeatParams;
    AscendC::CastDeq(dst, src, mask, repeatTime, repeatParams);
}

// __aicore__ inline void CastDeq(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestCastDeq3()
{
    AscendC::LocalTensor<int8_t> dst;
    AscendC::LocalTensor<int16_t> src;
    uint32_t count = 0;
    AscendC::CastDeq(dst, src, count);
}

// __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAddReluCast1()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::AddReluCast(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestAddReluCast2()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::AddReluCast(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestAddReluCast3()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint32_t count = 0;
    AscendC::AddReluCast(dst, src0, src1, count);
}

// __aicore__ inline void SubReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSubReluCast1()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask = 0xFFFFFFFFFFFFFFFF;
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::SubReluCast(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void SubReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams);
extern "C" __global__ __aicore__ void KernelTestSubReluCast2()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint64_t mask[2] = {0};
    uint8_t repeatTime = 1;
    AscendC::BinaryRepeatParams repeatParams;
    AscendC::SubReluCast(dst, src0, src1, mask, repeatTime, repeatParams);
}

// __aicore__ inline void SubReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1,
// const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestSubReluCast3()
{
    AscendC::LocalTensor<half> dst;
    AscendC::LocalTensor<float> src0;
    AscendC::LocalTensor<float> src1;
    uint32_t count = 0;
    AscendC::SubReluCast(dst, src0, src1, count);
}

// __aicore__ inline void SetDeqScale(half scale);
extern "C" __global__ __aicore__ void KernelTestSetDeqScale1()
{
    half scale = 0.0f;
    AscendC::SetDeqScale(scale);
}

// __aicore__ inline void SetDeqScale(float scale, int16_t offset, bool signMode);
extern "C" __global__ __aicore__ void KernelTestSetDeqScale2()
{
    float scale = 0.0f;
    int16_t offset = 0;
    bool signMode = true;
    AscendC::SetDeqScale(scale, offset, signMode);
}

// __aicore__ inline void SetDeqScale(const LocalTensor<T>& vdeq, const VdeqInfo& vdeqInfo);
extern "C" __global__ __aicore__ void KernelTestSetDeqScale3()
{
    AscendC::LocalTensor<uint64_t> vdeq;
    AscendC::VdeqInfo vdeqInfo;
    AscendC::SetDeqScale(vdeq, vdeqInfo);
}

#if (__NPU_ARCH__ == 3510)
// __aicore__ inline void Truncate(const LocalTensor<T> &dst, const LocalTensor<T> &src, const uint32_t count);
extern "C" __global__ __aicore__ void KernelTestTruncate1()
{
    AscendC::LocalTensor<float> dst;
    AscendC::LocalTensor<float> src;
    uint32_t count = 0;
    AscendC::Truncate<float, AscendC::RoundMode::CAST_FLOOR>(dst, src, count);
}
#endif
