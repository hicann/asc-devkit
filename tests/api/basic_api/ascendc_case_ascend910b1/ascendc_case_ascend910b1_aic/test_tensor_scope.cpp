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
#include <atomic>
#include <fstream>
#include <iostream>
#include "kernel_operator.h"

using namespace AscendC;

namespace {
std::atomic<uint64_t> g_raiseCount{0};

/******************************** Fixpipe ********************************/
template <typename DstT, typename SrcT, const FixpipeConfig& config>
using FixpipeOpPtr = void (*)(
    const GlobalTensor<DstT>& dstGlobal, const LocalTensor<SrcT>& srcLocal, const FixpipeParamsV220& intriParams);

template <typename DstT, typename SrcT, FixpipeOpPtr<DstT, SrcT, CFG_ROW_MAJOR> func>
void FixpipeOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = (uint8_t*)data0;

    LocalTensor<SrcT> srcLocal(addr0);
    GlobalTensor<DstT> dstGlobal;
    dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT*>(data1), dataSize);

    FixpipeParamsV220 fixpipeParams;
    fixpipeParams.nSize = 16;
    fixpipeParams.mSize = 16;
    fixpipeParams.srcStride = 1;
    fixpipeParams.dstStride = 1;
    fixpipeParams.quantPre = QuantMode_t::F322F16;

    fixpipeParams.ndNum = 1;
    fixpipeParams.srcNdStride = 0;
    fixpipeParams.dstNdStride = 0;
    func(dstGlobal, srcLocal, fixpipeParams);

    delete[] data0;
    delete[] data1;
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
using FixpipeL2LPtr = void (*)(
    const LocalTensor<DstT>& dstLocal, const LocalTensor<SrcT>& srcLocal, const FixpipeParamsV220& intriParams);

template <typename DstT, typename SrcT, FixpipeL2LPtr<DstT, SrcT, CFG_NZ> func>
void FixpipeOpTest2(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TPipe pipe;
    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 64 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]));

    LocalTensor<DstT> dstLocal(addr0);
    LocalTensor<SrcT> srcLocal(addr1);

    FixpipeParamsV220 fixpipeParams;
    fixpipeParams.nSize = 16;
    fixpipeParams.mSize = 16;
    fixpipeParams.srcStride = 1;
    fixpipeParams.dstStride = 1;
    fixpipeParams.quantPre = QuantMode_t::F322F16;
    fixpipeParams.ndNum = 1;
    fixpipeParams.srcNdStride = 0;
    fixpipeParams.dstNdStride = 0;
    func(dstLocal, srcLocal, fixpipeParams);

    delete[] data0;
    delete[] data1;
}

void FixpipeChannelSplitDstTypeTest()
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(TPosition::CO1);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(TPosition::CO1));

    LocalTensor<float> srcLocal(addr0);
    GlobalTensor<half> dstGlobal;
    dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ half*>(data1), dataSize);

    FixpipeParamsV220 fixpipeParams;
    fixpipeParams.nSize = 16;
    fixpipeParams.mSize = 16;
    fixpipeParams.srcStride = 16;
    fixpipeParams.dstStride = 16;
    fixpipeParams.ndNum = 1;
    fixpipeParams.srcNdStride = 0;
    fixpipeParams.dstNdStride = 0;
    fixpipeParams.isChannelSplit = true;

    Fixpipe<half, float>(dstGlobal, srcLocal, fixpipeParams);

    delete[] data0;
    delete[] data1;
}

/******************************** LoadData ********************************/
template <typename T>
using LodaData2dOpPtr =
    void (*)(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LoadData2DParams& loadDataParams);

template <typename T, LodaData2dOpPtr<T> func>
void LoadData2dOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TPipe pipe;
    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 64 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]));

    LocalTensor<T> dstLocal(addr0);
    LocalTensor<T> srcLocal(addr1);

    AscendC::LoadData2DParams loadDataParams;
    loadDataParams.repeatTimes = 1;
    loadDataParams.dstGap = 0;
    loadDataParams.srcStride = 1;
    loadDataParams.ifTranspose = false;
    int dstOffset = 512;
    int srcOffset = 512;
    func(dstLocal, srcLocal, loadDataParams);

    delete[] data0;
    delete[] data1;
}

template <typename T>
using LodaData2dG2LOpPtr =
    void (*)(const LocalTensor<T>& dstLocal, const GlobalTensor<T>& srcLocal, const LoadData2DParams& loadDataParams);

template <typename T, LodaData2dG2LOpPtr<T> func>
void LoadData2dG2LOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128 * 1024;
    int32_t* data0 = new int32_t[dataSize];

    TPipe pipe;
    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    LocalTensor<T> dstLocal(addr0);
    GlobalTensor<T> srcGlobal;
    srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data0), dataSize);

    AscendC::LoadData2DParams loadDataParams;
    loadDataParams.repeatTimes = 1;
    loadDataParams.dstGap = 0;
    loadDataParams.srcStride = 1;
    loadDataParams.ifTranspose = false;
    int dstOffset = 512;
    int srcOffset = 512;
    func(dstLocal, srcGlobal, loadDataParams);

    delete[] data0;
}

