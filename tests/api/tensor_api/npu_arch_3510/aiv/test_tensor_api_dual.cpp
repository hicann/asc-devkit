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

class Tensor_Api_Vector_Dual_3510 : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override {}

    void TearDown() override {}
};

template <typename T, typename Func>
__aicore__ inline void TestTransformDual(
    __gm__ T* z0, __gm__ T* z1, __gm__ T* x, __gm__ T* y, __ubuf__ T z0UB[2048], __ubuf__ T z1UB[2048],
    __ubuf__ T xUB[2048], __ubuf__ T yUB[2048])
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
    auto gmPtrZ0 = MakeMemPtr<Location::GM>(z0);
    auto gmPtrZ1 = MakeMemPtr<Location::GM>(z1);

    auto xGm = MakeTensor(gmPtrX, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto yGm = MakeTensor(gmPtrY, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto z0Gm = MakeTensor(gmPtrZ0, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto z1Gm = MakeTensor(gmPtrZ1, MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));

    auto xLocal = MakeTensor(MakeMemPtr(xUB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto yLocal = MakeTensor(MakeMemPtr(yUB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto z0Local = MakeTensor(MakeMemPtr(z0UB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));
    auto z1Local = MakeTensor(MakeMemPtr(z1UB), MakeFrameLayout<NDLayoutPtn>(_1{}, AscendC::Std::Int<TILE_LENGTH>{}));

    asc_copy_gm2ub_align(
        xLocal.Data().Get(), xGm.Data().Get(), BLK_NUM, burstLength, 0, 0, true, cacheMode, srcStride, dstStride);
    asc_copy_gm2ub_align(
        yLocal.Data().Get(), yGm.Data().Get(), BLK_NUM, burstLength, 0, 0, true, cacheMode, srcStride, dstStride);

    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    Transform<Func>(z0Local, z1Local, xLocal, yLocal);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    asc_copy_ub2gm_align(
        z0Gm.Data().Get(), z0Local.Data().Get(), BLK_NUM, burstLength, cacheMode, srcStride, dstStride);
    asc_copy_ub2gm_align(
        z1Gm.Data().Get(), z1Local.Data().Get(), BLK_NUM, burstLength, cacheMode, srcStride, dstStride);
}

#define VECTOR_DUAL_3510(Function, DataType)                                                          \
    TEST_F(Tensor_Api_Vector_Dual_3510, Vector_##Function##_##DataType)                               \
    {                                                                                                 \
        constexpr uint32_t TILE_LENGTH = 2048;                                                        \
                                                                                                      \
        __gm__ DataType x[TILE_LENGTH] = {0};                                                         \
        __gm__ DataType y[TILE_LENGTH] = {0};                                                         \
        __gm__ DataType z0[TILE_LENGTH] = {0};                                                        \
        __gm__ DataType z1[TILE_LENGTH] = {0};                                                        \
                                                                                                      \
        __ubuf__ DataType xUB[TILE_LENGTH] = {0};                                                     \
        __ubuf__ DataType yUB[TILE_LENGTH] = {0};                                                     \
        __ubuf__ DataType z0UB[TILE_LENGTH] = {0};                                                    \
        __ubuf__ DataType z1UB[TILE_LENGTH] = {0};                                                    \
                                                                                                      \
        TestTransformDual<DataType, AscendC::Te::Inst::Function>(z0, z1, x, y, z0UB, z1UB, xUB, yUB); \
        EXPECT_EQ(z0[0], z0UB[0]);                                                                    \
        EXPECT_EQ(z1[0], z1UB[0]);                                                                    \
    }

VECTOR_DUAL_3510(Mull, int32_t)
VECTOR_DUAL_3510(Mull, uint32_t)
