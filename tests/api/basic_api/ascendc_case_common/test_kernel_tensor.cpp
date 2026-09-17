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
#include "kernel_operator.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "mockcpp/mockcpp.hpp"

using namespace AscendC;
using namespace std;

namespace {
int32_t RaiseStubCreateTensor(int32_t i) { return 0; }
} // namespace

template <typename T>
class TEST_TENSOR_SET_BUFFER_LEN : public testing::Test {};

using SetBufferLenTypes = testing::Types<uint8_t, uint16_t, float, TensorTrait<float>>;
TYPED_TEST_SUITE(TEST_TENSOR_SET_BUFFER_LEN, SetBufferLenTypes);

TYPED_TEST(TEST_TENSOR_SET_BUFFER_LEN, ElementCount)
{
    using PrimType = PrimT<TypeParam>;
    TPipe pipe;
    TBuf<TPosition::VECCALC> buffer;
    pipe.InitBuffer(buffer, 1024);
    LocalTensor<TypeParam> tensor = buffer.Get<TypeParam>();
    for (uint32_t elementCount : {128U, 32U, 0U}) {
        tensor.SetBufferLen(elementCount);
        EXPECT_EQ(tensor.GetSize(), elementCount);
        EXPECT_EQ(tensor.address_.dataLen, elementCount * sizeof(PrimType));
    }
}

/* **************************** LocalTensor Print ****************************** */
struct PrintTensorParams {
    string dataType;
    int32_t printSize;
    string goldenStr;
};

class TEST_TENSOR_PRINT : public testing::Test, public testing::WithParamInterface<PrintTensorParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    TENSOR_PRINT, TEST_TENSOR_PRINT,
    ::testing::Values(
        PrintTensorParams{"uint32_t", 16, "0000 : 0 1 2 3 4 5 6 7 \n0008 : 8 9 10 11 12 13 14 15 \n"},
        PrintTensorParams{"uint32_t", 7, "0000 : 0 1 2 3 4 5 6 \n"},
        PrintTensorParams{"int32_t", 16, "0000 : 0 1 2 3 4 5 6 7 \n0008 : 8 9 10 11 12 13 14 15 \n"},
        PrintTensorParams{"int32_t", 7, "0000 : 0 1 2 3 4 5 6 \n"},
        PrintTensorParams{
            "half", 33,
            "0000 : 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 \n0016 : 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 "
            "\n0032 : 32 "
            "\n"},
        PrintTensorParams{"half", 17, "0000 : 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 \n0016 : 16 \n"},
        PrintTensorParams{"half", 7, "0000 : 0 1 2 3 4 5 6 \n"},
        PrintTensorParams{"float", 17, "0000 : 0 1 2 3 4 5 6 7 \n0008 : 8 9 10 11 12 13 14 15 \n0016 : 16 \n"},
        PrintTensorParams{"float", 7, "0000 : 0 1 2 3 4 5 6 \n"},
        PrintTensorParams{
            "uint16_t", 33,
            "0000 : 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 \n0016 : 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 "
            "\n0032 : 32 "
            "\n"},
        PrintTensorParams{"uint16_t", 17, "0000 : 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 \n0016 : 16 \n"},
        PrintTensorParams{"uint16_t", 7, "0000 : 0 1 2 3 4 5 6 \n"},
        PrintTensorParams{
            "uint8_t", 33,
            "0000 : 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 \n0032 : 32 "
            "\n"},
        PrintTensorParams{
            "int8_t", 33,
            "0000 : 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 \n0032 : 32 "
            "\n"},
        PrintTensorParams{
            "bool", 33, "0000 : 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 \n0032 : 1 \n"}));

template <typename T>
void GetPrintString(const string& dataType, const int32_t printSize, string& retStr)
{
    const int32_t maxLen = 64;
    TBuffAddr addr;
    addr.logicPos = 1;
    LocalTensor<T> tensor(addr);
    tensor.InitBuffer(0, maxLen);
    for (int32_t i = 0; i < maxLen; i++) {
        tensor.SetValue(i, T(i));
    }
    tensor.Print(printSize);
    retStr = tensor.os_.str();
}

