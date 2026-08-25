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
#include "include/tensor_api/tensor.h"

class Tensor_Api_Mmad_With_Bias : public testing::Test {
protected:
    void SetUp() { AscendC::SetGCoreType(1); }
    void TearDown() { AscendC::SetGCoreType(0); }
};

using namespace AscendC::Te;

#define MMAD_BIAS_ON_BIAS_INTERFACE_TEST(DST_TYPE, SRC_TYPE, BIAS_TYPE, M, N, K)                                       \
    TEST_F(Tensor_Api_Mmad_With_Bias, MmadInterfaceBiasOnBias_##DST_TYPE##_##SRC_TYPE##_##BIAS_TYPE##_##M##_##N##_##K) \
    {                                                                                                                  \
        uint8_t a2Buf[256 * 256 * sizeof(SRC_TYPE)] = {0};                                                             \
        uint8_t b2Buf[256 * 256 * sizeof(SRC_TYPE)] = {0};                                                             \
        uint8_t c2Buf[256 * 256 * sizeof(DST_TYPE)] = {0};                                                             \
        uint8_t biasBuf[256 * 256 * sizeof(BIAS_TYPE)] = {0};                                                          \
                                                                                                                       \
        auto a2Addr = reinterpret_cast<__ca__ SRC_TYPE*>(a2Buf);                                                       \
        auto l0aTensor = MakeTensor(                                                                                   \
            MakeMemPtr<Location::L0A>(a2Addr), MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<SRC_TYPE>>(M, K));      \
                                                                                                                       \
        auto b2Addr = reinterpret_cast<__cb__ SRC_TYPE*>(b2Buf);                                                       \
        auto l0bTensor = MakeTensor(                                                                                   \
            MakeMemPtr<Location::L0B>(b2Addr), MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<SRC_TYPE>>(K, N));      \
                                                                                                                       \
        auto c2Addr = reinterpret_cast<__cc__ DST_TYPE*>(c2Buf);                                                       \
        auto l0cTensor =                                                                                               \
            MakeTensor(MakeMemPtr<Location::L0C>(c2Addr), MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<>>(M, N));   \
                                                                                                                       \
        auto biasAddr = reinterpret_cast<__biasbuf__ BIAS_TYPE*>(biasBuf);                                             \
        auto biasTensor = MakeTensor(                                                                                  \
            MakeMemPtr<Location::BIAS>(biasAddr), MakeFrameLayout<NDLayoutPtn, LayoutTraitDefault<BIAS_TYPE>>(M, N));  \
                                                                                                                       \
        MmadParams para;                                                                                               \
        para.m = M;                                                                                                    \
        para.n = N;                                                                                                    \
        para.k = K;                                                                                                    \
        para.unitFlag = 0;                                                                                             \
        para.cmatrixInitVal = false;                                                                                   \
                                                                                                                       \
        auto mmadAtom = MakeMmad(MmadOperation{}, MmadTraitDefault{}).with(para);                                      \
        Mmad(mmadAtom, l0cTensor, l0aTensor, l0bTensor, biasTensor);                                                   \
        Mmad(mmadAtom, l0cTensor, l0aTensor, l0bTensor, biasTensor);                                                   \
        Mmad(mmadAtom, l0cTensor, l0aTensor, l0bTensor, biasTensor);                                                   \
        EXPECT_EQ(c2Addr[0], static_cast<DST_TYPE>(0));                                                                \
    }

#define MMAD_BIAS_ON_L0C_INTERFACE_TEST(DST_TYPE, SRC_TYPE, BIAS_TYPE, M, N, K)                                        \
    TEST_F(Tensor_Api_Mmad_With_Bias, MmadInterfaceBiasOnL0C_##DST_TYPE##_##SRC_TYPE##_##BIAS_TYPE##_##M##_##N##_##K)  \
    {                                                                                                                  \
        uint8_t a2Buf[256 * 256 * sizeof(SRC_TYPE)] = {0};                                                             \
        uint8_t b2Buf[256 * 256 * sizeof(SRC_TYPE)] = {0};                                                             \
        uint8_t c2Buf[256 * 256 * sizeof(DST_TYPE)] = {0};                                                             \
        uint8_t biasBuf[256 * 256 * sizeof(BIAS_TYPE)] = {0};                                                          \
                                                                                                                       \
        auto a2Addr = reinterpret_cast<__ca__ SRC_TYPE*>(a2Buf);                                                       \
        auto l0aTensor = MakeTensor(                                                                                   \
            MakeMemPtr<Location::L0A>(a2Addr), MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<SRC_TYPE>>(M, K));      \
                                                                                                                       \
        auto b2Addr = reinterpret_cast<__cb__ SRC_TYPE*>(b2Buf);                                                       \
        auto l0bTensor = MakeTensor(                                                                                   \
            MakeMemPtr<Location::L0B>(b2Addr), MakeFrameLayout<ZNLayoutPtn, LayoutTraitDefault<SRC_TYPE>>(K, N));      \
                                                                                                                       \
        auto c2Addr = reinterpret_cast<__cc__ DST_TYPE*>(c2Buf);                                                       \
        auto l0cTensor =                                                                                               \
            MakeTensor(MakeMemPtr<Location::L0C>(c2Addr), MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<>>(M, N));   \
                                                                                                                       \
        auto biasAddr = reinterpret_cast<__cc__ BIAS_TYPE*>(biasBuf);                                                  \
        auto biasTensor =                                                                                              \
            MakeTensor(MakeMemPtr<Location::L0C>(biasAddr), MakeFrameLayout<NZLayoutPtn, LayoutTraitDefault<>>(M, N)); \
                                                                                                                       \
        MmadParams para;                                                                                               \
        para.m = M;                                                                                                    \
        para.n = N;                                                                                                    \
        para.k = K;                                                                                                    \
        para.unitFlag = 0;                                                                                             \
        para.cmatrixInitVal = false;                                                                                   \
                                                                                                                       \
        auto mmadAtom = MakeMmad(MmadOperation{}, MmadTraitDefault{}).with(para);                                      \
        Mmad(mmadAtom, l0cTensor, l0aTensor, l0bTensor, biasTensor);                                                   \
        Mmad(mmadAtom, l0cTensor, l0aTensor, l0bTensor, biasTensor);                                                   \
        EXPECT_EQ(c2Addr[0], static_cast<DST_TYPE>(0));                                                                \
    }

MMAD_BIAS_ON_BIAS_INTERFACE_TEST(float, float, float, 16, 16, 16);
MMAD_BIAS_ON_BIAS_INTERFACE_TEST(float, bfloat16_t, float, 16, 16, 16);
MMAD_BIAS_ON_BIAS_INTERFACE_TEST(float, half, float, 16, 16, 16);
MMAD_BIAS_ON_BIAS_INTERFACE_TEST(int32_t, int8_t, int32_t, 32, 32, 32);

MMAD_BIAS_ON_L0C_INTERFACE_TEST(float, float, float, 16, 16, 16);
MMAD_BIAS_ON_L0C_INTERFACE_TEST(float, bfloat16_t, float, 16, 16, 16);
MMAD_BIAS_ON_L0C_INTERFACE_TEST(float, half, float, 16, 16, 16);
MMAD_BIAS_ON_L0C_INTERFACE_TEST(int32_t, int8_t, int32_t, 32, 32, 32);