/******************************** LoadData2DV2 ********************************/
template <typename T>
using LodaData2dv2L2LOpPtr =
    void (*)(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LoadData2DParamsV2& loadDataParams);

template <typename T, LodaData2dv2L2LOpPtr<T> func>
void LoadData2dv2L2LOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TPipe pipe;
    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 64 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]));

    LocalTensor<T> dstLocal(addr0);
    LocalTensor<T> srcLocal(addr1);
    LoadData2DParamsV2 params(0, 0, 1, 1, 0, 0, false, 0);
    func(dstLocal, srcLocal, params);

    delete[] data0;
    delete[] data1;
}

template <typename T>
using LodaData2dv2G2LOpPtr =
    void (*)(const LocalTensor<T>& dstLocal, const GlobalTensor<T>& srcLocal, const LoadData2DParamsV2& loadDataParams);

template <typename T, LodaData2dv2G2LOpPtr<T> func>
void LoadData2dv2G2LOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128 * 1024;
    int32_t* data0 = new int32_t[dataSize];

    TPipe pipe;
    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    LocalTensor<T> dstLocal(addr0);
    GlobalTensor<T> srcGlobal;
    srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data0), dataSize);

    LoadData2DParamsV2 params(0, 0, 1, 1, 0, 0, false, 0);
    func(dstLocal, srcGlobal, params);

    delete[] data0;
}

/******************************** Mmad ********************************/
template <typename DstT, typename Src0T, typename Src1T>
using MmadOpPtr = void (*)(
    const LocalTensor<DstT>& dstLocal, const LocalTensor<Src0T>& fmLocal, const LocalTensor<Src1T>& filterLocal,
    const MmadParams& mmadParams);

template <typename DstT, typename Src0T, typename Src1T, MmadOpPtr<DstT, Src0T, Src1T> func>
void MmadOpTest(std::vector<TPosition>& tensorPos)
{
    TPipe pipe;
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];
    int32_t* data2 = new int32_t[128 * 1024];

    TBuffAddr addr0, addr1, addr2;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));
    ;

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 32 * 1024;
    addr1.bufferAddr = 32 * 1024;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]));
    ;

    addr2.logicPos = static_cast<uint8_t>(tensorPos[2]);
    addr2.bufferHandle = nullptr;
    addr2.dataLen = 64 * 1024;
    addr2.bufferAddr = 0;
    addr2.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[2]));
    ;
    LocalTensor<DstT> outputLocal(addr0);
    LocalTensor<Src0T> input0Local(addr1);
    LocalTensor<Src1T> input1Local(addr2);

    MmadParams mmadParams;
    mmadParams.m = 16;
    mmadParams.n = 16;
    mmadParams.k = 16;
    mmadParams.cmatrixSource = 0; // bias in L0C
    mmadParams.cmatrixInitVal = false;
    func(outputLocal, input0Local, input1Local, mmadParams);

    delete[] data0;
    delete[] data1;
    delete[] data2;
}

int32_t RaiseStub(int32_t i)
{
    g_raiseCount++;
    return 0;
}
} // namespace

class TensorScopeTest : public testing::Test {
protected:
    void SetUp() {}
    void TearDown() { AscendC::CheckSyncState(); }
};

/******************************** 双目指令 ********************************/
struct CubeOpTestParams {
    void (*func)(std::vector<TPosition>&);
    std::string funcName;
    std::vector<TPosition> tensorPos;
    std::string illegalTensorPos;
    std::string illegalTensorPosName;
    std::string supportPos;
};

class CubeOpTestsuite : public testing::Test, public testing::WithParamInterface<CubeOpTestParams> {
protected:
    void SetUp() {}
    void TearDown()
    {
        AscendC::CheckSyncState();
        GlobalMockObject::verify();
    }
};