TEST_P(TEST_TENSOR_PRINT, TensorPrintTest)
{
    auto param = GetParam();
    string curStr;

    if (param.dataType == "uint32_t") {
        GetPrintString<uint32_t>(param.dataType, param.printSize, curStr);
    } else if (param.dataType == "int32_t") {
        GetPrintString<int32_t>(param.dataType, param.printSize, curStr);
    } else if (param.dataType == "half") {
        GetPrintString<half>(param.dataType, param.printSize, curStr);
    } else if (param.dataType == "float") {
        GetPrintString<float>(param.dataType, param.printSize, curStr);
    } else if (param.dataType == "uint16_t") {
        GetPrintString<uint16_t>(param.dataType, param.printSize, curStr);
    } else if (param.dataType == "uint8_t") {
        GetPrintString<uint8_t>(param.dataType, param.printSize, curStr);
    } else if (param.dataType == "int8_t") {
        GetPrintString<int8_t>(param.dataType, param.printSize, curStr);
    } else if (param.dataType == "bool") {
        GetPrintString<bool>(param.dataType, param.printSize, curStr);
    }
    EXPECT_EQ(curStr, param.goldenStr);
}

class TEST_TENSOR : public testing::Test {
protected:
    void SetUp() {}
    void TearDown()
    {
        ConstDefiner::Instance().allocatorUsed.clear();
        GlobalMockObject::verify();
    }
};
/* **************************** LocalTensor Operator[] ****************************** */
TEST_F(TEST_TENSOR, TensorBracketWithoutInitBuffer)
{
    const int32_t maxLen = 32;
    const int32_t offsetIndex = 16;
    int32_t* data = new int32_t[maxLen];
    for (int32_t i = 0; i < maxLen; i++) {
        data[i] = i;
    }

    TBuffAddr addr;
    addr.logicPos = 1;
    addr.bufferHandle = nullptr;
    addr.dataLen = maxLen * sizeof(int32_t);
    addr.bufferAddr = 0;
    addr.absAddr = (uint8_t*)data;

    LocalTensor<int32_t> tensor(addr);
    tensor.Print();
    LocalTensor<int32_t> tensorPart = tensor[offsetIndex];
    string str1 = tensor.os_.str();
    string str2 = tensorPart.os_.str();
    string str1Golden = "0000 : 0 1 2 3 4 5 6 7 \n0008 : 8 9 10 11 12 13 14 15 \n0016 : 16 17 18 19 20 21 22 23 \n0024 "
                        ": 24 25 26 27 28 29 30 31 \n";
    string str2Golden = "";
    EXPECT_EQ(str1, str1Golden);
    EXPECT_EQ(str2, str2Golden);
    EXPECT_EQ(tensorPart.GetSize(), maxLen - offsetIndex);
    EXPECT_EQ(tensorPart.GetLength(), (maxLen - offsetIndex) * sizeof(int32_t));
    EXPECT_EQ(tensor.address_.absAddr, (uint8_t*)data);
    EXPECT_EQ(tensorPart.address_.absAddr, (uint8_t*)data + offsetIndex * sizeof(int32_t));
    EXPECT_EQ(tensor.address_.bufferAddr, 0);
    EXPECT_EQ(tensorPart.address_.bufferAddr, 0 + +offsetIndex * sizeof(int32_t));

    delete[] data;
}

TEST_F(TEST_TENSOR, TensorBracketWithInitBuffer)
{
    const int32_t maxLen = 32;
    const int32_t offsetIndex = 24;

    TBuffAddr addr;
    addr.logicPos = 1;
    LocalTensor<int32_t> tensor(addr);
    tensor.InitBuffer(0, maxLen);
    for (int32_t i = 0; i < maxLen; i++) {
        tensor.SetValue(i, i);
    };
    tensor.Print();
    LocalTensor<int32_t> tensorPart = tensor[offsetIndex];
    string str1 = tensor.os_.str();
    string str2 = tensorPart.os_.str();
    string str1Golden = "0000 : 0 1 2 3 4 5 6 7 \n0008 : 8 9 10 11 12 13 14 15 \n0016 : 16 17 18 19 20 21 22 23 \n0024 "
                        ": 24 25 26 27 28 29 30 31 \n";
    string str2Golden = "";
    EXPECT_EQ(str1, str1Golden);
    EXPECT_EQ(str2, str2Golden);
    EXPECT_EQ(tensorPart.GetSize(), maxLen - offsetIndex);
    EXPECT_EQ(tensorPart.GetLength(), (maxLen - offsetIndex) * sizeof(int32_t));
}

