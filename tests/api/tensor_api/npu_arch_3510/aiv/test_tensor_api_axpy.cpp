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

class Tensor_Api_Vector_Axpy_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {}

    void TearDown() override {}
};

template <typename T, typename Func, typename S>
__aicore__ inline void TestTransformAxpy(__gm__ T* z, __gm__ T* x, S value, __ubuf__ T zUB[2048], __ubuf__ T xUB[2048])
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
    auto gmPtrZ = MakeMemPtr<Location::GM>(z);

    auto xGm = MakeTensor(gmPtrX, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto zGm = MakeTensor(gmPtrZ, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto zLocal = MakeTensor(MakeMemPtr(zUB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));

    asc_copy_gm2ub_align(
        xLocal.Data().Get(), xGm.Data().Get(), BLK_NUM, burstLength, 0, 0, true, cacheMode, srcStride, dstStride);

    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    Transform<Func>(zLocal, xLocal, value);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    asc_copy_ub2gm_align(zGm.Data().Get(), zLocal.Data().Get(), BLK_NUM, burstLength, cacheMode, srcStride, dstStride);
}

#define VECTOR_AXPY_3510(Function, DataType, ScalarType, ScalarValue)                                      \
    TEST_F(Tensor_Api_Vector_Axpy_3510, Vector_##Function##_##DataType)                                    \
    {                                                                                                      \
        constexpr uint32_t TILE_LENGTH = 2048;                                                             \
                                                                                                           \
        __gm__ DataType x[TILE_LENGTH] = {0};                                                              \
        __gm__ DataType z[TILE_LENGTH] = {0};                                                              \
                                                                                                           \
        __ubuf__ DataType xUB[TILE_LENGTH] = {0};                                                          \
        __ubuf__ DataType zUB[TILE_LENGTH] = {0};                                                          \
                                                                                                           \
        TestTransformAxpy<DataType, AscendC::Te::Inst::Function>(z, x, ScalarType(ScalarValue), zUB, xUB); \
        EXPECT_EQ(z[0], zUB[0]);                                                                           \
    }

VECTOR_AXPY_3510(Axpy, half, half, 2)
VECTOR_AXPY_3510(Axpy, float, float, 2)
