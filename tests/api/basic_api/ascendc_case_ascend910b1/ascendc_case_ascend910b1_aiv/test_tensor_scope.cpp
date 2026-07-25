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
#include "kernel_operator.h"

using namespace AscendC;

namespace {
std::atomic<uint64_t> g_raiseCount{0};

/******************************** 双目指令 ********************************/
template <typename T>
using BinaryOpPtr = void (*)(
    const AscendC::LocalTensor<T>& dstLocal, const AscendC::LocalTensor<T>& src0Local,
    const AscendC::LocalTensor<T>& src1Local, const int32_t& calCount);

template <typename T, BinaryOpPtr<T> func>
void BinaryOpTest(std::vector<TPosition>& tensorPos)
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
    LocalTensor<T> input1Local(addr2);

    func(outputLocal, input0Local, input1Local, dataSize);

    delete[] data0;
    delete[] data1;
    delete[] data2;
}

template <typename T, typename U>
using BinaryOp2TypePtr = void (*)(
    const AscendC::LocalTensor<T>& dstLocal, const AscendC::LocalTensor<U>& src0Local,
    const AscendC::LocalTensor<U>& src1Local, const int32_t& calCount);

template <typename T, typename U, BinaryOp2TypePtr<T, U> func>
void BinaryOp2TypeTest(std::vector<TPosition>& tensorPos)
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
    LocalTensor<U> input0Local(addr1);
    LocalTensor<U> input1Local(addr2);

    func(outputLocal, input0Local, input1Local, dataSize);

    delete[] data0;
    delete[] data1;
    delete[] data2;
}

/******************************** 单目指令 ********************************/
template <typename T>
using UnaryOpPtr = void (*)(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const int32_t& calCount);

template <typename T, UnaryOpPtr<T> func>
void UnaryOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = (uint8_t*)data0;

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 64 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = (uint8_t*)data1;
    LocalTensor<T> outputLocal(addr0);
    LocalTensor<T> input0Local(addr1);

    func(outputLocal, input0Local, dataSize);

    delete[] data0;
    delete[] data1;
}

template <typename T>
using BrcbOpPtr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& src0Local, const uint8_t repeatTimes,
    const BrcbRepeatParams& repeatParams);

template <typename T, BrcbOpPtr<T> func>
void BrcbOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = (uint8_t*)data0;

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 64 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = (uint8_t*)data1;
    LocalTensor<T> outputLocal(addr0);
    LocalTensor<T> input0Local(addr1);

    BrcbRepeatParams params{1, 8};
    uint8_t repeat = 1;
    func(outputLocal, input0Local, repeat, params);

    delete[] data0;
    delete[] data1;
}

/******************************** 双目标量指令 ********************************/
template <typename T, bool isSetMask>
using BinaryScalarOpPtr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const T& scalarValue, const int32_t& calCount);

template <typename T, BinaryScalarOpPtr<T, true> func>
void BinaryScalarOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = (uint8_t*)data0;

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 64 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = (uint8_t*)data1;
    LocalTensor<T> outputLocal(addr0);
    LocalTensor<T> input0Local(addr1);

    func(outputLocal, input0Local, static_cast<T>(1), dataSize);

    delete[] data0;
    delete[] data1;
}

/******************************** Duplicate指令 ********************************/
template <typename T>
using DuplicateOpPtr = void (*)(const LocalTensor<T>& dstLocal, const T& scalarValue, const int32_t& calCount);

template <typename T, DuplicateOpPtr<T> func>
void DuplicateOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];

    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = (uint8_t*)data0;
    LocalTensor<T> outputLocal(addr0);

    func(outputLocal, static_cast<T>(1), dataSize);

    delete[] data0;
}

/******************************** Cast指令 ********************************/
template <typename T1, typename T2>
using CastOpPtr = void (*)(
    const LocalTensor<T1>& dstLocal, const LocalTensor<T2>& srcLocal, const RoundMode& round_mode,
    const uint32_t calCount);

template <typename T1, typename T2, CastOpPtr<T1, T2> func>
void CastOpTest(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = (uint8_t*)data0;

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 64 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = (uint8_t*)data1;
    LocalTensor<T1> outputLocal(addr0);
    LocalTensor<T2> input0Local(addr1);

    func(outputLocal, input0Local, RoundMode::CAST_NONE, dataSize);

    delete[] data0;
    delete[] data1;
}

/******************************** DataCopy指令 ********************************/
template <typename T>
void DataCopyOpTest1(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TPipe pipe;
    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    LocalTensor<T> input0Local(addr0);
    GlobalTensor<T> outGlobal;
    outGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data1), 1024);
    DataCopyParams copyParams{1, 1, 8, 8};
    DataCopy(input0Local, outGlobal, copyParams);

    delete[] data0;
    delete[] data1;
}

template <typename T>
void DataCopyOpTest2(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TPipe pipe;
    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    LocalTensor<T> input0Local(addr0);
    GlobalTensor<T> outGlobal;
    outGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data1), 1024);
    Nd2NzParams copyParams{2, 2, 13, 48, 16, 11, 2, 48};
    DataCopy(input0Local, outGlobal, copyParams);

    delete[] data0;
    delete[] data1;
}

template <typename T>
void DataCopyOpTest3(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];
    int32_t* data1 = new int32_t[128 * 1024];

    TPipe pipe;
    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    LocalTensor<T> input0Local(addr0);
    GlobalTensor<T> outGlobal;
    outGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data1), 1024);
    DataCopyParams copyParams{1, 1, 8, 8};
    DataCopy(outGlobal, input0Local, copyParams);

    delete[] data0;
    delete[] data1;
}