/* **************************** LocalTensor Operator= ****************************** */
TEST_F(TEST_TENSOR, TensorEqual)
{
    const int32_t maxLen = 32;
    int32_t* data = new int32_t[maxLen];
    for (int32_t i = 0; i < maxLen; i++) {
        data[i] = i;
    }

    TBuffAddr addr;
    addr.logicPos = 1;
    addr.bufferHandle = nullptr;
    addr.dataLen = maxLen * sizeof(int32_t);
    addr.bufferAddr = 0;
    addr.absAddr = (uint8_t*)data;

    LocalTensor<int32_t> tensor1(addr);
    tensor1.Print();
    LocalTensor<int32_t> tensor2 = tensor1;
    string str1 = tensor1.os_.str();
    string str2 = tensor2.os_.str();
    string strGolden = "0000 : 0 1 2 3 4 5 6 7 \n0008 : 8 9 10 11 12 13 14 15 \n0016 : 16 17 18 19 20 21 22 23 \n0024 "
                       ": 24 25 26 27 28 29 30 31 \n";
    EXPECT_EQ(str1, strGolden);
    EXPECT_EQ(str2, "");
    EXPECT_EQ(tensor1.GetSize(), maxLen);
    EXPECT_EQ(tensor2.GetSize(), maxLen);
    EXPECT_EQ(tensor1.GetLength(), maxLen * sizeof(int32_t));
    EXPECT_EQ(tensor2.GetLength(), maxLen * sizeof(int32_t));
    EXPECT_EQ(tensor1.address_.absAddr, (uint8_t*)data);
    EXPECT_EQ(tensor2.address_.absAddr, (uint8_t*)data);
    EXPECT_EQ(tensor1.address_.bufferAddr, 0);
    EXPECT_EQ(tensor2.address_.bufferAddr, 0);

    delete[] data;
}

/* **************************** LocalTensor CopyConstruct ****************************** */
TEST_F(TEST_TENSOR, TensorConpyConstrcutWithInitBuffer)
{
    const int32_t maxLen = 32;
    TBuffAddr addr;
    addr.logicPos = 1;
    LocalTensor<int32_t> tensor1(addr);
    tensor1.InitBuffer(0, maxLen);
    for (int32_t i = 0; i < maxLen; i++) {
        tensor1.SetValue(i, i);
    };
    EXPECT_EQ(tensor1.os_.str(), "");
    tensor1.Print();
    LocalTensor<int32_t> tensor2(tensor1);
    string str1 = tensor1.os_.str();
    string str2 = tensor2.os_.str();
    string strGolden = "0000 : 0 1 2 3 4 5 6 7 \n0008 : 8 9 10 11 12 13 14 15 \n0016 : 16 17 18 19 20 21 22 23 \n0024 "
                       ": 24 25 26 27 28 29 30 31 \n";
    EXPECT_EQ(str1, strGolden);
    EXPECT_EQ(str2, "");
    EXPECT_EQ(tensor2.GetSize(), maxLen);
    EXPECT_EQ(tensor2.GetLength(), maxLen * sizeof(int32_t));
}

TEST_F(TEST_TENSOR, TensorCopyConstrcutWithoutInitBuffer)
{
    const int32_t maxLen = 32;
    int32_t* data = new int32_t[maxLen];
    for (int32_t i = 0; i < maxLen; i++) {
        data[i] = i;
    }

    TBuffAddr addr;
    addr.logicPos = 1;
    addr.bufferHandle = nullptr;
    addr.dataLen = maxLen * sizeof(int32_t);
    addr.bufferAddr = 0;
    addr.absAddr = (uint8_t*)data;

    LocalTensor<int32_t> tensor1(addr);
    tensor1.Print();
    LocalTensor<int32_t> tensor2(tensor1);
    string str1 = tensor1.os_.str();
    string str2 = tensor2.os_.str();
    string str1Golden = "0000 : 0 1 2 3 4 5 6 7 \n0008 : 8 9 10 11 12 13 14 15 \n0016 : 16 17 18 19 20 21 22 23 \n0024 "
                        ": 24 25 26 27 28 29 30 31 \n";
    string str2Golden = "";
    EXPECT_EQ(str1, str1Golden);
    EXPECT_EQ(str2, str2Golden);
    EXPECT_EQ(tensor2.GetSize(), maxLen);
    EXPECT_EQ(tensor2.GetLength(), maxLen * sizeof(int32_t));
    EXPECT_EQ(tensor1.address_.absAddr, (uint8_t*)data);
    EXPECT_EQ(tensor2.address_.absAddr, (uint8_t*)data);
    EXPECT_EQ(tensor1.address_.bufferAddr, 0);
    EXPECT_EQ(tensor2.address_.bufferAddr, 0);

    delete[] data;
}