INSTANTIATE_TEST_CASE_P(
    TEST_CUBE_OP, CubeOpTestsuite,
    ::testing::Values(
        //*********************************************************** 双目指令(14条)
        //***********************************************************
        // Fixpipe
        CubeOpTestParams{
            FixpipeOpTest<half, float, Fixpipe<half, float>>,
            "Fixpipe",
            {TPosition::VECIN},
            "src",
            "UB(VECIN)",
            "L0C Buffer(CO1)"},
        CubeOpTestParams{
            FixpipeOpTest2<half, float, Fixpipe<half, float, CFG_NZ>>,
            "Fixpipe",
            {TPosition::C1, TPosition::VECIN},
            "src",
            "UB(VECIN)",
            "L0C Buffer(CO1)"},
        // Fixpipe dst物理位置为L1
        CubeOpTestParams{
            FixpipeOpTest2<half, float, Fixpipe<half, float, CFG_NZ>>,
            "Fixpipe",
            {TPosition::CO2, TPosition::CO1},
            "dst",
            "GM(CO2)",
            "L1 Buffer(C1)"},
        // LoadData
        CubeOpTestParams{
            LoadData2dOpTest<half, LoadData>,
            "LoadData",
            {TPosition::A2, TPosition::CO1},
            "src",
            "L0C Buffer(CO1)",
            "L1 Buffer(A1/B1)/GM"},
        CubeOpTestParams{
            LoadData2dOpTest<half, LoadData>,
            "LoadData",
            {TPosition::CO2, TPosition::A1},
            "dst",
            "GM(CO2)",
            "L0A Buffer(A2)/L0B Buffer(B2)"},
        CubeOpTestParams{
            LoadData2dG2LOpTest<half, LoadData>,
            "LoadData",
            {TPosition::CO2},
            "dst",
            "GM(CO2)",
            "L1 Buffer(A1/B1)/L0A Buffer(A2)/L0B Buffer(B2)"},
        // LodaData2DV2
        CubeOpTestParams{
            LoadData2dv2L2LOpTest<half, LoadData>,
            "LoadData with LoadData2DParamsV2",
            {TPosition::CO2, TPosition::A1},
            "dst",
            "GM(CO2)",
            "L0A Buffer(A2)/L0B Buffer(B2)"},
        CubeOpTestParams{
            LoadData2dv2L2LOpTest<half, LoadData>,
            "LoadData with LoadData2DParamsV2",
            {TPosition::A2, TPosition::CO1},
            "src",
            "L0C Buffer(CO1)",
            "L1 Buffer(A1/B1)"},
        CubeOpTestParams{
            LoadData2dv2G2LOpTest<half, LoadData>,
            "LoadData with LoadData2DParamsV2",
            {TPosition::CO2},
            "dst",
            "GM(CO2)",
            "L1 Buffer(A1/B1)/L0A Buffer(A2)/L0B Buffer(B2)"},
        // Mmad
        CubeOpTestParams{
            MmadOpTest<half, half, half, Mmad>,
            "Mmad",
            {TPosition::A1, TPosition::A2, TPosition::B2},
            "dstLocal",
            "L1 Buffer(A1)",
            "L0C Buffer(CO1)"},
        CubeOpTestParams{
            MmadOpTest<half, half, half, Mmad>,
            "Mmad",
            {TPosition::CO1, TPosition::A1, TPosition::B2},
            "fmLocal",
            "L1 Buffer(A1)",
            "L0A Buffer(A2)"},
        CubeOpTestParams{
            MmadOpTest<half, half, half, Mmad>,
            "Mmad",
            {TPosition::CO1, TPosition::A2, TPosition::A1},
            "filterLocal",
            "L1 Buffer(A1)",
            "L0B Buffer(B2)"}));

TEST_P(CubeOpTestsuite, CubeOpTestCase)
{
    auto param = GetParam();
    SCOPED_TRACE(param.funcName + "." + param.illegalTensorPos);
    MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStub));
    const uint64_t startCount = g_raiseCount.load();
    param.func(param.tensorPos);
    EXPECT_GT(g_raiseCount.load() - startCount, 0);
}

TEST_F(TensorScopeTest, CheckTensorPhyPositionReportsUnsupportedPositions)
{
    MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStub));
    int32_t data[8] = {};
    TBuffAddr addr;
    addr.bufferHandle = nullptr;
    addr.dataLen = sizeof(data);
    addr.bufferAddr = 0;
    addr.absAddr = reinterpret_cast<uint8_t*>(data);
    const TPosition positions[] = {TPosition::GM,  TPosition::C1,        TPosition::VECOUT,      TPosition::VECCALC,
                                   TPosition::SPM, TPosition::C2PIPE2GM, TPosition::C2PIPE2LOCAL};

    for (const TPosition position : positions) {
        addr.logicPos = static_cast<uint8_t>(position);
        LocalTensor<float> tensor(addr);
        const uint64_t startCount = g_raiseCount.load();
        CheckTensorPhyPosition<Hardware::L0C>(tensor, "tensor", "L0C Buffer(CO1)", "DFX position check");
        EXPECT_GT(g_raiseCount.load() - startCount, 0);
    }
}

TEST_F(TensorScopeTest, FixpipeChannelSplitOnlySupportsFloatDst)
{
    static int32_t count = 0;
    MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStub));
    std::string fileName =
        "print_ut_aic_fixpipe_channel_split_" + std::to_string(getpid()) + "_" + std::to_string(count) + ".txt";
    freopen(fileName.c_str(), "w", stdout);

    FixpipeChannelSplitDstTypeTest();

    fclose(stdout);
    freopen("/dev/tty", "w", stdout);
    freopen("/dev/tty", "r", stdin);

    std::ifstream resultFile(fileName, std::ios::in);
    std::stringstream streambuffer;
    streambuffer << resultFile.rdbuf();
    std::string resultString(streambuffer.str());
    std::string goldenStr =
        "Failed to check isChannelSplit value in Fixpipe, isChannelSplit can be set true only when src and dst are "
        "both float";
    resultFile.close();

    EXPECT_TRUE(resultString.find(goldenStr) != std::string::npos);
    EXPECT_EQ(remove(fileName.c_str()), 0);
    count++;
}