template <typename T>
using DataCopyL2LNd2NzOpPtr =
    void (*)(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const Nd2NzParams& intriParams);

template <typename T, DataCopyL2LNd2NzOpPtr<T> func>
void DataCopyOpTest4(std::vector<TPosition>& tensorPos)
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
    Nd2NzParams copyParams{2, 2, 13, 48, 16, 11, 2, 48};
    func(dstLocal, srcLocal, copyParams);

    delete[] data0;
    delete[] data1;
}

template <typename DstT, typename SrcT>
using DataCopyL2LNd2NdOpPtr =
    void (*)(const LocalTensor<DstT>& dstLocal, const LocalTensor<SrcT>& srcLocal, const DataCopyParams& repeatParams);

template <typename DstT, typename SrcT, DataCopyL2LNd2NdOpPtr<DstT, SrcT> func>
void DataCopyOpTest5(std::vector<TPosition>& tensorPos)
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
    DataCopyParams copyParams{1, 1, 8, 8};
    func(dstLocal, srcLocal, copyParams);

    delete[] data0;
    delete[] data1;
}

// DataCopy Slice
template <typename T>
using DataCopySliceG2LOpPtr = void (*)(
    const LocalTensor<T>& dstLocal, const GlobalTensor<T>& srcGlobal, const SliceInfo dstSliceInfo[],
    const SliceInfo srcSliceInfo[], const uint32_t dimValue);

template <typename T, DataCopySliceG2LOpPtr<T> func>
void DataCopyOpTest6(std::vector<TPosition>& tensorPos)
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

    SliceInfo srcSliceInfoList[2] = {{16, 71, 8, 3, 88}, {0, 2, 1, 1, 3}};
    SliceInfo dstSliceInfoList[2] = {{0, 47, 0, 3, 48}, {0, 1, 0, 1, 2}};

    func(dstLocal, srcGlobal, dstSliceInfoList, srcSliceInfoList, 2);

    delete[] data0;
}

template <typename T>
using DataCopySliceL2GOpPtr = void (*)(
    const GlobalTensor<T>& dstGlobal, const LocalTensor<T>& srcLocal, const SliceInfo dstSliceInfo[],
    const SliceInfo srcSliceInfo[], const uint32_t dimValue);

template <typename T, DataCopySliceL2GOpPtr<T> func>
void DataCopyOpTest7(std::vector<TPosition>& tensorPos)
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

    LocalTensor<T> srcLocal(addr0);
    GlobalTensor<T> dstGlobal;
    dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data0), dataSize);

    SliceInfo srcSliceInfoList[2] = {{16, 71, 8, 3, 88}, {0, 2, 1, 1, 3}};
    SliceInfo dstSliceInfoList[2] = {{0, 47, 0, 3, 48}, {0, 1, 0, 1, 2}};

    func(dstGlobal, srcLocal, dstSliceInfoList, srcSliceInfoList, 2);

    delete[] data0;
}

// DataCopy Nz2NdParamsFull
template <typename T>
using DataCopyNz2NdFullOpPtr =
    void (*)(const GlobalTensor<T>& dstGlobal, const LocalTensor<T>& srcLocal, const Nz2NdParamsFull& intriParams);

template <typename T, DataCopyNz2NdFullOpPtr<T> func>
void DataCopyOpTest8(std::vector<TPosition>& tensorPos)
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

    LocalTensor<T> srcLocal(addr0);
    GlobalTensor<T> dstGlobal;
    dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data0), dataSize);

    Nz2NdParamsFull params{1, 32, 32, 1, 32, 32, 1};
    func(dstGlobal, srcLocal, params);

    delete[] data0;
}

// DataCopy DataCopyEnhancedParams
template <typename T>
using DataCopyEnhancedG2LOpPtr = void (*)(
    const LocalTensor<T>& dstLocal, const GlobalTensor<T>& srcGlobal, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

template <typename T, DataCopyEnhancedG2LOpPtr<T> func>
void DataCopyOpTest9(std::vector<TPosition>& tensorPos)
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

    DataCopyParams dataCopyParams;
    dataCopyParams.blockCount = 1;
    dataCopyParams.blockLen = 2 * sizeof(T);
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    DataCopyEnhancedParams enhancedParams;
    func(dstLocal, srcGlobal, dataCopyParams, enhancedParams);

    delete[] data0;
}

template <typename T>
using DataCopyEnhancedL2GOpPtr = void (*)(
    const GlobalTensor<T>& dstGlobal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

template <typename T, DataCopyEnhancedL2GOpPtr<T> func>
void DataCopyOpTest10(std::vector<TPosition>& tensorPos)
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

    LocalTensor<T> srcLocal(addr0);
    GlobalTensor<T> dstGlobal;
    dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data0), dataSize);

    DataCopyParams dataCopyParams;
    dataCopyParams.blockCount = 1;
    dataCopyParams.blockLen = 2 * sizeof(T);
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    DataCopyEnhancedParams enhancedParams;
    func(dstGlobal, srcLocal, dataCopyParams, enhancedParams);

    delete[] data0;
}

template <typename T>
using DataCopyEnhancedL2LOpPtr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams);

template <typename T, DataCopyEnhancedL2LOpPtr<T> func>
void DataCopyOpTest11(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128 * 1024;
    int32_t* data0 = new int32_t[dataSize];
    int32_t* data1 = new int32_t[dataSize];

    TPipe pipe;
    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 32 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]) + 32 * 1024);

    LocalTensor<T> dstLocal(addr0);
    LocalTensor<T> srcLocal(addr1);

    DataCopyParams dataCopyParams;
    dataCopyParams.blockCount = 1;
    dataCopyParams.blockLen = 2 * sizeof(T);
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    DataCopyEnhancedParams enhancedParams;
    func(dstLocal, srcLocal, dataCopyParams, enhancedParams);

    delete[] data0;
    delete[] data1;
}