#define USER_TAG 10
TEST_F(TEST_TENSOR, TensorSetUserTagTest)
{
    TTagType tagAddr = 0;
    TBuffAddr addr = {0};
    addr.bufferHandle = (TBufHandle)malloc(sizeof(TBufType));
    LocalTensor<int32_t> tensor(addr);
    // 传入数值
    int32_t userTag = USER_TAG;
    tensor.SetUserTag((TTagType)userTag);
    EXPECT_EQ(USER_TAG, tensor.GetUserTag());

    // 将指针转成TTagType作为userTag
    tensor.SetUserTag((TTagType)(uintptr_t)&userTag);
    EXPECT_EQ((TTagType)(uintptr_t)&userTag, tensor.GetUserTag());

    free(addr.bufferHandle);
}

TEST_F(TEST_TENSOR, TensorReinterpretCastTest)
{
    const int32_t maxLen = 64;
    TBuffAddr addr;
    addr.logicPos = 1;
    LocalTensor<int32_t> tensor1(addr);
    tensor1.InitBuffer(0, maxLen);

    LocalTensor<int16_t> tensorOut = tensor1.ReinterpretCast<int16_t>();
    EXPECT_EQ(tensorOut.address_.logicPos, tensor1.address_.logicPos);
    EXPECT_EQ(tensorOut.address_.bufferHandle, tensor1.address_.bufferHandle);
    EXPECT_EQ(tensorOut.address_.dataLen, 256);
    EXPECT_EQ(tensorOut.address_.bufferAddr, tensor1.address_.bufferAddr);
    EXPECT_EQ(tensorOut.address_.absAddr, tensor1.address_.absAddr);
}

TEST_F(TEST_TENSOR, LocalTensorSupportInt4Test)
{
    const int32_t num = 16;
    uint64_t data = 0xfedcba9876543210;

    TBuffAddr addr;
    addr.logicPos = 1;
    addr.bufferHandle = nullptr;
    addr.dataLen = num / INT4_TWO;
    addr.bufferAddr = 0;
    addr.absAddr = (uint8_t*)&data;

    LocalTensor<int4b_t> tensor1(addr);
    EXPECT_EQ(tensor1.GetSize(), 16);
    for (int i = 0; i < num; i++) {
        EXPECT_EQ(tensor1.GetValue(i), (int4b_t)i);
    }

    LocalTensor<int4b_t> tensor2 = tensor1[num / 2];
    for (int i = 0; i < num / 2; i++) {
        EXPECT_EQ(tensor2.GetValue(i), (int4b_t)(-8 + i));
    }

    for (int i = 0; i < num; i++) {
        tensor1.SetValue(i, (int4b_t)(0xf - i));
    }
    EXPECT_EQ(data, 0x123456789abcdef);

    LocalTensor<uint8_t> tensor3 = tensor1.ReinterpretCast<uint8_t>();
    EXPECT_EQ(tensor3.GetSize(), sizeof(data) / sizeof(uint8_t));
}

TEST_F(TEST_TENSOR, GlobalTensorSupportInt4Test)
{
    const int32_t num = 16;
    uint64_t data = 0xfedcba9876543210;
    uint8_t* dataPtr = (uint8_t*)&data;
    __gm__ int4b_t* gmAddr = (__gm__ int4b_t*)&data;

    GlobalTensor<int4b_t> glbTensor1;
    GlobalTensor<int4b_t> glbTensor2;
    glbTensor1.SetGlobalBuffer(gmAddr, num);
    glbTensor2 = glbTensor1[num / 2];

    EXPECT_EQ(glbTensor1.GetSize(), num);
    EXPECT_EQ(glbTensor1.GetPhyAddr(), (int4b_t*)&data);

    for (int i = 0; i < num / 2; i++) {
        EXPECT_EQ(glbTensor1.GetPhyAddr(i * 2), (int4b_t*)&dataPtr[i]);
    }

    for (int i = 0; i < num; i++) {
        EXPECT_EQ(glbTensor1.GetValue(i), (int4b_t)i);
    }

    for (int i = 0; i < num / 2; i++) {
        EXPECT_EQ(glbTensor2.GetValue(i), (int4b_t)(-8 + i));
    }

    for (int i = 0; i < num; i++) {
        glbTensor1.SetValue(i, (int4b_t)(0xf - i));
    }
    EXPECT_EQ(data, 0x123456789abcdef);
}

