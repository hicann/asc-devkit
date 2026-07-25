/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "kernel_operator.h"

using namespace AscendC;

extern "C" uint64_t GetDlogErrorCountForTest();

namespace {
/******************************** 双目指令 ********************************/
template <typename T>
using ScatterOpMaskBitPtr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint32_t>& dstOffsetLocal,
    const uint32_t dstBaseAddr, const uint64_t mask, const uint8_t repeatTimes, const uint8_t srcRepStride);

template <typename T, ScatterOpMaskBitPtr<T> func>
void ScatterOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];
    int32_t* data2 = new int32_t[128 * 1024];

    TBuffAddr addr0, addr1, addr2;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = (uint8_t*)data0;

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 32 * 1024;
    addr1.bufferAddr = 32 * 1024;
    addr1.absAddr = (uint8_t*)data1;

    addr2.logicPos = static_cast<uint8_t>(tensorPos[2]);
    addr2.bufferHandle = nullptr;
    addr2.dataLen = 64 * 1024;
    addr2.bufferAddr = 0;
    addr2.absAddr = (uint8_t*)data2;
    LocalTensor<T> outputLocal(addr0);
    LocalTensor<T> input0Local(addr1);
    LocalTensor<uint32_t> input1Local(addr2);

    func(outputLocal, input0Local, input1Local, 0, 64, 1, 8);

    delete[] data0;
    delete[] data1;
    delete[] data2;
}

template <typename T>
using ScatterOpMaskCountPtr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint32_t>& dstOffsetLocal,
    const uint32_t dstBaseAddr, const uint64_t mask[], const uint8_t repeatTimes, const uint8_t srcRepStride);

template <typename T, ScatterOpMaskCountPtr<T> func>
void ScatterOp2Test(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];
    int32_t* data2 = new int32_t[128 * 1024];

    TBuffAddr addr0, addr1, addr2;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = (uint8_t*)data0;

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 32 * 1024;
    addr1.bufferAddr = 32 * 1024;
    addr1.absAddr = (uint8_t*)data1;

    addr2.logicPos = static_cast<uint8_t>(tensorPos[2]);
    addr2.bufferHandle = nullptr;
    addr2.dataLen = 64 * 1024;
    addr2.bufferAddr = 0;
    addr2.absAddr = (uint8_t*)data2;
    LocalTensor<T> outputLocal(addr0);
    LocalTensor<T> input0Local(addr1);
    LocalTensor<uint32_t> input1Local(addr2);

    uint64_t mask[2] = {64, 0};
    func(outputLocal, input0Local, input1Local, 0, mask, 1, 8);

    delete[] data0;
    delete[] data1;
    delete[] data2;
}

int32_t RaiseStub(int32_t i) { return 0; }
} // namespace

class TensorScopeTest : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() {}
};

/******************************** 双目指令 ********************************/
struct OpTestParams {
    void (*func)(std::vector<TPosition>&);
    std::string funcName;
    std::vector<TPosition> tensorPos;
    std::string illegalTensorPos;
    std::string illegalTensorPosName;
    std::string supportPos;
};

class VectorOpTestsuite : public testing::Test, public testing::WithParamInterface<OpTestParams> {
protected:
    void SetUp() {}
    void TearDown() { GlobalMockObject::verify(); }
};

INSTANTIATE_TEST_CASE_P(
    TEST_VECTOR_OP, VectorOpTestsuite,
    ::testing::Values(
        //*********************************************************** 双目指令(14条)
        //***********************************************************
        // Add
        OpTestParams{
            ScatterOpTest<float, Scatter>,
            "Scatter",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "dstOffset",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        OpTestParams{
            ScatterOpTest<float, Scatter>,
            "Scatter",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        OpTestParams{
            ScatterOpTest<float, Scatter>,
            "Scatter",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        OpTestParams{
            ScatterOp2Test<float, Scatter>,
            "Scatter",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "dstOffset",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        OpTestParams{
            ScatterOp2Test<float, Scatter>,
            "Scatter",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        OpTestParams{
            ScatterOp2Test<float, Scatter>,
            "Scatter",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"}));

TEST_P(VectorOpTestsuite, VectorOpTestCase)
{
    MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStub));
    auto param = GetParam();
    SCOPED_TRACE(param.funcName + "." + param.illegalTensorPos);
    const uint64_t startCount = GetDlogErrorCountForTest();
    param.func(param.tensorPos);
    const uint64_t dlogErrorCount = GetDlogErrorCountForTest() - startCount;
    EXPECT_EQ(dlogErrorCount, 1);
}