template <typename T, typename U>
using DataCopyCO12DstL2LOpPtr =
    void (*)(const LocalTensor<T>& dstLocal, const LocalTensor<U>& srcLocal, const DataCopyCO12DstParams& intriParams);

template <typename T, typename U, DataCopyCO12DstL2LOpPtr<T, U> func>
void DataCopyOpTest12(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128 * 1024;
    int32_t* data0 = new int32_t[dataSize];
    int32_t* data1 = new int32_t[dataSize];

    TPipe pipe;
    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 32 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]) + 32 * 1024);

    LocalTensor<T> dstLocal(addr0);
    LocalTensor<U> srcLocal(addr1);

    DataCopyCO12DstParams params(16, 32, 8, 1, QuantMode_t::QF322B8_PRE, 0, false, false);
    func(dstLocal, srcLocal, params);

    delete[] data0;
    delete[] data1;
}

template <typename T, typename U>
using DataCopyCO12DstL2GOpPtr = void (*)(
    const GlobalTensor<T>& dstGlobal, const LocalTensor<U>& srcLocal, const DataCopyCO12DstParams& intriParams);

template <typename T, typename U, DataCopyCO12DstL2GOpPtr<T, U> func>
void DataCopyOpTest13(std::vector<TPosition>& tensorPos)
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

    LocalTensor<U> srcLocal(addr0);
    GlobalTensor<T> dstGlobal;
    dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data0), dataSize);

    DataCopyCO12DstParams params(16, 32, 8, 1, QuantMode_t::QF322B8_PRE, 0, false, false);
    func(dstGlobal, srcLocal, params);

    delete[] data0;
}

template <typename T>
using CopyOpMask2Ptr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint64_t mask[], const uint8_t repeatTimes,
    const CopyRepeatParams& repeatParams);

template <typename T, CopyOpMask2Ptr<T> func>
void CopyOpTest1(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128 * 1024;
    int32_t* data0 = new int32_t[dataSize];
    int32_t* data1 = new int32_t[dataSize];

    TPipe pipe;
    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 32 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]) + 32 * 1024);

    LocalTensor<T> dstLocal(addr0);
    LocalTensor<T> srcLocal(addr1);

    uint64_t mask[] = {0, 128};
    CopyRepeatParams params(1, 1, 8, 8);
    func(dstLocal, srcLocal, mask, 1, params);

    delete[] data0;
    delete[] data1;
}

template <typename T>
using CopyOpMask1Ptr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint64_t mask, const uint8_t repeatTimes,
    const CopyRepeatParams& repeatParams);

template <typename T, CopyOpMask1Ptr<T> func>
void CopyOpTest2(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128 * 1024;
    int32_t* data0 = new int32_t[dataSize];
    int32_t* data1 = new int32_t[dataSize];

    TPipe pipe;
    TBuffAddr addr0, addr1;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 32 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]) + 32 * 1024);

    LocalTensor<T> dstLocal(addr0);
    LocalTensor<T> srcLocal(addr1);

    uint64_t mask = 128;
    CopyRepeatParams params(1, 1, 8, 8);
    func(dstLocal, srcLocal, mask, 1, params);

    delete[] data0;
    delete[] data1;
}

/******************************** DataCopyPad指令 ********************************/
template <typename T>
using DataCopyPadG2LOp1Ptr = void (*)(
    const LocalTensor<T>& dstLocal, const GlobalTensor<T>& srcGlobal, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams);

template <typename T, DataCopyPadG2LOp1Ptr<T> func>
void DataCopyPadOpTest1(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];

    TPipe pipe;
    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    LocalTensor<T> input0Local(addr0);
    GlobalTensor<T> srcGlobal;
    srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data0), 1024);
    DataCopyParams copyParams{1, 20 * sizeof(T), 0, 0};
    DataCopyPadParams padParams{true, 0, 1, 3};
    func(input0Local, srcGlobal, copyParams, padParams);

    delete[] data0;
}

template <typename T>
using DataCopyPadL2GOp1Ptr =
    void (*)(const GlobalTensor<T>& dstGlobal, const LocalTensor<T>& srcLocal, const DataCopyParams& dataCopyParams);

template <typename T, DataCopyPadL2GOp1Ptr<T> func>
void DataCopyPadOpTest2(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];

    TPipe pipe;
    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    LocalTensor<T> input0Local(addr0);
    GlobalTensor<T> dstGlobal;
    dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data0), 1024);
    DataCopyParams copyParams{1, 20 * sizeof(T), 0, 0};
    func(dstGlobal, input0Local, copyParams);

    delete[] data0;
}

template <typename T>
using DataCopyPadL2LNd2NzOpPtr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams);

template <typename T, DataCopyPadL2LNd2NzOpPtr<T> func>
void DataCopyPadOpTest3(std::vector<TPosition>& tensorPos)
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
    DataCopyParams copyParams{1, 20 * sizeof(T), 0, 0};
    Nd2NzParams nd2nzParams{2, 2, 13, 48, 16, 11, 2, 48};
    func(dstLocal, srcLocal, copyParams, nd2nzParams);

    delete[] data0;
    delete[] data1;
}

template <typename T>
using DataCopyPadG2LOp2Ptr = void (*)(
    const LocalTensor<T>& dstLocal, const GlobalTensor<T>& srcGlobal, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<T>& padParams);