TEST_F(TEST_TENSOR, TestGetSetValue)
{
    int32_t tmp = g_coreType;
    g_coreType = AscendC::AIC_TYPE;
    const int32_t num = 16;
    uint64_t data = 0xfedcba9876543210;
    TBuffAddr addr;
    addr.logicPos = 10;
    addr.bufferHandle = nullptr;
    addr.dataLen = num / 1;
    addr.bufferAddr = 0;
    addr.absAddr = (uint8_t*)&data;
    LocalTensor<uint8_t> tensor1(addr);
    tensor1.SetValue(0, 0);
    uint8_t value = tensor1.GetValue(0);
    EXPECT_EQ(value, 0);
    g_coreType = tmp;
}

/* ********************************* CreateTensor Fuzz ********************************* */
struct CreateTensorParams {
    std::string type;
    TPosition pos;
    uint32_t addr;
    uint32_t tileSize;
    std::vector<std::string> errStrGolden;
};

class TEST_CREATE_TENSOR : public testing::Test, public testing::WithParamInterface<CreateTensorParams> {
protected:
    void SetUp() {}
    void TearDown() { GlobalMockObject::verify(); }
};

#ifdef __DAV_C220_VEC__
INSTANTIATE_TEST_CASE_P(
    CREATE_TENSOR_TEST, TEST_CREATE_TENSOR,
    ::testing::Values(
        CreateTensorParams{
            "float",
            TPosition::VECIN,
            0x1,
            1200000,
            {"addr input is 1, which shoule be 32 bytes align",
             "tensor size input is 1200000, which shoule be in range (0, 65536]",
             "addr input is 1, tensor length is 4800000 bytes, which exceeds max len 262144 bytes"}},
        CreateTensorParams{
            "int4b_t",
            TPosition::VECIN,
            31,
            1200001,
            {"addr input is 31, which shoule be 32 bytes align",
             "tensor size input is 1200001, which shoule be even number in range (0, 524288]",
             "addr input is 31, tensor length is 600000 bytes, which exceeds max len 262144 bytes"}}));

TEST_P(TEST_CREATE_TENSOR, TestCreateTensor)
{
    int32_t tmp = g_coreType;
    g_coreType = AscendC::AIC_TYPE;

    MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStubCreateTensor));
    static uint32_t count = 0;
    std::string fileName = "TestCreateTensor" + std::to_string(getpid()) + "_" + std::to_string(count) + ".txt";
    freopen(fileName.c_str(), "w", stdout);
    auto param = GetParam();
    AscendC::TPosition pos = param.pos;
    uint32_t addr = param.addr;
    uint32_t tileSize = param.tileSize;
    if (param.type == "int4b_t") {
        LocalTensor<int4b_t> tensor1;
        tensor1.CreateTensor<int4b_t>(pos, addr, tileSize);
        EXPECT_EQ(tensor1.GetSize(), param.tileSize / 2 * 2);
        EXPECT_EQ(tensor1.GetLength(), param.tileSize / 2);
        EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(param.pos));
    } else if (param.type == "float") {
        LocalTensor<float> tensor1;
        tensor1.CreateTensor<float>(pos, addr, tileSize);
        EXPECT_EQ(tensor1.GetSize(), param.tileSize);
        EXPECT_EQ(tensor1.GetLength(), param.tileSize * sizeof(float));
        EXPECT_EQ(tensor1.GetPosition(), static_cast<int32_t>(param.pos));
    }
    // 恢复printf
    fclose(stdout);
    freopen("/dev/tty", "w", stdout);
    freopen("/dev/tty", "r", stdin);
    count++;

    // 校验文本中的log信息包含真值里的字符串
    std::ifstream resultFile(fileName, std::ios::in);
    std::stringstream streambuffer;
    streambuffer << resultFile.rdbuf();
    std::string resultString(streambuffer.str());
    resultFile.close();
    EXPECT_EQ(remove(fileName.c_str()), 0);
    for (int i = 0; i < param.errStrGolden.size(); i++) {
        EXPECT_TRUE(resultString.find(param.errStrGolden[i]) != std::string::npos);
    }
    g_coreType = tmp;
}
#endif
