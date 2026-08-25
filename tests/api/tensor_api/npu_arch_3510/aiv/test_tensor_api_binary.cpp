/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include <gtest/gtest.h>
#include "c_api/stub/cce_stub.h"
#include "tensor_api/tensor.h"

class Tensor_Api_Vector_Binary_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {}

    void TearDown() override {}
};

template <typename T, typename Func>
__aicore__ inline void TestTransformBinary(
    __gm__ T* z, __gm__ T* x, __gm__ T* y, __ubuf__ T zUB[2048], __ubuf__ T xUB[2048], __ubuf__ T yUB[2048])
{
    constexpr uint32_t TILE_LENGTH = 2048;
    constexpr uint32_t BLK_NUM = 1;

    using namespace AscendC::Te;
    asc_init();

    constexpr uint8_t cacheMode = 0;
    constexpr uint32_t burstLength = TILE_LENGTH * sizeof(T);
    constexpr uint64_t srcStride = 0;
    constexpr uint32_t dstStride = 0;

    auto gmPtrX = MakeMemPtr<Location::GM>(x);
    auto gmPtrY = MakeMemPtr<Location::GM>(y);
    auto gmPtrZ = MakeMemPtr<Location::GM>(z);

    auto xGm = MakeTensor(gmPtrX, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto yGm = MakeTensor(gmPtrY, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto zGm = MakeTensor(gmPtrZ, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto yLocal = MakeTensor(MakeMemPtr(yUB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto zLocal = MakeTensor(MakeMemPtr(zUB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));

    asc_copy_gm2ub_align(
        xLocal.Data().Get(), xGm.Data().Get(), BLK_NUM, burstLength, 0, 0, true, cacheMode, srcStride, dstStride);
    asc_copy_gm2ub_align(
        yLocal.Data().Get(), yGm.Data().Get(), BLK_NUM, burstLength, 0, 0, true, cacheMode, srcStride, dstStride);

    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    Transform<Func>(zLocal, xLocal, yLocal);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    asc_copy_ub2gm_align(zGm.Data().Get(), zLocal.Data().Get(), BLK_NUM, burstLength, cacheMode, srcStride, dstStride);
}

#define VECTOR_BINARY_3510(Function, DataType)                                              \
    TEST_F(Tensor_Api_Vector_Binary_3510, Vector_##Function##_##DataType)                   \
    {                                                                                       \
        constexpr uint32_t TILE_LENGTH = 2048;                                              \
                                                                                            \
        __gm__ DataType x[TILE_LENGTH] = {0};                                               \
        __gm__ DataType y[TILE_LENGTH] = {0};                                               \
        __gm__ DataType z[TILE_LENGTH] = {0};                                               \
                                                                                            \
        __ubuf__ DataType xUB[TILE_LENGTH] = {0};                                           \
        __ubuf__ DataType yUB[TILE_LENGTH] = {0};                                           \
        __ubuf__ DataType zUB[TILE_LENGTH] = {0};                                           \
                                                                                            \
        TestTransformBinary<DataType, AscendC::Te::Inst::Function>(z, x, y, zUB, xUB, yUB); \
        EXPECT_EQ(z[0], zUB[0]);                                                            \
    }

VECTOR_BINARY_3510(Add, uint8_t)
VECTOR_BINARY_3510(Add, int8_t)
VECTOR_BINARY_3510(Add, uint16_t)
VECTOR_BINARY_3510(Add, int16_t)
VECTOR_BINARY_3510(Add, half)
VECTOR_BINARY_3510(Add, int32_t)
VECTOR_BINARY_3510(Add, uint32_t)
VECTOR_BINARY_3510(Add, bfloat16_t)

VECTOR_BINARY_3510(Sub, uint8_t)
VECTOR_BINARY_3510(Sub, int8_t)
VECTOR_BINARY_3510(Sub, uint16_t)
VECTOR_BINARY_3510(Sub, int16_t)
VECTOR_BINARY_3510(Sub, half)
VECTOR_BINARY_3510(Sub, int32_t)
VECTOR_BINARY_3510(Sub, uint32_t)
VECTOR_BINARY_3510(Sub, bfloat16_t)

VECTOR_BINARY_3510(Madd, half)
VECTOR_BINARY_3510(Madd, float)

VECTOR_BINARY_3510(And, uint8_t)
VECTOR_BINARY_3510(And, int8_t)
VECTOR_BINARY_3510(And, uint16_t)
VECTOR_BINARY_3510(And, int16_t)
VECTOR_BINARY_3510(And, half)
VECTOR_BINARY_3510(And, int32_t)
VECTOR_BINARY_3510(And, uint32_t)
VECTOR_BINARY_3510(And, bfloat16_t)
VECTOR_BINARY_3510(And, fp8_e5m2_t)
VECTOR_BINARY_3510(And, fp8_e4m3fn_t)

VECTOR_BINARY_3510(Select, uint8_t)
VECTOR_BINARY_3510(Select, int8_t)
VECTOR_BINARY_3510(Select, uint16_t)
VECTOR_BINARY_3510(Select, int16_t)
VECTOR_BINARY_3510(Select, half)
VECTOR_BINARY_3510(Select, int32_t)
VECTOR_BINARY_3510(Select, uint32_t)
VECTOR_BINARY_3510(Select, bfloat16_t)
VECTOR_BINARY_3510(Select, fp8_e5m2_t)
VECTOR_BINARY_3510(Select, fp8_e4m3fn_t)
VECTOR_BINARY_3510(Select, hifloat8_t)

VECTOR_BINARY_3510(AbsSub, half)
VECTOR_BINARY_3510(AbsSub, float)

VECTOR_BINARY_3510(Min, uint8_t)
VECTOR_BINARY_3510(Min, int8_t)
VECTOR_BINARY_3510(Min, uint16_t)
VECTOR_BINARY_3510(Min, int16_t)
VECTOR_BINARY_3510(Min, half)
VECTOR_BINARY_3510(Min, float)
VECTOR_BINARY_3510(Min, int32_t)
VECTOR_BINARY_3510(Min, uint32_t)
VECTOR_BINARY_3510(Min, bfloat16_t)

VECTOR_BINARY_3510(Max, uint8_t)
VECTOR_BINARY_3510(Max, int8_t)
VECTOR_BINARY_3510(Max, uint16_t)
VECTOR_BINARY_3510(Max, int16_t)
VECTOR_BINARY_3510(Max, int32_t)
VECTOR_BINARY_3510(Max, uint32_t)
VECTOR_BINARY_3510(Max, half)
VECTOR_BINARY_3510(Max, float)
VECTOR_BINARY_3510(Max, bfloat16_t)

VECTOR_BINARY_3510(Or, uint8_t)
VECTOR_BINARY_3510(Or, int8_t)
VECTOR_BINARY_3510(Or, uint16_t)
VECTOR_BINARY_3510(Or, int16_t)
VECTOR_BINARY_3510(Or, int32_t)
VECTOR_BINARY_3510(Or, uint32_t)
VECTOR_BINARY_3510(Or, half)
VECTOR_BINARY_3510(Or, float)

VECTOR_BINARY_3510(Mul, uint16_t)
VECTOR_BINARY_3510(Mul, int16_t)
VECTOR_BINARY_3510(Mul, int32_t)
VECTOR_BINARY_3510(Mul, uint32_t)
VECTOR_BINARY_3510(Mul, half)
VECTOR_BINARY_3510(Mul, float)
VECTOR_BINARY_3510(Mul, bfloat16_t)

VECTOR_BINARY_3510(ExpSubEven, float)
VECTOR_BINARY_3510(ExpSubOdd, float)

VECTOR_BINARY_3510(ShiftLeft, int8_t)
VECTOR_BINARY_3510(ShiftLeft, int16_t)
VECTOR_BINARY_3510(ShiftLeft, int32_t)

VECTOR_BINARY_3510(ShiftRight, int8_t)
VECTOR_BINARY_3510(ShiftRight, int16_t)
VECTOR_BINARY_3510(ShiftRight, int32_t)

VECTOR_BINARY_3510(Div, half)
VECTOR_BINARY_3510(Div, float)

VECTOR_BINARY_3510(Xor, uint8_t)
VECTOR_BINARY_3510(Xor, int8_t)
VECTOR_BINARY_3510(Xor, uint16_t)
VECTOR_BINARY_3510(Xor, int16_t)
VECTOR_BINARY_3510(Xor, int32_t)
VECTOR_BINARY_3510(Xor, uint32_t)

VECTOR_BINARY_3510(Prelu, half)
VECTOR_BINARY_3510(Prelu, float)