template <typename T, DataCopyPadG2LOp2Ptr<T> func>
void DataCopyPadOpTest4(std::vector<TPosition>& tensorPos)
{
    int32_t dataSize = 128;
    int32_t* data0 = new int32_t[128 * 1024];

    TPipe pipe;
    TBuffAddr addr0;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]));

    LocalTensor<T> input0Local(addr0);
    GlobalTensor<T> srcGlobal;
    srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(data0), 1024);
    DataCopyExtParams copyExtParams{1, 20 * sizeof(T), 0, 0, 0};
    DataCopyPadExtParams<T> padExtParams{true, 0, 1, 3};
    func(input0Local, srcGlobal, copyExtParams, DataCopyPadExtParams<T>());

    delete[] data0;
}

template <typename T>
using DataCopyPadL2LNd2NzOp2Ptr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyExtParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams);

template <typename T, DataCopyPadL2LNd2NzOp2Ptr<T> func>
void DataCopyPadOpTest5(std::vector<TPosition>& tensorPos)
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
    DataCopyExtParams copyExtParams{1, 20 * sizeof(T), 0, 0, 0};
    Nd2NzParams nd2nzParams{2, 2, 13, 48, 16, 11, 2, 48};
    func(dstLocal, srcLocal, copyExtParams, nd2nzParams);

    delete[] data0;
    delete[] data1;
}

/******************************** BilinearInterpolation ********************************/
template <typename T>
using BilinearInterpolationOpPtr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& src0Local, const LocalTensor<uint32_t>& src0OffsetLocal,
    const LocalTensor<T>& src1Local, uint64_t mask, uint8_t hRepeat, bool repeatMode, uint16_t dstBlkStride,
    uint16_t vROffset, uint8_t vRepeat, const LocalTensor<uint8_t>& sharedTmpBuffer);

template <typename T, BilinearInterpolationOpPtr<T> func>
void BilinearInterpolationOpTest(std::vector<TPosition>& tensorPos)
{
    TPipe pipe;
    TBuffAddr addr0, addr1, addr2, addr3, addr4;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]) + 0);

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 32 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]) + 32 * 1024);

    addr2.logicPos = static_cast<uint8_t>(tensorPos[2]);
    addr2.bufferHandle = nullptr;
    addr2.dataLen = 64 * 1024;
    addr2.bufferAddr = 0;
    addr2.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[2]) + 64 * 1024);

    addr3.logicPos = static_cast<uint8_t>(tensorPos[3]);
    addr3.bufferHandle = nullptr;
    addr3.dataLen = 96 * 1024;
    addr3.bufferAddr = 0;
    addr3.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[3]) + 96 * 1024);

    addr4.logicPos = static_cast<uint8_t>(TPosition::VECCALC);
    addr4.bufferHandle = nullptr;
    addr4.dataLen = 128 * 1024;
    addr4.bufferAddr = 0;
    addr4.absAddr = GetBaseAddrCpu(static_cast<int8_t>(TPosition::VECCALC) + 128 * 1024);

    LocalTensor<T> dstLocal(addr0);
    LocalTensor<T> src0Local(addr1);
    LocalTensor<uint32_t> src0OffsetLocal(addr2);
    LocalTensor<T> src1Local(addr3);
    LocalTensor<uint8_t> sharedTmpBuffer(addr4);

    uint64_t maskbit = 128;
    uint8_t hRepeat = 2;
    bool repeatMode = false;
    uint16_t dstBlkStride = 1;
    uint16_t vROffset = 128;
    uint8_t vRepeat = 2;
    func(
        dstLocal, src0Local, src0OffsetLocal, src1Local, maskbit, hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat,
        sharedTmpBuffer);
}

template <typename T>
using BilinearInterpolationBitmaskOpPtr = void (*)(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& src0Local, const LocalTensor<uint32_t>& src0OffsetLocal,
    const LocalTensor<T>& src1Local, uint64_t mask[], uint8_t hRepeat, bool repeatMode, uint16_t dstBlkStride,
    uint16_t vROffset, uint8_t vRepeat, const LocalTensor<uint8_t>& sharedTmpBuffer);

template <typename T, BilinearInterpolationBitmaskOpPtr<T> func>
void BilinearInterpolationBitmaskOpTest(std::vector<TPosition>& tensorPos)
{
    TPipe pipe;
    TBuffAddr addr0, addr1, addr2, addr3, addr4;
    addr0.logicPos = static_cast<uint8_t>(tensorPos[0]);
    addr0.bufferHandle = nullptr;
    addr0.dataLen = 32 * 1024;
    addr0.bufferAddr = 0;
    addr0.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[0]) + 0);

    addr1.logicPos = static_cast<uint8_t>(tensorPos[1]);
    addr1.bufferHandle = nullptr;
    addr1.dataLen = 32 * 1024;
    addr1.bufferAddr = 0;
    addr1.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[1]) + 32 * 1024);

    addr2.logicPos = static_cast<uint8_t>(tensorPos[2]);
    addr2.bufferHandle = nullptr;
    addr2.dataLen = 64 * 1024;
    addr2.bufferAddr = 0;
    addr2.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[2]) + 64 * 1024);

    addr3.logicPos = static_cast<uint8_t>(tensorPos[3]);
    addr3.bufferHandle = nullptr;
    addr3.dataLen = 96 * 1024;
    addr3.bufferAddr = 0;
    addr3.absAddr = GetBaseAddrCpu(static_cast<int8_t>(tensorPos[3]) + 96 * 1024);

    addr4.logicPos = static_cast<uint8_t>(TPosition::VECCALC);
    addr4.bufferHandle = nullptr;
    addr4.dataLen = 128 * 1024;
    addr4.bufferAddr = 0;
    addr4.absAddr = GetBaseAddrCpu(static_cast<int8_t>(TPosition::VECCALC) + 128 * 1024);

    LocalTensor<T> dstLocal(addr0);
    LocalTensor<T> src0Local(addr1);
    LocalTensor<uint32_t> src0OffsetLocal(addr2);
    LocalTensor<T> src1Local(addr3);
    LocalTensor<uint8_t> sharedTmpBuffer(addr4);

    uint64_t maskbit[] = {0, 128};
    uint8_t hRepeat = 2;
    bool repeatMode = false;
    uint16_t dstBlkStride = 1;
    uint16_t vROffset = 128;
    uint8_t vRepeat = 2;
    func(
        dstLocal, src0Local, src0OffsetLocal, src1Local, maskbit, hRepeat, repeatMode, dstBlkStride, vROffset, vRepeat,
        sharedTmpBuffer);
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
    void TearDown() {}
};

/******************************** 双目指令 ********************************/
struct ElemwiseOpTestParams {
    void (*func)(std::vector<TPosition>&);
    std::string funcName;
    std::vector<TPosition> tensorPos;
    std::string illegalTensorPos;
    std::string illegalTensorPosName;
    std::string supportPos;
};

class ElemwiseOpTestsuite : public testing::Test, public testing::WithParamInterface<ElemwiseOpTestParams> {
protected:
    void SetUp() {}
    void TearDown() { GlobalMockObject::verify(); }
};

INSTANTIATE_TEST_CASE_P(
    TEST_ELEMWISE_OP, ElemwiseOpTestsuite,
    ::testing::Values(
        //*********************************************************** 双目指令(14条)
        //***********************************************************
        // Add
        ElemwiseOpTestParams{
            BinaryOpTest<float, Add>,
            "Add",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Add>,
            "Add",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Add>,
            "Add",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Sub
        ElemwiseOpTestParams{
            BinaryOpTest<float, Sub>,
            "Sub",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Sub>,
            "Sub",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Sub>,
            "Sub",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Mul
        ElemwiseOpTestParams{
            BinaryOpTest<float, Mul>,
            "Mul",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Mul>,
            "Mul",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Mul>,
            "Mul",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Div
        ElemwiseOpTestParams{
            BinaryOpTest<float, Div>,
            "Div",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Div>,
            "Div",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Div>,
            "Div",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Max
        ElemwiseOpTestParams{
            BinaryOpTest<float, Max>,
            "Max",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Max>,
            "Max",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Max>,
            "Max",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Min
        ElemwiseOpTestParams{
            BinaryOpTest<float, Min>,
            "Min",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Min>,
            "Min",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Min>,
            "Min",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // And
        ElemwiseOpTestParams{
            BinaryOpTest<float, And>,
            "And",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, And>,
            "And",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, And>,
            "And",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Or
        ElemwiseOpTestParams{
            BinaryOpTest<float, Or>,
            "Or",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Or>,
            "Or",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, Or>,
            "Or",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // AddRelu
        ElemwiseOpTestParams{
            BinaryOpTest<float, AddRelu>,
            "AddRelu",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, AddRelu>,
            "AddRelu",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, AddRelu>,
            "AddRelu",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // FusedMulAdd
        ElemwiseOpTestParams{
            BinaryOpTest<float, FusedMulAdd>,
            "FusedMulAdd",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, FusedMulAdd>,
            "FusedMulAdd",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, FusedMulAdd>,
            "FusedMulAdd",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // SubRelu
        ElemwiseOpTestParams{
            BinaryOpTest<float, SubRelu>,
            "SubRelu",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, SubRelu>,
            "SubRelu",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, SubRelu>,
            "SubRelu",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // FusedMulAddRelu
        ElemwiseOpTestParams{
            BinaryOpTest<float, FusedMulAddRelu>,
            "FusedMulAddRelu",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, FusedMulAddRelu>,
            "FusedMulAddRelu",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, FusedMulAddRelu>,
            "FusedMulAddRelu",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // MulAddRelu
        ElemwiseOpTestParams{
            BinaryOpTest<float, MulAddRelu>,
            "MulAddRelu",
            {TPosition::VECIN, TPosition::VECIN, TPosition::A1},
            "src1",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, MulAddRelu>,
            "MulAddRelu",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOpTest<float, MulAddRelu>,
            "MulAddRelu",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // MulAddDst
        ElemwiseOpTestParams{
            BinaryOp2TypeTest<float, half, MulAddDst>,
            "MulAddDst",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOp2TypeTest<float, half, MulAddDst>,
            "MulAddDst",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOp2TypeTest<float, half, MulAddDst>,
            "MulAddDst",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // AddDeqRelu
        ElemwiseOpTestParams{
            BinaryOp2TypeTest<half, int32_t, AddDeqRelu>,
            "AddDeqRelu",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOp2TypeTest<half, int32_t, AddDeqRelu>,
            "AddDeqRelu",
            {TPosition::A1, TPosition::VECIN, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryOp2TypeTest<half, int32_t, AddDeqRelu>,
            "AddDeqRelu",
            {TPosition::VECIN, TPosition::A1, TPosition::VECIN},
            "src0",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        //*********************************************************** 单目指令(7条)
        //***********************************************************
        // Relu
        ElemwiseOpTestParams{
            UnaryOpTest<float, Relu>,
            "Relu",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            UnaryOpTest<float, Relu>,
            "Relu",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Exp
        ElemwiseOpTestParams{
            UnaryOpTest<float, Exp>,
            "Exp",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            UnaryOpTest<float, Exp>,
            "Exp",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Ln
        ElemwiseOpTestParams{
            UnaryOpTest<float, Ln>,
            "Ln",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            UnaryOpTest<float, Ln>,
            "Ln",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Abs
        ElemwiseOpTestParams{
            UnaryOpTest<float, Abs>,
            "Abs",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            UnaryOpTest<float, Abs>,
            "Abs",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Reciprocal
        ElemwiseOpTestParams{
            UnaryOpTest<float, Reciprocal>,
            "Reciprocal",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            UnaryOpTest<float, Reciprocal>,
            "Reciprocal",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Rsqrt
        ElemwiseOpTestParams{
            UnaryOpTest<float, Rsqrt>,
            "Rsqrt",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            UnaryOpTest<float, Rsqrt>,
            "Rsqrt",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Sqrt
        ElemwiseOpTestParams{
            UnaryOpTest<float, Sqrt>,
            "Sqrt",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            UnaryOpTest<float, Sqrt>,
            "Sqrt",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Not
        ElemwiseOpTestParams{
            UnaryOpTest<uint16_t, Not>,
            "Not",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            UnaryOpTest<uint16_t, Not>,
            "Not",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        //*********************************************************** 双目标量指令(7条)
        //***********************************************************
        // Adds
        ElemwiseOpTestParams{
            BinaryScalarOpTest<float, Adds<float, true>>,
            "Adds",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryScalarOpTest<float, Adds<float, true>>,
            "Adds",
            {TPosition::B1, TPosition::VECIN},
            "dst",
            "L1 Buffer(B1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Muls
        ElemwiseOpTestParams{
            BinaryScalarOpTest<float, Muls<float, true>>,
            "Muls",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryScalarOpTest<float, Muls<float, true>>,
            "Muls",
            {TPosition::B1, TPosition::VECIN},
            "dst",
            "L1 Buffer(B1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Maxs
        ElemwiseOpTestParams{
            BinaryScalarOpTest<float, Maxs<float, true>>,
            "Maxs",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryScalarOpTest<float, Maxs<float, true>>,
            "Maxs",
            {TPosition::B1, TPosition::VECIN},
            "dst",
            "L1 Buffer(B1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // Mins
        ElemwiseOpTestParams{
            BinaryScalarOpTest<float, Mins<float, true>>,
            "Mins",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryScalarOpTest<float, Mins<float, true>>,
            "Mins",
            {TPosition::B1, TPosition::VECIN},
            "dst",
            "L1 Buffer(B1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // ShiftLeft
        ElemwiseOpTestParams{
            BinaryScalarOpTest<uint32_t, ShiftLeft<uint32_t, true>>,
            "ShiftLeft",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryScalarOpTest<uint32_t, ShiftLeft<uint32_t, true>>,
            "ShiftLeft",
            {TPosition::B1, TPosition::VECIN},
            "dst",
            "L1 Buffer(B1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // ShiftRight
        ElemwiseOpTestParams{
            BinaryScalarOpTest<uint32_t, ShiftRight<uint32_t, true>>,
            "ShiftRight",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryScalarOpTest<uint32_t, ShiftRight<uint32_t, true>>,
            "ShiftRight",
            {TPosition::B1, TPosition::VECIN},
            "dst",
            "L1 Buffer(B1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // LeakyRelu
        ElemwiseOpTestParams{
            BinaryScalarOpTest<half, LeakyRelu<half, true>>,
            "LeakyRelu",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BinaryScalarOpTest<half, LeakyRelu<half, true>>,
            "LeakyRelu",
            {TPosition::B1, TPosition::VECIN},
            "dst",
            "L1 Buffer(B1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        //*********************************************************** Brcb指令(1条)
        //***********************************************************
        // Brcb
        ElemwiseOpTestParams{
            BrcbOpTest<uint16_t, Brcb>,
            "Brcb",
            {TPosition::VECIN, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BrcbOpTest<uint16_t, Brcb>,
            "Brcb",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        //*********************************************************** Duplicate指令(1条)
        //***********************************************************
        // Duplicate
        ElemwiseOpTestParams{
            DuplicateOpTest<uint16_t, Duplicate>,
            "Duplicate",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        //*********************************************************** Cast指令(1条)
        //***********************************************************
        // Cast
        ElemwiseOpTestParams{
            CastOpTest<uint8_t, half, Cast>,
            "Cast",
            {TPosition::A1, TPosition::VECIN},
            "dst",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // DataCopy
        ElemwiseOpTestParams{
            DataCopyOpTest1<float>,
            "DataCopy from GlobalTensor to LocalTensor with DataCopyParams",
            {TPosition::C2},
            "dst",
            "BiasTable Buffer(C2)",
            "L1 Buffer(A1/B1/C1)/UB(VECIN)"},
        ElemwiseOpTestParams{
            DataCopyOpTest2<float>,
            "DataCopy from GlobalTensor to LocalTensor with Nd2NzParams",
            {TPosition::C2},
            "dst",
            "BiasTable Buffer(C2)",
            "L1 Buffer(A1/B1)/UB(VECIN)"},
        ElemwiseOpTestParams{
            DataCopyOpTest3<float>,
            "DataCopy from LocalTensor to GlobalTensor with DataCopyParams",
            {TPosition::C2},
            "src",
            "BiasTable Buffer(C2)",
            "L1 Buffer(A1/B1)/UB(VECOUT)"},
        ElemwiseOpTestParams{
            DataCopyOpTest4<float, DataCopy>,
            "DataCopy from LocalTensor to LocalTensor with Nd2NzParams",
            {TPosition::VECIN, TPosition::A1},
            "dst",
            "UB(VECIN)",
            "L1 Buffer(TSCM)"},
        ElemwiseOpTestParams{
            DataCopyOpTest4<float, DataCopy>,
            "DataCopy from LocalTensor to LocalTensor with Nd2NzParams",
            {TPosition::TSCM, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECCALC/VECOUT)"},
        ElemwiseOpTestParams{
            DataCopyOpTest5<float, float, DataCopy>,
            "DataCopy from LocalTensor(L1 Buffer(A1/B1/C1)) to LocalTensor with DataCopyParams",
            {TPosition::TSCM, TPosition::C1},
            "dst",
            "L1 Buffer(TSCM)",
            "BiasTable Buffer(C2)/Fixpipe Buffer(C2PIPE2GM)"},
        ElemwiseOpTestParams{
            DataCopyOpTest5<float, float, DataCopy>,
            "DataCopy from LocalTensor to LocalTensor with DataCopyParams",
            {TPosition::TSCM, TPosition::C2},
            "src",
            "BiasTable Buffer(C2)",
            "UB(VECIN/VECCALC/VECOUT)/L1 Buffer(A1/B1/C1)"},
        ElemwiseOpTestParams{
            DataCopyOpTest5<float, float, DataCopy>,
            "DataCopy from LocalTensor(UB(VECIN/VECCALC/VECOUT)) to LocalTensor with DataCopyParams",
            {TPosition::C2, TPosition::VECIN},
            "dst",
            "BiasTable Buffer(C2)",
            "UB(VECCALC/VECOUT)/L1 Buffer(TSCM)"},
        ElemwiseOpTestParams{
            DataCopyOpTest5<float, uint16_t, DataCopy>,
            "DataCopy from LocalTensor to LocalTensor with T / U",
            {TPosition::B2, TPosition::C1},
            "dst",
            "L0B Buffer(B2)",
            "BiasTable Buffer(C2)"},
        ElemwiseOpTestParams{
            DataCopyOpTest5<float, uint16_t, DataCopy>,
            "DataCopy from LocalTensor to LocalTensor with T / U",
            {TPosition::C2, TPosition::B2},
            "src",
            "L0B Buffer(B2)",
            "L1 Buffer(C1)"},
        ElemwiseOpTestParams{
            DataCopyOpTest6<float, DataCopy>,
            "DataCopy with SliceInfo",
            {TPosition::C2},
            "dst",
            "BiasTable Buffer(C2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            DataCopyOpTest7<float, DataCopy>,
            "DataCopy with SliceInfo",
            {TPosition::C2},
            "src",
            "BiasTable Buffer(C2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            DataCopyOpTest8<float, DataCopy>,
            "DataCopy with Nz2NdParamsFull",
            {TPosition::C2},
            "src",
            "BiasTable Buffer(C2)",
            "UB(VECOUT)"},
        ElemwiseOpTestParams{
            DataCopyOpTest9<float, DataCopy>,
            "DataCopy from GlobalTensor to LocalTensor with DataCopyEnhancedParams",
            {TPosition::C2},
            "dst",
            "BiasTable Buffer(C2)",
            "L1 Buffer(A1/B1)/UB(VECIN)"},
        ElemwiseOpTestParams{
            DataCopyOpTest10<float, DataCopy>,
            "DataCopy from LocalTensor to GlobalTensor with DataCopyEnhancedParams",
            {TPosition::C2},
            "src",
            "BiasTable Buffer(C2)",
            "L1 Buffer(A1/B1)/UB(VECOUT)"},
        ElemwiseOpTestParams{
            DataCopyOpTest11<float, DataCopy>,
            "DataCopy from LocalTensor(UB(VECIN/VECCALC/VECOUT)) to LocalTensor with DataCopyEnhancedParams",
            {TPosition::C2, TPosition::VECIN},
            "dst",
            "BiasTable Buffer(C2)",
            "UB(VECCALC/VECOUT)/L1 Buffer(A1/B1/TSCM)"},
        ElemwiseOpTestParams{
            DataCopyOpTest11<float, DataCopy>,
            "DataCopy from LocalTensor(L1 Buffer(A1/B1)) to LocalTensor with DataCopyEnhancedParams",
            {TPosition::C2, TPosition::A1},
            "dst",
            "BiasTable Buffer(C2)",
            "L0C Buffer(CO1)"},
        ElemwiseOpTestParams{
            DataCopyOpTest11<float, DataCopy>,
            "DataCopy from LocalTensor(L0C Buffer(CO1)) to LocalTensor with DataCopyEnhancedParams",
            {TPosition::C2, TPosition::CO1},
            "dst",
            "BiasTable Buffer(C2)",
            "UB(CO2)"},
        ElemwiseOpTestParams{
            DataCopyOpTest11<float, DataCopy>,
            "DataCopy from LocalTensor to LocalTensor with DataCopyEnhancedParams",
            {TPosition::C2, TPosition::CO2},
            "src",
            "GM(CO2)",
            "UB(VECIN)"},
        ElemwiseOpTestParams{
            DataCopyOpTest12<int8_t, float, DataCopy<int8_t, float>>,
            "DataCopy from LocalTensor to LocalTensor with DataCopyCO12DstParams",
            {TPosition::A1, TPosition::CO2},
            "src",
            "GM(CO2)",
            "L0C Buffer(CO1)"},
        ElemwiseOpTestParams{
            DataCopyOpTest12<int8_t, float, DataCopy<int8_t, float>>,
            "DataCopy from LocalTensor to LocalTensor with DataCopyCO12DstParams",
            {TPosition::C2, TPosition::CO1},
            "dst",
            "BiasTable Buffer(C2)",
            "L1 Buffer(A1)"},
        ElemwiseOpTestParams{
            DataCopyOpTest12<int8_t, float, DataCopy<int8_t, float>>,
            "DataCopy from LocalTensor to LocalTensor with DataCopyCO12DstParams",
            {TPosition::C2, TPosition::CO1},
            "dst",
            "BiasTable Buffer(C2)",
            "L1 Buffer(A1)"},
        ElemwiseOpTestParams{
            DataCopyOpTest13<int8_t, float, DataCopy<int8_t, float>>,
            "DataCopy from LocalTensor to GlobalTensor with DataCopyCO12DstParams",
            {TPosition::CO2},
            "src",
            "GM(CO2)",
            "L0C Buffer(CO1)"},
        // DataCopyPad
        ElemwiseOpTestParams{
            DataCopyPadOpTest1<float, DataCopyPad>,
            "DataCopyPad from GlobalTensor to LocalTensor with DataCopyPadParams",
            {TPosition::CO2},
            "dst",
            "GM(CO2)",
            "UB(VECIN/VECOUT)"},
        ElemwiseOpTestParams{
            DataCopyPadOpTest2<float, DataCopyPad>,
            "DataCopyPad from LocalTensor to GlobalTensor with DataCopyParams",
            {TPosition::CO2},
            "src",
            "GM(CO2)",
            "UB(VECIN/VECOUT)"},
        ElemwiseOpTestParams{
            DataCopyPadOpTest3<float, DataCopyPad>,
            "DataCopyPad with Nd2NzParams",
            {TPosition::A1, TPosition::CO2},
            "src",
            "GM(CO2)",
            "UB(VECIN/VECOUT)"},
        ElemwiseOpTestParams{
            DataCopyPadOpTest3<float, DataCopyPad>,
            "DataCopyPad with Nd2NzParams",
            {TPosition::CO2, TPosition::VECIN},
            "dst",
            "GM(CO2)",
            "L1 Buffer(TSCM)"},
        ElemwiseOpTestParams{
            DataCopyPadOpTest4<float, DataCopyPad>,
            "DataCopyPad from GM to UB(VECIN/VECOUT)",
            {TPosition::CO2},
            "dst",
            "GM(CO2)",
            "UB(VECIN/VECOUT)"},
        ElemwiseOpTestParams{
            DataCopyPadOpTest5<float, DataCopyPad>,
            "DataCopyPad with Nd2NzParams",
            {TPosition::CO2, TPosition::VECIN},
            "dst",
            "GM(CO2)",
            "L1 Buffer(TSCM)"},
        ElemwiseOpTestParams{
            DataCopyPadOpTest5<float, DataCopyPad>,
            "DataCopyPad with Nd2NzParams",
            {TPosition::TSCM, TPosition::A1},
            "src",
            "L1 Buffer(A1)",
            "UB(VECIN/VECOUT)"},
        // Copy
        ElemwiseOpTestParams{
            CopyOpTest1<float, Copy>,
            "Copy",
            {TPosition::VECIN, TPosition::CO2},
            "src",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            CopyOpTest1<float, Copy>,
            "Copy",
            {TPosition::CO2, TPosition::VECIN},
            "dst",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            CopyOpTest2<float, Copy>,
            "Copy",
            {TPosition::VECIN, TPosition::CO2},
            "src",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            CopyOpTest2<float, Copy>,
            "Copy",
            {TPosition::CO2, TPosition::VECIN},
            "dst",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        // BilinearInterpolation
        ElemwiseOpTestParams{
            BilinearInterpolationOpTest<half, BilinearInterpolation>,
            "BilinearInterpolation",
            {TPosition::CO2, TPosition::VECIN, TPosition::VECIN, TPosition::VECIN},
            "dstLocal",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BilinearInterpolationOpTest<half, BilinearInterpolation>,
            "BilinearInterpolation",
            {TPosition::VECIN, TPosition::CO2, TPosition::VECIN, TPosition::VECIN},
            "src0Local",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BilinearInterpolationOpTest<half, BilinearInterpolation>,
            "BilinearInterpolation",
            {TPosition::VECIN, TPosition::VECIN, TPosition::CO2, TPosition::VECIN},
            "src0OffsetLocal",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BilinearInterpolationOpTest<half, BilinearInterpolation>,
            "BilinearInterpolation",
            {TPosition::VECIN, TPosition::VECIN, TPosition::VECIN, TPosition::CO2},
            "src1Local",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BilinearInterpolationBitmaskOpTest<half, BilinearInterpolation>,
            "BilinearInterpolation",
            {TPosition::CO2, TPosition::VECIN, TPosition::VECIN, TPosition::VECIN},
            "dstLocal",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BilinearInterpolationBitmaskOpTest<half, BilinearInterpolation>,
            "BilinearInterpolation",
            {TPosition::VECIN, TPosition::CO2, TPosition::VECIN, TPosition::VECIN},
            "src0Local",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BilinearInterpolationBitmaskOpTest<half, BilinearInterpolation>,
            "BilinearInterpolation",
            {TPosition::VECIN, TPosition::VECIN, TPosition::CO2, TPosition::VECIN},
            "src0OffsetLocal",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"},
        ElemwiseOpTestParams{
            BilinearInterpolationBitmaskOpTest<half, BilinearInterpolation>,
            "BilinearInterpolation",
            {TPosition::VECIN, TPosition::VECIN, TPosition::VECIN, TPosition::CO2},
            "src1Local",
            "GM(CO2)",
            "UB(VECIN/VECOUT/VECCALC)"}

        ));

TEST_P(ElemwiseOpTestsuite, ElemwiseOpTestCase)
{
    auto param = GetParam();
    SCOPED_TRACE(param.funcName + "." + param.illegalTensorPos);
    MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStub));
    const uint64_t startCount = g_raiseCount.load();
    param.func(param.tensorPos);
    EXPECT_GT(g_raiseCount.load() - startCount, 0);
}
