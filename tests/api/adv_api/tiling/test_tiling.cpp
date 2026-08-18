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
#include "graph/tensor.h"
#include <dlfcn.h>
#include <csignal>
#define private public
#define protected public
#include "include/adv_api/activation/softmax_tiling.h"
#include "tiling_api.h"
#include "platform_stub.h"
#include "impl/adv_api/tiling/matmul/math_util.h"
#include "impl/adv_api/tiling/matmul/matmul_tiling_algorithm.h"
#include "impl/adv_api/detail/host_log.h"
#include "tiling/platform/platform_ascendc.h"
using namespace AscendC;
using namespace ge;
using namespace std;
using namespace matmul_tiling;

class TestTiling : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() {}
};
extern void platfrom_stub_set_chip_version(const char* num);
extern void platfrom_stub_set_npuarch(const char* num);

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
TEST_F(TestTiling, testTransDataTilingUnalignedHw)
{
    uint32_t maxSize;
    uint32_t minSize;
    int32_t n = 16;
    int32_t c = 16;
    int32_t d = 3;
    int32_t h = 3;
    int32_t w = 3;
    int32_t c0 = 16;
    int32_t n0 = 16;
    int32_t c1 = (c + c0 - 1) / c0;
    int32_t n1 = (n + n0 - 1) / n0;
    int32_t hw0 = 16;
    int32_t hw1 = (h * w + hw0 - 1) / hw0;
    auto ncdhwShape = ge::Shape({n, c, d, h, w});
    auto ndc1hwc0Shape = ge::Shape({n, d, c1, h, w, c0});
    auto fractalzShape = ge::Shape({d, c1, h, w, n1, n0, c0});
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    TransDataConfig config = {DataFormat::NCDHW, DataFormat::NDC1HWC0};
    bool ret =
        GetTransDataMaxMinTmpSize(plat, ncdhwShape, ndc1hwc0Shape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 1632);
    EXPECT_EQ(minSize, 1632);

    config = {DataFormat::NDC1HWC0, DataFormat::NCDHW};
    ret =
        GetTransDataMaxMinTmpSize(plat, ndc1hwc0Shape, ncdhwShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 2048);
    EXPECT_EQ(minSize, 2048);

    config = {DataFormat::NCDHW, DataFormat::FRACTAL_Z_3D};
    ret =
        GetTransDataMaxMinTmpSize(plat, ncdhwShape, fractalzShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 26112);
    EXPECT_EQ(minSize, 26112);

    config = {DataFormat::FRACTAL_Z_3D, DataFormat::NCDHW};
    ret =
        GetTransDataMaxMinTmpSize(plat, fractalzShape, ncdhwShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, n1 * n0 * c1 * c0 * d * hw0 * hw1 * 2);
    EXPECT_EQ(minSize, n1 * n0 * c1 * c0 * d * hw0 * hw1 * 2);
}

TEST_F(TestTiling, testTransDataTilingAlignedHw)
{
    uint32_t maxSize;
    uint32_t minSize;
    int32_t n = 5;
    int32_t c = 30;
    int32_t d = 2;
    int32_t h = 4;
    int32_t w = 8;
    int32_t c0 = 16;
    int32_t n0 = 16;
    int32_t c1 = (c + c0 - 1) / c0;
    int32_t n1 = (n + n0 - 1) / n0;
    int32_t hw0 = 16;
    int32_t hw1 = (h * w + hw0 - 1) / hw0;
    auto ncdhwShape = ge::Shape({n, c, d, h, w});
    auto ndc1hwc0Shape = ge::Shape({n, d, c1, h, w, c0});
    auto fractalzShape = ge::Shape({d, c1, h, w, n1, n0, c0});
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    TransDataConfig config = {DataFormat::NCDHW, DataFormat::NDC1HWC0};
    bool ret =
        GetTransDataMaxMinTmpSize(plat, ncdhwShape, ndc1hwc0Shape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 4224);
    EXPECT_EQ(minSize, 4224);

    config = {DataFormat::NDC1HWC0, DataFormat::NCDHW};
    ret =
        GetTransDataMaxMinTmpSize(plat, ndc1hwc0Shape, ncdhwShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 4608);
    EXPECT_EQ(minSize, 4608);

    config = {DataFormat::NCDHW, DataFormat::FRACTAL_Z_3D};
    ret =
        GetTransDataMaxMinTmpSize(plat, ncdhwShape, fractalzShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 69376);
    EXPECT_EQ(minSize, 69376);

    config = {DataFormat::FRACTAL_Z_3D, DataFormat::NCDHW};
    ret =
        GetTransDataMaxMinTmpSize(plat, fractalzShape, ncdhwShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, n1 * n0 * c1 * c0 * d * hw0 * hw1 * 2 * 2);
    EXPECT_EQ(minSize, n1 * n0 * c1 * c0 * d * hw0 * hw1 * 2 * 2);
}

TEST_F(TestTiling, TestLgammaTiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxSize;
    uint32_t minSize;

    GetLgammaMaxMinTmpSize(shape, 2, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 128 * 128 * 4);
    EXPECT_EQ(minSize, 128 * 128 * 4);

    GetLgammaMaxMinTmpSize(shape, 4, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 128 * 128 * 4);
    EXPECT_EQ(minSize, 128 * 128 * 4);

    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetLgammaTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 13);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestRintTiling)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto RintShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetRintMaxMinTmpSize(plat, RintShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetRintTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetRintMaxMinTmpSize(plat, RintShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetRintTmpBufferFactorSize(plat, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestBitwiseAndTiling)
{
    fe::PlatFormInfos platformInfo;
    uint32_t typeSize = 2u;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto BitwiseAndShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetBitwiseAndMaxMinTmpSize(plat, BitwiseAndShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetBitwiseAndTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetBitwiseAndMaxMinTmpSize(plat, BitwiseAndShape, typeSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetBitwiseAndTmpBufferFactorSize(plat, typeSize, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestLogicalAndTiling)
{
    fe::PlatFormInfos platformInfo;
    uint32_t typeSize = 2u;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto LogicalAndShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetLogicalAndMaxMinTmpSize(plat, LogicalAndShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetLogicalAndTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetLogicalAndMaxMinTmpSize(plat, LogicalAndShape, typeSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetLogicalAndTmpBufferFactorSize(plat, typeSize, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestLogicalAndsTiling)
{
    fe::PlatFormInfos platformInfo;
    uint32_t typeSize = 2u;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto LogicalAndsShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetLogicalAndsMaxMinTmpSize(plat, LogicalAndsShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetLogicalAndsTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetLogicalAndsMaxMinTmpSize(plat, LogicalAndsShape, typeSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetLogicalAndsTmpBufferFactorSize(plat, typeSize, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestDigammaTilingFp32)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxSize;
    uint32_t minSize;

    GetDigammaMaxMinTmpSize(shape, 4, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    GetDigammaMaxMinTmpSize(shape, 4, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shapeDims = {8};
    shape = ge::Shape(shapeDims);
    GetDigammaMaxMinTmpSize(shape, 4, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    GetDigammaMaxMinTmpSize(shape, 4, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetDigammaTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestDigammaTilingHalf)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxSize;
    uint32_t minSize;

    GetDigammaMaxMinTmpSize(shape, 2, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 131072);
    EXPECT_EQ(minSize, 131072);

    shapeDims = {8};
    shape = ge::Shape(shapeDims);
    GetDigammaMaxMinTmpSize(shape, 2, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 1024);
    EXPECT_EQ(minSize, 1024);

    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetDigammaTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestLogicalOrTiling)
{
    fe::PlatFormInfos platformInfo;
    uint32_t typeSize = 2u;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto LogicalOrShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetLogicalOrMaxMinTmpSize(plat, LogicalOrShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetLogicalOrTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetLogicalOrMaxMinTmpSize(plat, LogicalOrShape, typeSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetLogicalOrTmpBufferFactorSize(plat, typeSize, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestLogicalOrsTiling)
{
    fe::PlatFormInfos platformInfo;
    uint32_t typeSize = 2u;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto LogicalOrsShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetLogicalOrsMaxMinTmpSize(plat, LogicalOrsShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetLogicalOrsTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetLogicalOrsMaxMinTmpSize(plat, LogicalOrsShape, typeSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetLogicalOrsTmpBufferFactorSize(plat, typeSize, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestLogicalNotTiling)
{
    fe::PlatFormInfos platformInfo;
    uint32_t typeSize = 2u;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto LogicalNotShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetLogicalNotMaxMinTmpSize(plat, LogicalNotShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetLogicalNotTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetLogicalNotMaxMinTmpSize(plat, LogicalNotShape, typeSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetLogicalNotTmpBufferFactorSize(plat, typeSize, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestIsNanTiling)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto IsNanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetIsNanMaxMinTmpSize(plat, IsNanShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetIsNanTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetIsNanMaxMinTmpSize(plat, IsNanShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetIsNanTmpBufferFactorSize(plat, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestIsInfTiling)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto IsInfShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetIsInfMaxMinTmpSize(plat, IsInfShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetIsInfTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetIsInfMaxMinTmpSize(plat, IsInfShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetIsInfTmpBufferFactorSize(plat, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestFmaTiling)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto FmaShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetFmaMaxMinTmpSize(plat, FmaShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetFmaTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetFmaMaxMinTmpSize(plat, FmaShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetFmaTmpBufferFactorSize(plat, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSinCosTiling)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto SinCosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetSinCosMaxMinTmpSize(plat, SinCosShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 128 * 128 * 2 * 4 + 32);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 4 + 32);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetSinCosTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 2);
    EXPECT_EQ(extraBuf, 32);

    GetSinCosMaxMinTmpSize(plat, SinCosShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 128 * 128 * 2 * 4 + 32);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 4 + 32);

    GetSinCosTmpBufferFactorSize(plat, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestWhereTiling)
{
    gert::TilingContext* context = fe::GetFakeTilingContext();
    auto plat = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    std::vector<int64_t> shapeDims = {128, 128};
    auto WhereShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetWhereMaxMinTmpSize(plat, WhereShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetWhereTmpBufferFactorSize(plat, sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);

    GetWhereMaxMinTmpSize(plat, WhereShape, sizeof(half), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    GetWhereTmpBufferFactorSize(plat, sizeof(half), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSoftMaxFlashV3Tiling)
{
    std::vector<int64_t> shapeDims = {8, 1024};
    optiling::SoftMaxTiling tilingData;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t maxSumTypeSize = 4;
    uint32_t inputTypeSize = 2;
    uint32_t softmaxflashV3NeedMinLength = 0;
    uint32_t softmaxflashV3NeedMaxLength = 0;
    GetSoftMaxFlashV3MaxMinTmpSize(
        softmaxShape, inputTypeSize, maxSumTypeSize, softmaxflashV3NeedMaxLength, softmaxflashV3NeedMinLength, false,
        false);

    EXPECT_EQ(softmaxflashV3NeedMinLength, (8 * 64 + 8 * 1024) * 4);
    EXPECT_EQ(softmaxflashV3NeedMaxLength, (8 * 64 + 8 * 1024) * 4);

    softmaxflashV3NeedMinLength = 0;
    softmaxflashV3NeedMaxLength = 0;
    GetSoftMaxFlashV3MaxMinTmpSize(
        softmaxShape, inputTypeSize, maxSumTypeSize, softmaxflashV3NeedMaxLength, softmaxflashV3NeedMinLength, true,
        false);

    EXPECT_EQ(softmaxflashV3NeedMinLength, (8 * 64 + 8 * 1024 + 8 * 8) * 4);
    EXPECT_EQ(softmaxflashV3NeedMaxLength, (8 * 64 + 8 * 1024 + 8 * 8) * 4);

    uint32_t workLength = 76 * 1024;
    SoftMaxFlashV3TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 8);

    AscendC::tiling::SoftMaxTiling tilingDataNotOp;
    SoftMaxFlashV3TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, true, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 8);
}

TEST_F(TestTiling, TestSoftMaxTiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    optiling::SoftMaxTiling tilingData;
    AscendC::tiling::SoftMaxTiling tilingDataNotOp;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t softmaxTmpSize = 100 * 1024 * 4;
    uint32_t softmaxNeedMinSize = GetSoftMaxMinTmpSize(softmaxShape, 2, true);
    EXPECT_EQ(softmaxNeedMinSize, 128 * (16 + 128) * 4);
    uint32_t softmaxFlashNeedMinSize = GetSoftMaxFlashMinTmpSize(softmaxShape, 2, true, true);
    EXPECT_EQ(softmaxFlashNeedMinSize, 128 * (16 + 128) * 4);
    softmaxFlashNeedMinSize = GetSoftMaxFlashMinTmpSize(softmaxShape, 4, true, true);
    EXPECT_EQ(softmaxFlashNeedMinSize, 128 * (8 + 128) * 4);
    softmaxFlashNeedMinSize = GetSoftMaxFlashMinTmpSize(softmaxShape, 4, false, true);
    EXPECT_EQ(softmaxFlashNeedMinSize, (128 * (8 + 128) + 64 * 4 + 128) * 4);
    uint32_t softmaxGradNeedMinSize = GetSoftMaxGradMinTmpSize(softmaxShape, 2, true, true);
    EXPECT_EQ(softmaxGradNeedMinSize, 128 * 4);
    softmaxGradNeedMinSize = GetSoftMaxGradMinTmpSize(softmaxShape, 4, true, true);
    EXPECT_EQ(softmaxGradNeedMinSize, 128 * 4);
    softmaxGradNeedMinSize = GetSoftMaxGradMinTmpSize(softmaxShape, 0, true, true);
    EXPECT_EQ(softmaxGradNeedMinSize, 0);

    uint32_t softmaxNeedMaxSize = GetSoftMaxMaxTmpSize(softmaxShape, 2, true);
    EXPECT_EQ(softmaxNeedMaxSize, 128 * (16 + 128 + 64) * 4);
    softmaxNeedMaxSize = GetSoftMaxMaxTmpSize(softmaxShape, 1, true);
    EXPECT_EQ(softmaxNeedMaxSize, 0);
    uint32_t softmaxFlashNeedMaxSize = GetSoftMaxFlashMaxTmpSize(softmaxShape, 2, true, true);
    EXPECT_EQ(softmaxFlashNeedMaxSize, 128 * (16 + 128 + 64) * 4);
    softmaxFlashNeedMaxSize = GetSoftMaxFlashMaxTmpSize(softmaxShape, 4, false, true);
    EXPECT_EQ(softmaxFlashNeedMaxSize, 128 * (8 + 128 + 64) * 4);
    softmaxFlashNeedMaxSize = GetSoftMaxFlashMaxTmpSize(softmaxShape, 4, true, true);
    EXPECT_EQ(softmaxFlashNeedMaxSize, 128 * (8 + 128 + 64) * 4);
    softmaxFlashNeedMaxSize = GetSoftMaxFlashMaxTmpSize(softmaxShape, 1, true, true);
    EXPECT_EQ(softmaxFlashNeedMaxSize, 0);
    uint32_t softmaxGradNeedMaxSize = GetSoftMaxGradMaxTmpSize(softmaxShape, 2, true, true);
    EXPECT_EQ(softmaxGradNeedMaxSize, 128 * (16 * 2 + 128 * 3 + 64) * 4);
    softmaxGradNeedMaxSize = GetSoftMaxGradMaxTmpSize(softmaxShape, 4, true, true);
    EXPECT_EQ(softmaxGradNeedMaxSize, 128 * (8 + 128 + 64) * 4);
    softmaxGradNeedMaxSize = GetSoftMaxGradMaxTmpSize(softmaxShape, 1, true, true);
    EXPECT_EQ(softmaxGradNeedMaxSize, 0);
    SoftMaxTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingData);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    bool flag = IsBasicBlockInSoftMax(tilingData);
    EXPECT_EQ(flag, true);
    SoftMaxFlashTilingFunc(softmaxShape, 2, 77952, tilingData, true);
    EXPECT_EQ(tilingData.get_reduceM(), 32);
    SoftMaxFlashTilingFunc(softmaxShape, 2, 77952, tilingData, false);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    SoftMaxGradTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingData, false);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    SoftMaxGradTilingFunc(softmaxShape, 4, softmaxTmpSize, tilingData, false);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    SoftMaxGradTilingFunc(softmaxShape, 2, 133120, tilingData, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);

    SoftMaxTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
    flag = IsBasicBlockInSoftMax(tilingDataNotOp);
    EXPECT_EQ(flag, true);
    SoftMaxFlashTilingFunc(softmaxShape, 2, 77952, tilingDataNotOp, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 32);
    SoftMaxFlashTilingFunc(softmaxShape, 2, 77952, tilingDataNotOp, false);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
    SoftMaxGradTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingDataNotOp, false);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
    SoftMaxGradTilingFunc(softmaxShape, 4, softmaxTmpSize, tilingDataNotOp, false);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
    SoftMaxGradTilingFunc(softmaxShape, 2, 133120, tilingDataNotOp, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
}

TEST_F(TestTiling, TestSoftMaxFlashV2TilingMaxMinTmpSize)
{
    uint32_t softmaxflashV2NeedMinLength = 0;
    uint32_t softmaxflashV2NeedMaxLength = 0;

    std::vector<int64_t> shapeDims = {3, 3, 448};
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t dataTypeSize1 = 2;
    uint32_t dataTypeSize2 = 2;
    uint32_t isUpdate = 0;
    uint32_t isBasicBlock = 0;
    uint32_t isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 17504);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 19008);

    shapeDims = {7, 1072};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 31296);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 32256);

    shapeDims = {1, 2, 3, 1, 2, 1, 16};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 2240);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 4608);

    shapeDims = {2, 6, 1, 16};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 2240);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 4608);

    shapeDims = {6, 1664};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 41184);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 41856);

    shapeDims = {2, 1760};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 15200);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 15200);

    shapeDims = {1, 5536};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 23232);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 23232);
    dataTypeSize2 = 4;
    isUpdate = 1;
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 22752);
    dataTypeSize1 = 4;
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 320);

    shapeDims = {2, 2, 2352};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 38816);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 38912);

    shapeDims = {2, 2, 2, 480};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 16672);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 17920);

    dataTypeSize1 = 4;
    dataTypeSize2 = 4;
    isUpdate = 1;
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 2304);

    shapeDims = {2, 3632};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 1;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 29824);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 29824);

    shapeDims = {2, 4, 96};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 1;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 4608);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 6144);

    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, dataTypeSize1, 1, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 0);

    softmaxflashV2NeedMaxLength =
        GetSoftMaxFlashV2MaxTmpSize(softmaxShape, 1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 0);

    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, 1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 0);

    softmaxflashV2NeedMaxLength =
        GetSoftMaxFlashV2MaxTmpSize(softmaxShape, dataTypeSize1, 1, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 0);
}

TEST_F(TestTiling, TestSoftMaxFlashV2Tiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    optiling::SoftMaxTiling tilingData;
    AscendC::tiling::SoftMaxTiling tilingDataNotOp;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t maxSumTypeSize = 2;
    uint32_t inputTypeSize = 2;
    uint32_t softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, false);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 128 * (16 + 128) * 4);
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 * 2 + 128 * (128 + 16 * 2)) * 4);
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 128 * (128 + 16) * 4);

    uint32_t softmaxflashV2NeedMaxLength =
        GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, false);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 128 * (128 + 64 + 16) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 128 * (128 + 64 + 16 * 2) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 128 * (128 + 64 + 16) * 4);

    maxSumTypeSize = 4;
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 * 2 + 128 * (128 + 16 + 8)) * 4);
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 128 * (128 + 8) * 4);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 128 * (128 + 64 + 8 * 2) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 128 * (128 + 64 + 8) * 4);

    uint32_t workLength = 100 * 1024;
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, false, false);
    EXPECT_EQ(tilingData.get_reduceM(), 120);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, false, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, false);
    EXPECT_EQ(tilingData.get_reduceM(), 120);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);

    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, false, false);
    EXPECT_EQ(tilingDataNotOp.reduceM, 120);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, false, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, true, false);
    EXPECT_EQ(tilingDataNotOp.reduceM, 120);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, true, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);

    inputTypeSize = 4;
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 128 * (16)) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 128 * (64 + 8) * 4);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);

    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, true, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
}

TEST_F(TestTiling, TestSoftMaxFlashV2TilingBasicBlock)
{
    std::vector<int64_t> shapeDims = {8, 1024};
    optiling::SoftMaxTiling tilingData;
    AscendC::tiling::SoftMaxTiling tilingDataNotOp;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t maxSumTypeSize = 4;
    uint32_t inputTypeSize = 4;
    uint32_t softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (64 + 8 * (16)) * 4);
    uint32_t softmaxflashV2NeedMaxLength =
        GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 8 * (8 + 64) * 4);

    uint32_t workLength = 32 * 1024;
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 8);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, true, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 8);

    inputTypeSize = 2;
    workLength = 64 * 1024;
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 8 * (16 + 1024 + 8)) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 8 * (8 + 1024 + 64) * 4);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 8);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, true, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 8);
}

TEST_F(TestTiling, TestLogSoftMaxTiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    optiling::LogSoftMaxTiling tilingData;
    AscendC::tiling::LogSoftMaxTiling tilingDataNotOp;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t softmaxTmpSize = 100 * 1024 * 4;
    uint32_t softmaxNeedMinSize = GetLogSoftMaxMinTmpSize(softmaxShape, 2, true);
    EXPECT_EQ(softmaxNeedMinSize, 128 * (16 + 128) * 4);
    uint32_t softmaxNeedMaxSize = GetLogSoftMaxMaxTmpSize(softmaxShape, 2, true);
    EXPECT_EQ(softmaxNeedMaxSize, 128 * (16 + 128 + 64) * 4);

    LogSoftMaxTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingData);
    EXPECT_EQ(tilingData.get_reduceM(), 64);

    LogSoftMaxTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
}

TEST_F(TestTiling, TestWelfordUpdateTiling)
{
    std::vector<int64_t> shapeDims1d = {1, 128};
    auto shape1d = ge::Shape(shapeDims1d);
    uint32_t maxSize = 0;
    uint32_t minSize = 0;
    uint32_t dtypeTSize = sizeof(half);
    uint32_t dtypeUSize = sizeof(float);
    bool isReuseSource = false;
    GetWelfordUpdateMaxMinTmpSize(shape1d, dtypeTSize, dtypeUSize, isReuseSource, false, maxSize, minSize);
    EXPECT_EQ(minSize, 0);
    EXPECT_EQ(maxSize, 0);
}

TEST_F(TestTiling, TestWelfordFinalizeTiling)
{
    std::vector<int64_t> shapeDims1d = {64};
    auto shape1d = ge::Shape(shapeDims1d);
    uint32_t maxSize = 0;
    uint32_t minSize = 0;
    uint32_t dtypeSize = sizeof(float);
    bool isReuseSource = false;
    GetWelfordFinalizeMaxMinTmpSize(shape1d, dtypeSize, isReuseSource, maxSize, minSize);
    EXPECT_EQ(minSize, 768);
    EXPECT_EQ(maxSize, 768);
}

TEST_F(TestTiling, TestLayerNormMaxMinTmpSize_ASCEND950)
{
    const uint32_t stackBufferSize = 0;
    const uint32_t typeSize = sizeof(float);
    std::vector<int64_t> shapeDims = {128, 88, 88};
    auto layernormShape = ge::Shape(shapeDims);
    bool isReuseSource = false;
    bool isComputeRstd = false;
    bool isOnlyOutput = false;
    uint32_t minValue = 100;
    uint32_t maxValue = 100;
    // requires extra tmp space for temporary rstd results
    uint32_t rLength = static_cast<uint32_t>(shapeDims[1]);
    uint32_t rLengthWithPadding = (rLength + 64 - 1) / 64 * 64;
    uint32_t aLength = static_cast<uint32_t>(shapeDims[0]);
    // round rLengthWithPadding to the nearest 128 and include varianceLength and rstdLength
    uint32_t totalLength = (rLengthWithPadding / 64 + 128 - 1) / 128 * 128 + aLength * 2;
    GetLayerNormMaxMinTmpSize(layernormShape, typeSize, isReuseSource, isComputeRstd, isOnlyOutput, maxValue, minValue);
    EXPECT_EQ(maxValue, totalLength * typeSize);
    EXPECT_EQ(minValue, totalLength * typeSize);
}

TEST_F(TestTiling, TestLayerNormMaxMinTmpSize_ASCEND950_OutputRstd)
{
    const uint32_t stackBufferSize = 0;
    const uint32_t typeSize = sizeof(float);
    std::vector<int64_t> shapeDims = {128, 88, 88};
    auto layernormShape = ge::Shape(shapeDims);
    bool isReuseSource = false;
    bool isComputeRstd = true;
    bool isOnlyOutput = false;
    uint32_t minValue = 100;
    uint32_t maxValue = 100;
    // round rLength to the nearest 64
    uint32_t rLength = static_cast<uint32_t>(shapeDims[1]);
    uint32_t rLengthWithPadding = (rLength + 64 - 1) / 64 * 64;
    uint32_t aLength = static_cast<uint32_t>(shapeDims[0]);
    // round rLengthWithPadding to the nearest 128 and include varianceLength
    uint32_t totalLength = (rLengthWithPadding / 64 + 128 - 1) / 128 * 128 + aLength;
    GetLayerNormMaxMinTmpSize(layernormShape, typeSize, isReuseSource, isComputeRstd, isOnlyOutput, maxValue, minValue);
    EXPECT_EQ(maxValue, totalLength * typeSize);
    EXPECT_EQ(minValue, totalLength * typeSize);
}

TEST_F(TestTiling, TestLayerNormRstdTiling)
{
    const uint32_t stackBufferSize = 100 * 1024;
    const uint32_t typeSize = sizeof(float);
    std::vector<int64_t> shapeDims = {128, 88};
    auto layernormShape = ge::Shape(shapeDims);
    bool isReuseSource = false;
    bool isComputeRstd = true;
    bool isOnlyOutput = false;
    optiling::LayerNormSeparateTiling tiling;
    AscendC::tiling::LayerNormSeparateTiling tilingNotOp;
    uint32_t minValue = 0;
    uint32_t maxValue = 0;
    GetLayerNormMaxMinTmpSize(layernormShape, typeSize, isReuseSource, isComputeRstd, isOnlyOutput, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * typeSize + 128 * typeSize);
    EXPECT_EQ(minValue, 128 * typeSize + 128 * typeSize);
    GetNormalizeMaxMinTmpSize(
        layernormShape, typeSize, typeSize, isReuseSource, isComputeRstd, isOnlyOutput, maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);
    GetLayerNormNDTilingInfo(layernormShape, stackBufferSize, typeSize, isReuseSource, isComputeRstd, tiling);
    GetLayerNormNDTilingInfo(layernormShape, 0, typeSize, isReuseSource, isComputeRstd, tiling);
    EXPECT_EQ(tiling.get_rLength(), 88);
    EXPECT_EQ(tiling.get_rHeadLength(), 64);
    GetLayerNormNDTilingInfo(layernormShape, stackBufferSize, typeSize, isReuseSource, isComputeRstd, tilingNotOp);
    GetLayerNormNDTilingInfo(layernormShape, 0, typeSize, isReuseSource, isComputeRstd, tilingNotOp);
    EXPECT_EQ(tilingNotOp.rLength, 88);
    EXPECT_EQ(tilingNotOp.rHeadLength, 64);
}

TEST_F(TestTiling, TestLayernormGradTiling1982)
{
    std::vector<int64_t> shapeDims = {128, 128, 128, 128, 128, 128};
    auto layernormgradShape = ge::Shape(shapeDims);
    optiling::LayerNormGradTiling tiling;

    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    uint32_t maxMinValueNoReuse = 128 * 2 * 4;
    uint32_t maxMinValueReuse = 0;
    AscendC::GetLayerNormGradMaxMinTmpSize(layernormgradShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, maxMinValueNoReuse);
    EXPECT_EQ(minValue, maxMinValueNoReuse);

    AscendC::GetLayerNormGradMaxMinTmpSize(layernormgradShape, 4, true, maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);

    AscendC::GetLayerNormGradMaxMinTmpSize(layernormgradShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, maxMinValueNoReuse);
    EXPECT_EQ(minValue, maxMinValueNoReuse);
}

TEST_F(TestTiling, TestAntiquantTilingNoTransposeFP4)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1, 5120};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = false;
    uint32_t maxValue;
    uint32_t minValue;
    GetAscendAntiQuantMaxMinTmpSize(
        srcShape, offsetShape, isTranspose, ge::DT_FLOAT4_E2M1, ge::DT_FLOAT16, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);
    uint32_t maxLiveNodeCnt = 1;
    uint32_t extraBuf = 1;
    GetAscendAntiQuantTmpBufferFactorSize(
        srcShape, offsetShape, isTranspose, ge::DT_FLOAT4_E2M1, ge::DT_FLOAT16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAntiquantTilingTransposeFP4)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1, 5120};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = true;
    uint32_t maxValue;
    uint32_t minValue;
    GetAscendAntiQuantMaxMinTmpSize(
        srcShape, offsetShape, isTranspose, ge::DT_FLOAT4_E2M1, ge::DT_FLOAT16, maxValue, minValue);
    EXPECT_EQ(minValue, 10240);
    EXPECT_EQ(maxValue, 10240);
    uint32_t maxLiveNodeCnt = 1;
    uint32_t extraBuf = 1;
    GetAscendAntiQuantTmpBufferFactorSize(
        srcShape, offsetShape, isTranspose, ge::DT_FLOAT4_E2M1, ge::DT_FLOAT16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 10240);
}

TEST_F(TestTiling, TestAntiquantizeTilingNoTransposeFP4)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1, 5120};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = false;
    uint32_t maxValue;
    uint32_t minValue;
    GetAntiQuantizeMaxMinTmpSize(
        srcShape, offsetShape, isTranspose, ge::DT_FLOAT4_E2M1, ge::DT_FLOAT16, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);
    uint32_t maxLiveNodeCnt = 1;
    uint32_t extraBuf = 1;
    GetAntiQuantizeTmpBufferFactorSize(
        srcShape, offsetShape, ge::DT_FLOAT4_E2M1, ge::DT_FLOAT16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestDequantizeTiling)
{
    // 2d input shape
    std::vector<int64_t> shape_dims = {10, 32};
    auto shape = ge::Shape(shape_dims);
    uint32_t maxValue;
    uint32_t minValue;

    GetDequantizeMaxMinTmpSize(shape, 2, maxValue, minValue);
    EXPECT_EQ(minValue, 4 * (64 + 32 + 40));
    EXPECT_EQ(maxValue, 4 * (64 + 32 * 10 + 40));

    uint32_t maxLivedNodesCnt = 0;
    uint32_t extraBuf = 1;
    GetDequantizeTmpBufferFactorSize(shape, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(maxLivedNodesCnt, 1);
    EXPECT_EQ(extraBuf, 104);

    // 1d input shape
    std::vector<int64_t> shape_dims_1d = {320};
    auto shape_1d = ge::Shape(shape_dims_1d);

    GetDequantizeMaxMinTmpSize(shape_1d, 2, maxValue, minValue);
    EXPECT_EQ(minValue, 4 * (64 + 1 * 320 + 328));
    EXPECT_EQ(maxValue, 4 * (64 + 1 * 320 + 328));

    GetDequantizeTmpBufferFactorSize(shape_1d, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(maxLivedNodesCnt, 2);
    EXPECT_EQ(extraBuf, 72);

    std::vector<int64_t> shape3_dims = {10, 32, 3};
    auto shape_3d = ge::Shape(shape3_dims);
    GetDequantizeTmpBufferFactorSize(shape_3d, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(maxLivedNodesCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAntiquantizeTilingNoTransposePerChannelHalf)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1, 5120};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = false;
    uint32_t maxValue;
    uint32_t minValue;
    GetAntiQuantizeMaxMinTmpSize(srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_FLOAT16, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt = 1;
    uint32_t extraBuf = 1;
    GetAntiQuantizeTmpBufferFactorSize(srcShape, offsetShape, ge::DT_INT8, ge::DT_FLOAT16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAntiquantizeTilingNoTransposePerChannel)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1, 5120};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = false;
    uint32_t maxValue;
    uint32_t minValue;
    GetAntiQuantizeMaxMinTmpSize(srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt = 1;
    uint32_t extraBuf = 0;
    GetAntiQuantizeTmpBufferFactorSize(srcShape, offsetShape, ge::DT_INT8, ge::DT_BF16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAntiquantizeTilingNoTransposePerTensor)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = false;
    uint32_t maxValue;
    uint32_t minValue;
    GetAntiQuantizeMaxMinTmpSize(srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt = 1;
    uint32_t extraBuf = 1;
    GetAntiQuantizeTmpBufferFactorSize(srcShape, offsetShape, ge::DT_INT8, ge::DT_BF16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, testAdvanceSortTilingOnlyDataOutput)
{
    std::vector<int64_t> shapeDims = {32, 32};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_INT16;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = false;
    bool hasDstIndex = false;
    bool isReuseSource = false;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 9728);
    EXPECT_EQ(minValue, 9728);
}

TEST_F(TestTiling, testAdvanceSortTilingOnlyDataOutputB8)
{
    std::vector<int64_t> shapeDims = {32, 32};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_UINT8;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = false;
    bool hasDstIndex = false;
    bool isReuseSource = false;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 7680);
    EXPECT_EQ(minValue, 7680);
}

TEST_F(TestTiling, testAdvanceSortTilingOnlyDataOutputB64)
{
    std::vector<int64_t> shapeDims = {32, 32};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_INT64;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = false;
    bool hasDstIndex = false;
    bool isReuseSource = false;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 15872);
    EXPECT_EQ(minValue, 15872);
}

TEST_F(TestTiling, testAdvanceSortTilingDescendOrder)
{
    std::vector<int64_t> shapeDims = {1023};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_UINT32;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = true;
    bool hasSrcIndex = false;
    bool hasDstIndex = false;
    bool isReuseSource = false;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 11776);
    EXPECT_EQ(minValue, 11776);
}

TEST_F(TestTiling, testAdvanceSortTilingWithExtraDstIndex)
{
    std::vector<int64_t> shapeDims = {300};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_FLOAT;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = false;
    bool hasDstIndex = true;
    bool isReuseSource = false;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 5312);
    EXPECT_EQ(minValue, 5312);
}

TEST_F(TestTiling, testAdvanceSortTilingWithExtraDstIndexForB8)
{
    std::vector<int64_t> shapeDims = {300};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_UINT8;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = false;
    bool hasDstIndex = true;
    bool isReuseSource = false;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 2112);
    EXPECT_EQ(minValue, 2112);
}

TEST_F(TestTiling, testAdvanceSortTilingWithBothSrcDstIndex)
{
    std::vector<int64_t> shapeDims = {4096};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_UINT16;
    ge::DataType indexType = ge::DT_UINT64;
    bool isDescend = false;
    bool hasSrcIndex = true;
    bool hasDstIndex = true;
    bool isReuseSource = false;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 70144);
    EXPECT_EQ(minValue, 70144);
}

TEST_F(TestTiling, testAdvanceSortTilingOnlyDataOutputReuseSource)
{
    std::vector<int64_t> shapeDims = {32, 32};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_INT16;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = false;
    bool hasDstIndex = false;
    bool isReuseSource = true;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 7680);
    EXPECT_EQ(minValue, 7680);
}

TEST_F(TestTiling, testAdvanceSortTilingDescendOrderReuseSource)
{
    std::vector<int64_t> shapeDims = {1023};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_UINT32;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = true;
    bool hasSrcIndex = false;
    bool hasDstIndex = false;
    bool isReuseSource = true;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 7680);
    EXPECT_EQ(minValue, 7680);
}

TEST_F(TestTiling, testAdvanceSortTilingWithExtraDstIndexReuseSource)
{
    std::vector<int64_t> shapeDims = {32, 32};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_INT32;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = false;
    bool hasDstIndex = true;
    bool isReuseSource = true;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 11776);
    EXPECT_EQ(minValue, 11776);
}

TEST_F(TestTiling, testAdvanceSortTilingWithBothSrcDstIndexReuseSource)
{
    std::vector<int64_t> shapeDims = {32, 32};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_INT16;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = true;
    bool hasDstIndex = true;
    bool isReuseSource = true;
    SortConfig config = {SortType::RADIX_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 7680);
    EXPECT_EQ(minValue, 7680);
}

TEST_F(TestTiling, testAdvanceSortTilingMergeSortHalf)
{
    std::vector<int64_t> shapeDims = {32, 32};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_FLOAT16;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = true;
    bool hasDstIndex = true;
    bool isReuseSource = true;
    SortConfig config = {SortType::MERGE_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 32 * 32 * 8);
    EXPECT_EQ(minValue, 32 * 32 * 8);
}

TEST_F(TestTiling, testAdvanceSortTilingMergeSortFloat)
{
    std::vector<int64_t> shapeDims = {32, 32};
    auto srcShape = ge::Shape(shapeDims);
    ge::DataType valueType = ge::DT_FLOAT;
    ge::DataType indexType = ge::DT_UINT32;
    bool isDescend = false;
    bool hasSrcIndex = true;
    bool hasDstIndex = true;
    bool isReuseSource = true;
    SortConfig config = {SortType::MERGE_SORT, isDescend, hasSrcIndex, hasDstIndex};
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetSortMaxMinTmpSize(srcShape, valueType, indexType, isReuseSource, config, maxValue, minValue);

    EXPECT_EQ(maxValue, 32 * 32 * 8);
    EXPECT_EQ(minValue, 32 * 32 * 8);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeNomal_isInitIndexTrue_Float_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = true;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 10;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 256);
    EXPECT_EQ(tilingData.get_allDataSize(), 64);
    EXPECT_EQ(tilingData.get_innerDataSize(), 128);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 16);
    EXPECT_EQ(tilingData.get_maskOffset(), 16);
    EXPECT_EQ(tilingData.get_maskVreducev2FourBytes(), 20);
    EXPECT_EQ(tilingData.get_maskVreducev2TwoBytes(), 40);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 1024);
    EXPECT_EQ(minValue, 1024);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 256);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 64);
    EXPECT_EQ(tilingDataNotOp.innerDataSize, 128);
    EXPECT_EQ(tilingDataNotOp.sortRepeat, 2);
    EXPECT_EQ(tilingDataNotOp.kAlignFourBytes, 16);
    EXPECT_EQ(tilingDataNotOp.kAlignTwoBytes, 16);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 16);
    EXPECT_EQ(tilingDataNotOp.maskVreducev2FourBytes, 20);
    EXPECT_EQ(tilingDataNotOp.maskVreducev2TwoBytes, 40);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeNomal_isInitIndexFalse_Float_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 10;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 320);
    EXPECT_EQ(tilingData.get_allDataSize(), 64);
    EXPECT_EQ(tilingData.get_innerDataSize(), 128);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 16);
    EXPECT_EQ(tilingData.get_maskOffset(), 16);
    EXPECT_EQ(tilingData.get_maskVreducev2FourBytes(), 20);
    EXPECT_EQ(tilingData.get_maskVreducev2TwoBytes(), 40);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 256);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, false, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 1280);
    EXPECT_EQ(minValue, 1280);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 320);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 64);
    EXPECT_EQ(tilingDataNotOp.innerDataSize, 128);
    EXPECT_EQ(tilingDataNotOp.sortRepeat, 2);
    EXPECT_EQ(tilingDataNotOp.kAlignFourBytes, 16);
    EXPECT_EQ(tilingDataNotOp.kAlignTwoBytes, 16);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 16);
    EXPECT_EQ(tilingDataNotOp.maskVreducev2FourBytes, 20);
    EXPECT_EQ(tilingDataNotOp.maskVreducev2TwoBytes, 40);
    EXPECT_EQ(tilingDataNotOp.srcIndexOffset, 256);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeNomal_isInitIndexTrue_Half_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = true;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 10;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 512);
    EXPECT_EQ(tilingData.get_allDataSize(), 64);
    EXPECT_EQ(tilingData.get_innerDataSize(), 256);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 16);
    EXPECT_EQ(tilingData.get_maskOffset(), 16);
    EXPECT_EQ(tilingData.get_maskVreducev2FourBytes(), 20);
    EXPECT_EQ(tilingData.get_maskVreducev2TwoBytes(), 40);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 1024);
    EXPECT_EQ(minValue, 1024);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 512);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 64);
    EXPECT_EQ(tilingDataNotOp.innerDataSize, 256);
    EXPECT_EQ(tilingDataNotOp.sortRepeat, 2);
    EXPECT_EQ(tilingDataNotOp.kAlignFourBytes, 16);
    EXPECT_EQ(tilingDataNotOp.kAlignTwoBytes, 16);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 16);
    EXPECT_EQ(tilingDataNotOp.maskVreducev2FourBytes, 20);
    EXPECT_EQ(tilingDataNotOp.maskVreducev2TwoBytes, 40);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeNomal_isInitIndexFalse_Half_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 10;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 640);
    EXPECT_EQ(tilingData.get_allDataSize(), 64);
    EXPECT_EQ(tilingData.get_innerDataSize(), 256);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 16);
    EXPECT_EQ(tilingData.get_maskOffset(), 16);
    EXPECT_EQ(tilingData.get_maskVreducev2FourBytes(), 20);
    EXPECT_EQ(tilingData.get_maskVreducev2TwoBytes(), 40);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 512);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, false, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 1280);
    EXPECT_EQ(minValue, 1280);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 640);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 64);
    EXPECT_EQ(tilingDataNotOp.innerDataSize, 256);
    EXPECT_EQ(tilingDataNotOp.sortRepeat, 2);
    EXPECT_EQ(tilingDataNotOp.kAlignFourBytes, 16);
    EXPECT_EQ(tilingDataNotOp.kAlignTwoBytes, 16);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 16);
    EXPECT_EQ(tilingDataNotOp.maskVreducev2FourBytes, 20);
    EXPECT_EQ(tilingDataNotOp.maskVreducev2TwoBytes, 40);
    EXPECT_EQ(tilingDataNotOp.srcIndexOffset, 512);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexTrue_Float_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = true;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, false, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 64);
    EXPECT_EQ(tilingData.get_maskOffset(), 10);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 256);
    EXPECT_EQ(minValue, 256);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, false, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 32);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 64);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 10);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexFalse_Float_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_maskOffset(), 10);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 96);

    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, false, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 384);
    EXPECT_EQ(minValue, 384);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 32);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 10);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 96);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexTrue_Half_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = true;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 128);
    EXPECT_EQ(tilingData.get_maskOffset(), 10);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, false, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 256);
    EXPECT_EQ(minValue, 256);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 32);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 128);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 10);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexFalse_Half_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, false, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_maskOffset(), 10);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 192);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 384);
    EXPECT_EQ(minValue, 384);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, false, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 32);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 10);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 192);
}

TEST_F(TestTiling, TestTopkTiling_DataTypeSize0_FAILED)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 0;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    auto res = TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, false, tilingData);
    EXPECT_EQ(res, false);

    auto resNotOp = TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, false, tilingDataNotOp);
    EXPECT_EQ(resNotOp, false);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexFalse_Half_k)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    int32_t k = 13;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_maskOffset(), 13);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 192);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 384);
    EXPECT_EQ(minValue, 384);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 32);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 13);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 192);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexFalse_Float_k32)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 32;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    AscendC::tiling::TopkTiling tilingDataNotOp;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_maskOffset(), 32);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 96);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 384);
    EXPECT_EQ(minValue, 384);

    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.allDataSize, 32);
    EXPECT_EQ(tilingDataNotOp.maskOffset, 32);
    EXPECT_EQ(tilingDataNotOp.tmpLocalSize, 96);
}

TEST_F(TestTiling, TestTopkTiling_RadixTopKModeSmall_isInitIndexFalse)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    ge::DataType valueType = ge::DT_INT16;
    bool isReuseSource = false;
    bool isInitIndex = false;
    TopKConfig config = {TopKAlgo::RADIX_SELECT, TopKOrder::UNSET, true};

    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetTopKMaxMinTmpSize(
        inner, outter, k, isReuseSource, isInitIndex, topkMode, true, valueType, config, maxValue, minValue);
    EXPECT_EQ(maxValue, 1696);
    EXPECT_EQ(minValue, 1696);
}

TEST_F(TestTiling, TestTopkTiling_RadixTopKModeNormal_isInitIndexFalse)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    ge::DataType valueType = ge::DT_INT16;
    bool isReuseSource = false;
    bool isInitIndex = false;
    TopKConfig config = {TopKAlgo::RADIX_SELECT, TopKOrder::UNSET, true};

    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetTopKMaxMinTmpSize(
        inner, outter, k, isReuseSource, isInitIndex, topkMode, true, valueType, config, maxValue, minValue);
    EXPECT_EQ(maxValue, 1696);
    EXPECT_EQ(minValue, 1696);
}

TEST_F(TestTiling, TestTopkTiling_RadixTopKModeNormal_isInitIndexTrue)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    ge::DataType valueType = ge::DT_INT16;
    bool isReuseSource = false;
    bool isInitIndex = true;
    TopKConfig config = {TopKAlgo::RADIX_SELECT, TopKOrder::UNSET, true};

    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetTopKMaxMinTmpSize(
        inner, outter, k, isReuseSource, isInitIndex, topkMode, true, valueType, config, maxValue, minValue);
    EXPECT_EQ(maxValue, 1568);
    EXPECT_EQ(minValue, 1568);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeNomal_isInitIndexFalse)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 10;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);

    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, false, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 1280);
    EXPECT_EQ(minValue, 1280);
}

TEST_F(TestTiling, TestPowerTiling)
{
    std::vector<int64_t> shapeDims = {1, 512};
    auto powerShape = ge::Shape(shapeDims);
    uint32_t maxVal;
    uint32_t minVal;

    GetPowerMaxMinTmpSize(powerShape, powerShape, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 3);
    EXPECT_EQ(maxVal, 512 * 4 * 3);
    GetPowerMaxMinTmpSize(powerShape, powerShape, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 0);
    EXPECT_EQ(minVal, 0);
    GetPowerMaxMinTmpSize(powerShape, powerShape, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 3);
    EXPECT_EQ(minVal, 512 * 4 * 3);

    std::vector<int64_t> scalar_shape = {1};
    auto scalarShape = ge::Shape(scalar_shape);
    GetPowerMaxMinTmpSize(powerShape, scalarShape, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 3);
    EXPECT_EQ(maxVal, 512 * 4 * 3);
    GetPowerMaxMinTmpSize(powerShape, scalarShape, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 0);
    EXPECT_EQ(minVal, 0);
    GetPowerMaxMinTmpSize(scalarShape, powerShape, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 3);
    EXPECT_EQ(minVal, 512 * 4 * 3);
}

TEST_F(TestTiling, TestPowerTilingFactorSize)
{
    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;

    GetPowerTmpBufferFactorSize(false, true, false, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 3);
    EXPECT_EQ(extraBuf, 0);

    GetPowerTmpBufferFactorSize(false, true, false, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 6);
    EXPECT_EQ(extraBuf, 0);

    GetPowerTmpBufferFactorSize(false, true, true, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestCosTilingFloatWithConfig)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto cosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;

    AscendC::CosConfig polyConfig = {AscendC::CosAlgo::POLYNOMIAL_APPROXIMATION};
    AscendC::CosConfig radinConfig = {AscendC::CosAlgo::RADIAN_REDUCTION};

    AscendC::GetCosMaxMinTmpSize(polyConfig, cosShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);
    AscendC::GetCosMaxMinTmpSize(radinConfig, cosShape, 4, true, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 4 + 32);

    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    GetCosTmpBufferFactorSize(polyConfig, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
    GetCosTmpBufferFactorSize(radinConfig, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 2);
    EXPECT_EQ(extraBuf, 32);
}

TEST_F(TestTiling, TestCosTilingHalfWithConfig)
{
    std::vector<int64_t> shapeDims = {512};
    auto cosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;

    AscendC::CosConfig polyConfig = {AscendC::CosAlgo::POLYNOMIAL_APPROXIMATION};
    AscendC::CosConfig radinConfig = {AscendC::CosAlgo::RADIAN_REDUCTION};

    AscendC::GetCosMaxMinTmpSize(polyConfig, cosShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);

    AscendC::GetCosMaxMinTmpSize(radinConfig, cosShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 512 * 2 * 4 + 32);
    EXPECT_EQ(minValue, 512 * 2 * 4 + 32);

    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    GetCosTmpBufferFactorSize(polyConfig, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
    GetCosTmpBufferFactorSize(radinConfig, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestHypotTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto atanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetHypotMaxMinTmpSize(atanShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);
    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    GetHypotTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSinTilingFloatWithConfig)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto sinShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;

    AscendC::SinConfig polyConfig = {AscendC::SinAlgo::POLYNOMIAL_APPROXIMATION};
    AscendC::SinConfig radinConfig = {AscendC::SinAlgo::RADIAN_REDUCTION};

    AscendC::GetSinMaxMinTmpSize(polyConfig, sinShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);
    AscendC::GetSinMaxMinTmpSize(radinConfig, sinShape, 4, true, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 4 + 32);

    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    GetSinTmpBufferFactorSize(polyConfig, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
    GetSinTmpBufferFactorSize(radinConfig, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 2);
    EXPECT_EQ(extraBuf, 32);
}

TEST_F(TestTiling, TestSinTilingHalfWithConfig)
{
    std::vector<int64_t> shapeDims = {512};
    auto sinShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;

    AscendC::SinConfig polyConfig = {AscendC::SinAlgo::POLYNOMIAL_APPROXIMATION};
    AscendC::SinConfig radinConfig = {AscendC::SinAlgo::RADIAN_REDUCTION};

    AscendC::GetSinMaxMinTmpSize(polyConfig, sinShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);

    AscendC::GetSinMaxMinTmpSize(radinConfig, sinShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 512 * 2 * 4 + 32);
    EXPECT_EQ(minValue, 512 * 2 * 4 + 32);

    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    GetSinTmpBufferFactorSize(polyConfig, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
    GetSinTmpBufferFactorSize(radinConfig, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestConfusionTransposeTiling)
{
    const uint32_t stackBufferSize = 0;
    const uint32_t typeSize = 4;

    std::vector<int64_t> shapeDims = {32, 64, 128};
    auto srcShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;

    AscendC::GetConfusionTransposeMaxMinTmpSize(srcShape, typeSize, 13, maxValue, minValue);
    AscendC::GetConfusionTransposeMaxMinTmpSize(srcShape, typeSize, 14, maxValue, minValue);
    AscendC::GetConfusionTransposeMaxMinTmpSize(srcShape, typeSize, 15, maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);

    optiling::ConfusionTransposeTiling tiling;
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 13, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 14, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 15, tiling);
}

TEST_F(TestTiling, testReduceMaxTiling)
{
    uint32_t maxSize;
    uint32_t minSize;
    auto shape = ge::Shape({16, 8});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 16});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 32});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 8 * 4);
    EXPECT_EQ(minSize, 16 * 8 * 4);

    shape = ge::Shape({16, 64});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 64 * 4);
    EXPECT_EQ(minSize, 16 * 64 * 4);

    shape = ge::Shape({16, 64});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_INT32, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 64 * 4);
    EXPECT_EQ(minSize, 16 * 64 * 4);

    shape = ge::Shape({16, 16});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 64});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 16 * 2);
    EXPECT_EQ(minSize, 16 * 16 * 2);

    shape = ge::Shape({16, 128});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 2);
    EXPECT_EQ(minSize, 16 * 128 * 2);

    shape = ge::Shape({16, 7});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 8 * 4);
    EXPECT_EQ(minSize, 16 / 2 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);

    shape = ge::Shape({256, 16});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({256, 16});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_INT32, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 7});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 16 * 2);
    EXPECT_EQ(minSize, 16 / 2 * 16 * 2);

    shape = ge::Shape({17, 127});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 2);
    EXPECT_EQ(minSize, 16 * 128 * 2);

    shape = ge::Shape({256, 16});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);
}

TEST_F(TestTiling, testReduceMinTiling)
{
    uint32_t maxSize;
    uint32_t minSize;
    auto shape = ge::Shape({16, 8});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 16});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 32});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 8 * 4);
    EXPECT_EQ(minSize, 16 * 8 * 4);

    shape = ge::Shape({16, 64});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 64 * 4);
    EXPECT_EQ(minSize, 16 * 64 * 4);

    shape = ge::Shape({16, 64});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_INT32, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 64 * 4);
    EXPECT_EQ(minSize, 16 * 64 * 4);

    shape = ge::Shape({16, 16});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 64});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 16 * 2);
    EXPECT_EQ(minSize, 16 * 16 * 2);

    shape = ge::Shape({16, 128});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 2);
    EXPECT_EQ(minSize, 16 * 128 * 2);

    shape = ge::Shape({16, 7});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 8 * 4);
    EXPECT_EQ(minSize, 16 / 2 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);

    shape = ge::Shape({256, 16});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({256, 16});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_INT32, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 7});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 16 * 2);
    EXPECT_EQ(minSize, 16 / 2 * 16 * 2);

    shape = ge::Shape({17, 127});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 2);
    EXPECT_EQ(minSize, 16 * 128 * 2);

    shape = ge::Shape({256, 16});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);
}

TEST_F(TestTiling, testReduceSumTiling)
{
    uint32_t maxSize;
    uint32_t minSize;
    auto shape = ge::Shape({128, 128});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, (64 * 128) * 4);
    EXPECT_EQ(minSize, (64 * 128) * 4);

    shape = ge::Shape({16, 32});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 128});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 32});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_INT32, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 128});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_INT32, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 7});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 8 * 8 * 4);
    EXPECT_EQ(minSize, 8 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);

    shape = ge::Shape({256, 16});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({256, 16});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_INT32, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);
}

#else
TEST_F(TestTiling, MultiCoreSmallMN)
{
    matmul_tiling::MultiCoreMatmulTiling rnnMatmul3, rnnMatmul4, rnnMatmul5;
    rnnMatmul3.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    rnnMatmul3.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    rnnMatmul3.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_FLOAT);
    rnnMatmul3.SetBiasType(
        matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    rnnMatmul3.SetSingleRange(-1, -1, -1, -1, -1, -1);
    rnnMatmul3.EnableMultiCoreSplitK(true);
    auto ret = rnnMatmul3.EnableBias(true);
    ret = rnnMatmul3.SetDim(24);
    ret = rnnMatmul3.SetOrgShape(5, 40, 986);
    ret = rnnMatmul3.SetShape(5, 10, 986);
    ret = rnnMatmul3.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    optiling::TCubeTiling tilingData;
    ret = rnnMatmul3.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 63552);
}

TEST_F(TestTiling, MatmulApiTilingFP32OverFlow)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    matmul_tiling::MatmulApiTiling stft(plat);
    stft.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    stft.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT, true);
    stft.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    stft.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    auto ret = stft.SetOrgShape(378, 168, 960);
    ret = stft.SetShape(378, 168, 960);
    ret = stft.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    ret = stft.GetTiling(tilingData);
    tilingData.set_iterateOrder(1);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 407040);
}

TEST_F(TestTiling, MatmulApiTilingFP32OverFlowND2NZ)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    matmul_tiling::MatmulApiTiling stft(plat);
    stft.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    stft.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT, true);
    stft.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    stft.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    auto ret = stft.SetOrgShape(378, 168, 960);
    ret = stft.SetShape(378, 168, 960);
    ret = stft.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    stft.SetMatmulConfigParams(1, false, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    stft.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    ret = stft.GetTiling(tilingData);
    tilingData.set_iterateOrder(1);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 194560);
    EXPECT_GE(tilingData.get_transLength(), 90112);
}

TEST_F(TestTiling, MatmulApiTilingATransFP32OverFlow)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    matmul_tiling::MatmulApiTiling stft(plat);
    stft.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT, true);
    stft.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT, true);
    stft.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    stft.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    auto ret = stft.SetOrgShape(378, 168, 960);
    ret = stft.SetShape(378, 168, 960);
    ret = stft.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    ret = stft.GetTiling(tilingData);
    tilingData.set_iterateOrder(1);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 303104);
}

TEST_F(TestTiling, MatmulMDLND2NZNoFullLoad)
{
    matmul_tiling::MultiCoreMatmulTiling rnnMatmul3, rnnMatmul4, rnnMatmul5;
    rnnMatmul3.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8, true);
    rnnMatmul3.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_INT8);
    rnnMatmul3.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT16);
    rnnMatmul3.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT32);
    auto ret = rnnMatmul3.EnableBias(true);
    ret = rnnMatmul3.SetDim(8);
    ret = rnnMatmul3.SetOrgShape(1024, 5120, 3584);
    ret = rnnMatmul3.SetShape(1024, 5120, 3584);
    ret = rnnMatmul3.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    rnnMatmul3.SetMatmulConfigParams(1, false, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    rnnMatmul3.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    optiling::TCubeTiling tilingDataA;
    ret = rnnMatmul3.GetTiling(tilingDataA);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingDataA.get_shareL1Size(), 98304);
    EXPECT_GE(tilingDataA.get_transLength(), 65536);

    rnnMatmul4.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    rnnMatmul4.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8, true);
    rnnMatmul4.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT16);
    rnnMatmul4.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT32);
    ret = rnnMatmul4.EnableBias(true);
    ret = rnnMatmul4.SetDim(8);
    ret = rnnMatmul4.SetOrgShape(5120, 1024, 3584);
    ret = rnnMatmul4.SetShape(5120, 1024, 3584);
    ret = rnnMatmul4.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    rnnMatmul4.SetMatmulConfigParams(1, false, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    rnnMatmul4.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    optiling::TCubeTiling tilingDataB;
    ret = rnnMatmul4.GetTiling(tilingDataB);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingDataB.get_shareL1Size(), 163840);
    EXPECT_GE(tilingDataB.get_transLength(), 69632);
}

TEST_F(TestTiling, MatmulMDLND2NZFullLoad)
{
    matmul_tiling::MultiCoreMatmulTiling rnnMatmul3, rnnMatmul4;
    rnnMatmul3.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    rnnMatmul3.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8, true);
    rnnMatmul3.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT16);
    rnnMatmul3.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT32);
    auto ret = rnnMatmul3.EnableBias(true);
    ret = rnnMatmul3.SetDim(8);
    ret = rnnMatmul3.SetOrgShape(32, 2048, 64);
    ret = rnnMatmul3.SetShape(32, 2048, 64);
    ret = rnnMatmul3.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    rnnMatmul3.SetMatmulConfigParams(1, false, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    rnnMatmul3.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    optiling::TCubeTiling tilingDataA;
    ret = rnnMatmul3.GetTiling(tilingDataA);

    EXPECT_EQ(ret, 0);
    EXPECT_EQ(tilingDataA.get_shareL1Size(), 18432);
    EXPECT_EQ(tilingDataA.get_transLength(), 16384);

    rnnMatmul4.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8, true);
    rnnMatmul4.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    rnnMatmul4.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT16);
    rnnMatmul4.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT32);
    ret = rnnMatmul4.EnableBias(true);
    ret = rnnMatmul4.SetDim(8);
    ret = rnnMatmul4.SetOrgShape(2048, 32, 64);
    ret = rnnMatmul4.SetShape(2048, 32, 64);
    ret = rnnMatmul4.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    rnnMatmul4.SetMatmulConfigParams(1, false, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    rnnMatmul4.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    optiling::TCubeTiling tilingDataB;
    ret = rnnMatmul4.GetTiling(tilingDataB);

    matmul_tiling::SysTilingTempBufSize bufSize;
    MultiCoreMatmulGetTmpBufSize(tilingDataB, bufSize);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingDataB.get_shareL1Size(), 34816);
    EXPECT_GE(tilingDataB.get_transLength(), 33792);
}

TEST_F(TestTiling, MatmulApiTilingFP32)
{
    matmul_tiling::MatmulApiTiling stft;
    stft.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    stft.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT, true);
    stft.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    stft.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    auto ret = stft.SetOrgShape(28, 784, 128);
    ret = stft.SetShape(28, 784, 112);
    ret = stft.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    ret = stft.GetTiling(tilingData);
    tilingData.set_iterateOrder(1);

    matmul_tiling::SysTilingTempBufSize bufSize;
    MatmulGetTmpBufSize(tilingData, bufSize);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 64512);
}

TEST_F(TestTiling, TestArithProgressionTiling)
{
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetArithProgressionMaxMinTmpSize(maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);
}

TEST_F(TestTiling, TestArangeTiling)
{
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetArangeMaxMinTmpSize(maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);
}

TEST_F(TestTiling, PlatformConstructor)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    MatmulApiTiling tiling(plat);
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(32, 4096, 80);
    tiling.SetOrgShape(32, 4096, 80);
    tiling.EnableBias(true);
    tiling.SetBias(true);
    tiling.SetBufferSpace(256 * 1024, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    tiling.PrintTilingDataInfo(tilingData);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 66560);

    AscendC::tiling::TCubeTiling tilingDataNotOp;
    ret = tiling.GetTiling(tilingDataNotOp);

    tiling.PrintTilingDataInfo(tilingDataNotOp);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingDataNotOp.shareL1Size, 66560);
}

TEST_F(TestTiling, TestMatmulApiTilingL0DB)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    MatmulApiTiling tiling(plat);
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(2048, 20480, 16);
    tiling.SetOrgShape(2048, 20480, 16);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1, -1);
    tiling.SetMatmulConfigParams({1, false, ScheduleType::OUTER_PRODUCT, MatrixTraverse::FIRSTM});
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 163840);

    AscendC::tiling::TCubeTiling tilingDataNotOp;
    ret = tiling.GetTiling(tilingDataNotOp);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingDataNotOp.shareL1Size, 163840);
}

TEST_F(TestTiling, TestMatmulApiTilingL0DBError)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    MatmulApiTiling tiling(plat);
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(16, 16, 2048);
    tiling.SetOrgShape(16, 16, 2048);
    tiling.SetFixSplit(16, 16, -1);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1, -1);
    tiling.SetMatmulConfigParams({1, false, ScheduleType::OUTER_PRODUCT, MatrixTraverse::FIRSTN});
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);

    AscendC::tiling::TCubeTiling tilingDataNotOp;
    ret = tiling.GetTiling(tilingDataNotOp);
    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, TestMatmulApiTilingNormL0DB)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    MatmulApiTiling tiling(plat);
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(20480, 2048, 16);
    tiling.SetOrgShape(20480, 2048, 16);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1, -1);
    tiling.SetMatmulConfigParams({0, false, ScheduleType::OUTER_PRODUCT, MatrixTraverse::FIRSTN});
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 65536);
}

TEST_F(TestTiling, TestMatmulApiTilingBmmL0DBError)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    MatmulApiTiling tiling(plat);
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(16, 16, 2048);
    tiling.SetOrgShape(16, 16, 2048);
    tiling.SetFixSplit(16, 16, -1);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1, -1);
    tiling.SetMatmulConfigParams({0, false, ScheduleType::OUTER_PRODUCT, MatrixTraverse::FIRSTN});
    tiling.SetBatchNum(2);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, TestInt4BaseK)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    MatmulApiTiling tiling(plat);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT4);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT4);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.EnableBias(false);
    tiling.SetShape(144, 256, 32);
    tiling.SetOrgShape(144, 256, 32);
    tiling.SetBufferSpace(256 * 1024, 128 * 1024, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(tilingData.get_baseK() % 64, 0);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 16384);
}

TEST_F(TestTiling, Tiling_310p_NotAligned)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND310P,
        .l1Size = 1048576,
        .l0CSize = 262144,
        .ubSize = 262144,
        .l0ASize = 65536,
        .l0BSize = 65536};
    matmul_tiling::MultiCoreMatmulTiling rnnMatmul3(plat);
    rnnMatmul3.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::VECTOR, matmul_tiling::DataType ::DT_FLOAT16, false);
    rnnMatmul3.SetBType(
        matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_FLOAT16, true);
    rnnMatmul3.SetCType(
        matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT16);
    auto ret = rnnMatmul3.EnableBias(false);
    ret = rnnMatmul3.SetOrgShape(1, 494, 128);
    ret = rnnMatmul3.SetShape(1, 494, 128);
    ret = rnnMatmul3.SetBufferSpace(1046528, 262144); // will use all buffer space if not explicitly specified
    ret = rnnMatmul3.SetFixSplit(16, 256, 16);
    rnnMatmul3.SetTraverse(matmul_tiling::MatrixTraverse::FIRSTN);
    optiling::TCubeTiling tilingData;
    ret = rnnMatmul3.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, Tiling_BatchMatmul)
{
    matmul_tiling::BatchMatmulTiling bmm;
    optiling::TCubeTiling tilingData;
    int ret = bmm.GetTiling(tilingData);

    matmul_tiling::SysTilingTempBufSize bufSize;
    BatchMatmulGetTmpBufSize(tilingData, bufSize);
    EXPECT_EQ(ret, -1);

    bmm.bufferPool_.l1Size = 256;
    bmm.bufferPool_.l0CSize = 256;
    bmm.bufferPool_.ubSize = 256;
    bmm.bufferPool_.btSize = 256;
    ret = bmm.SetBufferSpace(16, -16, 16, 16);
    EXPECT_EQ(ret, -1);
    ret = bmm.SetBufferSpace(16, 16, -16, 16);
    EXPECT_EQ(ret, -1);
    ret = bmm.SetBufferSpace(16, 16, 16, -16);
    EXPECT_EQ(ret, -1);

    bmm.bufferPool_.l1Size = 256;
    bmm.bufferPool_.l0CSize = 256;
    bmm.bufferPool_.ubSize = 256;
    bmm.bufferPool_.btSize = 256;
    bmm.bufferPool_.l0ASize = 256;
    bmm.bufferPool_.l0BSize = 256;
    bmm.bufferPool_.l0CSize = 256;
    bmm.aType_.dataType = matmul_tiling::DataType::DT_INT8;
    bmm.bType_.dataType = matmul_tiling::DataType::DT_INT8;
    bmm.cType_.dataType = matmul_tiling::DataType::DT_INT8;
    bmm.baseM = 256;
    bmm.baseK = 256;
    bool retParam = bmm.CheckSetParam();
    EXPECT_EQ(retParam, false);

    bmm.baseM = 1;
    bmm.baseK = 16;
    bmm.baseN = 256;
    retParam = bmm.CheckSetParam();
    EXPECT_EQ(retParam, false);

    bmm.baseM = 16;
    bmm.baseK = 1;
    bmm.baseN = 32;
    retParam = bmm.CheckSetParam();
    EXPECT_EQ(retParam, false);

    bmm.socVersion = platform_ascendc::SocVersion::ASCEND910;
    bmm.isBias = true;
    bmm.biasType_.pos = TPosition::TSCM;
    retParam = bmm.CheckSetParam();
    EXPECT_EQ(retParam, false);

    EXPECT_EQ(bmm.SetSingleBatch(2, 2), 0);
}

TEST_F(TestTiling, Tiling_BatchMatmulWithCppStruct)
{
    matmul_tiling::BatchMatmulTiling bmm;
    AscendC::tiling::TCubeTiling tilingData;
    int ret = bmm.GetTiling(tilingData);

    matmul_tiling::SysTilingTempBufSize bufSize;
    EXPECT_EQ(BatchMatmulGetTmpBufSizeV2(tilingData, bufSize), 0);
    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, ATscmCase310P)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(32, 4096, 80);
    tiling.SetOrgShape(32, 4096, 80);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(256 * 1024, -1, -1);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 65536);
    EXPECT_GE(tilingData.get_transLength(), 34816);
}

TEST_F(TestTiling, ATscmCase910B)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(32, 4096, 80);
    tiling.SetOrgShape(32, 4096, 80);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(256 * 1024, -1, -1);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND910B;
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 66560);
}

TEST_F(TestTiling, BmmFailedCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(32, 4096, 80);
    tiling.SetOrgShape(32, 4096, 80);
    tiling.SetALayout(1, 32, 2, 3, 80);
    tiling.SetBLayout(1, 4096, 2, 3, 80);
    tiling.SetCLayout(1, 32, 2, 3, 4096);
    tiling.SetBatchNum(2);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(256 * 1024, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, TestSetBatch)
{
    MatmulApiTiling tiling;
    int batchA = 3;
    int batchB = 3;
    int m = 32;
    int n = 32;
    int k = 32;
    tiling.SetBatchNum(1);
    tiling.SetBatchInfoForNormal(batchA, batchB, m, n, k);
    tiling.SetBatchNum(0);
    tiling.SetBatchNum(1);
    tiling.SetBatchInfoForNormal(0, 0, 0, 0, 0);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, BmmNormalFailedCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBatchInfoForNormal(2, 2, 32, 4096, 80);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(256 * 1024, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, BTscmCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(512, 32, 512);
    tiling.SetOrgShape(512, 32, 512);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(512 * 1024 - 64 - 32 * 512 * 2, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 65664);
}

TEST_F(TestTiling, ATscmBTscmCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 72, 1024);
    tiling.SetOrgShape(1024, 72, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 320);
}

TEST_F(TestTiling, ATransBTransmCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, true);
    tiling.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, true);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1023, 72, 1023);
    tiling.SetOrgShape(1023, 72, 1023);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    tiling.SetMatmulConfigParams(0);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_transLength(), 81920);
}

TEST_F(TestTiling, L1CacheUBCase01NoCache)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 10240, 1280);
    tiling.SetOrgShape(1024, 10240, 1280);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 458752);
    EXPECT_GE(tilingData.get_transLength(), 163840);
    EXPECT_EQ(tilingData.get_depthAL1CacheUB(), 0);
    EXPECT_EQ(tilingData.get_depthBL1CacheUB(), 0);
}

TEST_F(TestTiling, L1CacheUBCase01NoCacheND2NZ)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 10240, 1280);
    tiling.SetOrgShape(1024, 10240, 1280);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 458752);
    EXPECT_GE(tilingData.get_transLength(), 131072);
    EXPECT_EQ(tilingData.get_depthAL1CacheUB(), 2);
    EXPECT_EQ(tilingData.get_depthBL1CacheUB(), 4);
}

TEST_F(TestTiling, L1CacheUBCase02NeiABFullLoad)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(4096, 2560, 320);
    tiling.SetOrgShape(4096, 2560, 320);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 491520);
    EXPECT_GE(tilingData.get_transLength(), 163840);
    EXPECT_EQ(tilingData.get_depthAL1CacheUB(), 0);
    EXPECT_EQ(tilingData.get_depthBL1CacheUB(), 3);
}

TEST_F(TestTiling, L1CacheUBCase03BothABFullLoad)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(16, 16, 2);
    tiling.SetOrgShape(16, 16, 2);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 1088);
    EXPECT_GE(tilingData.get_transLength(), 1024);
    EXPECT_EQ(tilingData.get_depthAL1CacheUB(), 1);
    EXPECT_EQ(tilingData.get_depthBL1CacheUB(), 1);
}

TEST_F(TestTiling, L1CacheUBCase04OnlyAFullLoad)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(16, 12288, 16);
    tiling.SetOrgShape(16, 12288, 16);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 393984);
    EXPECT_GE(tilingData.get_transLength(), 393728);
    EXPECT_EQ(tilingData.get_depthAL1CacheUB(), 1);
}

TEST_F(TestTiling, L1CacheUBCase04OnlyBFullLoad)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(12288, 16, 16);
    tiling.SetOrgShape(12288, 16, 16);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 393984);
    EXPECT_GE(tilingData.get_transLength(), 393216);
    EXPECT_EQ(tilingData.get_depthBL1CacheUB(), 1);
}

TEST_F(TestTiling, L1CacheUBCase05BothCache)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(8);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetOrgShape(1024, 5120, 640);
    tiling.SetShape(1024, 5120, 640);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 507904);
    EXPECT_GE(tilingData.get_transLength(), 163840);
    EXPECT_EQ(tilingData.get_depthAL1CacheUB(), 1);
    EXPECT_EQ(tilingData.get_depthBL1CacheUB(), 2);
}

TEST_F(TestTiling, L1CacheUBCase06AMatIsTSCM)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(16, 64, 4096);
    tiling.SetOrgShape(16, 64, 4096);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 327680);
    EXPECT_GE(tilingData.get_transLength(), 131584);
    EXPECT_EQ(tilingData.get_depthAL1CacheUB(), 0);
    EXPECT_EQ(tilingData.get_depthBL1CacheUB(), 1);
}

TEST_F(TestTiling, L1CacheUBCase07BMatIsTSCM)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(64, 16, 4096);
    tiling.SetOrgShape(64, 16, 4096);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 262144);
    EXPECT_GE(tilingData.get_transLength(), 133120);
    EXPECT_EQ(tilingData.get_depthAL1CacheUB(), 0);
    EXPECT_EQ(tilingData.get_depthBL1CacheUB(), 0);
}

TEST_F(TestTiling, L1CacheUBCase08MultiCore)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(8);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetOrgShape(4096, 2560, 320);
    tiling.SetShape(4096, 2560, 320);
    tiling.EnableBias(true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.SetMatmulConfigParams(1, true);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 507904);
    EXPECT_GE(tilingData.get_transLength(), 501760);
}

TEST_F(TestTiling, ATscmFP32Case)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(128, 8, 8192);
    tiling.SetOrgShape(128, 8, 8192);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(512 * 1024 - 64 - 128 * 16 * 4, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 32832);
}

TEST_F(TestTiling, FP32Case)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(2048, 40, 2048);
    tiling.SetOrgShape(2048, 40, 2048);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 286912);
}

TEST_F(TestTiling, FP32Case2)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT, true);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT, true);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(2048, 40, 2048);
    tiling.SetOrgShape(2048, 40, 2048);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 262336);
}

TEST_F(TestTiling, BothFullLoadCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(32, 64, 128);
    tiling.SetOrgShape(32, 64, 128);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 49408);
}

TEST_F(TestTiling, AFullLoadCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(32, 512000, 128);
    tiling.SetOrgShape(32, 512000, 128);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 279552);
}

TEST_F(TestTiling, AFullLoadND2NZCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(32, 512000, 128);
    tiling.SetOrgShape(32, 512000, 128);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    tiling.SetMatmulConfigParams(1, false, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 278528);
    EXPECT_GE(tilingData.get_transLength(), 270336);
}

TEST_F(TestTiling, BFullLoadCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(102400, 32, 128);
    tiling.SetOrgShape(102400, 32, 128);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 270464);
}

TEST_F(TestTiling, BothNotFullLoadCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(512, 1024, 128000);
    tiling.SetOrgShape(512, 1024, 128000);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 230400);
}

TEST_F(TestTiling, BothNotFullLoadND2NZCase)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(512, 1024, 128000);
    tiling.SetOrgShape(512, 1024, 128000);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    tiling.SetMatmulConfigParams(1, false, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 196608);
    EXPECT_GE(tilingData.get_transLength(), 131072);
}

TEST_F(TestTiling, TestMatmulApiTilngCase1)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    tiling.SetDoubleBuffer(false, false, false, false, false, false);
    tiling.SetTraverse(MatrixTraverse::FIRSTM);
    tiling.SetMadType(MatrixMadType::NORMAL);
    tiling.SetSplitRange(128, 128, 128);
    tiling.SetFixSplit(16, 16, 16);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 393280);
    EXPECT_EQ(tilingData.get_baseM(), 16);
}

TEST_F(TestTiling, TestMatmulApiTilngCase2)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(64, 64, 64);
    tiling.SetOrgShape(64, 64, 64);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    tiling.SetDoubleBuffer(true, true, true, true, true, true);
    tiling.SetTraverse(MatrixTraverse::FIRSTM);
    tiling.SetMadType(MatrixMadType::NORMAL);

    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 16384);
    EXPECT_EQ(tilingData.get_baseM(), 64);
}

TEST_F(TestTiling, TestMatmulApiTilngCase3)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(2048, 64, 64);
    tiling.SetOrgShape(2048, 64, 64);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    tiling.SetDoubleBuffer(true, true, true, true, true, true);
    tiling.SetMadType(MatrixMadType::NORMAL);

    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 270336);
    EXPECT_EQ(tilingData.get_baseM(), 256);
}

TEST_F(TestTiling, TestMatmulApiTilngCase4)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(64, 2048, 64);
    tiling.SetOrgShape(64, 2048, 64);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    tiling.SetDoubleBuffer(true, true, true, true, true, true);
    tiling.SetTraverse(MatrixTraverse::FIRSTM);
    tiling.SetMadType(MatrixMadType::NORMAL);

    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 270336);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseM(), 64);
}

TEST_F(TestTiling, TestMatmulApiTilngCase5)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    tiling.SetDoubleBuffer(false, false, false, false, false, false);
    tiling.SetTraverse(MatrixTraverse::FIRSTM);
    tiling.SetMadType(MatrixMadType::NORMAL);
    tiling.SetFixSplit(64, -1, -1);

    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 394240);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseM(), 64);

    tiling.SetFixSplit(-1, 16, -1);
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 393280);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseN(), 16);
}

TEST_F(TestTiling, TestMatmulApiTilngCase6)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    tiling.SetDoubleBuffer(false, false, false, false, false, false);
    tiling.SetTraverse(MatrixTraverse::FIRSTM);
    tiling.SetMadType(MatrixMadType::NORMAL);
    tiling.SetFixSplit(-1, 64, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 295168);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngWithCppStruct)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    tiling.SetDoubleBuffer(false, false, false, false, false, false);
    tiling.SetTraverse(MatrixTraverse::FIRSTM);
    tiling.SetMadType(MatrixMadType::NORMAL);
    tiling.SetFixSplit(-1, 64, -1);
    TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.shareL1Size, 295168);
    EXPECT_GE(tilingData.transLength, 0);
    matmul_tiling::SysTilingTempBufSize bufSize;
    EXPECT_EQ(MatmulGetTmpBufSizeV2(tilingData, bufSize), 0);
}

TEST_F(TestTiling, TestMatmulApiTilng310B)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    tiling.SetDoubleBuffer(false, false, false, false, false, false);
    tiling.SetTraverse(MatrixTraverse::FIRSTM);
    tiling.SetMadType(MatrixMadType::NORMAL);
    tiling.SetFixSplit(-1, 64, -1);
    tiling.SetMatmulConfigParams(0);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310B;
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 295168);
    EXPECT_GE(tilingData.get_transLength(), 131072);
}

TEST_F(TestTiling, TestDiffKaKbMatmulApiTilng)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024, 1280);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    tiling.SetDoubleBuffer(false, false, false, false, false, false);
    tiling.SetTraverse(MatrixTraverse::FIRSTM);
    tiling.SetMadType(MatrixMadType::NORMAL);
    tiling.SetFixSplit(64, -1, -1);

    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 394240);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseM(), 64);

    tiling.SetFixSplit(-1, 16, -1);
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 393280);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseN(), 16);

    EXPECT_EQ(tilingData.get_Ka(), 1024);
    EXPECT_EQ(tilingData.get_Kb(), 1280);
}

TEST_F(TestTiling, TestMatmulApiTilngIteratorOrder)
{
    SingleCoreStatus status;
    MatmulApiTiling tiling;
    tiling.SetShape(128, 128, 128);
    tiling.SetOrgShape(128, 128, 128);
    status.l1Status.kAL1 = 1;
    status.l1Status.kBL1 = 1;

    MatmulTilingAlgorithm algoIns(&tiling);

    int32_t ret = algoIns.GetIteratorOrder(status, 128, 128, 128);
    EXPECT_EQ(ret, 0);

    status.l1Status.kAL1 = 8;
    status.l1Status.kBL1 = 1;
    ret = algoIns.GetIteratorOrder(status, 128, 128, 128);
    EXPECT_EQ(ret, 0);

    status.l1Status.kAL1 = 1;
    status.l1Status.kBL1 = 8;
    ret = algoIns.GetIteratorOrder(status, 128, 128, 128);
    EXPECT_EQ(ret, 0);
}

TEST_F(TestTiling, TestSetBufferSpace)
{
    MatmulApiTiling tiling;
    tiling.SetBufferSpace(1024);
    EXPECT_EQ(tiling.bufferPool_.l1Size, 1024);
}

TEST_F(TestTiling, TestCosTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto cosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetCosMaxMinTmpSize(cosShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 4 * 3);
    AscendC::GetCosMaxMinTmpSize(cosShape, 4, true, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 4 * 2);
    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    GetCosTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 3);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestCosTilingFloat512)
{
    std::vector<int64_t> shapeDims = {512};
    auto cosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetCosMaxMinTmpSize(cosShape, 4, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 3);
    AscendC::GetCosMaxMinTmpSize(cosShape, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 2);
}

TEST_F(TestTiling, TestCosTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto cosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetCosMaxMinTmpSize(cosShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 8 * 2);
    EXPECT_EQ(minValue, 256 * 8);
    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    GetCosTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 8);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAtanTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto atanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAtanMaxMinTmpSize(atanShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 4 * 5);
    EXPECT_EQ(minValue, 256 * 5);
}

TEST_F(TestTiling, TestAtanTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto atanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAtanMaxMinTmpSize(atanShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 12);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 12);
    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    GetAtanTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 12);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestClampTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto atanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetClampMaxMinTmpSize(atanShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 1);
    EXPECT_EQ(minValue, 64 * 1);
}

TEST_F(TestTiling, TestClampTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto atanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetClampMaxMinTmpSize(atanShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 128 * 1);
    EXPECT_EQ(maxValue, 128 * 128 * 1);
    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    GetClampTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSoftMaxTiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    optiling::SoftMaxTiling tilingData;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t softmaxTmpSize = 100 * 1024 * 4;
    uint32_t softmaxNeedMinSize = GetSoftMaxMinTmpSize(softmaxShape, 2, true);
    EXPECT_EQ(softmaxNeedMinSize, (16 + 128 + 64) * 4);
    uint32_t softmaxFlashNeedMinSize = GetSoftMaxFlashMinTmpSize(softmaxShape, 2, true, true);
    EXPECT_EQ(softmaxFlashNeedMinSize, (16 * 4 + 128 * 2) * 4);
    softmaxFlashNeedMinSize = GetSoftMaxFlashMinTmpSize(softmaxShape, 4, true, true);
    EXPECT_EQ(softmaxFlashNeedMinSize, (8 * 4 + 128 * 2) * 4);
    softmaxFlashNeedMinSize = GetSoftMaxFlashMinTmpSize(softmaxShape, 4, false, true);
    EXPECT_EQ(softmaxFlashNeedMinSize, (8 + 128 + 64) * 4);
    uint32_t softmaxGradNeedMinSize = GetSoftMaxGradMinTmpSize(softmaxShape, 2, true, true);
    EXPECT_EQ(softmaxGradNeedMinSize, (16 * 2 + 128 * 3 + 64) * 4);
    softmaxGradNeedMinSize = GetSoftMaxGradMinTmpSize(softmaxShape, 4, true, true);
    EXPECT_EQ(softmaxGradNeedMinSize, (8 + 128 + 64) * 4);
    softmaxGradNeedMinSize = GetSoftMaxGradMinTmpSize(softmaxShape, 4, false, true);
    EXPECT_EQ(softmaxGradNeedMinSize, (8 * 2 + 128 + 64) * 4);
    softmaxGradNeedMinSize = GetSoftMaxGradMinTmpSize(softmaxShape, 0, true, true);
    EXPECT_EQ(softmaxGradNeedMinSize, 0);

    uint32_t softmaxNeedMaxSize = GetSoftMaxMaxTmpSize(softmaxShape, 2, true);
    EXPECT_EQ(softmaxNeedMaxSize, 128 * (16 + 128 + 64) * 4);
    softmaxNeedMaxSize = GetSoftMaxMaxTmpSize(softmaxShape, 1, true);
    EXPECT_EQ(softmaxNeedMaxSize, 0);
    uint32_t softmaxFlashNeedMaxSize = GetSoftMaxFlashMaxTmpSize(softmaxShape, 2, true, true);
    EXPECT_EQ(softmaxFlashNeedMaxSize, 128 * (16 * 4 + 128 * 2) * 4);
    softmaxFlashNeedMaxSize = GetSoftMaxFlashMaxTmpSize(softmaxShape, 4, false, true);
    EXPECT_EQ(softmaxFlashNeedMaxSize, 128 * (8 + 128 + 64) * 4);
    softmaxFlashNeedMaxSize = GetSoftMaxFlashMaxTmpSize(softmaxShape, 4, true, true);
    EXPECT_EQ(softmaxFlashNeedMaxSize, 128 * (8 * 4 + 128 * 2) * 4);
    softmaxFlashNeedMaxSize = GetSoftMaxFlashMaxTmpSize(softmaxShape, 1, true, true);
    EXPECT_EQ(softmaxFlashNeedMaxSize, 0);
    uint32_t softmaxGradNeedMaxSize = GetSoftMaxGradMaxTmpSize(softmaxShape, 2, true, true);
    EXPECT_EQ(softmaxGradNeedMaxSize, 128 * (16 * 2 + 128 * 3 + 64) * 4);
    softmaxGradNeedMaxSize = GetSoftMaxGradMaxTmpSize(softmaxShape, 4, true, true);
    EXPECT_EQ(softmaxGradNeedMaxSize, 128 * (8 + 128 + 64) * 4);
    softmaxGradNeedMaxSize = GetSoftMaxGradMaxTmpSize(softmaxShape, 1, true, true);
    EXPECT_EQ(softmaxGradNeedMaxSize, 0);
    SoftMaxTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingData);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    bool flag = IsBasicBlockInSoftMax(tilingData);
    EXPECT_EQ(flag, true);
    SoftMaxFlashTilingFunc(softmaxShape, 2, 77952, tilingData, true);
    EXPECT_EQ(tilingData.get_reduceM(), 32);
    SoftMaxFlashTilingFunc(softmaxShape, 2, 77952, tilingData, false);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    SoftMaxGradTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingData, false);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    SoftMaxGradTilingFunc(softmaxShape, 4, softmaxTmpSize, tilingData, false);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    SoftMaxGradTilingFunc(softmaxShape, 2, 133120, tilingData, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);

    AscendC::tiling::SoftMaxTiling tilingDataNotOp;
    SoftMaxFlashTilingFunc(softmaxShape, 2, 77952, tilingDataNotOp, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 32);
    flag = IsBasicBlockInSoftMax(tilingDataNotOp);
    EXPECT_EQ(flag, true);
    SoftMaxGradTilingFunc(softmaxShape, 2, 133120, tilingDataNotOp, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
    SoftMaxTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
}

TEST_F(TestTiling, TestLogSoftMaxTiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    optiling::LogSoftMaxTiling tilingData;
    AscendC::tiling::LogSoftMaxTiling tilingDataNotOp;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t softmaxTmpSize = 100 * 1024 * 4;
    uint32_t softmaxNeedMinSize = GetLogSoftMaxMinTmpSize(softmaxShape, 2, true);
    uint32_t softmaxNeedMaxSize = GetLogSoftMaxMaxTmpSize(softmaxShape, 2, true);

    LogSoftMaxTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingData);
    EXPECT_EQ(tilingData.get_reduceM(), 64);

    LogSoftMaxTilingFunc(softmaxShape, 2, softmaxTmpSize, tilingDataNotOp);
    EXPECT_EQ(tilingDataNotOp.reduceM, 64);
}

TEST_F(TestTiling, TestSoftMaxFlashV2TilingMaxMinTmpSize)
{
    uint32_t softmaxflashV2NeedMinLength = 0;
    uint32_t softmaxflashV2NeedMaxLength = 0;

    std::vector<int64_t> shapeDims = {3, 3, 448};
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t dataTypeSize1 = 2;
    uint32_t dataTypeSize2 = 2;
    uint32_t isUpdate = 0;
    uint32_t isBasicBlock = 0;
    uint32_t isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 19584);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 19584);

    shapeDims = {7, 1072};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 32704);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 32704);

    shapeDims = {1, 2, 3, 1, 2, 1, 16};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 5376);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 5376);

    shapeDims = {2, 6, 1, 16};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 5376);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 5376);

    shapeDims = {6, 1664};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 42240);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 42240);

    shapeDims = {2, 1760};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 14848);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 14848);

    shapeDims = {1, 5536};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 22528);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 22528);

    shapeDims = {2, 2, 2352};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 39168);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 39168);

    shapeDims = {2, 2, 2, 480};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 0;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 18432);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 18432);

    shapeDims = {2, 3632};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 1;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 29824);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 29824);

    shapeDims = {2, 4, 96};
    softmaxShape = ge::Shape(shapeDims);
    dataTypeSize1 = 2;
    dataTypeSize2 = 2;
    isUpdate = 1;
    isBasicBlock = 0;
    isFlashOutputBrc = 1;

    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 6144);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(
        softmaxShape, dataTypeSize1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 6144);

    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, dataTypeSize1, 1, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 0);

    softmaxflashV2NeedMaxLength =
        GetSoftMaxFlashV2MaxTmpSize(softmaxShape, 1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 0);

    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, 1, dataTypeSize2, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 0);

    softmaxflashV2NeedMaxLength =
        GetSoftMaxFlashV2MaxTmpSize(softmaxShape, dataTypeSize1, 1, isUpdate, isBasicBlock, isFlashOutputBrc);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 0);
}

TEST_F(TestTiling, TestSoftMaxFlashV2Tiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    optiling::SoftMaxTiling tilingData;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t maxSumTypeSize = 2;
    uint32_t inputTypeSize = 2;
    uint32_t softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, false);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 64 + 16 * 2) * 4);
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 64 + 16 * 2) * 4);
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 8 * (128 + 64 + 16) * 4);

    // isFlashOutputBrc
    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, false, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 64 + 16 * 2) * 4 * 16);
    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 64 + 16 * 2) * 4 * 16);
    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 64 + 16) * 4 * 16);

    uint32_t softmaxflashV2NeedMaxLength =
        GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, false);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 128 * (128 + 64 + 16 * 2) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 128 * (128 + 64 + 16 * 2) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 64 * (128 + 64 + 16) * 4);

    maxSumTypeSize = 4;
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 64 + 8 * 2) * 4);
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 8 * (128 + 64 + 8) * 4);

    // isFlashOutputBrc
    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 64 + 8 * 2) * 4 * 16);
    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (128 + 64 + 8) * 4 * 16);

    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, false);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 128 * (128 + 64 + 8 * 2) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, false, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 64 * (128 + 64 + 8) * 4);

    uint32_t workLength = 100 * 1024;
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, false, false);
    EXPECT_EQ(tilingData.get_reduceM(), 120);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, false, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, false);
    EXPECT_EQ(tilingData.get_reduceM(), 120);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);

    // isFlashOutputBrc
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, false, false, true);
    EXPECT_EQ(tilingData.get_reduceM(), 112);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, false, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, false, true);
    EXPECT_EQ(tilingData.get_reduceM(), 112);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);

    inputTypeSize = 4;
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 8 * (64 + 8) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 64 * (64 + 8) * 4);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);

    // isFlashOutputBrc
    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (64 + 8) * 4 * 8);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 64);
}
TEST_F(TestTiling, TestSoftMaxFlashV2TilingBasicBlock)
{
    std::vector<int64_t> shapeDims = {8, 1024};
    optiling::SoftMaxTiling tilingData;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t maxSumTypeSize = 4;
    uint32_t inputTypeSize = 4;
    uint32_t softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 8 * (8 + 128) * 4);
    uint32_t softmaxflashV2NeedMaxLength =
        GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 8 * (8 + 128) * 4);

    // isFlashOutputBrc
    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (8 + 128) * 4 * 8);

    uint32_t workLength = 32 * 1024;
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 8);

    inputTypeSize = 2;
    workLength = 64 * 1024;
    softmaxflashV2NeedMinLength = GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, 8 * (8 + 1024 + 64 * 2) * 4);
    softmaxflashV2NeedMaxLength = GetSoftMaxFlashV2MaxTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true);
    EXPECT_EQ(softmaxflashV2NeedMaxLength, 8 * (8 + 1024 + 64 * 2) * 4);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 8);

    // isFlashOutputBrc
    softmaxShape = ge::Shape({64, 512});
    softmaxflashV2NeedMinLength =
        GetSoftMaxFlashV2MinTmpSize(softmaxShape, inputTypeSize, maxSumTypeSize, true, true, true);
    EXPECT_EQ(softmaxflashV2NeedMinLength, (8 + 512 + 64) * 4 * 16);
    SoftMaxFlashV2TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 16);

    AscendC::tiling::SoftMaxTiling tilingDataNotOp;
    SoftMaxFlashV2TilingFunc(
        softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, true, true, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 16);
}

TEST_F(TestTiling, TestSoftMaxFlashV3Tiling)
{
    std::vector<int64_t> shapeDims = {10, 1024};
    optiling::SoftMaxTiling tilingData;
    auto softmaxShape = ge::Shape(shapeDims);
    uint32_t maxSumTypeSize = 4;
    uint32_t inputTypeSize = 2;
    uint32_t softmaxflashV3NeedMinLength = 0;
    uint32_t softmaxflashV3NeedMaxLength = 0;
    GetSoftMaxFlashV3MaxMinTmpSize(
        softmaxShape, inputTypeSize, maxSumTypeSize, softmaxflashV3NeedMaxLength, softmaxflashV3NeedMinLength, false,
        false);

    EXPECT_EQ(softmaxflashV3NeedMinLength, (8 * 5 + 2 * 1024 + 64) * 4);
    EXPECT_EQ(softmaxflashV3NeedMaxLength, 10 * (8 * 5 + 2 * 1024 + 64) * 4);

    softmaxflashV3NeedMinLength = 0;
    softmaxflashV3NeedMaxLength = 0;
    GetSoftMaxFlashV3MaxMinTmpSize(
        softmaxShape, inputTypeSize, 0, softmaxflashV3NeedMaxLength, softmaxflashV3NeedMinLength, false, false);

    EXPECT_EQ(softmaxflashV3NeedMinLength, 0);
    EXPECT_EQ(softmaxflashV3NeedMaxLength, 0);

    uint32_t workLength = 76 * 1024;
    SoftMaxFlashV3TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingData, true, true);
    EXPECT_EQ(tilingData.get_reduceM(), 8);

    AscendC::tiling::SoftMaxTiling tilingDataNotOp;
    SoftMaxFlashV3TilingFunc(softmaxShape, inputTypeSize, maxSumTypeSize, workLength, tilingDataNotOp, true, true);
    EXPECT_EQ(tilingDataNotOp.reduceM, 8);
}

TEST_F(TestTiling, TestAsinTmpBufferFacotrHalfWithoutBasicBlock)
{
    uint32_t maxLivedNodes = 0xffff;
    uint32_t extraBuffer = 0xffff;
    GetAsinTmpBufferFactorSize(2, maxLivedNodes, extraBuffer);
    EXPECT_EQ(maxLivedNodes, 6);
    EXPECT_EQ(extraBuffer, 0);
}

TEST_F(TestTiling, TestAsinTmpBufferFacotrFloatWithoutBasicBlock)
{
    uint32_t maxLivedNodes = 0xffff;
    uint32_t extraBuffer = 0xffff;
    GetAsinTmpBufferFactorSize(4, maxLivedNodes, extraBuffer);
    EXPECT_EQ(maxLivedNodes, 2);
    EXPECT_EQ(extraBuffer, 0);
}

TEST_F(TestTiling, TestAsinTilingHalf128)
{
    std::vector<int64_t> shapeDims = {128};
    auto asinShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAsinMaxMinTmpSize(asinShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 256 * 6);
    EXPECT_EQ(minValue, 256 * 6);
}

TEST_F(TestTiling, TestAsinTilingFloat)
{
    std::vector<int64_t> shapeDims = {32};
    auto asinShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAsinMaxMinTmpSize(asinShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 256 * 2);
    EXPECT_EQ(minValue, 256 * 2);
}

TEST_F(TestTiling, TestAsinTilingHalf16K)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto asinShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAsinMaxMinTmpSize(asinShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 6 * 2);
    EXPECT_EQ(minValue, 256 * 6);
}

TEST_F(TestTiling, TestAsinTilingFloat16K)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto asinShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAsinMaxMinTmpSize(asinShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 4);
    EXPECT_EQ(minValue, 256 * 2);
}

TEST_F(TestTiling, TestSinhTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto sinhShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetSinhMaxMinTmpSize(sinhShape, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 1);
    EXPECT_EQ(maxValue, 128 * 128 * 1 * 4);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetSinhTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSinhTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto sinhShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetSinhMaxMinTmpSize(sinhShape, 2, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 4);
    EXPECT_EQ(maxValue, 128 * 128 * 4 * 2);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetSinhTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestRoundTiling)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    std::vector<int64_t> shapeDims = {128, 128};
    auto tanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 0;
    platform_ascendc::SocVersion socVersion = plat.GetSocVersion();
    GetRoundMaxMinTmpSize(plat, tanShape, 4, false, maxValue, minValue);
    GetRoundTmpBufferFactorSize(plat, 4, maxLiveNodeCnt, extraBuf);
    GetRoundMaxMinTmpSize(plat, tanShape, 2, false, maxValue, minValue);
    GetRoundTmpBufferFactorSize(plat, 2, maxLiveNodeCnt, extraBuf);
}

TEST_F(TestTiling, TestTanTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto tanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetTanMaxMinTmpSize(tanShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 4 * 4);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetTanTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestTanTilingFloat512)
{
    std::vector<int64_t> shapeDims = {512};
    auto tanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetTanMaxMinTmpSize(tanShape, 4, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 4);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetTanTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestTanTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto tanShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetTanMaxMinTmpSize(tanShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 10 * 2);
    EXPECT_EQ(minValue, 256 * 10);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetTanTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 10);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TEstSwiGLUTilingHalf)
{
    std::vector<int64_t> shapeDims = {10, 512};
    auto swigluShape = ge::Shape(shapeDims);
    uint32_t maxValue;
    uint32_t minValue;
    GetSwiGLUMaxMinTmpSize(swigluShape, 2, maxValue, minValue, false);
    EXPECT_EQ(maxValue, 61440);
    EXPECT_EQ(minValue, 1536);
}

TEST_F(TestTiling, TEstSwiGLUTilingFloat)
{
    std::vector<int64_t> shapeDims = {64, 256};
    auto swigluShape = ge::Shape(shapeDims);
    uint32_t maxValue;
    uint32_t minValue;
    GetSwiGLUMaxMinTmpSize(swigluShape, 4, maxValue, minValue, false);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);
}

TEST_F(TestTiling, TEstSwiGLUTilingHalf1024)
{
    std::vector<int64_t> shapeDims = {21, 512};
    auto swigluShape = ge::Shape(shapeDims);
    uint32_t maxValue;
    uint32_t minValue;
    GetSwiGLUMaxMinTmpSize(swigluShape, 2, maxValue, minValue, false);
    EXPECT_EQ(maxValue, 129024);
    EXPECT_EQ(minValue, 1536);
}

TEST_F(TestTiling, TestSwiGLUFactorFloat)
{
    std::vector<int64_t> shapeDims = {22, 512};
    auto swigluShape = ge::Shape(shapeDims);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetSwiGLUTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSwiGLUFactorHalf)
{
    std::vector<int64_t> shapeDims = {21, 512};
    auto swigluShape = ge::Shape(shapeDims);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetSwiGLUTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 6);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestFmodTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto fmodShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetFmodMaxMinTmpSize(fmodShape, 4, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256);
    EXPECT_EQ(maxValue, 128 * 128 * 1 * 4);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetFmodTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestFmodTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto fmodShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetFmodMaxMinTmpSize(fmodShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 8);
    EXPECT_EQ(maxValue, 128 * 128 * 8 * 2);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetFmodTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 8);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestFmodTilingHalf512)
{
    std::vector<int64_t> shapeDims = {512};
    auto truncShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetFmodMaxMinTmpSize(truncShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 512 * 8 * 2);
    EXPECT_EQ(minValue, 256 * 8);
}

TEST_F(TestTiling, TestTruncTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto truncShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetTruncMaxMinTmpSize(truncShape, 4, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 1);
    EXPECT_EQ(maxValue, 128 * 128 * 1 * 4);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetTruncTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestTruncTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto truncShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetTruncMaxMinTmpSize(truncShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 2);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 2);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetTruncTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 2);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestTruncTilingHalf512)
{
    std::vector<int64_t> shapeDims = {512};
    auto truncShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetTruncMaxMinTmpSize(truncShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 512 * 2 * 2);
    EXPECT_EQ(minValue, 256 * 2);
}

TEST_F(TestTiling, TestAcosTmpBufferFacotrHalfWithoutBasicBlock)
{
    uint32_t maxLivedNodes = 0xffff;
    uint32_t extraBuffer = 0xffff;
    GetAcosTmpBufferFactorSize(2, maxLivedNodes, extraBuffer);
    EXPECT_EQ(maxLivedNodes, 6);
    EXPECT_EQ(extraBuffer, 0);
}

TEST_F(TestTiling, TestAcosTmpBufferFacotrFloatWithoutBasicBlock)
{
    uint32_t maxLivedNodes = 0xffff;
    uint32_t extraBuffer = 0xffff;
    GetAcosTmpBufferFactorSize(4, maxLivedNodes, extraBuffer);
    EXPECT_EQ(maxLivedNodes, 2);
    EXPECT_EQ(extraBuffer, 0);
}

TEST_F(TestTiling, TestAcosTilingHalf128)
{
    std::vector<int64_t> shapeDims = {128};
    auto acosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAcosMaxMinTmpSize(acosShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 6);
    EXPECT_EQ(maxValue, 256 * 6);
}

TEST_F(TestTiling, TestAcosTilingFloat)
{
    std::vector<int64_t> shapeDims = {32};
    auto acosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAcosMaxMinTmpSize(acosShape, 4, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 2);
    EXPECT_EQ(maxValue, 256 * 2);
}

TEST_F(TestTiling, TestTanhTiling)
{
    uint32_t maxVal = 0;
    uint32_t minVal = 0;
    GetTanhMaxMinTmpSize(ge::Shape({128}), 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 128 * 4 * 1);
    EXPECT_EQ(minVal, 256 * 1);
    GetTanhMaxMinTmpSize(ge::Shape({32}), 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 4);
    EXPECT_EQ(minVal, 256 * 4);
    uint32_t extraBuf = 123;
    uint32_t maxLivedNodesCnt = 123;
    GetTanhTmpBufferFactorSize(4, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(extraBuf, 0);
    EXPECT_EQ(maxLivedNodesCnt, 1);
    GetTanhTmpBufferFactorSize(2, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(extraBuf, 0);
    EXPECT_EQ(maxLivedNodesCnt, 4);
}

TEST_F(TestTiling, TestSigmoidTiling)
{
    std::vector<int64_t> shapeDims = {128};
    auto sigmoidShape = ge::Shape(shapeDims);
    uint32_t maxVal;
    uint32_t minVal;
    GetSigmoidMaxMinTmpSize(sigmoidShape, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 128 * 4);
    EXPECT_EQ(minVal, 256);
}

TEST_F(TestTiling, TestLogTilingMaxMin)
{
    std::vector<int64_t> shapeDims = {128};
    auto logShape = ge::Shape(shapeDims);
    uint32_t maxVal;
    uint32_t minVal;
    GetLogMaxMinTmpSize(logShape, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 0);
    EXPECT_EQ(minVal, 0);
    GetLog2MaxMinTmpSize(logShape, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 0);
    EXPECT_EQ(minVal, 0);
    GetLog2MaxMinTmpSize(logShape, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 4 * 128);
    EXPECT_EQ(minVal, 256);
    GetLog10MaxMinTmpSize(logShape, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 0);
    EXPECT_EQ(minVal, 0);
}

TEST_F(TestTiling, TestLogTilingFactor)
{
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetLogTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
    GetLog10TmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
    GetLog2TmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 2);
    EXPECT_EQ(extraBuf, 0);
    GetLog2TmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAcosTilingHalf16K)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto acosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAcosMaxMinTmpSize(acosShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 6 * 2);
    EXPECT_EQ(minValue, 256 * 6);
}

TEST_F(TestTiling, TestAcosTilingFloat16K)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto acosShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetAcosMaxMinTmpSize(acosShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 4);
    EXPECT_EQ(minValue, 256 * 2);
}

TEST_F(TestTiling, TestAsinhTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto asinhShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetAsinhMaxMinTmpSize(asinhShape, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 3);
    EXPECT_EQ(maxValue, 128 * 128 * 3 * 4);

    AscendC::GetAsinhMaxMinTmpSize(ge::Shape({32}), 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 3);
    EXPECT_EQ(maxValue, 256 * 3);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    AscendC::GetAsinhTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 3);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAsinhTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto asinhShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetAsinhMaxMinTmpSize(asinhShape, 2, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 3);
    EXPECT_EQ(maxValue, 128 * 128 * 3 * 2);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    AscendC::GetAsinhTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 3);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAcoshTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto acoshShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetAcoshMaxMinTmpSize(acoshShape, 2, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 2);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 2);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetAcoshTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 2);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAcoshTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto acoshShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetAcoshMaxMinTmpSize(acoshShape, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 1);
    EXPECT_EQ(maxValue, 128 * 128 * 1 * 4);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    AscendC::GetAcoshTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSelectWithBytesMaskTilingSameAxis)
{
    const auto shape = ge::Shape({8, 128});
    const auto scalarShape = ge::Shape({1});
    uint32_t maxValue;
    uint32_t minValue;
    GetSelectWithBytesMaskMaxMinTmpSize(shape, scalarShape, 2, shape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512);
    GetSelectWithBytesMaskMaxMinTmpSize(scalarShape, shape, 2, shape, 1, false, maxValue, minValue);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512);
}

TEST_F(TestTiling, TestSelectWithBytesMaskTilingSameAxisLargeShape)
{
    const auto shape = ge::Shape({128, 128});
    const auto scalarShape = ge::Shape({1});
    uint32_t maxValue;
    uint32_t minValue;
    GetSelectWithBytesMaskMaxMinTmpSize(shape, scalarShape, 2, shape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 4096 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512);
    GetSelectWithBytesMaskMaxMinTmpSize(scalarShape, shape, 2, shape, 1, false, maxValue, minValue);
    EXPECT_EQ(minValue, 4096 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512);
}

TEST_F(TestTiling, TestSelectWithBytesMaskTilingSameAxisSmallShape)
{
    const auto shape = ge::Shape({1, 16});
    const auto scalarShape = ge::Shape({1});
    uint32_t maxValue;
    uint32_t minValue;
    GetSelectWithBytesMaskMaxMinTmpSize(shape, scalarShape, 2, shape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 1024);
    GetSelectWithBytesMaskMaxMinTmpSize(scalarShape, shape, 2, shape, 1, false, maxValue, minValue);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 1024);
}

TEST_F(TestTiling, TestSelectWithBytesMaskTilingDiffAxis)
{
    const auto srcShape = ge::Shape({8, 128});
    const auto scalarShape = ge::Shape({1});
    const auto maskShape = ge::Shape({8, 160});
    uint32_t maxValue;
    uint32_t minValue;
    GetSelectWithBytesMaskMaxMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512);
    GetSelectWithBytesMaskMaxMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, false, maxValue, minValue);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512 + 8 * 128);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512 + 8 * 128);
    GetSelectWithBytesMaskMaxMinTmpSize(scalarShape, srcShape, 2, maskShape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512);
}

TEST_F(TestTiling, TestSelectWithBytesMaskTilingDiffAxisLargeShape)
{
    const auto srcShape = ge::Shape({128, 128});
    const auto scalarShape = ge::Shape({1});
    const auto maskShape = ge::Shape({128, 160});
    uint32_t maxValue;
    uint32_t minValue;
    GetSelectWithBytesMaskMaxMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 4096 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512);
    GetSelectWithBytesMaskMaxMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, false, maxValue, minValue);
    EXPECT_EQ(minValue, 4096 * 2 + 512 + 128 * 128);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512 + 128 * 128);
    GetSelectWithBytesMaskMaxMinTmpSize(scalarShape, srcShape, 2, maskShape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 4096 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512);
}

TEST_F(TestTiling, TestSelectWithBytesMaskTilingDiffAxisSmallShape)
{
    const auto srcShape = ge::Shape({1, 16});
    const auto scalarShape = ge::Shape({1});
    const auto maskShape = ge::Shape({1, 32});
    uint32_t maxValue;
    uint32_t minValue;
    GetSelectWithBytesMaskMaxMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 1024);
    GetSelectWithBytesMaskMaxMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, false, maxValue, minValue);
    EXPECT_EQ(minValue, 1024 + 32);
    EXPECT_EQ(maxValue, 1024 + 32);
    GetSelectWithBytesMaskMaxMinTmpSize(scalarShape, srcShape, 2, maskShape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 1024);
}

TEST_F(TestTiling, TestSelectTilingSameAxis)
{
    const auto shape = ge::Shape({8, 128});
    const auto scalarShape = ge::Shape({1});
    uint32_t maxValue;
    uint32_t minValue;
    maxValue = GetSelectWithBytesMaskMaxTmpSize(shape, scalarShape, 2, shape, 1, true);
    minValue = GetSelectWithBytesMaskMinTmpSize(shape, scalarShape, 2, shape, 1, true);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512);
    maxValue = GetSelectWithBytesMaskMaxTmpSize(shape, scalarShape, 2, shape, 1, false);
    minValue = GetSelectWithBytesMaskMinTmpSize(shape, scalarShape, 2, shape, 1, false);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512);
}

TEST_F(TestTiling, TestSelectTilingSameAxisLargeShape)
{
    const auto shape = ge::Shape({128, 128});
    const auto scalarShape = ge::Shape({1});
    uint32_t maxValue;
    uint32_t minValue;
    maxValue = GetSelectWithBytesMaskMaxTmpSize(shape, scalarShape, 2, shape, 1, true);
    minValue = GetSelectWithBytesMaskMinTmpSize(shape, scalarShape, 2, shape, 1, true);
    EXPECT_EQ(minValue, 4096 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512);
    maxValue = GetSelectWithBytesMaskMaxTmpSize(shape, scalarShape, 2, shape, 1, false);
    minValue = GetSelectWithBytesMaskMinTmpSize(shape, scalarShape, 2, shape, 1, false);
    EXPECT_EQ(minValue, 4096 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512);
}

TEST_F(TestTiling, TestSelectTilingSameAxisSmallShape)
{
    const auto shape = ge::Shape({1, 16});
    const auto scalarShape = ge::Shape({1});
    uint32_t maxValue;
    uint32_t minValue;
    maxValue = GetSelectWithBytesMaskMaxTmpSize(shape, scalarShape, 2, shape, 1, true);
    minValue = GetSelectWithBytesMaskMinTmpSize(shape, scalarShape, 2, shape, 1, true);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 1024);
    maxValue = GetSelectWithBytesMaskMaxTmpSize(shape, scalarShape, 2, shape, 1, false);
    minValue = GetSelectWithBytesMaskMinTmpSize(shape, scalarShape, 2, shape, 1, false);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 1024);
}

TEST_F(TestTiling, TestSelectTilingDiffAxis)
{
    const auto srcShape = ge::Shape({8, 128});
    const auto scalarShape = ge::Shape({1});
    const auto maskShape = ge::Shape({8, 160});
    uint32_t maxValue;
    uint32_t minValue;
    maxValue = GetSelectWithBytesMaskMaxTmpSize(srcShape, scalarShape, 2, maskShape, 1, true);
    minValue = GetSelectWithBytesMaskMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, true);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512);
    maxValue = GetSelectWithBytesMaskMaxTmpSize(srcShape, scalarShape, 2, maskShape, 1, false);
    minValue = GetSelectWithBytesMaskMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, false);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512 + 8 * 128);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512 + 8 * 128);
    maxValue = GetSelectWithBytesMaskMaxTmpSize(scalarShape, srcShape, 2, maskShape, 1, true);
    minValue = GetSelectWithBytesMaskMinTmpSize(scalarShape, srcShape, 2, maskShape, 1, true);
    GetSelectWithBytesMaskMaxMinTmpSize(scalarShape, srcShape, 2, maskShape, 1, true, maxValue, minValue);
    EXPECT_EQ(minValue, 128 * 8 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 8 * 2 + 512);
}

TEST_F(TestTiling, TestSelectTilingDiffAxisLargeShape)
{
    const auto srcShape = ge::Shape({128, 128});
    const auto scalarShape = ge::Shape({1});
    const auto maskShape = ge::Shape({128, 160});
    uint32_t maxValue;
    uint32_t minValue;
    maxValue = GetSelectWithBytesMaskMaxTmpSize(srcShape, scalarShape, 2, maskShape, 1, true);
    minValue = GetSelectWithBytesMaskMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, true);
    EXPECT_EQ(minValue, 4096 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512);
    maxValue = GetSelectWithBytesMaskMaxTmpSize(srcShape, scalarShape, 2, maskShape, 1, false);
    minValue = GetSelectWithBytesMaskMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, false);
    EXPECT_EQ(minValue, 4096 * 2 + 512 + 128 * 128);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512 + 128 * 128);
    maxValue = GetSelectWithBytesMaskMaxTmpSize(scalarShape, srcShape, 2, maskShape, 1, true);
    minValue = GetSelectWithBytesMaskMinTmpSize(scalarShape, srcShape, 2, maskShape, 1, true);
    EXPECT_EQ(minValue, 4096 * 2 + 512);
    EXPECT_EQ(maxValue, 128 * 128 * 2 + 512);
}

TEST_F(TestTiling, TestSelectTilingDiffAxisSmallShape)
{
    const auto srcShape = ge::Shape({1, 16});
    const auto scalarShape = ge::Shape({1});
    const auto maskShape = ge::Shape({1, 32});
    uint32_t maxValue;
    uint32_t minValue;
    maxValue = GetSelectWithBytesMaskMaxTmpSize(srcShape, scalarShape, 2, maskShape, 1, true);
    minValue = GetSelectWithBytesMaskMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, true);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 1024);
    maxValue = GetSelectWithBytesMaskMaxTmpSize(srcShape, scalarShape, 2, maskShape, 1, false);
    minValue = GetSelectWithBytesMaskMinTmpSize(srcShape, scalarShape, 2, maskShape, 1, false);
    EXPECT_EQ(minValue, 1024 + 32);
    EXPECT_EQ(maxValue, 1024 + 32);
    maxValue = GetSelectWithBytesMaskMaxTmpSize(scalarShape, srcShape, 2, maskShape, 1, true);
    minValue = GetSelectWithBytesMaskMinTmpSize(scalarShape, srcShape, 2, maskShape, 1, true);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 1024);
}

TEST_F(TestTiling, TestLayernormTiling)
{
    const uint32_t stackBufferSize = 100 * 1024;
    const uint32_t typeSize = 4;

    std::vector<int64_t> shapeDims = {128, 128, 128, 128, 128, 128};
    auto layernormShape = ge::Shape(shapeDims);
    const bool isReuseSource = false;
    optiling::LayerNormTiling tilling;
    AscendC::tiling::LayerNormTiling tilingNotOp;

    uint32_t minValue = 0;
    uint32_t maxValue = 0;

    AscendC::GetLayerNormMaxMinTmpSize(layernormShape, typeSize, isReuseSource, maxValue, minValue);
    EXPECT_EQ(maxValue, 3 * (128 * 128 * 128) * typeSize + 2 * (128 * 128) * typeSize);
    EXPECT_EQ(minValue, 3 * 128 * typeSize + 2 * (128 * 128) * typeSize);

    AscendC::GetLayerNormNDTillingInfo(layernormShape, 0, typeSize, isReuseSource, tilling);
    AscendC::GetLayerNormNDTillingInfo(layernormShape, stackBufferSize, typeSize, isReuseSource, tilling);
    EXPECT_EQ(tilling.get_tmpBufSize(), stackBufferSize / sizeof(float));

    AscendC::GetLayerNormNDTilingInfo(layernormShape, 0, typeSize, isReuseSource, tilling);
    AscendC::GetLayerNormNDTilingInfo(layernormShape, stackBufferSize, typeSize, isReuseSource, tilling);
    EXPECT_EQ(tilling.get_tmpBufSize(), stackBufferSize / sizeof(float));

    AscendC::GetLayerNormNDTilingInfo(layernormShape, 0, typeSize, isReuseSource, tilingNotOp);
    AscendC::GetLayerNormNDTilingInfo(layernormShape, stackBufferSize, typeSize, isReuseSource, tilingNotOp);
    EXPECT_EQ(tilingNotOp.tmpBufSize, stackBufferSize / sizeof(float));
}

TEST_F(TestTiling, TestGroupnormTiling)
{
    const uint32_t stackBufferSize = 100 * 1024;
    const uint32_t typeSize = 4;
    const uint32_t groupNum = 4;

    std::vector<int64_t> shapeDims = {16, 16, 8, 8};
    auto groupnormShape = ge::Shape(shapeDims);
    const bool isReuseSource = false;
    optiling::GroupNormTiling tilling;
    AscendC::tiling::GroupNormTiling tillingNotOp;

    uint32_t minValue = 0;
    uint32_t maxValue = 0;

    AscendC::GetGroupNormMaxMinTmpSize(groupnormShape, typeSize, isReuseSource, groupNum, maxValue, minValue);
    EXPECT_EQ(maxValue, 3 * (16 * 16 * 8 * 8) * typeSize + 2 * groupNum * 16 * typeSize);
    EXPECT_EQ(minValue, 3 * (16 / 4 * 8 * 8) * typeSize + 2 * groupNum * 16 * typeSize);

    AscendC::GetGroupNormNDTilingInfo(groupnormShape, stackBufferSize, typeSize, isReuseSource, groupNum, tilling);
    EXPECT_EQ(tilling.get_tmpBufSize(), stackBufferSize / sizeof(float));

    AscendC::GetGroupNormNDTilingInfo(groupnormShape, stackBufferSize, typeSize, isReuseSource, groupNum, tillingNotOp);
    EXPECT_EQ(tillingNotOp.tmpBufSize, stackBufferSize / sizeof(float));
}
TEST_F(TestTiling, TestRmsnormTiling)
{
    constexpr uint32_t bLength = 4;
    constexpr uint32_t sLength = 32;
    constexpr uint32_t hLength = 16;
    constexpr uint32_t bsLength = bLength * sLength;
    constexpr uint32_t bshLength = bLength * sLength * hLength;
    std::vector<int64_t> shapeDims = {bLength, sLength, hLength};
    auto shape = ge::Shape(shapeDims);
    constexpr uint32_t typeSize = 4;
    constexpr uint32_t ONE_BLK_FLOAT = 8;

    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    // common scene
    bool res = AscendC::GetRmsNormMaxMinTmpSize(shape, typeSize, maxValue, minValue);
    const uint32_t goldenMax = (bshLength + bsLength) * typeSize;
    uint32_t goldenMin = (hLength + ONE_BLK_FLOAT) * typeSize;
    EXPECT_EQ(res, true);
    EXPECT_EQ(maxValue, goldenMax);
    EXPECT_EQ(minValue, goldenMin);

    // basic block scene 1: input shape is illegal, fail to get minSize
    res = AscendC::GetRmsNormMaxMinTmpSize(shape, typeSize, maxValue, minValue, true);
    EXPECT_EQ(res, false);

    constexpr uint32_t BASIC_BLK_HLENGTH = 64;
    constexpr uint32_t BASIC_BLK_BSLENGTH = 8;
    shapeDims[2] = BASIC_BLK_HLENGTH;
    auto shape_basic_blk = ge::Shape(shapeDims); // 4,32,64
    // basic block scene 2: get minSize successfully
    res = AscendC::GetRmsNormMaxMinTmpSize(shape_basic_blk, typeSize, maxValue, minValue, true);
    goldenMin = (64 + 8) * typeSize;
    EXPECT_EQ(res, true);
    EXPECT_EQ(minValue, goldenMin);

    // basic block scene: get basic block using minTmpSize
    // goldenMin should be (BASIC_BLK_HLENGTH(64) * BASIC_BLK_BSLENGTH(8) + bsLength) * typeSize
    optiling::RmsNormTiling tiling;
    AscendC::tiling::RmsNormTiling tilingNotOp;
    res = AscendC::GetRmsNormTilingInfo(shape_basic_blk, shape_basic_blk, minValue, typeSize, tilingNotOp, true);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tilingNotOp.mainBshLength, 64);
    EXPECT_EQ(tilingNotOp.mainBsLength, 1);

    uint32_t tmpSize = (64 + 8) * 4; // shape: 4,32,64
    res = AscendC::GetRmsNormTilingInfo(shape_basic_blk, shape_basic_blk, minValue, typeSize, tiling, true);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tiling.get_mainBshLength(), 64);
    EXPECT_EQ(tiling.get_mainBsLength(), 1);

    auto shape1 = ge::Shape({1, 7, 16});
    res = AscendC::GetRmsNormMaxMinTmpSize(shape1, typeSize, maxValue, minValue);
    goldenMin = (8 + 16) * typeSize;
    EXPECT_EQ(minValue, goldenMin);

    uint32_t stackBufferSize = 100 * 1024;
    // common scene: get tiling info successfully, shape: 1,7,16
    res = AscendC::GetRmsNormTilingInfo(shape1, shape1, stackBufferSize, typeSize, tiling);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tiling.get_mainBshLength(), 1 * 7 * 16);
    EXPECT_EQ(tiling.get_mainBsLength(), 7);

    stackBufferSize = hLength;
    // common scene: fail to get tiling info because of small stack buffer
    res = AscendC::GetRmsNormTilingInfo(shape, shape, stackBufferSize, typeSize, tiling);
    EXPECT_EQ(res, false);

    // basic block scene: get basic block tiling info successfully
    stackBufferSize = 100 * 1024; // shape: 4,32,64
    res = AscendC::GetRmsNormTilingInfo(shape_basic_blk, shape_basic_blk, stackBufferSize, typeSize, tiling, true);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tiling.get_mainBshLength(), 4 * 32 * 64);

    // basic block scene: get basic block tiling info successfully
    stackBufferSize = (8 * 128 + 7) * 4;
    auto shape2 = ge::Shape({1, 8, 128});
    res = AscendC::GetRmsNormTilingInfo(shape2, shape2, stackBufferSize, typeSize, tiling, true);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tiling.get_mainBshLength(), 896);
    EXPECT_EQ(tiling.get_mainBsLength(), 7);

    stackBufferSize = (8 * 128 + 8) * 4; // shape: 1,8,128
    res = AscendC::GetRmsNormTilingInfo(shape2, shape2, stackBufferSize, typeSize, tiling, true);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tiling.get_mainBshLength(), 128 * 8);
    EXPECT_EQ(tiling.get_mainBsLength(), 8);

    stackBufferSize = (8 * 128 + 9) * 4; // shape: 1,8,128
    res = AscendC::GetRmsNormTilingInfo(shape2, shape2, stackBufferSize, typeSize, tiling, true);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tiling.get_mainBshLength(), 128 * 8);
    EXPECT_EQ(tiling.get_mainBsLength(), 8);

    // general case: bs > 256, set bs to 2*255+2
    stackBufferSize = 32 * 512 * 4;
    auto shape3 = ge::Shape({1, 512, 16}); // bs bigger than max_repeat(255)
    res = AscendC::GetRmsNormTilingInfo(shape3, shape3, stackBufferSize, typeSize, tiling);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tiling.get_mainBshLength(), 255 * 16);
    EXPECT_EQ(tiling.get_mainBsLength(), 255);
    EXPECT_EQ(tiling.get_tailBsLength(), 2);
    EXPECT_EQ(tiling.get_loopRound(), 2);

    // abnormal case: input shape != original shape
    res = AscendC::GetRmsNormTilingInfo(shape2, shape3, stackBufferSize, typeSize, tiling);
    EXPECT_EQ(res, false);

    // abnormal case: basic block doesnot support h >= 2048
    stackBufferSize = 16 * 2048 * 4;
    auto shape4 = ge::Shape({1, 8, 2048});
    res = AscendC::GetRmsNormTilingInfo(shape4, shape4, stackBufferSize, typeSize, tiling, true);
    EXPECT_EQ(res, false);

    stackBufferSize = 2048;
    shape4 = ge::Shape({14, 1, 56});
    res = AscendC::GetRmsNormTilingInfo(shape4, shape4, stackBufferSize, typeSize, tiling);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tiling.get_mainBshLength(), 448);
    EXPECT_EQ(tiling.get_mainBsLength(), 8);
    EXPECT_EQ(tiling.get_tailBshLength(), 336);
    EXPECT_EQ(tiling.get_tailBsLength(), 6);
    EXPECT_EQ(tiling.get_loopRound(), 1);

    stackBufferSize = 2080;
    res = AscendC::GetRmsNormTilingInfo(shape4, shape4, stackBufferSize, typeSize, tiling);
    EXPECT_EQ(res, true);
    EXPECT_EQ(tiling.get_mainBshLength(), 504);
    EXPECT_EQ(tiling.get_mainBsLength(), 9);
    EXPECT_EQ(tiling.get_tailBshLength(), 280);
    EXPECT_EQ(tiling.get_tailBsLength(), 5);
    EXPECT_EQ(tiling.get_loopRound(), 1);
}

TEST_F(TestTiling, TestBatchnormTiling)
{
    constexpr uint32_t bLength = 8;
    constexpr uint32_t sLength = 1;
    constexpr uint32_t hLength = 16;
    constexpr uint32_t originalBLength = 8;
    constexpr uint32_t shLength = sLength * hLength;
    constexpr uint32_t bshLength = originalBLength * sLength * hLength;
    std::vector<int64_t> shapeDims = {bLength, sLength, hLength};
    std::vector<int64_t> originShape_dims = {originalBLength, sLength, hLength};
    auto shape = ge::Shape(shapeDims);
    auto originShape = ge::Shape(originShape_dims);
    bool reuseSrc = false;
    constexpr uint32_t typeSize = 4;
    constexpr uint32_t halfTypeSize = 2;

    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    // common scene
    bool res = AscendC::GetBatchNormMaxMinTmpSize(shape, originShape, typeSize, reuseSrc, maxValue, minValue, false);
    const uint32_t goldenMax = (3 * bshLength + 2 * shLength) * typeSize;
    uint32_t goldenMin = (3 * originalBLength * 8 + 2 * shLength) * typeSize;
    EXPECT_EQ(res, true);
    EXPECT_EQ(maxValue, goldenMax);
    EXPECT_EQ(minValue, goldenMin);

    // basic block scene 1: input shape is illegal, fail to get minSize
    res = AscendC::GetBatchNormMaxMinTmpSize(shape, originShape, typeSize, reuseSrc, maxValue, minValue, true);
    EXPECT_EQ(res, false);

    constexpr uint32_t BASIC_BLK_BLENGTH = 2;
    constexpr uint32_t BASIC_BLK_SHLENGTH = 64;
    originShape_dims[1] = 4;
    originShape_dims[2] = 16;
    auto shape_basic_blk = ge::Shape(originShape_dims);
    // basic block scene 2: get minSize successfully
    res = AscendC::GetBatchNormMaxMinTmpSize(
        shape_basic_blk, shape_basic_blk, typeSize, reuseSrc, maxValue, minValue, true);
    goldenMin = (3 * originalBLength * BASIC_BLK_SHLENGTH + 2 * 4 * 16) * typeSize;
    EXPECT_EQ(res, true);
    EXPECT_EQ(minValue, goldenMin);

    // basic block scene: get basic block using minTmpSize, shape = [8,4,16,2]
    optiling::BatchNormTiling tiling;
    AscendC::tiling::BatchNormTiling tilingNotOp;
    res = AscendC::GetBatchNormNDTilingInfo(
        shape_basic_blk, shape_basic_blk, minValue, typeSize, reuseSrc, tilingNotOp, true);
    EXPECT_EQ(res, true);

    res =
        AscendC::GetBatchNormNDTilingInfo(shape_basic_blk, shape_basic_blk, minValue, typeSize, reuseSrc, tiling, true);
    EXPECT_EQ(res, true);
    res = AscendC::GetBatchNormMaxMinTmpSize(
        shape_basic_blk, shape_basic_blk, halfTypeSize, reuseSrc, maxValue, minValue, true);
    res = AscendC::GetBatchNormNDTilingInfo(
        shape_basic_blk, shape_basic_blk, minValue, halfTypeSize, reuseSrc, tiling, true);
    EXPECT_EQ(res, true);

    uint32_t stackBufferSize = 100 * 1024;
    // common scene: get tiling info successfully
    res =
        AscendC::GetBatchNormNDTilingInfo(originShape, originShape, stackBufferSize, typeSize, reuseSrc, tiling, false);
    EXPECT_EQ(res, true);

    stackBufferSize = bLength;
    // common scene: fail to get tiling info because of small stack buffer
    res =
        AscendC::GetBatchNormNDTilingInfo(originShape, originShape, stackBufferSize, typeSize, reuseSrc, tiling, false);
    EXPECT_EQ(res, false);

    // basic block scene: get basic block tiling info successfully
    stackBufferSize = 100 * 1024;
    res = AscendC::GetBatchNormNDTilingInfo(
        shape_basic_blk, shape_basic_blk, stackBufferSize, typeSize, reuseSrc, tiling, true);
    EXPECT_EQ(res, true);

    // basic block scene: fail to get basic block using buffer less than minValue
    goldenMin = (3 * originalBLength * BASIC_BLK_SHLENGTH + 2 * 4 * 6 - 1) * typeSize;
    res = AscendC::GetBatchNormMaxMinTmpSize(
        shape_basic_blk, shape_basic_blk, typeSize, reuseSrc, maxValue, minValue, true);
    res = AscendC::GetBatchNormNDTilingInfo(
        shape_basic_blk, shape_basic_blk, goldenMin, typeSize, reuseSrc, tiling, true);
    EXPECT_EQ(res, false);
}

TEST_F(TestTiling, TestDeepnormTiling)
{
    const uint32_t stackBufferSize = 100 * 1024;
    const uint32_t typeSize = 4;
    const int64_t bLength = 2;
    const int64_t sLength = 8;
    const int64_t hLength = 128;
    const int64_t oriHLength = 120;

    std::vector<int64_t> shapeDims = {bLength, sLength, hLength};
    std::vector<int64_t> original_shape_dims = {bLength, sLength, oriHLength};
    auto deepnormShape = ge::Shape(shapeDims);
    auto oriDeepNormShape = ge::Shape(original_shape_dims);
    const bool varFalse = false;
    const bool varTrue = true;
    optiling::DeepNormTiling tiling;
    AscendC::tiling::DeepNormTiling tilingNotOp;

    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    // reuse = false, isbasic = false
    AscendC::GetDeepNormMaxMinTmpSize(deepnormShape, typeSize, varFalse, varFalse, maxValue, minValue);
    EXPECT_EQ(minValue, 3 * hLength * typeSize + 2 * (bLength * sLength) * typeSize);
    EXPECT_EQ(maxValue, 3 * (bLength * sLength * hLength) * typeSize + 2 * (bLength * sLength) * typeSize);

    // isbasic = true,  b*s must be divisible by 8
    std::vector<int64_t> wrong_shape_dims = {1, 9, 64};
    auto wrongDeepNormShape = ge::Shape(wrong_shape_dims);
    bool res = AscendC::GetDeepNormMaxMinTmpSize(wrongDeepNormShape, typeSize, varFalse, varTrue, maxValue, minValue);
    EXPECT_EQ(res, false);

    // isbasic = true, hLength must be divisible by 64
    wrong_shape_dims = {2, 8, 72};
    wrongDeepNormShape = ge::Shape(wrong_shape_dims);
    res = AscendC::GetDeepNormMaxMinTmpSize(wrongDeepNormShape, typeSize, varFalse, varTrue, maxValue, minValue);
    EXPECT_EQ(res, false);

    AscendC::GetDeepNormTilingInfo(
        deepnormShape, oriDeepNormShape, stackBufferSize, typeSize, varFalse, varFalse, tiling);
    EXPECT_EQ(tiling.get_tmpBufSize(), stackBufferSize / sizeof(float));
    AscendC::GetDeepNormTilingInfo(
        deepnormShape, oriDeepNormShape, stackBufferSize, typeSize, varFalse, varFalse, tilingNotOp);
    EXPECT_EQ(tilingNotOp.tmpBufSize, stackBufferSize / sizeof(float));

    // originalB = b, originalH = h
    wrong_shape_dims = {1, 8, 128}; // originalb != b
    wrongDeepNormShape = ge::Shape(wrong_shape_dims);
    res = AscendC::GetDeepNormTilingInfo(
        deepnormShape, wrongDeepNormShape, stackBufferSize, typeSize, varFalse, varFalse, tiling);
    EXPECT_EQ(res, false);

    // hlength must align to 32
    wrong_shape_dims = {1, 8, 4};
    wrongDeepNormShape = ge::Shape(wrong_shape_dims);
    res = AscendC::GetDeepNormTilingInfo(
        wrongDeepNormShape, wrongDeepNormShape, stackBufferSize, typeSize, varFalse, varFalse, tiling);
    EXPECT_EQ(res, false);

    // originalHlength <= hLength
    wrong_shape_dims = {2, 8, 136};
    wrongDeepNormShape = ge::Shape(wrong_shape_dims);
    res = AscendC::GetDeepNormTilingInfo(
        deepnormShape, wrongDeepNormShape, stackBufferSize, typeSize, varFalse, varFalse, tiling);
    EXPECT_EQ(res, false);

    // when basicblock, b*s must be divisible by 8
    wrong_shape_dims = {1, 4, 64};
    wrongDeepNormShape = ge::Shape(wrong_shape_dims);
    res = AscendC::GetDeepNormTilingInfo(
        wrongDeepNormShape, wrongDeepNormShape, stackBufferSize, typeSize, varFalse, varTrue, tiling);
    EXPECT_EQ(res, false);

    // when isbasicblock, origianlH must equal to H
    res = AscendC::GetDeepNormTilingInfo(
        deepnormShape, oriDeepNormShape, stackBufferSize, typeSize, varFalse, varTrue, tiling);
    EXPECT_EQ(res, false);

    // hLength <= 255 * 8
    wrong_shape_dims = {1, 4, 2048};
    wrongDeepNormShape = ge::Shape(wrong_shape_dims);
    res = AscendC::GetDeepNormTilingInfo(
        wrongDeepNormShape, wrongDeepNormShape, stackBufferSize, typeSize, varFalse, varFalse, tiling);
    EXPECT_EQ(res, false);

    // assume initial oneTmpSize is 9*n, update to 8*n for efficiency when isBasicBlock
    // tiling.oneTmpSize before update: 704   after update: 512
    std::vector<int64_t> basicblk_shape_dims = {4, 4, 64};
    auto basicblkDeepNormShape = ge::Shape(basicblk_shape_dims);
    res = AscendC::GetDeepNormTilingInfo(
        basicblkDeepNormShape, basicblkDeepNormShape, 8874, typeSize, varFalse, varTrue, tiling);
    EXPECT_EQ(tiling.get_oneTmpSize(), 512);
}

TEST_F(TestTiling, TestExpTiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto expShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;

    // float   isReuseSrc = false  3 tmpBuffer
    AscendC::GetExpMaxMinTmpSize(expShape, 4, false, maxValue, minValue);
    EXPECT_EQ(minValue, 3 * 256);
    EXPECT_EQ(maxValue, 3 * 128 * 128 * 4);
    // float   isReuseSrc = true   2 tmpBuffer
    AscendC::GetExpMaxMinTmpSize(expShape, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 2 * 256);
    EXPECT_EQ(maxValue, 2 * 128 * 128 * 4);
    // half    4 tmpBuffer
    AscendC::GetExpMaxMinTmpSize(expShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 4 * 256);
    EXPECT_EQ(maxValue, 4 * 128 * 128 * 4);
    AscendC::GetExpMaxMinTmpSize(expShape, 2, true, maxValue, minValue);
    EXPECT_EQ(minValue, 4 * 256);
    EXPECT_EQ(maxValue, 4 * 128 * 128 * 4);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    AscendC::GetExpTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 3);
    EXPECT_EQ(extraBuf, 0);
    AscendC::GetExpTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 8);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestMatmulApiTilngFactorSplit1)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(176, 1680, 608);
    tiling.SetOrgShape(176, 1680, 608);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 196608);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseM(), 128);
}

TEST_F(TestTiling, TestMatmulApiTilngFactorSplit2)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1680, 176, 608);
    tiling.SetOrgShape(1680, 176, 608);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 290816);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseM(), 128);
}

TEST_F(TestTiling, TestMatmulApiTilngDimfactors)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(8272, 3216, 16);
    tiling.SetOrgShape(8272, 3216, 16);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);

    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 372736);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseK(), 16);
}

// when A matrix or B matrix is in TSCM, then calculaye loadSize should ignore it;
TEST_F(TestTiling, TestMatmulApiTilngBMatrixTSCM)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(32, 64, 128);
    tiling.SetOrgShape(32, 64, 128);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(2048, -1, 9362);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 2048);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseK(), 32);
}

TEST_F(TestTiling, TestMatmulApiTilngFailed1)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(83, 592, 160);
    tiling.SetOrgShape(83, 592, 160);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);
}

// single m = 970, upper round is 976 which is 61 times of 16. the value is not good.
// so tiling func will round to 992, which is 62 times of 16
TEST_F(TestTiling, TestMatmulApiTilngFailed2)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(970, 32, 182);
    tiling.SetOrgShape(970, 32, 182);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);
}

// when B matrix is full load, l1Status.nBL1 * l0Status.nL0 is large then coreStatus.n or
// then calculate bl1size is smaller then the baseN * baseK * depthB1,
// which result in final L1Size is larged then the provided size;
TEST_F(TestTiling, TestMatmulApiTilngFailed3)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(83, 592, 160);
    tiling.SetOrgShape(83, 592, 160);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);
}

// baseM * basek + BIAS are larger then l1size
TEST_F(TestTiling, TestMatmulApiTilngFailed4)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(32, 64, 128);
    tiling.SetOrgShape(32, 64, 128);
    tiling.SetFixSplit(32, 64, -1);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(4096 * 4 + 64 * 4, -1, -1, 1024);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreCase1)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(24);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetOrgShape(65536, 32768, 65536);
    tiling.SetShape(65536, 32768, 65536);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 393216);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreCase2)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(2);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetOrgShape(64, 64, 64);
    tiling.SetShape(64, 64, 64);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 12288);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreCase3)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(2);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetOrgShape(65536, 64, 32);
    tiling.SetShape(65536, 64, 32);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 264192);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreCase4)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(2);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetOrgShape(2048, 2048, 256);
    tiling.SetShape(2048, 2048, 256);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 393216);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreWithCppStruct)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(2);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetOrgShape(2048, 2048, 256);
    tiling.SetShape(2048, 2048, 256);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    AscendC::tiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.shareL1Size, 393216);
    EXPECT_GE(tilingData.transLength, 0);
    matmul_tiling::SysTilingTempBufSize bufSize;
    EXPECT_EQ(MultiCoreMatmulGetTmpBufSizeV2(tilingData, bufSize), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngKNotAlign)
{
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(24);

    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    tilingApi.SetOrgShape(192, 384, 1952);
    tilingApi.SetShape(192, 384, 1952);
    tilingApi.EnableBias(false);

    tilingApi.SetBufferSpace(-1, -1, -1);
    int64_t res = tilingApi.GetTiling(tilingData);

    EXPECT_EQ(res, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 129024);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreBTSCM)
{
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(24);

    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    tilingApi.SetOrgShape(65536, 64, 64);
    tilingApi.SetShape(65536, 64, 64);
    tilingApi.EnableBias(false);

    tilingApi.SetBufferSpace(-1, -1, -1);
    int64_t res = tilingApi.GetTiling(tilingData);

    EXPECT_EQ(res, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 262144);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreBTSCM1)
{
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(48);

    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    tilingApi.SetOrgShape(64, 256 * 4, 256 * 4);
    tilingApi.SetShape(64, 256 * 4, 256 * 4);
    tilingApi.EnableBias(false);

    tilingApi.SetBufferSpace(-1, -1, -1);
    int64_t res = tilingApi.GetTiling(tilingData);

    EXPECT_EQ(res, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 65536);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreBTSCM2)
{
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(48);

    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    tilingApi.SetOrgShape(48, 256, 256);
    tilingApi.SetShape(48, 256, 256);
    tilingApi.EnableBias(false);

    tilingApi.SetBufferSpace(-1, -1, -1);
    int64_t res = tilingApi.GetTiling(tilingData);

    EXPECT_EQ(res, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 8192);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreBTSCM3)
{
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(48);

    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    tilingApi.SetOrgShape(16, 512, 512);
    tilingApi.SetShape(16, 512, 512);
    tilingApi.EnableBias(false);

    tilingApi.SetBufferSpace(-1, -1, -1);
    int64_t res = tilingApi.GetTiling(tilingData);

    EXPECT_EQ(res, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 16384);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreBTSCM4)
{
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(48);

    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    tilingApi.SetOrgShape(2560, 256, 256);
    tilingApi.SetShape(2560, 256, 256);
    tilingApi.EnableBias(false);

    tilingApi.SetBufferSpace(-1, -1, -1);
    int64_t res = tilingApi.GetTiling(tilingData);

    EXPECT_EQ(res, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 32768);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngSingleCoreFullLoadCase)
{
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(24);

    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, true);
    tilingApi.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    tilingApi.SetOrgShape(2048, 2048, 204);
    tilingApi.SetShape(2048, 2048, 204);
    tilingApi.EnableBias(false);
    tilingApi.SetBufferSpace(-1, -1, -1);
    int64_t res = tilingApi.GetTiling(tilingData);

    EXPECT_EQ(res, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 458752);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngSingleCoreFullLoadND2NZCase)
{
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(24);

    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, true);
    tilingApi.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    tilingApi.SetOrgShape(2048, 2048, 204);
    tilingApi.SetShape(2048, 2048, 204);
    tilingApi.EnableBias(false);
    tilingApi.SetBufferSpace(-1, -1, -1);
    tilingApi.SetMatmulConfigParams(1, false, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    tilingApi.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    int64_t res = tilingApi.GetTiling(tilingData);

    EXPECT_EQ(res, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 98304);
    EXPECT_GE(tilingData.get_transLength(), 65536);
}

TEST_F(TestTiling, TestMatmulApiTilngMultiCoreBTSCM5)
{
    optiling::TCubeTiling tilingData;
    MultiCoreMatmulTiling tilingApi;
    tilingApi.SetDim(12);

    tilingApi.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tilingApi.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tilingApi.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);

    tilingApi.SetOrgShape(32, 256, 768);
    tilingApi.SetShape(32, 256, 768);
    tilingApi.EnableBias(true);
    tilingApi.SetAlignSplit(-1, 64, -1);
    tilingApi.SetBufferSpace(-1, -1, -1);
    int64_t res = tilingApi.GetTiling(tilingData);

    EXPECT_EQ(res, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 24832);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestConcatTiling)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    const uint32_t elemCount = 128;

    AscendC::GetConcatTmpSize(plat, elemCount, 2);
}

TEST_F(TestTiling, TestSortTiling)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    const uint32_t elemCount = 128;

    AscendC::GetSortTmpSize(plat, elemCount, 4);
}

TEST_F(TestTiling, TestUnPadTiling)
{
    const uint32_t stackBufferSize = 100 * 1024;
    const uint32_t typeSize = 4;

    std::vector<int64_t> shapeDims = {32, 32};
    auto srcShape = ge::Shape(shapeDims);
    optiling::UnPadTiling tiling;
    AscendC::tiling::UnPadTiling tilingNotOp;

    AscendC::UnPadTilingFunc(srcShape, 0, typeSize, tiling);
    AscendC::UnPadTilingFunc(srcShape, stackBufferSize, typeSize, tiling);
    AscendC::UnPadTilingFunc(srcShape, 0, typeSize, tilingNotOp);
    AscendC::UnPadTilingFunc(srcShape, stackBufferSize, typeSize, tilingNotOp);
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetUnPadMaxMinTmpSize(plat, srcShape, typeSize, maxValue, minValue);
}

TEST_F(TestTiling, TestPadTiling)
{
    const uint32_t stackBufferSize = 100 * 1024;
    const uint32_t typeSize = 4;

    std::vector<int64_t> shapeDims = {32, 32};
    std::vector<int64_t> ori_shape_dims = {32, 31};
    auto srcShape = ge::Shape(shapeDims);
    auto oriSrcShape = ge::Shape(ori_shape_dims);
    optiling::PadTiling tiling;
    AscendC::tiling::PadTiling tilingNotOp;

    AscendC::PadTilingFunc(srcShape, oriSrcShape, stackBufferSize, typeSize, tiling);
    AscendC::PadTilingFunc(srcShape, oriSrcShape, 0, typeSize, tiling);
    AscendC::PadTilingFunc(srcShape, oriSrcShape, stackBufferSize, typeSize, tilingNotOp);
    AscendC::PadTilingFunc(srcShape, oriSrcShape, 0, typeSize, tilingNotOp);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetPadMaxMinTmpSize(srcShape, typeSize, maxValue, minValue);
}

TEST_F(TestTiling, TestPadTilingBrcbFractalTailRepeatTimes)
{
    const uint32_t typeSize = 2;
    struct BrcbFractalTailCase {
        uint32_t stackBufferSize;
        int64_t srcWidth;
        uint32_t expectedFractalTail;
        uint32_t expectedRepeatTimes;
        uint32_t expectedRepeatTimesTail;
    };

    const std::vector<BrcbFractalTailCase> testCases = {
        {4 * 1024, 64, 0, 0, 0},        {4 * 1024, 65, 1, 0, 1},        {4 * 1024, 71, 7, 0, 1},
        {4 * 1024, 72, 8, 0, 1},        {4 * 1024, 73, 9, 0, 2},        {256 * 1024, 2024, 2024, 0, 253},
        {256 * 1024, 2025, 2025, 1, 0}, {256 * 1024, 2032, 2032, 1, 0}, {256 * 1024, 2033, 2033, 1, 1},
    };

    for (const auto& testCase : testCases) {
        SCOPED_TRACE(testCase.srcWidth);
        auto srcShape = ge::Shape({1, testCase.srcWidth});
        auto oriSrcShape = ge::Shape({1, testCase.srcWidth - 1});
        optiling::PadTiling tiling;

        AscendC::PadTilingFunc(srcShape, oriSrcShape, testCase.stackBufferSize, typeSize, tiling);

        EXPECT_EQ(tiling.get_brcbFractalTail(), testCase.expectedFractalTail);
        EXPECT_EQ(tiling.get_brcbFractalTailRepeatTimes(), testCase.expectedRepeatTimes);
        EXPECT_EQ(tiling.get_brcbFractalTailRepeatTimesTail(), testCase.expectedRepeatTimesTail);
    }
}

TEST_F(TestTiling, TestLayernormGradTiling)
{
    const uint32_t stackBufferSize = 100 * 1024;

    std::vector<int64_t> shapeDims = {128, 128, 128, 128, 128, 128};
    auto layernormgradShape = ge::Shape(shapeDims);
    optiling::LayerNormGradTiling tiling;
    AscendC::tiling::LayerNormGradTiling tilingNotOp;

    AscendC::GetLayerNormGradNDTilingInfo(layernormgradShape, stackBufferSize, 4, false, tiling);
    EXPECT_EQ(tiling.get_stackBufferSize(), stackBufferSize);

    AscendC::GetLayerNormGradNDTilingInfo(layernormgradShape, stackBufferSize, 4, false, tilingNotOp);
    EXPECT_EQ(tilingNotOp.stackBufferSize, stackBufferSize);

    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetLayerNormGradMaxMinTmpSize(layernormgradShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, (128 * 128 * 128) * 9 * 2);
    EXPECT_EQ(minValue, 128 * 128 * 9 * 2);

    AscendC::GetLayerNormGradMaxMinTmpSize(layernormgradShape, 4, true, maxValue, minValue);
    EXPECT_EQ(maxValue, (128 * 128 * 128) * 4 * 4);
    EXPECT_EQ(minValue, 128 * 128 * 4 * 4);

    AscendC::GetLayerNormGradMaxMinTmpSize(layernormgradShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, (128 * 128 * 128) * 6 * 4);
    EXPECT_EQ(minValue, 128 * 128 * 6 * 4);
}

TEST_F(TestTiling, TestLayernormGradBetaTiling)
{
    const uint32_t stackBufferSize = 100 * 1024 * 1024;
    const uint32_t typeSize = 4;

    std::vector<int64_t> shapeDims = {128, 128, 128, 128, 128, 128};
    auto layernormgradbetaShape = ge::Shape(shapeDims);
    const bool isReuseSource = false;

    optiling::LayerNormGradBetaTiling tiling;
    AscendC::tiling::LayerNormGradBetaTiling tilingNotOp;

    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetLayerNormGradBetaMaxMinTmpSize(layernormgradbetaShape, 4, isReuseSource, maxValue, minValue);
    EXPECT_EQ(maxValue, (128 * 128 * 128) * 4 * 2);
    EXPECT_EQ(minValue, 128 * 4 * 2);

    AscendC::GetLayerNormGradBetaMaxMinTmpSize(layernormgradbetaShape, 2, isReuseSource, maxValue, minValue);
    EXPECT_EQ(maxValue, (128 * 128 * 128) * 4 * 4);
    EXPECT_EQ(minValue, 128 * 4 * 4);

    AscendC::GetLayerNormGradBetaNDTilingInfo(layernormgradbetaShape, stackBufferSize, typeSize, isReuseSource, tiling);
    EXPECT_EQ(tiling.get_stackBufferSize(), stackBufferSize / sizeof(float));

    AscendC::GetLayerNormGradBetaNDTilingInfo(
        layernormgradbetaShape, stackBufferSize, typeSize, isReuseSource, tilingNotOp);
    EXPECT_EQ(tilingNotOp.stackBufferSize, stackBufferSize / sizeof(float));
}

TEST_F(TestTiling, TestConfusionTransposeTiling)
{
    const uint32_t stackBufferSize = 100 * 1024;
    const uint32_t typeSize = 2;

    std::vector<int64_t> shapeDims = {1, 2, 64, 32};
    auto srcShape = ge::Shape(shapeDims);
    optiling::ConfusionTransposeTiling tiling;
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 1, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 2, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 3, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 4, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 5, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 6, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 7, tiling);
}

TEST_F(TestTiling, TestConfusionTransposeTilingWithCppStruct)
{
    const uint32_t stackBufferSize = 100 * 1024;
    const uint32_t typeSize = 2;

    std::vector<int64_t> shapeDims = {1, 2, 64, 32};
    auto srcShape = ge::Shape(shapeDims);
    AscendC::tiling::ConfusionTransposeTiling tiling;
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 1, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 2, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 3, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 4, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 5, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 6, tiling);
    AscendC::GetConfusionTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 7, tiling);
}

TEST_F(TestTiling, TestGetTransposeTilingInfoWithCppStruct)
{
    const uint32_t stackBufferSize = 100 * 1024;
    const uint32_t typeSize = 2;

    std::vector<int64_t> shapeDims = {1, 2, 64, 32};
    auto srcShape = ge::Shape(shapeDims);
    AscendC::tiling::ConfusionTransposeTiling tiling;
    AscendC::GetTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 1, tiling);
    AscendC::GetTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 2, tiling);
    AscendC::GetTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 3, tiling);
    AscendC::GetTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 4, tiling);
    AscendC::GetTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 5, tiling);
    AscendC::GetTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 6, tiling);
    AscendC::GetTransposeTilingInfo(srcShape, stackBufferSize, typeSize, 7, tiling);
}

TEST_F(TestTiling, TestMatmulApiTilngL0BNoDB)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(16, 2048, 128);
    tiling.SetOrgShape(2048, 2048, 128);
    tiling.SetFixSplit(16, 2048, -1);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(453632, 131072, -1, 1024);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 266240);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngL0ANoDB)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(2048, 16, 128);
    tiling.SetOrgShape(2048, 2048, 128);
    tiling.SetFixSplit(2048, 16, -1);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(453632, 131072, -1, 1024);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 266240);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

class DynamicRnnTiling {
public:
    int32_t sysAivCoreNum;
    int32_t batch;
    int32_t inputSize;
    int32_t hiddenSize;
    int32_t maxUbSize;
    optiling::TCubeTiling matmulTiling;
    int32_t baseM;
    int32_t baseN;
    int32_t baseK;
    int32_t singleM;
    int32_t singleN;
    int32_t singleK;
    int32_t usedCoreNum;
};

class DynamicRNNTilingDataTik2 {
public:
    optiling::TCubeTiling inputMMParam;
    optiling::TCubeTiling hiddenMMParam;
};

struct RnnParams {
    uint32_t batch;
    uint32_t inputSize;
    uint32_t hiddenSize;
    uint32_t sysAivCoreNum;
    uint32_t maxUbSize;
};

class RnnTilingbTestSuite : public testing::Test, public testing::WithParamInterface<RnnParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(
    TEST_RNN_TILING, RnnTilingbTestSuite,
    ::testing::Values(
        RnnParams{1280, 256, 128, 48, 24 * 1024}, RnnParams{36, 512, 256, 48, 128 * 1024 - 64},
        RnnParams{48, 512, 256, 48, 128 * 1024 - 64}, RnnParams{64, 512, 256, 48, 128 * 1024 - 64},
        RnnParams{36, 768, 1024, 48, 128 * 1024 - 64}, RnnParams{48, 768, 1024, 48, 128 * 1024 - 64},
        RnnParams{64, 768, 1024, 48, 128 * 1024 - 64}, RnnParams{16, 16, 512, 48, 128 * 1024 - 64},
        RnnParams{64, 256, 256, 48, 128 * 1024 - 64}, RnnParams{64, 128, 128, 48, 128 * 1024 - 64},
        RnnParams{64, 256, 128, 48, 128 * 1024 - 64}, RnnParams{64, 512, 128, 48, 128 * 1024 - 64},
        RnnParams{64, 512, 256, 48, 128 * 1024 - 64}, RnnParams{1280, 256, 128, 48, 128 * 1024 - 64},
        RnnParams{1280, 256, 256, 48, 128 * 1024 - 64}, RnnParams{1280, 512, 128, 48, 128 * 1024 - 64},
        RnnParams{1280, 512, 256, 48, 128 * 1024 - 64}, RnnParams{1920, 256, 128, 48, 128 * 1024 - 64},
        RnnParams{1920, 256, 256, 48, 128 * 1024 - 64}, RnnParams{1920, 512, 128, 48, 128 * 1024 - 64},
        RnnParams{1920, 512, 256, 48, 128 * 1024 - 64}, RnnParams{2560, 256, 128, 48, 128 * 1024 - 64},
        RnnParams{2560, 256, 256, 48, 128 * 1024 - 64}, RnnParams{2560, 512, 128, 48, 128 * 1024 - 64},
        RnnParams{2560, 512, 256, 48, 128 * 1024 - 64}, RnnParams{48, 512, 256, 48, 128 * 1024 - 64},
        RnnParams{64, 1536, 1024, 48, 128 * 1024 - 64}, RnnParams{2560, 5120, 9760, 48, 128 * 1024 - 64},
        RnnParams{479, 96, 381, 48, 128 * 1024 - 64}));

TEST_P(RnnTilingbTestSuite, TestMatmulApiTilngRnnRealCase)
{
    auto param = GetParam();
    DynamicRNNTilingDataTik2 tilingData;
    DynamicRnnTiling rnnParams;
    matmul_tiling::MultiCoreMatmulTiling rnnMatmul, rnnMatmul1, rnnMatmul2;
    rnnParams.batch = param.batch;
    rnnParams.inputSize = param.inputSize;
    rnnParams.hiddenSize = param.hiddenSize;
    rnnParams.sysAivCoreNum = param.sysAivCoreNum;
    rnnParams.maxUbSize = param.maxUbSize;
    int32_t dataType = 0;

    bool isFullLoadWeightOne = false;
    bool isFullLoadWeight = false;

    rnnMatmul.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
    rnnMatmul.SetBType(
        matmul_tiling::TPosition::TSCM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
    rnnMatmul.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_FLOAT);
    rnnMatmul.SetBiasType(
        matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
    rnnMatmul1.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
    rnnMatmul1.SetBType(
        matmul_tiling::TPosition::TSCM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
    rnnMatmul1.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_FLOAT);
    rnnMatmul1.SetBiasType(
        matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
    rnnMatmul2.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
    rnnMatmul2.SetBType(
        matmul_tiling::TPosition::TSCM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
    rnnMatmul2.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_FLOAT);
    rnnMatmul2.SetBiasType(
        matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
    // full loaded
    auto ret = rnnMatmul.EnableBias(true);
    ret = rnnMatmul.SetDim(rnnParams.sysAivCoreNum / 4);
    int32_t input_align = MathUtil::CeilDivision(rnnParams.inputSize, 16) * 16;
    int32_t hidden_align = MathUtil::CeilDivision(rnnParams.hiddenSize, 16) * 16;
    std::vector<int32_t> factorList;
    MathUtil::GetNonFactorMap(factorList, 2, 4);
    // EXPECT_EQ(MathUtil::IsEqual(input_align, hidden_align), true);
    // EXPECT_EQ(MathUtil::IsEqual(input_align, hidden_align), true);
    ret = rnnMatmul.SetOrgShape(rnnParams.batch, rnnParams.hiddenSize * 4, input_align + hidden_align);
    ret = rnnMatmul.SetShape(rnnParams.batch, rnnParams.hiddenSize, rnnParams.inputSize + rnnParams.hiddenSize);
    ret = rnnMatmul.SetBufferSpace(-1, rnnParams.maxUbSize, rnnParams.maxUbSize);
    ret = rnnMatmul.SetAlignSplit(-1, 64, -1);
    ret = rnnMatmul.GetTiling(rnnParams.matmulTiling);
    if (ret == 0) { // 0 success full    1 not full
        isFullLoadWeight = true;
    }
    if (isFullLoadWeight) {
        std::cout << "full load two weight" << std::endl;
        int32_t dim, mDim, nDim;
        rnnParams.baseM = rnnMatmul.GetBaseM();
        rnnParams.baseN = rnnMatmul.GetBaseN();
        rnnParams.baseK = rnnMatmul.GetBaseK(); // get output info after cut
        ret = rnnMatmul.GetSingleShape(
            rnnParams.singleM, rnnParams.singleN, rnnParams.singleK); // get single process info
        ret = rnnMatmul.GetCoreNum(
            dim, mDim,
            nDim); // get used numBlocks after multi-cores cut, carried by user to kernel， contrl Kernel
        // input mm
        int32_t l1_left = 512 * 1024 - 64 - rnnParams.singleN * (input_align + hidden_align) * sizeof(float) * 2;
        ret = rnnMatmul1.SetBufferSpace(l1_left, rnnParams.maxUbSize, rnnParams.maxUbSize);
        ret = rnnMatmul1.SetOrgShape(rnnParams.batch, rnnParams.hiddenSize * 4, rnnParams.inputSize);
        ret = rnnMatmul1.SetShape(rnnParams.batch, rnnParams.hiddenSize, rnnParams.inputSize);
        ret = rnnMatmul1.SetSingleShape(rnnParams.singleM, rnnParams.singleN, rnnParams.inputSize);
        ret = rnnMatmul1.SetFixSplit(rnnParams.baseM, rnnParams.baseN, -1);
        ret = rnnMatmul1.EnableBias(true);
        ret = rnnMatmul1.SetDim(dim * 4);
        ret = rnnMatmul1.SetAlignSplit(-1, 64, -1);
        ret = rnnMatmul1.GetTiling(tilingData.inputMMParam);
        tilingData.inputMMParam.set_singleCoreN(rnnParams.singleN / 4);
        int32_t l1UsedSize = (tilingData.inputMMParam.get_baseM() * tilingData.inputMMParam.get_baseK() *
                                  tilingData.inputMMParam.get_depthA1() +
                              rnnParams.singleN * (hidden_align + input_align) * 2) *
                             sizeof(float);
        EXPECT_LT(l1UsedSize, 512 * 1024 - 64);
        // hidden mm
        l1_left = 512 * 1024 - 64 - rnnParams.singleN * (input_align + hidden_align) * sizeof(float) * 2;
        ret = rnnMatmul2.SetBufferSpace(l1_left, rnnParams.maxUbSize, rnnParams.maxUbSize);
        ret = rnnMatmul2.SetOrgShape(rnnParams.batch, rnnParams.hiddenSize * 4, rnnParams.hiddenSize);
        ret = rnnMatmul2.SetShape(rnnParams.batch, rnnParams.hiddenSize, rnnParams.hiddenSize);
        ret = rnnMatmul2.SetSingleShape(rnnParams.singleM, rnnParams.singleN, rnnParams.hiddenSize);
        ret = rnnMatmul2.SetFixSplit(rnnParams.baseM, rnnParams.baseN, -1);
        ret = rnnMatmul2.EnableBias(false);
        ret = rnnMatmul2.SetAlignSplit(-1, 64, -1);
        ret = rnnMatmul2.SetDim(dim * 4);

        ret = rnnMatmul2.GetTiling(tilingData.hiddenMMParam);
        tilingData.hiddenMMParam.set_singleCoreN(rnnParams.singleN / 4);
        l1UsedSize = (tilingData.hiddenMMParam.get_baseM() * tilingData.hiddenMMParam.get_baseK() *
                          tilingData.hiddenMMParam.get_depthA1() +
                      rnnParams.singleN * (hidden_align + input_align) * 2) *
                     sizeof(float);
        EXPECT_LT(l1UsedSize, 512 * 1024 - 64);
        rnnParams.usedCoreNum = dim * 4;
    } else { // part of full loaded
        // two matmul time sharing
        auto ret = rnnMatmul.EnableBias(true);
        ret = rnnMatmul.SetDim(rnnParams.sysAivCoreNum / 4);
        ret = rnnMatmul.SetOrgShape(rnnParams.batch, rnnParams.hiddenSize * 4, input_align + hidden_align);
        ret = rnnMatmul.SetShape(rnnParams.batch, rnnParams.hiddenSize, max(rnnParams.inputSize, rnnParams.hiddenSize));
        ret = rnnMatmul.SetBufferSpace(-1, rnnParams.maxUbSize, rnnParams.maxUbSize);
        ret = rnnMatmul.SetAlignSplit(-1, 64, -1);
        ret = rnnMatmul.GetTiling(rnnParams.matmulTiling);
        if (ret == 0) { // 0 success full  1 not full
            isFullLoadWeightOne = true;
        }
        if (isFullLoadWeightOne) {
            std::cout << "only load one weight" << std::endl;
            int32_t dim, mDim, nDim;
            rnnParams.baseM = rnnMatmul.GetBaseM();
            rnnParams.baseN = rnnMatmul.GetBaseN();
            rnnParams.baseK = rnnMatmul.GetBaseK(); // get output info after cut
            ret = rnnMatmul.GetSingleShape(
                rnnParams.singleM, rnnParams.singleN,
                rnnParams.singleK); // get single process info
            ret = rnnMatmul.GetCoreNum(
                dim, mDim,
                nDim); // get used numBlocks after multi-cores cut, carried by user to kernel， contrl Kernel business
            // input mm
            ret = rnnMatmul1.SetBufferSpace(-1, rnnParams.maxUbSize, rnnParams.maxUbSize);
            ret = rnnMatmul1.SetOrgShape(rnnParams.batch, rnnParams.hiddenSize * 4, rnnParams.inputSize);
            ret = rnnMatmul1.SetShape(rnnParams.batch, rnnParams.hiddenSize, rnnParams.inputSize);
            ret = rnnMatmul1.SetSingleShape(rnnParams.singleM, rnnParams.singleN, rnnParams.inputSize);
            ret = rnnMatmul1.SetFixSplit(rnnParams.baseM, rnnParams.baseN, -1);
            ret = rnnMatmul1.EnableBias(true);
            ret = rnnMatmul1.SetDim(dim);
            ret = rnnMatmul1.SetAlignSplit(-1, 64, -1);
            ret = rnnMatmul1.GetTiling(tilingData.inputMMParam);
            tilingData.inputMMParam.set_singleCoreN(rnnParams.singleN / 4);
            int32_t l1UsedSize = (tilingData.inputMMParam.get_baseM() * tilingData.inputMMParam.get_baseK() *
                                      tilingData.inputMMParam.get_depthA1() +
                                  rnnParams.singleN * (input_align) * 2) *
                                 sizeof(float);
            EXPECT_LT(l1UsedSize, 512 * 1024 - 64);
            // hidden mm
            ret = rnnMatmul2.SetBufferSpace(-1, rnnParams.maxUbSize, rnnParams.maxUbSize);
            ret = rnnMatmul2.SetOrgShape(rnnParams.batch, rnnParams.hiddenSize * 4, rnnParams.hiddenSize);
            ret = rnnMatmul2.SetShape(rnnParams.batch, rnnParams.hiddenSize, rnnParams.hiddenSize);
            ret = rnnMatmul2.SetSingleShape(rnnParams.singleM, rnnParams.singleN, rnnParams.hiddenSize);
            ret = rnnMatmul2.SetFixSplit(rnnParams.baseM, rnnParams.baseN, -1);
            ret = rnnMatmul2.EnableBias(false);
            ret = rnnMatmul2.SetAlignSplit(-1, 64, -1);
            ret = rnnMatmul2.SetDim(dim);
            ret = rnnMatmul2.GetTiling(tilingData.hiddenMMParam);
            tilingData.hiddenMMParam.set_singleCoreN(rnnParams.singleN / 4);
            l1UsedSize = (tilingData.hiddenMMParam.get_baseM() * tilingData.hiddenMMParam.get_baseK() *
                              tilingData.hiddenMMParam.get_depthA1() +
                          rnnParams.singleN * (hidden_align) * 2) *
                         sizeof(float);
            EXPECT_LT(l1UsedSize, 512 * 1024 - 64);
            rnnParams.usedCoreNum = dim * 4;
        } else { // no cache, reset AB，mm cache mechanism lose efficacy
            std::cout << "can not load any weight" << std::endl;
            rnnMatmul.SetAType(
                matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
            rnnMatmul.SetBType(
                matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
            rnnMatmul.SetCType(
                matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_FLOAT);
            rnnMatmul.SetBiasType(
                matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);

            rnnMatmul1.SetAType(
                matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
            rnnMatmul1.SetBType(
                matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
            rnnMatmul1.SetCType(
                matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_FLOAT);
            rnnMatmul1.SetBiasType(
                matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);

            rnnMatmul2.SetAType(
                matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
            rnnMatmul2.SetBType(
                matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
            rnnMatmul2.SetCType(
                matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_FLOAT);
            rnnMatmul2.SetBiasType(
                matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::ND, (matmul_tiling::DataType)dataType);
            auto ret = rnnMatmul.EnableBias(true);
            ret = rnnMatmul.SetDim(rnnParams.sysAivCoreNum);
            ret = rnnMatmul.SetOrgShape(
                rnnParams.batch, rnnParams.hiddenSize * 4, rnnParams.inputSize + rnnParams.hiddenSize);
            ret = rnnMatmul.SetShape(rnnParams.batch, rnnParams.hiddenSize, rnnParams.inputSize + rnnParams.hiddenSize);
            ret = rnnMatmul.SetBufferSpace(
                -1, rnnParams.maxUbSize,
                rnnParams.maxUbSize); // set the space that can be used, by default, all space of the chip is used.
            ret = rnnMatmul.GetTiling(rnnParams.matmulTiling);
            int32_t dim, mDim, nDim;
            rnnParams.baseM = rnnMatmul.GetBaseM();
            rnnParams.baseN = rnnMatmul.GetBaseN();
            rnnParams.baseK = rnnMatmul.GetBaseK(); // get output info
            ret = rnnMatmul.GetSingleShape(
                rnnParams.singleM, rnnParams.singleN,
                rnnParams.singleK); // get single core data
            ret = rnnMatmul.GetCoreNum(
                dim, mDim,
                nDim); // get used numBlocks after multi-cores cut, carried by user to kernel， contrl Kernel business
            // input mm
            ret = rnnMatmul1.SetBufferSpace(-1, rnnParams.maxUbSize, rnnParams.maxUbSize);
            ret = rnnMatmul1.SetOrgShape(rnnParams.batch, rnnParams.hiddenSize * 4, rnnParams.inputSize);
            ret = rnnMatmul1.SetShape(rnnParams.batch, rnnParams.hiddenSize, rnnParams.inputSize);
            ret = rnnMatmul1.SetSingleShape(rnnParams.singleM, rnnParams.singleN, rnnParams.inputSize);
            ret = rnnMatmul1.SetFixSplit(rnnParams.baseM, rnnParams.baseN, -1);
            ret = rnnMatmul1.EnableBias(true);
            ret = rnnMatmul1.SetDim(dim);
            ret = rnnMatmul1.GetTiling(tilingData.inputMMParam);
            int32_t l1UsedSize = (tilingData.inputMMParam.get_baseM() * tilingData.inputMMParam.get_baseK() *
                                      tilingData.inputMMParam.get_depthA1() +
                                  tilingData.inputMMParam.get_baseN() * tilingData.inputMMParam.get_baseK() *
                                      tilingData.inputMMParam.get_depthB1()) *
                                 sizeof(float);
            EXPECT_LT(l1UsedSize, 512 * 1024 - 64);
            // hidden mm
            ret = rnnMatmul2.SetBufferSpace(-1, rnnParams.maxUbSize, rnnParams.maxUbSize);
            ret = rnnMatmul2.SetOrgShape(rnnParams.batch, rnnParams.hiddenSize * 4, rnnParams.hiddenSize);
            ret = rnnMatmul2.SetShape(rnnParams.batch, rnnParams.hiddenSize, rnnParams.hiddenSize);
            ret = rnnMatmul2.SetSingleShape(rnnParams.singleM, rnnParams.singleN, rnnParams.hiddenSize);
            ret = rnnMatmul2.SetFixSplit(rnnParams.baseM, rnnParams.baseN, -1);
            ret = rnnMatmul2.EnableBias(false);
            ret = rnnMatmul2.SetDim(dim);
            ret = rnnMatmul2.GetTiling(tilingData.hiddenMMParam);
            l1UsedSize = (tilingData.hiddenMMParam.get_baseM() * tilingData.hiddenMMParam.get_baseK() *
                              tilingData.hiddenMMParam.get_depthA1() +
                          tilingData.hiddenMMParam.get_baseN() * tilingData.hiddenMMParam.get_baseK() *
                              tilingData.hiddenMMParam.get_depthB1()) *
                         sizeof(float);
            EXPECT_LT(l1UsedSize, 512 * 1024 - 64);
            rnnParams.usedCoreNum = dim;
        }
    }
}

TEST_F(TestTiling, TestMatmulApiTilngSetShapeZero)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(64, 2048, 0);
    tiling.SetOrgShape(64, 2048, 0);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);

    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);
    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, TestLgammaTilingFp32)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxSize;
    uint32_t minSize;
    GetLgammaMaxMinTmpSize(shape, 4, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 458752);
    EXPECT_EQ(minSize, 1792);

    GetLgammaMaxMinTmpSize(shape, 4, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 524288);
    EXPECT_EQ(minSize, 2048);

    shapeDims = {8};
    shape = ge::Shape(shapeDims);
    GetLgammaMaxMinTmpSize(shape, 4, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 2048);
    EXPECT_EQ(minSize, 2048);

    GetLgammaMaxMinTmpSize(shape, 4, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 1792);
    EXPECT_EQ(minSize, 1792);

    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetLgammaTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 8);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestLgammaTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxSize;
    uint32_t minSize;

    GetLgammaMaxMinTmpSize(shape, 2, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 128 * 128 * 2 * 13 * 2);
    EXPECT_EQ(minSize, 13 * 2 * 256);

    shapeDims = {8};
    shape = ge::Shape(shapeDims);
    GetLgammaMaxMinTmpSize(shape, 2, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 256 * 13 * 2);
    EXPECT_EQ(minSize, 256 * 13 * 2);

    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetLgammaTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 13);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case1)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);

    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 99328);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case2)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(4096, 4096, 4096);
    tiling.SetOrgShape(4096, 4096, 4096);
    tiling.EnableBias(true);
    tiling.SetDequantType(DequantType::TENSOR);
    tiling.SetBufferSpace(-1, -1, -1);

    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 99328);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case3)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(32, 16, 32);
    tiling.SetOrgShape(32, 16, 32);
    tiling.EnableBias(true);
    tiling.SetDequantType(DequantType::TENSOR);
    tiling.SetBufferSpace(-1, -1, -1);

    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 2176);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case4)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(16, 64, 16);
    tiling.SetOrgShape(16, 64, 16);
    tiling.EnableBias(false);
    tiling.SetDequantType(DequantType::TENSOR);
    tiling.SetBufferSpace(-1, -1, -1);

    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 2560);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case5)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(32, 4096, 80);
    tiling.SetOrgShape(32, 4096, 80);
    tiling.EnableBias(true);
    tiling.SetDequantType(DequantType::TENSOR);
    tiling.SetBufferSpace(256 * 1024, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 197632);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case6)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::TSCM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(512, 32, 512);
    tiling.SetOrgShape(512, 32, 512);
    tiling.EnableBias(true);
    tiling.SetDequantType(DequantType::TENSOR);
    tiling.SetBufferSpace(512 * 1024 - 128 - 32 * 512 * 2, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 262272);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case7)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(176, 1680, 608);
    tiling.SetOrgShape(176, 1680, 608);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, 128 * 1024, -1);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 98304);
    EXPECT_GE(tilingData.get_transLength(), 0);
    EXPECT_EQ(tilingData.get_baseM(), 128);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case8)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(24);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetOrgShape(65536, 32768, 65536);
    tiling.SetShape(65536, 32768, 65536);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 98304);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestAscendQuantTiling)
{
    std::vector<int64_t> shapeDims = {512};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxValue;
    uint32_t minValue;
    GetAscendQuantMaxMinTmpSize(shape, 2, maxValue, minValue);
    EXPECT_EQ(minValue, 2 * 256);
    EXPECT_EQ(maxValue, 2 * 512);
    uint32_t maxLivedNodesCnt = 0;
    uint32_t extraBuf = 1;
    GetAscendQuantTmpBufferFactorSize(maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(maxLivedNodesCnt, 2);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestQuantizeTiling)
{
    std::vector<int64_t> shapeDims = {512};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxValue;
    uint32_t minValue;
    GetQuantizeMaxMinTmpSize(shape, 2, maxValue, minValue);
    EXPECT_EQ(minValue, 2 * 256);
    EXPECT_EQ(maxValue, 2 * 512);
    uint32_t maxLivedNodesCnt = 0;
    uint32_t extraBuf = 1;
    GetQuantizeTmpBufferFactorSize(maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(maxLivedNodesCnt, 2);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAscendDequantTiling)
{
    // 2d input shape
    std::vector<int64_t> shape_dims = {10, 32};
    auto shape = ge::Shape(shape_dims);
    uint32_t maxValue;
    uint32_t minValue;

    GetAscendDequantMaxMinTmpSize(shape, 2, maxValue, minValue);
    EXPECT_EQ(minValue, 4 * (64 + 32 + 40));
    EXPECT_EQ(maxValue, 4 * (64 + 32 * 10 + 40));

    uint32_t maxLivedNodesCnt = 0;
    uint32_t extraBuf = 1;
    GetAscendDequantTmpBufferFactorSize(shape, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(maxLivedNodesCnt, 1);
    EXPECT_EQ(extraBuf, 104);

    // 1d input shape
    std::vector<int64_t> shape_dims_1d = {320};
    auto shape_1d = ge::Shape(shape_dims_1d);

    GetAscendDequantMaxMinTmpSize(shape_1d, 2, maxValue, minValue);
    EXPECT_EQ(minValue, 4 * (64 + 1 * 320 + 328));
    EXPECT_EQ(maxValue, 4 * (64 + 1 * 320 + 328));

    GetAscendDequantTmpBufferFactorSize(shape_1d, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(maxLivedNodesCnt, 2);
    EXPECT_EQ(extraBuf, 72);

    std::vector<int64_t> shape3_dims = {10, 32, 3};
    auto shape_3d = ge::Shape(shape3_dims);
    GetAscendDequantTmpBufferFactorSize(shape_3d, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(maxLivedNodesCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAntiquantTilingNoTransposePerChannelHalf)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1, 5120};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = false;
    uint32_t maxValue;
    uint32_t minValue;
    GetAscendAntiQuantMaxMinTmpSize(
        srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_FLOAT16, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);
    uint32_t maxLiveNodeCnt = 1;
    uint32_t extraBuf = 1;
    GetAscendAntiQuantTmpBufferFactorSize(
        srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_FLOAT16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAntiquantTilingNoTransposePerChannel)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1, 5120};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = false;
    uint32_t maxValue;
    uint32_t minValue;
    GetAscendAntiQuantMaxMinTmpSize(srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxValue, minValue);
    uint32_t expectValue = 5120 * 2 * sizeof(float) + 64 * 640 * sizeof(float);
    EXPECT_EQ(minValue, expectValue);
    EXPECT_EQ(maxValue, expectValue);

    uint32_t maxLiveNodeCnt = 1;
    uint32_t extraBuf = 1;
    GetAscendAntiQuantTmpBufferFactorSize(
        srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, expectValue);
}

TEST_F(TestTiling, TestAntiquantTilingNoTransposePerTensor)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = false;
    uint32_t maxValue;
    uint32_t minValue;
    GetAscendAntiQuantMaxMinTmpSize(srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxValue, minValue);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 640 * 5120 * sizeof(float));

    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 1;
    GetAscendAntiQuantTmpBufferFactorSize(
        srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAntiquantTilingTransposePerChannel)
{
    std::vector<int64_t> srcDims = {64, 512};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {64, 1};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = true;
    uint32_t maxValue;
    uint32_t minValue;
    GetAscendAntiQuantMaxMinTmpSize(srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxValue, minValue);
    EXPECT_EQ(minValue, 80 * 64 * sizeof(float));
    EXPECT_EQ(maxValue, 80 * 64 * sizeof(float));

    uint32_t maxLiveNodeCnt = 1;
    uint32_t extraBuf = 0;
    GetAscendAntiQuantTmpBufferFactorSize(
        srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 80 * 64 * sizeof(float));
}

TEST_F(TestTiling, TestAntiquantTilingTransposePerTensor)
{
    std::vector<int64_t> srcDims = {640, 5120};
    auto srcShape = ge::Shape(srcDims);
    std::vector<int64_t> offsetDSms = {1};
    auto offsetShape = ge::Shape(offsetDSms);
    bool isTranspose = true;
    uint32_t maxValue;
    uint32_t minValue;
    GetAscendAntiQuantMaxMinTmpSize(srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxValue, minValue);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 640 * 5120 * sizeof(float));
    uint32_t maxLiveNodeCnt = 0;
    uint32_t extraBuf = 1;
    GetAscendAntiQuantTmpBufferFactorSize(
        srcShape, offsetShape, isTranspose, ge::DT_INT8, ge::DT_BF16, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestGeluTiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto geluShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetGeluMaxMinTmpSize(geluShape, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 3 * 128 * 128 * 4);
    EXPECT_EQ(minValue, 3 * 256);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case9)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(2);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetOrgShape(64, 64, 64);
    tiling.SetShape(64, 64, 64);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 6144);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestErfTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto erfShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetErfMaxMinTmpSize(erfShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 3 * 4);
    EXPECT_EQ(minValue, 256 * 3);
}

TEST_F(TestTiling, TestErfTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto erfShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetErfMaxMinTmpSize(erfShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 8);
    EXPECT_EQ(minValue, 256 * 8);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetErfTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 8);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestErfcTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto erfcShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetErfcMaxMinTmpSize(erfcShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 7 * 4);
    EXPECT_EQ(minValue, 256 * 7);
}

TEST_F(TestTiling, TestErfcTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto erfcShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetErfcMaxMinTmpSize(erfcShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 16);
    EXPECT_EQ(minValue, 256 * 16);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetErfcTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 16);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case10)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(2);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetOrgShape(65536, 64, 32);
    tiling.SetShape(65536, 64, 32);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 263168);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case11)
{
    MultiCoreMatmulTiling tiling;
    tiling.SetDim(2);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetOrgShape(2048, 2048, 256);
    tiling.SetShape(2048, 2048, 256);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 393216);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case12)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND310P,
        .l1Size = 1048576,
        .l0CSize = 262144,
        .ubSize = 262144,
        .l0ASize = 65536,
        .l0BSize = 65536};
    matmul_tiling::MultiCoreMatmulTiling rnnMatmul3(plat);
    rnnMatmul3.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT8, false);
    rnnMatmul3.SetBType(
        matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType::DT_INT8, true);
    rnnMatmul3.SetCType(
        matmul_tiling::TPosition::VECCALC, matmul_tiling::CubeFormat::NZ, matmul_tiling::DataType ::DT_FLOAT16);
    auto ret = rnnMatmul3.EnableBias(false);
    ret = rnnMatmul3.SetOrgShape(1, 494, 128);
    ret = rnnMatmul3.SetShape(1, 494, 128);
    ret = rnnMatmul3.SetBufferSpace(
        1046528, 262144); // set the space that can be used, by default, all space of the chip is used.
    ret = rnnMatmul3.SetFixSplit(16, 256, 16);
    ret = rnnMatmul3.SetDequantType(DequantType::TENSOR);
    rnnMatmul3.SetTraverse(matmul_tiling::MatrixTraverse::FIRSTN);
    optiling::TCubeTiling tilingData;
    ret = rnnMatmul3.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 67584);
    EXPECT_GE(tilingData.get_transLength(), 2048);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8Case13)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8, true);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8, true);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);

    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 164864);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilngInt8ND2NZCase13)
{
    MatmulApiTiling tiling;
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8, true);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT8, true);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);
    tiling.SetMatmulConfigParams(1, false, ScheduleType::INNER_PRODUCT, MatrixTraverse::NOSET, true);
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;

    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 163840);
    EXPECT_GE(tilingData.get_transLength(), 131072);
}

TEST_F(TestTiling, TestCoshTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto coshShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetCoshMaxMinTmpSize(coshShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 4);
    EXPECT_EQ(minValue, 256 * 2);
}

TEST_F(TestTiling, TestCoshTilingFloat512)
{
    std::vector<int64_t> shapeDims = {512};
    auto coshShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetCoshMaxMinTmpSize(coshShape, 4, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 512 * 4 * 2);
    EXPECT_EQ(minValue, 256 * 2);
}

TEST_F(TestTiling, TestCoshTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto coshShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetCoshMaxMinTmpSize(coshShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 6);
    EXPECT_EQ(minValue, 256 * 6);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetCoshTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 6);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSinTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto sinShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetSinMaxMinTmpSize(sinShape, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 2 * 256);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 4);
    GetSinMaxMinTmpSize(sinShape, 4, false, maxValue, minValue);
    EXPECT_EQ(minValue, 3 * 256);
    EXPECT_EQ(maxValue, 128 * 128 * 3 * 4);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetSinTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 3);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSinTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto sinShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetSinMaxMinTmpSize(sinShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 8 * 2);
    EXPECT_EQ(minValue, 8 * 256);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetSinTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 8);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAscendSumTiling)
{
    uint32_t n = 8;
    uint32_t maxValue;
    uint32_t minValue;
    GetSumMaxMinTmpSize(n, 2, true, maxValue, minValue);
    EXPECT_EQ(minValue, 32);
    EXPECT_EQ(maxValue, 32);

    maxValue = 0;
    minValue = 0;
    GetSumMaxMinTmpSize(n, 4, false, maxValue, minValue);
    EXPECT_EQ(minValue, 32);
    EXPECT_EQ(maxValue, 32);
}

TEST_F(TestTiling, TestAscendSiluTiling)
{
    std::vector<int64_t> shapeDims = {512};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxValue;
    uint32_t minValue;
    GetSiluTmpSize(shape, 8, true, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);
}

TEST_F(TestTiling, TestAscendSwishTiling)
{
    std::vector<int64_t> shapeDims = {512};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxValue;
    uint32_t minValue;
    GetSwishTmpSize(shape, 8, true, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);
}

TEST_F(TestTiling, TestAscendXorTiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto xorShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetXorMaxMinTmpSize(xorShape, 2, true, maxValue, minValue);
    EXPECT_EQ(maxValue, 128 * 128 * 1 * 2);
    EXPECT_EQ(minValue, 1 * 256);
    GetXorTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestFracTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto fracShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetFracMaxMinTmpSize(fracShape, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetFracTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestFracTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto fracShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetFracMaxMinTmpSize(fracShape, 2, true, maxValue, minValue);
    EXPECT_EQ(minValue, 1024);
    EXPECT_EQ(maxValue, 131072);
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetFracTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
TEST_F(TestTiling, TestTopkTiling_TopKModeNomal_isInitIndexTrue_Float_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = true;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 10;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 256);
    EXPECT_EQ(tilingData.get_allDataSize(), 64);
    EXPECT_EQ(tilingData.get_innerDataSize(), 128);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_mrgSortRepeat(), 16);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 16);
    EXPECT_EQ(tilingData.get_maskOffset(), 16);
    EXPECT_EQ(tilingData.get_maskVreducev2FourBytes(), 20);
    EXPECT_EQ(tilingData.get_maskVreducev2TwoBytes(), 40);
    EXPECT_EQ(tilingData.get_mrgSortSrc1offset(), 2);
    EXPECT_EQ(tilingData.get_mrgSortSrc2offset(), 4);
    EXPECT_EQ(tilingData.get_mrgSortSrc3offset(), 6);
    EXPECT_EQ(tilingData.get_mrgSortTwoQueueSrc1Offset(), 2);
    EXPECT_EQ(tilingData.get_mrgFourQueueTailPara1(), 128);
    EXPECT_EQ(tilingData.get_mrgFourQueueTailPara2(), 1);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 1024);
    EXPECT_EQ(minValue, 1024);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeNomal_isInitIndexFalse_Float_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 10;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 320);
    EXPECT_EQ(tilingData.get_allDataSize(), 64);
    EXPECT_EQ(tilingData.get_innerDataSize(), 128);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_mrgSortRepeat(), 16);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 16);
    EXPECT_EQ(tilingData.get_maskOffset(), 16);
    EXPECT_EQ(tilingData.get_maskVreducev2FourBytes(), 20);
    EXPECT_EQ(tilingData.get_maskVreducev2TwoBytes(), 40);
    EXPECT_EQ(tilingData.get_mrgSortSrc1offset(), 2);
    EXPECT_EQ(tilingData.get_mrgSortSrc2offset(), 4);
    EXPECT_EQ(tilingData.get_mrgSortSrc3offset(), 6);
    EXPECT_EQ(tilingData.get_mrgSortTwoQueueSrc1Offset(), 2);
    EXPECT_EQ(tilingData.get_mrgFourQueueTailPara1(), 128);
    EXPECT_EQ(tilingData.get_mrgFourQueueTailPara2(), 1);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 256);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, false, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 1280);
    EXPECT_EQ(minValue, 1280);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeNomal_isInitIndexTrue_Half_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = true;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 10;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 512);
    EXPECT_EQ(tilingData.get_allDataSize(), 64);
    EXPECT_EQ(tilingData.get_innerDataSize(), 256);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_mrgSortRepeat(), 16);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 16);
    EXPECT_EQ(tilingData.get_maskOffset(), 16);
    EXPECT_EQ(tilingData.get_maskVreducev2FourBytes(), 20);
    EXPECT_EQ(tilingData.get_maskVreducev2TwoBytes(), 40);
    EXPECT_EQ(tilingData.get_mrgSortSrc1offset(), 4);
    EXPECT_EQ(tilingData.get_mrgSortSrc2offset(), 8);
    EXPECT_EQ(tilingData.get_mrgSortSrc3offset(), 12);
    EXPECT_EQ(tilingData.get_mrgSortTwoQueueSrc1Offset(), 4);
    EXPECT_EQ(tilingData.get_mrgFourQueueTailPara1(), 128);
    EXPECT_EQ(tilingData.get_mrgFourQueueTailPara2(), 2);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 1024);
    EXPECT_EQ(minValue, 1024);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeNomal_isInitIndexFalse_Half_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 10;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 640);
    EXPECT_EQ(tilingData.get_allDataSize(), 64);
    EXPECT_EQ(tilingData.get_innerDataSize(), 256);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_mrgSortRepeat(), 16);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 16);
    EXPECT_EQ(tilingData.get_maskOffset(), 16);
    EXPECT_EQ(tilingData.get_maskVreducev2FourBytes(), 20);
    EXPECT_EQ(tilingData.get_maskVreducev2TwoBytes(), 40);
    EXPECT_EQ(tilingData.get_mrgSortSrc1offset(), 4);
    EXPECT_EQ(tilingData.get_mrgSortSrc2offset(), 8);
    EXPECT_EQ(tilingData.get_mrgSortSrc3offset(), 12);
    EXPECT_EQ(tilingData.get_mrgSortTwoQueueSrc1Offset(), 4);
    EXPECT_EQ(tilingData.get_mrgFourQueueTailPara1(), 128);
    EXPECT_EQ(tilingData.get_mrgFourQueueTailPara2(), 2);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 512);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, false, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 1280);
    EXPECT_EQ(minValue, 1280);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexTrue_Float_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = true;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, false, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 112);
    EXPECT_EQ(tilingData.get_topkMrgSrc1MaskSizeOffset(), 96);
    EXPECT_EQ(tilingData.get_maskOffset(), 10);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 320);
    EXPECT_EQ(minValue, 320);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexFalse_Float_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_topkMrgSrc1MaskSizeOffset(), 64);
    EXPECT_EQ(tilingData.get_maskOffset(), 10);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 112);
    EXPECT_EQ(tilingData.get_topkNSmallSrcIndexOffset(), 80);

    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, false, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 576);
    EXPECT_EQ(minValue, 576);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexTrue_Half_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = true;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_topkMrgSrc1MaskSizeOffset(), 128);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 160);
    EXPECT_EQ(tilingData.get_maskOffset(), 10);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, false, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 448);
    EXPECT_EQ(minValue, 448);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexFalse_Half_Inner64)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    ("Ascend910B");
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, false, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_topkMrgSrc1MaskSizeOffset(), 160);
    EXPECT_EQ(tilingData.get_maskOffset(), 10);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 256);
    EXPECT_EQ(tilingData.get_topkNSmallSrcIndexOffset(), 192);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 448);
    EXPECT_EQ(minValue, 448);
}

TEST_F(TestTiling, TestTopkTiling_DataTypeSize0_FAILED)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 10;
    uint32_t dataTypeSize = 0;
    bool isReuseSource = true;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    auto res = TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, false, tilingData);
    EXPECT_EQ(res, false);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexFalse_Half_k)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    int32_t k = 13;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 16);
    EXPECT_EQ(tilingData.get_topkMrgSrc1MaskSizeOffset(), 128);
    EXPECT_EQ(tilingData.get_maskOffset(), 13);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 224);
    EXPECT_EQ(tilingData.get_topkNSmallSrcIndexOffset(), 160);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 448);
    EXPECT_EQ(minValue, 448);
    k = 17;
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 24);
    k = 21;
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 24);
    k = 25;
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 32);
    k = 29;
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 32);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall_isInitIndexFalse_Float_k32)
{
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 32;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_allDataSize(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 32);
    EXPECT_EQ(tilingData.get_topkMrgSrc1MaskSizeOffset(), 64);
    EXPECT_EQ(tilingData.get_maskOffset(), 32);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 112);
    EXPECT_EQ(tilingData.get_topkNSmallSrcIndexOffset(), 80);
    GetTopKMaxMinTmpSize(plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, 4, maxValue, minValue);
    EXPECT_EQ(maxValue, 448);
    EXPECT_EQ(minValue, 448);
}

#endif

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
TEST_F(TestTiling, TestTopkTiling_TopKModeNormal310P_FLOAT)
{
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 64;
    const int32_t k = 32;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = true;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend310P");
    platfrom_stub_set_npuarch("2002");
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 1088);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 1024);
    EXPECT_EQ(tilingData.get_innerDataSize(), 512);
    EXPECT_EQ(tilingData.get_sortRepeat(), 4);
    EXPECT_EQ(tilingData.get_copyUbToUbBlockCount(), 64);
    EXPECT_EQ(tilingData.get_mrgSortSrc1offset(), 8);
    EXPECT_EQ(tilingData.get_mrgSortSrc2offset(), 16);
    EXPECT_EQ(tilingData.get_mrgSortSrc3offset(), 24);
    EXPECT_EQ(tilingData.get_mrgSortRepeat(), 16);
    EXPECT_EQ(tilingData.get_maskOffset(), 32);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 32);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 32);
    GetTopKMaxMinTmpSize(
        plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, dataTypeSize, maxValue, minValue);
    EXPECT_EQ(maxValue, 4352);
    EXPECT_EQ(minValue, 4352);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeNormal310P_HALF)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend310P");
    platfrom_stub_set_npuarch("2002");
    enum TopKMode topkMode = TopKMode::TOPK_NORMAL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 8;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = true;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 576);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 512);
    EXPECT_EQ(tilingData.get_innerDataSize(), 256);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_copyUbToUbBlockCount(), 16);
    EXPECT_EQ(tilingData.get_mrgSortSrc1offset(), 8);
    EXPECT_EQ(tilingData.get_mrgSortSrc2offset(), 16);
    EXPECT_EQ(tilingData.get_mrgSortSrc3offset(), 24);
    EXPECT_EQ(tilingData.get_mrgSortRepeat(), 8);
    EXPECT_EQ(tilingData.get_maskOffset(), 16);
    EXPECT_EQ(tilingData.get_kAlignTwoBytes(), 16);
    EXPECT_EQ(tilingData.get_kAlignFourBytes(), 8);
    GetTopKMaxMinTmpSize(
        plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, dataTypeSize, maxValue, minValue);
    EXPECT_EQ(maxValue, 1152);
    EXPECT_EQ(minValue, 1152);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall310P_FLOAT)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend310P");
    platfrom_stub_set_npuarch("2002");
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 5;
    uint32_t dataTypeSize = 4;
    bool isReuseSource = true;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 512);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 288);
    EXPECT_EQ(tilingData.get_innerDataSize(), 256);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_copyUbToUbBlockCount(), 32);
    EXPECT_EQ(tilingData.get_mrgSortSrc1offset(), 256);
    EXPECT_EQ(tilingData.get_vreduceIdxMask0(), 31);
    EXPECT_EQ(tilingData.get_vreduceValMask0(), 31);
    EXPECT_EQ(tilingData.get_vreduceValMask1(), 0);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 288);
    EXPECT_EQ(tilingData.get_maskOffset(), 5);
    GetTopKMaxMinTmpSize(
        plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, dataTypeSize, maxValue, minValue);
    EXPECT_EQ(maxValue, 2048);
    EXPECT_EQ(minValue, 2048);
}

TEST_F(TestTiling, TestTopkTiling_TopKModeSmall310P_HALF)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    platfrom_stub_set_chip_version("Ascend310P");
    platfrom_stub_set_npuarch("2002");
    enum TopKMode topkMode = TopKMode::TOPK_NSMALL;
    bool isInitIndex = false;
    const int32_t outter = 1;
    const int32_t inner = 32;
    const int32_t k = 32;
    uint32_t dataTypeSize = 2;
    bool isReuseSource = false;
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    optiling::TopkTiling tilingData;
    TopKTilingFunc(plat, inner, outter, k, dataTypeSize, isInitIndex, topkMode, true, tilingData);
    EXPECT_EQ(tilingData.get_tmpLocalSize(), 512);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 320);
    EXPECT_EQ(tilingData.get_innerDataSize(), 256);
    EXPECT_EQ(tilingData.get_sortRepeat(), 2);
    EXPECT_EQ(tilingData.get_copyUbToUbBlockCount(), 16);
    EXPECT_EQ(tilingData.get_mrgSortSrc1offset(), 256);
    EXPECT_EQ(tilingData.get_vreduceIdxMask0(), 4294967295);
    EXPECT_EQ(tilingData.get_vreduceValMask0(), 65535);
    EXPECT_EQ(tilingData.get_vreduceValMask1(), 65535);
    EXPECT_EQ(tilingData.get_srcIndexOffset(), 320);
    EXPECT_EQ(tilingData.get_maskOffset(), 32);
    GetTopKMaxMinTmpSize(
        plat, inner, outter, isReuseSource, isInitIndex, topkMode, true, dataTypeSize, maxValue, minValue);
    EXPECT_EQ(maxValue, 1024);
    EXPECT_EQ(minValue, 1024);
}
#endif

TEST_F(TestTiling, TestArithProgression)
{
    uint32_t maxValue;
    uint32_t minValue;
    GetArithProgressionMaxMinTmpSize(maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);
}

TEST_F(TestTiling, TestArange)
{
    uint32_t maxValue;
    uint32_t minValue;
    GetArangeMaxMinTmpSize(maxValue, minValue);
    EXPECT_EQ(maxValue, 0);
    EXPECT_EQ(minValue, 0);
}

TEST_F(TestTiling, TestGeGLUTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto GeGLUShape = ge::Shape(shapeDims);
    uint32_t GeGLUNeedMaxSize;
    uint32_t GeGLUNeedMinSize;
    GetGeGLUMaxMinTmpSize(GeGLUShape, 4, true, GeGLUNeedMaxSize, GeGLUNeedMinSize);
    EXPECT_EQ(GeGLUNeedMaxSize, 0);
    EXPECT_EQ(GeGLUNeedMinSize, 0);

    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetGeGLUTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestGeGLUTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto GeGLUShape = ge::Shape(shapeDims);
    uint32_t GeGLUNeedMaxSize;
    uint32_t GeGLUNeedMinSize;
    GetGeGLUMaxMinTmpSize(GeGLUShape, 2, true, GeGLUNeedMaxSize, GeGLUNeedMinSize);
    EXPECT_EQ(GeGLUNeedMaxSize, 131072);
    EXPECT_EQ(GeGLUNeedMinSize, 1024);
    GetGeGLUMaxMinTmpSize(GeGLUShape, 2, true, GeGLUNeedMaxSize, GeGLUNeedMinSize);
    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetGeGLUTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)
TEST_F(TestTiling, TestDigammaTilingFp32)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxSize;
    uint32_t minSize;
    GetDigammaMaxMinTmpSize(shape, 4, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 393216);
    EXPECT_EQ(minSize, 1536);

    GetDigammaMaxMinTmpSize(shape, 4, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 458752);
    EXPECT_EQ(minSize, 1792);

    shapeDims = {8};
    shape = ge::Shape(shapeDims);
    GetDigammaMaxMinTmpSize(shape, 4, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 1792);
    EXPECT_EQ(minSize, 1792);

    GetDigammaMaxMinTmpSize(shape, 4, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 1536);
    EXPECT_EQ(minSize, 1536);

    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetDigammaTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 7);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestDigammaTilingHalf)
{
    fe::PlatFormInfos platformInfo;
    auto plat = platform_ascendc::PlatformAscendC(&platformInfo);
    std::vector<int64_t> shapeDims = {128, 128};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxSize;
    uint32_t minSize;

    GetDigammaMaxMinTmpSize(shape, 2, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 128 * 128 * 2 * 8 * 2);
    EXPECT_EQ(minSize, 8 * 2 * 256);

    shapeDims = {8};
    shape = ge::Shape(shapeDims);
    GetDigammaMaxMinTmpSize(shape, 2, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 256 * 8 * 2);
    EXPECT_EQ(minSize, 256 * 8 * 2);

    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetDigammaTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 16);
    EXPECT_EQ(extraBuf, 0);
}
#endif

TEST_F(TestTiling, TestAtanhTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto aTanhShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetAtanhMaxMinTmpSize(aTanhShape, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 1);
    EXPECT_EQ(maxValue, 128 * 128 * 4 * 1);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    AscendC::GetAtanhTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAtanhTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto aTanhShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    AscendC::GetAtanhMaxMinTmpSize(aTanhShape, 2, true, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 4);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 4);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    AscendC::GetAtanhTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestSignTiling)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto signShape = ge::Shape(shapeDims);
    uint32_t signNeedMaxSize;
    uint32_t signNeedMinSize;
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetSignMaxMinTmpSize(signShape, 2, false, signNeedMaxSize, signNeedMinSize);
    EXPECT_EQ(signNeedMaxSize, 3 * 128 * 128 * 2);
    EXPECT_EQ(signNeedMinSize, 3 * 256);

    GetSignMaxMinTmpSize(signShape, 4, false, signNeedMaxSize, signNeedMinSize);
    EXPECT_EQ(signNeedMaxSize, 3 * 128 * 128 * 4);
    EXPECT_EQ(signNeedMinSize, 3 * 256);

    GetSignTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 3);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAscendMeanTiling)
{
    uint32_t n = 8;
    uint32_t maxValue;
    uint32_t minValue;
    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;

    GetMeanMaxMinTmpSize(n, 2, 2, true, maxValue, minValue);
    EXPECT_EQ(minValue, 32);
    EXPECT_EQ(maxValue, 32);

    maxValue = 0;
    minValue = 0;
    GetMeanMaxMinTmpSize(n, 4, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 32);
    EXPECT_EQ(maxValue, 32);

    GetMeanMaxMinTmpSize(n, 2, 4, true, maxValue, minValue);
    EXPECT_EQ(minValue, 96);
    EXPECT_EQ(maxValue, 96);

    GetMeanTmpBufferFactorSize(4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestAxpyTiling)
{
    uint32_t maxVal = 0;
    uint32_t minVal = 0;
    GetAxpyMaxMinTmpSize(ge::Shape({128}), 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 0);
    EXPECT_EQ(minVal, 0);
    GetAxpyMaxMinTmpSize(ge::Shape({256}), 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 4 * 2);
    EXPECT_EQ(minVal, 256 * 4);
    GetAxpyMaxMinTmpSize(ge::Shape({32}), 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 4);
    EXPECT_EQ(minVal, 256 * 4);
    uint32_t extraBuf = 123;
    uint32_t maxLivedNodesCnt = 123;
    GetAxpyTmpBufferFactorSize(4, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(extraBuf, 0);
    EXPECT_EQ(maxLivedNodesCnt, 1);
    GetAxpyTmpBufferFactorSize(2, maxLivedNodesCnt, extraBuf);
    EXPECT_EQ(extraBuf, 0);
    EXPECT_EQ(maxLivedNodesCnt, 4);
}

TEST_F(TestTiling, TestCeilTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto ceilShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetCeilMaxMinTmpSize(ceilShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 1);
    EXPECT_EQ(maxValue, 128 * 128 * 1 * 4);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetCeilTmpBufferFactorSize(sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 1);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestCeilTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto ceilShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetCeilMaxMinTmpSize(ceilShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 2);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 2);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetCeilTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 2);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestCeilTilingHalf512)
{
    std::vector<int64_t> shapeDims = {512};
    auto ceilShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetCeilMaxMinTmpSize(ceilShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 512 * 2 * 2);
    EXPECT_EQ(minValue, 256 * 2);
}

TEST_F(TestTiling, TestFloorTilingFloat)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto floorShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetFloorMaxMinTmpSize(floorShape, sizeof(float), false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetFloorTmpBufferFactorSize(sizeof(float), maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 0);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestFloorTilingHalf)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto floorShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetFloorMaxMinTmpSize(floorShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 2);
    EXPECT_EQ(maxValue, 128 * 128 * 2 * 2);

    uint32_t maxLiveNodeCnt;
    uint32_t extraBuf;
    GetFloorTmpBufferFactorSize(2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 2);
    EXPECT_EQ(extraBuf, 0);
}

TEST_F(TestTiling, TestFloorTilingHalf512)
{
    std::vector<int64_t> shapeDims = {512};
    auto floorShape = ge::Shape(shapeDims);
    uint32_t maxValue = 0;
    uint32_t minValue = 0;
    GetFloorMaxMinTmpSize(floorShape, 2, false, maxValue, minValue);
    EXPECT_EQ(maxValue, 512 * 2 * 2);
    EXPECT_EQ(minValue, 256 * 2);
}

TEST_F(TestTiling, TestReGluFloat16OrBf16)
{
    const std::vector<int64_t> srcShapeDims = {8, 128};
    const auto srcShape = ge::Shape(srcShapeDims);
    uint32_t maxValue;
    uint32_t minValue;
    GetReGluMaxMinTmpSize(srcShape, 2, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256 * 6);
    EXPECT_EQ(maxValue, 8 * 128 * 2 * 6);
}

TEST_F(TestTiling, TestReGluFloat32)
{
    const std::vector<int64_t> srcShapeDims = {8, 128};
    const auto srcShape = ge::Shape(srcShapeDims);
    uint32_t maxValue;
    uint32_t minValue;
    GetReGluMaxMinTmpSize(srcShape, 4, false, maxValue, minValue);
    EXPECT_EQ(minValue, 256);
    EXPECT_EQ(maxValue, 256);
}

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201

TEST_F(TestTiling, TestBroadCast220)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    uint32_t firstDim = 32;
    uint32_t lastDim = 32;
    std::vector<int64_t> srcShapeDims = {firstDim, 1};
    auto srcShape = ge::Shape(srcShapeDims);
    std::vector<int64_t> dstShapeDims = {firstDim, lastDim};
    auto dstShape = ge::Shape(dstShapeDims);
    uint32_t maxValue{0};
    uint32_t minValue{0};
    constexpr uint32_t halfSize = 2;
    constexpr uint32_t halfOneBlockElementNum = 16;
    constexpr uint32_t minHalfAlignSize = halfOneBlockElementNum * halfOneBlockElementNum * halfSize;
    constexpr uint32_t BRCB_ONE_SIZE = 8;
    uint32_t firstDimAlignNum = (firstDim + BRCB_ONE_SIZE - 1) / BRCB_ONE_SIZE * BRCB_ONE_SIZE;
    uint32_t maxHalfAlignSize = firstDimAlignNum * halfOneBlockElementNum * halfSize;
    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, halfSize, true, maxValue, minValue);
    EXPECT_EQ(minValue, minHalfAlignSize);
    EXPECT_EQ(maxValue, maxHalfAlignSize);

    srcShapeDims = {firstDim, 1};
    srcShape = ge::Shape(srcShapeDims);
    uint32_t lastDimNotAlign = 31;
    dstShapeDims = {firstDim, lastDimNotAlign};
    dstShape = ge::Shape(dstShapeDims);

    uint32_t dstRepeatSize = (lastDimNotAlign + halfOneBlockElementNum - 1) / halfOneBlockElementNum;
    uint32_t numBlocksAlign = dstRepeatSize * halfOneBlockElementNum;
    uint32_t minCopyTempBufferSize = halfOneBlockElementNum * numBlocksAlign * halfSize;
    auto minHalfNotAlignSize = minHalfAlignSize + minCopyTempBufferSize;

    uint32_t maxCopyTempBufferSize = firstDim * numBlocksAlign * halfSize;
    uint32_t maxHalfNotAlignValue = maxHalfAlignSize + maxCopyTempBufferSize;

    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, halfSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, minHalfNotAlignSize);
    EXPECT_EQ(maxValue, maxHalfNotAlignValue);

    constexpr uint32_t int8Size = 1;
    srcShapeDims = {firstDim, 1};
    srcShape = ge::Shape(srcShapeDims);
    dstShapeDims = {firstDim, lastDim};
    dstShape = ge::Shape(dstShapeDims);
    const uint32_t alignSrcSize =
        ((firstDim + halfOneBlockElementNum - 1) / halfOneBlockElementNum) * halfOneBlockElementNum;
    uint32_t alignDstSize =
        ((firstDim * lastDim + halfOneBlockElementNum - 1) / halfOneBlockElementNum) * halfOneBlockElementNum;
    uint32_t castTempBufferSize = (alignSrcSize + alignDstSize) * halfSize;
    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, int8Size, false, maxValue, minValue);
    EXPECT_EQ(minValue, minHalfAlignSize + castTempBufferSize);
    EXPECT_EQ(maxValue, maxHalfAlignSize + castTempBufferSize);

    srcShapeDims = {firstDim, 1};
    srcShape = ge::Shape(srcShapeDims);
    dstShapeDims = {firstDim, lastDimNotAlign};
    dstShape = ge::Shape(dstShapeDims);
    alignDstSize =
        ((firstDim * lastDimNotAlign + halfOneBlockElementNum - 1) / halfOneBlockElementNum) * halfOneBlockElementNum;
    castTempBufferSize = (alignSrcSize + alignDstSize) * halfSize;
    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, int8Size, false, maxValue, minValue);
    EXPECT_EQ(minValue, minHalfNotAlignSize + castTempBufferSize);
    EXPECT_EQ(maxValue, maxHalfNotAlignValue + castTempBufferSize);
}

TEST_F(TestTiling, TestPowerTiling)
{
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    auto platformPtr = platform_ascendc::PlatformAscendCManager::GetInstance("Ascend910B");
    std::vector<int64_t> shapeDims = {512};
    auto powerShape = ge::Shape(shapeDims);
    uint32_t maxVal;
    uint32_t minVal;
    GetPowerMaxMinTmpSize(powerShape, powerShape, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 4 + 256);
    EXPECT_EQ(minVal, 256 * 4 + 256);
    GetPowerMaxMinTmpSize(powerShape, powerShape, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 6);
    EXPECT_EQ(minVal, 256 * 6);
    GetPowerMaxMinTmpSize(powerShape, powerShape, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 2 * 14 + 256);
    EXPECT_EQ(minVal, 256 * 7 + 256);
    std::vector<int64_t> scalar_shape = {1};
    auto scalarShape = ge::Shape(scalar_shape);
    GetPowerMaxMinTmpSize(powerShape, scalarShape, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 2 * 14 + 256);
    EXPECT_EQ(minVal, 256 * 7 + 256);
    GetPowerMaxMinTmpSize(powerShape, scalarShape, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 7);
    EXPECT_EQ(minVal, 256 * 7);
    GetPowerMaxMinTmpSize(powerShape, scalarShape, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 5 + 256);
    EXPECT_EQ(minVal, 256 * 5 + 256);

    std::vector<int64_t> shape1 = {16};
    auto powerShape1 = ge::Shape(shape1);
    GetPowerMaxMinTmpSize(powerShape1, scalarShape, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 5 + 256);
    EXPECT_EQ(minVal, 256 * 5 + 256);
    GetPowerMaxMinTmpSize(powerShape1, scalarShape, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 7 + 256);
    EXPECT_EQ(minVal, 256 * 7 + 256);
    GetPowerMaxMinTmpSize(powerShape1, scalarShape, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 7);
    EXPECT_EQ(minVal, 256 * 7);
    GetPowerMaxMinTmpSize(powerShape1, powerShape1, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 4 + 256);
    EXPECT_EQ(minVal, 256 * 4 + 256);
    GetPowerMaxMinTmpSize(powerShape1, powerShape1, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 7 + 256);
    EXPECT_EQ(minVal, 256 * 7 + 256);
    GetPowerMaxMinTmpSize(powerShape1, powerShape1, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 6);
    EXPECT_EQ(minVal, 256 * 6);
}

TEST_F(TestTiling, TestPowerTilingFactorSize)
{
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    auto platformPtr = platform_ascendc::PlatformAscendCManager::GetInstance("Ascend910B");
    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetPowerTmpBufferFactorSize(false, true, false, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 5);
    EXPECT_EQ(extraBuf, 256);
    GetPowerTmpBufferFactorSize(false, true, true, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 7);
    EXPECT_EQ(extraBuf, 0);
    GetPowerTmpBufferFactorSize(false, true, false, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 14);
    EXPECT_EQ(extraBuf, 256);
    GetPowerTmpBufferFactorSize(true, true, false, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 4);
    EXPECT_EQ(extraBuf, 256);
    GetPowerTmpBufferFactorSize(true, true, true, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 6);
    EXPECT_EQ(extraBuf, 0);
    GetPowerTmpBufferFactorSize(true, true, false, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 14);
    EXPECT_EQ(extraBuf, 256);
}
#endif

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002

TEST_F(TestTiling, TestPowerTilingV200)
{
    platfrom_stub_set_chip_version("Ascend310P");
    platfrom_stub_set_npuarch("2002");
    auto platformPtr = platform_ascendc::PlatformAscendCManager::GetInstance("Ascend310P");
    std::vector<int64_t> shapeDims = {512};
    auto powerShape = ge::Shape(shapeDims);
    uint32_t maxVal;
    uint32_t minVal;
    GetPowerMaxMinTmpSize(powerShape, powerShape, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 5 + 256);
    EXPECT_EQ(minVal, 256 * 5 + 256);
    GetPowerMaxMinTmpSize(powerShape, powerShape, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 7);
    EXPECT_EQ(minVal, 256 * 7);
    GetPowerMaxMinTmpSize(powerShape, powerShape, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 2 * 16 + 256);
    EXPECT_EQ(minVal, 256 * 8 + 256);
    std::vector<int64_t> scalar_shape = {1};
    auto scalarShape = ge::Shape(scalar_shape);
    GetPowerMaxMinTmpSize(powerShape, scalarShape, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 2 * 16 + 256);
    EXPECT_EQ(minVal, 256 * 8 + 256);
    GetPowerMaxMinTmpSize(powerShape, scalarShape, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 8);
    EXPECT_EQ(minVal, 256 * 8);
    GetPowerMaxMinTmpSize(powerShape, scalarShape, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 512 * 4 * 6 + 256);
    EXPECT_EQ(minVal, 256 * 6 + 256);

    std::vector<int64_t> shape1 = {16};
    auto powerShape1 = ge::Shape(shape1);
    GetPowerMaxMinTmpSize(powerShape1, scalarShape, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 6 + 256);
    EXPECT_EQ(minVal, 256 * 6 + 256);
    GetPowerMaxMinTmpSize(powerShape1, scalarShape, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 8 + 256);
    EXPECT_EQ(minVal, 256 * 8 + 256);
    GetPowerMaxMinTmpSize(powerShape1, scalarShape, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 8);
    EXPECT_EQ(minVal, 256 * 8);
    GetPowerMaxMinTmpSize(powerShape1, powerShape1, false, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 5 + 256);
    EXPECT_EQ(minVal, 256 * 5 + 256);
    GetPowerMaxMinTmpSize(powerShape1, powerShape1, false, 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 8 + 256);
    EXPECT_EQ(minVal, 256 * 8 + 256);
    GetPowerMaxMinTmpSize(powerShape1, powerShape1, true, 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 7);
    EXPECT_EQ(minVal, 256 * 7);
}

TEST_F(TestTiling, TestPowerTilingFactorSizeV200)
{
    platfrom_stub_set_chip_version("Ascend310P");
    platfrom_stub_set_npuarch("2002");
    auto platformPtr = platform_ascendc::PlatformAscendCManager::GetInstance("Ascend310P");
    uint32_t maxLiveNodeCnt = 0xffff;
    uint32_t extraBuf = 0xffff;
    GetPowerTmpBufferFactorSize(false, true, false, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 6);
    EXPECT_EQ(extraBuf, 256);
    GetPowerTmpBufferFactorSize(false, true, true, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 8);
    EXPECT_EQ(extraBuf, 0);
    GetPowerTmpBufferFactorSize(false, true, false, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 16);
    EXPECT_EQ(extraBuf, 256);
    GetPowerTmpBufferFactorSize(true, true, false, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 5);
    EXPECT_EQ(extraBuf, 256);
    GetPowerTmpBufferFactorSize(true, true, true, 4, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 7);
    EXPECT_EQ(extraBuf, 0);
    GetPowerTmpBufferFactorSize(true, true, false, 2, maxLiveNodeCnt, extraBuf);
    EXPECT_EQ(maxLiveNodeCnt, 16);
    EXPECT_EQ(extraBuf, 256);
}

TEST_F(TestTiling, TestLastBroadCast200)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    platfrom_stub_set_chip_version("Ascend310P");
    platfrom_stub_set_npuarch("2002");
    uint32_t firstDim = 32;
    uint32_t lastDim = 32;
    std::vector<int64_t> srcShapeDims = {firstDim, 1};
    auto srcShape = ge::Shape(srcShapeDims);
    std::vector<int64_t> dstShapeDims = {firstDim, lastDim};
    auto dstShape = ge::Shape(dstShapeDims);
    uint32_t maxValue{0};
    uint32_t minValue{0};
    constexpr uint32_t halfSize = 2;
    constexpr uint32_t halfOneBlockElementNum = 16;
    constexpr uint32_t MAX_BLOCK_NUM = 8;
    constexpr uint32_t ONE_BLOCK_SIZE = 32;
    uint32_t minTmpBufferSize = halfOneBlockElementNum * ((lastDim + MAX_BLOCK_NUM - 1) / MAX_BLOCK_NUM) * halfSize;
    uint32_t minHalfAlignSize = ONE_BLOCK_SIZE + +minTmpBufferSize;
    uint32_t maxHalfAlignSize = ONE_BLOCK_SIZE + firstDim * lastDim * halfSize;
    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, halfSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, minHalfAlignSize);
    EXPECT_EQ(maxValue, maxHalfAlignSize);

    constexpr uint32_t int8Size = 1;
    const uint32_t alignSrcSize =
        ((firstDim + halfOneBlockElementNum - 1) / halfOneBlockElementNum) * halfOneBlockElementNum;
    const uint32_t alignDstSize =
        ((firstDim * lastDim + halfOneBlockElementNum - 1) / halfOneBlockElementNum) * halfOneBlockElementNum;
    const uint32_t castTempBufferSize = (alignSrcSize + alignDstSize) * halfSize;
    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, int8Size, false, maxValue, minValue);
    EXPECT_EQ(minValue, minHalfAlignSize + castTempBufferSize);
    EXPECT_EQ(maxValue, maxHalfAlignSize + castTempBufferSize);
}

TEST_F(TestTiling, TestFirstBroadCast200)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    platfrom_stub_set_chip_version("Ascend310P");
    platfrom_stub_set_npuarch("2002");
    uint32_t firstDim = 32;
    uint32_t lastDim = 32;
    std::vector<int64_t> srcShapeDims = {1, lastDim};
    auto srcShape = ge::Shape(srcShapeDims);
    std::vector<int64_t> dstShapeDims = {firstDim, lastDim};
    auto dstShape = ge::Shape(dstShapeDims);
    uint32_t maxValue{0};
    uint32_t minValue{0};
    constexpr uint32_t halfSize = 2;
    constexpr uint32_t ONE_BLOCK_SIZE = 32;
    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, halfSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, ONE_BLOCK_SIZE);
    EXPECT_EQ(maxValue, ONE_BLOCK_SIZE);

    constexpr uint32_t int8Size = 1;
    constexpr uint32_t HALF_ONE_BLK_SIZE = 16;
    const uint32_t alignSrcSize = ((lastDim + HALF_ONE_BLK_SIZE - 1) / HALF_ONE_BLK_SIZE) * HALF_ONE_BLK_SIZE;
    const uint32_t alignDstSize =
        ((firstDim * lastDim + HALF_ONE_BLK_SIZE - 1) / HALF_ONE_BLK_SIZE) * HALF_ONE_BLK_SIZE;
    const uint32_t castTempBufferSize = (alignSrcSize + alignDstSize) * halfSize;
    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, int8Size, false, maxValue, minValue);
    EXPECT_EQ(minValue, ONE_BLOCK_SIZE + castTempBufferSize);
    EXPECT_EQ(maxValue, ONE_BLOCK_SIZE + castTempBufferSize);
}

TEST_F(TestTiling, TestOneElementBroadCast200)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    platfrom_stub_set_chip_version("Ascend310P");
    platfrom_stub_set_npuarch("2002");
    uint32_t srcDim = 1;
    uint32_t dstDim = 32;
    std::vector<int64_t> srcShapeDims = {srcDim};
    auto srcShape = ge::Shape(srcShapeDims);
    std::vector<int64_t> dstShapeDims = {dstDim};
    auto dstShape = ge::Shape(dstShapeDims);
    uint32_t maxValue{0};
    uint32_t minValue{0};
    constexpr uint32_t halfSize = 2;
    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, halfSize, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);

    constexpr uint32_t int8Size = 1;
    constexpr uint32_t HALF_ONE_BLK_SIZE = 16;
    constexpr uint32_t ONE_BLOCK_SIZE = 32;
    const uint32_t alignSrcSize = ((srcDim + HALF_ONE_BLK_SIZE - 1) / HALF_ONE_BLK_SIZE) * HALF_ONE_BLK_SIZE;
    const uint32_t alignDstSize = ((dstDim + HALF_ONE_BLK_SIZE - 1) / HALF_ONE_BLK_SIZE) * HALF_ONE_BLK_SIZE;
    const uint32_t castTempBufferSize = (alignSrcSize + alignDstSize) * halfSize;
    GetBroadCastMaxMinTmpSize(plat, srcShape, dstShape, int8Size, false, maxValue, minValue);
    EXPECT_EQ(minValue, castTempBufferSize + ONE_BLOCK_SIZE);
    EXPECT_EQ(maxValue, castTempBufferSize + ONE_BLOCK_SIZE);
}
#endif

TEST_F(TestTiling, testTransDataTilingUnalignedHw)
{
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    uint32_t maxSize;
    uint32_t minSize;
    int32_t n = 16;
    int32_t c = 16;
    int32_t d = 3;
    int32_t h = 3;
    int32_t w = 3;
    int32_t c0 = 16;
    int32_t n0 = 16;
    int32_t c1 = (c + c0 - 1) / c0;
    int32_t n1 = (n + n0 - 1) / n0;
    int32_t hw0 = 16;
    int32_t hw1 = (h * w + hw0 - 1) / hw0;
    auto ncdhwShape = ge::Shape({n, c, d, h, w});
    auto ndc1hwc0Shape = ge::Shape({n, d, c1, h, w, c0});
    auto fractalzShape = ge::Shape({d, c1, h, w, n1, n0, c0});
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    TransDataConfig config = {DataFormat::NCDHW, DataFormat::NDC1HWC0};
    bool ret =
        GetTransDataMaxMinTmpSize(plat, ncdhwShape, ndc1hwc0Shape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 1632);
    EXPECT_EQ(minSize, 1632);

    config = {DataFormat::NDC1HWC0, DataFormat::NCDHW};
    ret =
        GetTransDataMaxMinTmpSize(plat, ndc1hwc0Shape, ncdhwShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 2048);
    EXPECT_EQ(minSize, 2048);

    config = {DataFormat::NCDHW, DataFormat::FRACTAL_Z_3D};
    ret =
        GetTransDataMaxMinTmpSize(plat, ncdhwShape, fractalzShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 26112);
    EXPECT_EQ(minSize, 26112);

    config = {DataFormat::FRACTAL_Z_3D, DataFormat::NCDHW};
    ret =
        GetTransDataMaxMinTmpSize(plat, fractalzShape, ncdhwShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, n1 * n0 * c1 * c0 * d * hw0 * hw1 * 2);
    EXPECT_EQ(minSize, n1 * n0 * c1 * c0 * d * hw0 * hw1 * 2);
}

TEST_F(TestTiling, testTransDataTilingAlignedHw)
{
    platfrom_stub_set_chip_version("Ascend910B");
    platfrom_stub_set_npuarch("2201");
    uint32_t maxSize;
    uint32_t minSize;
    int32_t n = 5;
    int32_t c = 30;
    int32_t d = 2;
    int32_t h = 4;
    int32_t w = 8;
    int32_t c0 = 16;
    int32_t n0 = 16;
    int32_t c1 = (c + c0 - 1) / c0;
    int32_t n1 = (n + n0 - 1) / n0;
    int32_t hw0 = 16;
    int32_t hw1 = (h * w + hw0 - 1) / hw0;
    auto ncdhwShape = ge::Shape({n, c, d, h, w});
    auto ndc1hwc0Shape = ge::Shape({n, d, c1, h, w, c0});
    auto fractalzShape = ge::Shape({d, c1, h, w, n1, n0, c0});
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    TransDataConfig config = {DataFormat::NCDHW, DataFormat::NDC1HWC0};
    bool ret =
        GetTransDataMaxMinTmpSize(plat, ncdhwShape, ndc1hwc0Shape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 4224);
    EXPECT_EQ(minSize, 4224);

    config = {DataFormat::NDC1HWC0, DataFormat::NCDHW};
    ret =
        GetTransDataMaxMinTmpSize(plat, ndc1hwc0Shape, ncdhwShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 4608);
    EXPECT_EQ(minSize, 4608);

    config = {DataFormat::NCDHW, DataFormat::FRACTAL_Z_3D};
    ret =
        GetTransDataMaxMinTmpSize(plat, ncdhwShape, fractalzShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, 69376);
    EXPECT_EQ(minSize, 69376);

    config = {DataFormat::FRACTAL_Z_3D, DataFormat::NCDHW};
    ret =
        GetTransDataMaxMinTmpSize(plat, fractalzShape, ncdhwShape, ge::DataType::DT_FLOAT16, config, maxSize, minSize);

    EXPECT_TRUE(ret);
    EXPECT_EQ(maxSize, n1 * n0 * c1 * c0 * d * hw0 * hw1 * 2 * 2);
    EXPECT_EQ(minSize, n1 * n0 * c1 * c0 * d * hw0 * hw1 * 2 * 2);
}

TEST_F(TestTiling, TestReduceXorSumTilingInt16)
{
    std::vector<int64_t> shapeDims = {128, 128};
    auto shape = ge::Shape(shapeDims);
    uint32_t maxSize;
    uint32_t minSize;
    GetReduceXorSumMaxMinTmpSize(shape, 2, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 65536);
    EXPECT_EQ(minSize, 65536);

    GetReduceXorSumMaxMinTmpSize(shape, 2, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 98304);
    EXPECT_EQ(minSize, 98304);

    shapeDims = {8};
    shape = ge::Shape(shapeDims);
    GetReduceXorSumMaxMinTmpSize(shape, 2, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 768);
    EXPECT_EQ(minSize, 768);

    GetReduceXorSumMaxMinTmpSize(shape, 2, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 512);
    EXPECT_EQ(minSize, 512);
}

TEST_F(TestTiling, testReduceProdTiling)
{
    uint32_t maxSize;
    uint32_t minSize;
    auto shape = ge::Shape({128, 128});
    GetReduceProdMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 128 * 32 + 64 * 4 + 256);
    EXPECT_EQ(minSize, 128 * 32 + 64 * 4 + 256);

    shape = ge::Shape({16, 128});
    GetReduceProdMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 256);
    EXPECT_EQ(minSize, 256);

    shape = ge::Shape({16, 7});
    GetReduceProdMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 8 * 4);
    EXPECT_EQ(minSize, 16 / 2 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceProdMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);

    shape = ge::Shape({256, 16});
    GetReduceProdMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);
}

TEST_F(TestTiling, testReduceMaxTiling)
{
    uint32_t maxSize;
    uint32_t minSize;
    auto shape = ge::Shape({16, 8});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 16});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 32});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 8 * 4);
    EXPECT_EQ(minSize, 16 * 8 * 4);

    shape = ge::Shape({16, 64});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 64 * 4);
    EXPECT_EQ(minSize, 16 * 64 * 4);

    shape = ge::Shape({16, 16});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 64});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 16 * 2);
    EXPECT_EQ(minSize, 16 * 16 * 2);

    shape = ge::Shape({16, 128});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 2);
    EXPECT_EQ(minSize, 16 * 128 * 2);

    shape = ge::Shape({16, 7});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 8 * 4);
    EXPECT_EQ(minSize, 16 / 2 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);

    shape = ge::Shape({256, 16});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 7});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 16 * 2);
    EXPECT_EQ(minSize, 16 / 2 * 16 * 2);

    shape = ge::Shape({17, 127});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 2);
    EXPECT_EQ(minSize, 16 * 128 * 2);

    shape = ge::Shape({256, 16});
    GetReduceMaxMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);
}

TEST_F(TestTiling, testReduceMinTiling)
{
    uint32_t maxSize;
    uint32_t minSize;
    auto shape = ge::Shape({16, 8});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 16});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 32});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 8 * 4);
    EXPECT_EQ(minSize, 16 * 8 * 4);

    shape = ge::Shape({16, 64});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 64 * 4);
    EXPECT_EQ(minSize, 16 * 64 * 4);

    shape = ge::Shape({16, 16});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 64});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 16 * 2);
    EXPECT_EQ(minSize, 16 * 16 * 2);

    shape = ge::Shape({16, 128});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 2);
    EXPECT_EQ(minSize, 16 * 128 * 2);

    shape = ge::Shape({16, 7});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 8 * 4);
    EXPECT_EQ(minSize, 16 / 2 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);

    shape = ge::Shape({256, 16});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 7});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 16 * 2);
    EXPECT_EQ(minSize, 16 / 2 * 16 * 2);

    shape = ge::Shape({17, 127});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 2);
    EXPECT_EQ(minSize, 16 * 128 * 2);

    shape = ge::Shape({256, 16});
    GetReduceMinMaxMinTmpSize(shape, ge::DataType::DT_FLOAT16, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);
}

TEST_F(TestTiling, testReduceSumTiling)
{
    uint32_t maxSize;
    uint32_t minSize;
    auto shape = ge::Shape({128, 128});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, (64 * 128) * 4);
    EXPECT_EQ(minSize, (64 * 128) * 4);

    shape = ge::Shape({16, 32});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 128});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 7});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 8 * 8 * 4);
    EXPECT_EQ(minSize, 8 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);

    shape = ge::Shape({256, 16});
    GetReduceSumMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);
}

TEST_F(TestTiling, testReduceMeanTiling)
{
    uint32_t maxSize;
    uint32_t minSize;
    auto shape = ge::Shape({128, 128});
    GetReduceMeanMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, (128 * 64) * 4);
    EXPECT_EQ(minSize, (128 * 64) * 4);

    shape = ge::Shape({128, 32});
    GetReduceMeanMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 128});
    GetReduceMeanMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 7});
    GetReduceMeanMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 8 * 8 * 4);
    EXPECT_EQ(minSize, 8 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceMeanMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);
}

TEST_F(TestTiling, testReduceAnyTiling)
{
    uint32_t maxSize;
    uint32_t minSize;
    auto shape = ge::Shape({16, 8});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 16});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 32});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 8 * 4);
    EXPECT_EQ(minSize, 16 * 8 * 4);

    shape = ge::Shape({16, 64});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 64 * 4);
    EXPECT_EQ(minSize, 16 * 64 * 4);

    shape = ge::Shape({16, 7});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 8 * 4);
    EXPECT_EQ(minSize, 16 / 2 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);

    shape = ge::Shape({256, 16});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({128, 128});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, (128 * 1 * 2) + (128 * 16 * 2));
    EXPECT_EQ(minSize, (128 * 1 * 2) + (128 * 16 * 2));

    shape = ge::Shape({16, 128});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, (128 * 1 * 2) + (16 * 16 * 2));
    EXPECT_EQ(minSize, (128 * 1 * 2) + (16 * 16 * 2));

    shape = ge::Shape({16, 7});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 32);
    EXPECT_EQ(minSize, 16 / 2 * 32);

    shape = ge::Shape({17, 127});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128);
    EXPECT_EQ(minSize, 16 * 128);

    shape = ge::Shape({256, 16});
    GetReduceAnyMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);
}

TEST_F(TestTiling, testReduceAllTiling)
{
    uint32_t maxSize;
    uint32_t minSize;

    auto shape = ge::Shape({16, 8});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 16});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);

    shape = ge::Shape({16, 32});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 8 * 4);
    EXPECT_EQ(minSize, 16 * 8 * 4);

    shape = ge::Shape({16, 64});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 64 * 4);
    EXPECT_EQ(minSize, 16 * 64 * 4);

    shape = ge::Shape({16, 7});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 8 * 4);
    EXPECT_EQ(minSize, 16 / 2 * 8 * 4);

    shape = ge::Shape({17, 127});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128 * 4);
    EXPECT_EQ(minSize, 16 * 128 * 4);

    shape = ge::Shape({256, 16});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_FLOAT, ReducePattern::RA, true, true, maxSize, minSize);

    shape = ge::Shape({128, 128});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::AR, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, (128 * 1 * 2) + (128 * 16 * 2));
    EXPECT_EQ(minSize, (128 * 1 * 2) + (128 * 16 * 2));

    shape = ge::Shape({16, 128});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::AR, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, (128 * 1 * 2) + (16 * 16 * 2));
    EXPECT_EQ(minSize, (128 * 1 * 2) + (16 * 16 * 2));

    shape = ge::Shape({16, 7});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 / 2 * 32);
    EXPECT_EQ(minSize, 16 / 2 * 32);

    shape = ge::Shape({17, 127});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::RA, true, false, maxSize, minSize);
    EXPECT_EQ(maxSize, 16 * 128);
    EXPECT_EQ(minSize, 16 * 128);

    shape = ge::Shape({256, 16});
    GetReduceAllMaxMinTmpSize(shape, ge::DataType::DT_UINT8, ReducePattern::RA, true, true, maxSize, minSize);
    EXPECT_EQ(maxSize, 0);
    EXPECT_EQ(minSize, 0);
}

TEST_F(TestTiling, TestCumSum)
{
    uint32_t firstDim = 32;
    uint32_t lastDim = 16;
    std::vector<int64_t> srcShapeDims = {firstDim, lastDim};
    auto srcShape = ge::Shape(srcShapeDims);
    uint32_t maxValue{0};
    uint32_t minValue{0};
    constexpr uint32_t halfSize = 2;
    constexpr uint32_t transDataTo5HDAddrListSize = 16;
    uint32_t minHalfSize = transDataTo5HDAddrListSize * lastDim * 3 * sizeof(uint16_t);
    uint32_t alignOutter =
        (firstDim + transDataTo5HDAddrListSize - 1) / transDataTo5HDAddrListSize * transDataTo5HDAddrListSize;
    uint32_t maxHalfSize = alignOutter * lastDim * 3 * sizeof(uint16_t);

    GetCumSumMaxMinTmpSize(srcShape, halfSize, true, false, maxValue, minValue);
    EXPECT_EQ(minValue, minHalfSize);
    EXPECT_EQ(maxValue, maxHalfSize);

    constexpr uint32_t floatSize = 4;
    uint32_t minFloatSize = transDataTo5HDAddrListSize * lastDim * 2 * sizeof(float);
    uint32_t maxFloatSize = alignOutter * lastDim * 2 * sizeof(float);

    GetCumSumMaxMinTmpSize(srcShape, floatSize, true, false, maxValue, minValue);
    EXPECT_EQ(minValue, minFloatSize);
    EXPECT_EQ(maxValue, maxFloatSize);

    maxHalfSize = minHalfSize = firstDim * lastDim * sizeof(float);
    GetCumSumMaxMinTmpSize(srcShape, halfSize, false, false, maxValue, minValue);
    EXPECT_EQ(minValue, minHalfSize);
    EXPECT_EQ(maxValue, maxHalfSize);

    GetCumSumMaxMinTmpSize(srcShape, floatSize, false, false, maxValue, minValue);
    EXPECT_EQ(minValue, 0);
    EXPECT_EQ(maxValue, 0);
}
TEST_F(TestTiling, tiling_compute_error)
{
    MultiCoreMatmulTiling tiling;
    tiling.isBias = true;
    tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    tiling.biasType_.pos = TPosition::TSCM;
    int ret = tiling.Compute();
    EXPECT_EQ(ret, -1);

    BatchMatmulTiling bmm_tiling;
    bmm_tiling.isBias = true;
    bmm_tiling.socVersion = platform_ascendc::SocVersion::ASCEND310P;
    bmm_tiling.biasType_.pos = TPosition::TSCM;
    ret = bmm_tiling.Compute();
    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, TestWelfordUpdateTiling)
{
    std::vector<int64_t> shapeDims1d = {1, 128};
    auto shape1d = ge::Shape(shapeDims1d);
    uint32_t maxsize = 0;
    uint32_t minsize = 0;
    uint32_t dtypesizeT = 2; // half type
    uint32_t dtypesizeU = 4; // float type
    bool isReuseSource = false;
    GetWelfordUpdateMaxMinTmpSize(shape1d, dtypesizeT, dtypesizeU, isReuseSource, false, maxsize, minsize);
    EXPECT_EQ(minsize, 3 * 256);
    EXPECT_EQ(maxsize, 2 * 3 * 256);

    std::vector<int64_t> shapeDims2d = {1, 72};
    auto shape2d = ge::Shape(shapeDims2d);
    dtypesizeT = 4; // float type
    dtypesizeU = 4; // float type
    isReuseSource = false;
    GetWelfordUpdateMaxMinTmpSize(shape2d, dtypesizeT, dtypesizeU, isReuseSource, false, maxsize, minsize);
    EXPECT_EQ(minsize, 2 * 256);
    EXPECT_EQ(maxsize, 2 * 2 * 256);

    std::vector<int64_t> shapeDims3d = {1, 256};
    auto shape3d = ge::Shape(shapeDims3d);
    dtypesizeT = 4; // float type
    dtypesizeU = 4; // float type
    isReuseSource = true;
    GetWelfordUpdateMaxMinTmpSize(shape3d, dtypesizeT, dtypesizeU, isReuseSource, false, maxsize, minsize);
    EXPECT_EQ(minsize, 1 * 256);
    EXPECT_EQ(maxsize, 4 * 1 * 256);
}

TEST_F(TestTiling, TestWelfordFinalizeTiling)
{
    std::vector<int64_t> shape_dims_1d = {64};
    auto shape_1d = ge::Shape(shape_dims_1d);
    uint32_t maxsize = 0;
    uint32_t minsize = 0;
    uint32_t dtypesize = 4; // float type
    bool isReuseSource = false;
    GetWelfordFinalizeMaxMinTmpSize(shape_1d, dtypesize, isReuseSource, maxsize, minsize);
    EXPECT_EQ(minsize, 4 * 256);
    EXPECT_EQ(maxsize, 4 * 256);

    std::vector<int64_t> shape_dims_2d = {4096};
    auto shape_2d = ge::Shape(shape_dims_2d);
    isReuseSource = false;
    GetWelfordFinalizeMaxMinTmpSize(shape_2d, dtypesize, isReuseSource, maxsize, minsize);
    EXPECT_EQ(minsize, 4 * 256);
    EXPECT_EQ(maxsize, 2 * (256 + 4096 * 4));

    std::vector<int64_t> shape_dims_3d = {8};
    auto shape_3d = ge::Shape(shape_dims_3d);
    isReuseSource = false;
    GetWelfordFinalizeMaxMinTmpSize(shape_3d, dtypesize, isReuseSource, maxsize, minsize);
    EXPECT_EQ(minsize, 4 * 256);
    EXPECT_EQ(maxsize, 4 * 256);

    std::vector<int64_t> shape_dims_4d = {72};
    auto shape_4d = ge::Shape(shape_dims_4d);
    isReuseSource = false;
    GetWelfordFinalizeMaxMinTmpSize(shape_4d, dtypesize, isReuseSource, maxsize, minsize);
    EXPECT_EQ(minsize, 4 * 256);
    EXPECT_EQ(maxsize, 2 * (256 + 72 * 4));
}

TEST_F(TestTiling, TestDropOutTiling)
{
    std::vector<int64_t> shape_dims_1d = {2, 8};
    auto shape_1d = ge::Shape(shape_dims_1d);
    uint32_t maxsize = 0;
    uint32_t minsize = 0;
    uint32_t dtypesize = 4; // float type
    bool isReuseSource = true;
    GetDropOutMaxMinTmpSize(shape_1d, dtypesize, isReuseSource, maxsize, minsize);
    EXPECT_EQ(minsize, 512);
    EXPECT_EQ(maxsize, 512);
}

TEST_F(TestTiling, TestNormalizeTiling)
{
    // T: float + U: float
    uint32_t A = 4;
    uint32_t alignA = 8;
    uint32_t R = 32;
    std::vector<int64_t> shapeDims = {A, R}; // [A, R]
    auto shapeInput = ge::Shape(shapeDims);
    uint32_t maxsize = 0;
    uint32_t minsize = 0;
    uint32_t dtypesizeT = 4;
    uint32_t dtypesizeU = 4;
    bool isReuseSource = false;
    GetNormalizeMaxMinTmpSize(shapeInput, dtypesizeU, dtypesizeT, isReuseSource, true, false, maxsize, minsize);
    EXPECT_EQ(minsize, (2 * R) * sizeof(float));
    EXPECT_EQ(maxsize, (2 * R + 2 * alignA * R) * sizeof(float));

    // T: half + U: half
    dtypesizeT = 2;
    dtypesizeU = 2;
    GetNormalizeMaxMinTmpSize(shapeInput, dtypesizeU, dtypesizeT, isReuseSource, true, false, maxsize, minsize);
    EXPECT_EQ(minsize, (2 * R + 2 * R) * sizeof(float));
    EXPECT_EQ(maxsize, (2 * R + 2 * alignA * R) * sizeof(float));

    // T: half + U: float
    dtypesizeT = 2;
    dtypesizeU = 4;
    GetNormalizeMaxMinTmpSize(shapeInput, dtypesizeU, dtypesizeT, isReuseSource, true, false, maxsize, minsize);
    EXPECT_EQ(minsize, (2 * R) * sizeof(float));
    EXPECT_EQ(maxsize, (2 * R + 2 * alignA * R) * sizeof(float));
}

TEST_F(TestTiling, TestLayerNormRstdTiling)
{
    uint32_t A = 4;
    uint32_t R = 32;
    uint32_t AlignA = 8;
    std::vector<int64_t> shapeDims = {A, R}; // [A, R]
    auto shapeInput = ge::Shape(shapeDims);
    uint32_t maxsize = 0;
    uint32_t minsize = 0;
    bool isReuseSource = false;
    GetLayerNormMaxMinTmpSize(shapeInput, sizeof(float), isReuseSource, true, false, maxsize, minsize);
    EXPECT_EQ(minsize, (2 * R + AlignA) * sizeof(float));
    EXPECT_EQ(maxsize, (AlignA + 2 * R + 2 * AlignA * R) * sizeof(float));

    GetLayerNormMaxMinTmpSize(shapeInput, sizeof(float), isReuseSource, true, false, maxsize, minsize);
    EXPECT_EQ(minsize, (2 * R + AlignA) * sizeof(float));
    EXPECT_EQ(maxsize, (AlignA + 2 * R + 2 * AlignA * R) * sizeof(float));

    GetLayerNormMaxMinTmpSize(shapeInput, sizeof(float), isReuseSource, true, false, maxsize, minsize);
    EXPECT_EQ(minsize, (2 * R + AlignA) * sizeof(float));
    EXPECT_EQ(maxsize, (AlignA + 2 * R + 2 * AlignA * R) * sizeof(float));

    A = 32;
    R = 8;
    std::vector<int64_t> shapeDims2 = {A, R}; // [A, R]
    auto shapeInput2 = ge::Shape(shapeDims2);
    GetLayerNormMaxMinTmpSize(shapeInput2, sizeof(float), isReuseSource, true, false, maxsize, minsize);
    EXPECT_EQ(minsize, (2 * R + 2 * R + A) * sizeof(float));
    EXPECT_EQ(maxsize, (A + 2 * R + 2 * A * R) * sizeof(float));
    optiling::LayerNormSeparateTiling tiling;
    uint32_t stackBufferSize = 4096;
    shapeDims = {A, R};
    auto layernormShape = ge::Shape(shapeDims);
    GetLayerNormNDTilingInfo(layernormShape, stackBufferSize, sizeof(float), false, true, tiling);
    GetLayerNormNDTilingInfo(layernormShape, 0, sizeof(float), false, true, tiling);

    AscendC::tiling::LayerNormSeparateTiling tilingNotOp;
    GetLayerNormNDTilingInfo(layernormShape, stackBufferSize, sizeof(float), false, true, tilingNotOp);
    GetLayerNormNDTilingInfo(layernormShape, 0, sizeof(float), false, true, tilingNotOp);
}

TEST_F(TestTiling, TestNZFp32UnalignedK)
{
    matmul_tiling::PlatformInfo plat{
        .socVersion = platform_ascendc::SocVersion::ASCEND910B,
        .l1Size = 524288,
        .l0CSize = 131072,
        .ubSize = 196608,
        .l0ASize = 65536,
        .l0BSize = 65536};
    matmul_tiling::MatmulApiTiling tiling(plat);
    tiling.SetAType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetCType(TPosition::GM, CubeFormat::NZ, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(192, 248, 160);
    tiling.SetOrgShape(192, 248, 160);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    tiling.GetTiling(tilingData);

    EXPECT_GE(tilingData.get_shareL1Size(), 155648);
    EXPECT_GE(tilingData.get_transLength(), 0);
    int32_t baseK = tiling.GetBaseK();
    EXPECT_EQ(baseK % 16, 0);
}

TEST_F(TestTiling, MultiCoreSparse)
{
    matmul_tiling::MatmulApiTiling sparseMatmul;
    sparseMatmul.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    sparseMatmul.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8, true);
    sparseMatmul.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT32);
    sparseMatmul.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    sparseMatmul.SetSparse(true);
    auto ret = sparseMatmul.SetOrgShape(1024, 64, 128);
    ret = sparseMatmul.SetShape(1024, 64, 128);
    ret = sparseMatmul.SetFixSplit(-1, -1, 80);
    ret = sparseMatmul.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    ret = sparseMatmul.GetTiling(tilingData);
    tilingData.set_iterateOrder(1);

    matmul_tiling::SysTilingTempBufSize bufSize;
    MatmulGetTmpBufSize(tilingData, bufSize);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 139264);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, MultiCoreSparseAlignBaseK)
{
    matmul_tiling::MatmulApiTiling sparseMatmul;
    sparseMatmul.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    sparseMatmul.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8, true);
    sparseMatmul.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT32);
    sparseMatmul.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    sparseMatmul.SetSparse(true);
    auto ret = sparseMatmul.SetOrgShape(28, 784, 16);
    ret = sparseMatmul.SetShape(28, 784, 16);
    ret = sparseMatmul.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    ret = sparseMatmul.GetTiling(tilingData);
    tilingData.set_iterateOrder(1);

    matmul_tiling::SysTilingTempBufSize bufSize;
    MatmulGetTmpBufSize(tilingData, bufSize);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 55296);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, MultiCoreSparseADbOff)
{
    matmul_tiling::MatmulApiTiling sparseMatmul;
    sparseMatmul.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    sparseMatmul.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8, true);
    sparseMatmul.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT32);
    sparseMatmul.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    sparseMatmul.SetSparse(true);
    auto ret = sparseMatmul.SetOrgShape(1024, 64, 128);
    ret = sparseMatmul.SetShape(1024, 64, 128);
    ret = sparseMatmul.SetFixSplit(1024, 16, -1);
    ret = sparseMatmul.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    ret = sparseMatmul.GetTiling(tilingData);
    tilingData.set_iterateOrder(1);

    matmul_tiling::SysTilingTempBufSize bufSize;
    MatmulGetTmpBufSize(tilingData, bufSize);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 139264);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, MultiCoreSparseBDbOff)
{
    matmul_tiling::MatmulApiTiling sparseMatmul;
    sparseMatmul.SetAType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    sparseMatmul.SetBType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8, true);
    sparseMatmul.SetCType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT32);
    sparseMatmul.SetBiasType(
        matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_INT8);
    sparseMatmul.SetSparse(true);
    auto ret = sparseMatmul.SetOrgShape(64, 1024, 128);
    ret = sparseMatmul.SetShape(64, 1024, 128);
    ret = sparseMatmul.SetFixSplit(16, 1024, -1);
    ret = sparseMatmul.SetBufferSpace(-1, -1, -1);
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    ret = sparseMatmul.GetTiling(tilingData);
    tilingData.set_iterateOrder(1);

    matmul_tiling::SysTilingTempBufSize bufSize;
    MatmulGetTmpBufSize(tilingData, bufSize);
    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.get_shareL1Size(), 139264);
    EXPECT_GE(tilingData.get_transLength(), 0);
}

TEST_F(TestTiling, TestMatmulApiTilingIsBTransKMisAlign)
{
    matmul_tiling::MultiCoreMatmulTiling tiling;

    tiling.SetDim(24);
    tiling.SetAType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT, true);
    tiling.SetCType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1948, 27648, 24);
    tiling.SetOrgShape(1948, 27648, 24);
    tiling.EnableBias(false);
    tiling.SetBufferSpace(-1, -1, -1, -1);

    AscendC::tiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_GE(tilingData.shareL1Size, 319488);
    EXPECT_GE(tilingData.transLength, 0);

    EXPECT_EQ(tilingData.baseM, 128);
    EXPECT_EQ(tilingData.baseN, 256);
    EXPECT_EQ(tilingData.baseK, 24);
    EXPECT_EQ(tilingData.depthA1, 16);
    EXPECT_EQ(tilingData.depthB1, 5);
    EXPECT_EQ(tilingData.stepM, 16);
    EXPECT_EQ(tilingData.stepN, 5);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33CheckL0ABUpdateBaseMN)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(160, 160, 320);
    tiling.SetShape(160, 160, 320);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_LE(MathUtil::CeilDivision(tilingData.get_singleCoreM(), tilingData.get_baseM()), 3);
    EXPECT_LE(MathUtil::CeilDivision(tilingData.get_singleCoreK(), tilingData.get_baseK()), 3);
    EXPECT_EQ(tilingData.get_stepM(), MathUtil::CeilDivision(tilingData.get_singleCoreM(), tilingData.get_baseM()));
    EXPECT_LE(tilingData.get_stepKa(), 3);
    EXPECT_EQ(tilingData.get_stepKa(), tilingData.get_stepKb());
    EXPECT_EQ(tilingData.get_stepKa(), MathUtil::CeilDivision(tilingData.get_singleCoreK(), tilingData.get_baseK()));
    EXPECT_EQ(tilingData.get_baseM(), 80);
    EXPECT_EQ(tilingData.get_baseN(), 144);
    EXPECT_EQ(tilingData.get_baseK(), 112);
    EXPECT_EQ(tilingData.get_stepM(), 2);
    EXPECT_EQ(tilingData.get_stepKa(), 3);
    EXPECT_EQ(tilingData.get_stepKb(), 3);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33CheckL0ACUpdateBaseKN)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(800, 2048, 64);
    tiling.SetShape(800, 2048, 64);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_LE(MathUtil::CeilDivision(tilingData.get_singleCoreM(), tilingData.get_baseM()), 3);
    EXPECT_LE(MathUtil::CeilDivision(tilingData.get_singleCoreK(), tilingData.get_baseK()), 3);
    EXPECT_EQ(tilingData.get_stepM(), MathUtil::CeilDivision(tilingData.get_singleCoreM(), tilingData.get_baseM()));
    EXPECT_LE(tilingData.get_stepKa(), 3);
    EXPECT_EQ(tilingData.get_stepKa(), tilingData.get_stepKb());
    EXPECT_EQ(tilingData.get_stepKa(), MathUtil::CeilDivision(tilingData.get_singleCoreK(), tilingData.get_baseK()));
    EXPECT_EQ(tilingData.get_baseM(), 272);
    EXPECT_EQ(tilingData.get_baseN(), 112);
    EXPECT_EQ(tilingData.get_baseK(), 32);
    EXPECT_EQ(tilingData.get_stepM(), 3);
    EXPECT_EQ(tilingData.get_stepKa(), 2);
    EXPECT_EQ(tilingData.get_stepKb(), 2);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33SetCorrectBase)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(384, 512, 192);
    tiling.SetShape(384, 512, 192);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    tiling.SetFixSplit(128, 256, 64);
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_EQ(tilingData.get_baseM(), 128);
    EXPECT_EQ(tilingData.get_baseN(), 256);
    EXPECT_EQ(tilingData.get_baseK(), 64);
    EXPECT_EQ(tilingData.get_stepM(), 3);
    EXPECT_EQ(tilingData.get_stepKa(), 3);
    EXPECT_EQ(tilingData.get_stepKb(), 3);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33Set0BaseM)
{
    matmul_tiling::MultiCoreMatmulTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(384, 512, 192);
    tiling.SetShape(384, 512, 192);
    tiling.SetSingleShape(384, 512, 192);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    auto ret = tiling.SetFixSplit(0);
    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33SetSmallBaseM)
{
    matmul_tiling::MultiCoreMatmulTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(384, 512, 192);
    tiling.SetShape(384, 512, 192);
    tiling.SetSingleShape(384, 512, 192);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    tiling.SetFixSplit(96);
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33MultiDepthB1)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetAType(
        matmul_tiling::TPosition::TSCM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT, true);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(32, 1024, 32);
    tiling.SetShape(32, 1024, 32);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    tiling.SetDequantType(DequantType::TENSOR);
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_LE(MathUtil::CeilDivision(tilingData.get_singleCoreM(), tilingData.get_baseM()), 3);
    EXPECT_LE(MathUtil::CeilDivision(tilingData.get_singleCoreK(), tilingData.get_baseK()), 3);
    EXPECT_EQ(tilingData.get_stepM(), MathUtil::CeilDivision(tilingData.get_singleCoreM(), tilingData.get_baseM()));
    EXPECT_LE(tilingData.get_stepKa(), 3);
    EXPECT_EQ(tilingData.get_stepKa(), tilingData.get_stepKb());
    EXPECT_EQ(tilingData.get_stepKa(), MathUtil::CeilDivision(tilingData.get_singleCoreK(), tilingData.get_baseK()));
    EXPECT_EQ(tilingData.get_baseM(), 32);
    EXPECT_EQ(tilingData.get_baseN(), 256);
    EXPECT_EQ(tilingData.get_baseK(), 32);
    EXPECT_EQ(tilingData.get_stepM(), 1);
    EXPECT_EQ(tilingData.get_stepKa(), 1);
    EXPECT_EQ(tilingData.get_stepKb(), 1);
    EXPECT_EQ(tilingData.get_depthB1(), 4);
    EXPECT_EQ(tilingData.get_stepN(), 2);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33CheckL0AFail)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(1024, 512, 512);
    tiling.SetShape(1024, 512, 512);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33Fp16CheckL0CFail)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT16);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT16);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT16);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(480, 512, 384);
    tiling.SetShape(480, 512, 384);
    tiling.SetFixSplit(-1, 256);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    optiling::TCubeTiling tilingData;
    tilingData.set_usedCoreNum(1);
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33CheckL0BUpdateK)
{
    matmul_tiling::MatmulApiTiling tiling;
    matmul_tiling::MatmulTilingAlgorithm tilingAlgo(&tiling);
    int32_t baseN = 128;
    int32_t baseK = 256;
    auto ret = tilingAlgo.CheckL0BSize(1024, 256, baseN, baseK);
    EXPECT_EQ(ret, true);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33CheckL0BFail)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetFixSplit(128, 256);
    matmul_tiling::MatmulTilingAlgorithm tilingAlgo(&tiling);
    int32_t baseN = 512;
    int32_t baseK = 512;
    auto ret = tilingAlgo.CheckL0BSize(1024, 512, baseN, baseK);
    EXPECT_EQ(ret, false);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33CheckL0CUpdateM)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetFixSplit(-1, 256);
    matmul_tiling::MatmulTilingAlgorithm tilingAlgo(&tiling);
    int32_t baseM = 256;
    int32_t baseN = 256;
    auto ret = tilingAlgo.CheckL0CSize(384, 512, baseM, baseN);
    EXPECT_EQ(ret, true);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33CheckL0CFail)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetFixSplit(-1, 256);
    matmul_tiling::MatmulTilingAlgorithm tilingAlgo(&tiling);
    int32_t baseM = 512;
    int32_t baseN = 256;
    auto ret = tilingAlgo.CheckL0CSize(1024, 512, baseM, baseN);
    EXPECT_EQ(ret, false);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33UpdateStepNFail)
{
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(384, 1024, 512);
    tiling.SetShape(384, 1024, 512);
    tiling.tiling_.set_singleCoreM(384);
    tiling.tiling_.set_singleCoreN(1024);
    tiling.tiling_.set_singleCoreK(512);
    tiling.tiling_.set_baseM(128);
    tiling.tiling_.set_baseN(256);
    tiling.tiling_.set_baseK(512);
    tiling.scheduleType = ScheduleType::N_BUFFER_33;
    matmul_tiling::MatmulTilingAlgorithm tilingAlgo(&tiling);
    CoreStatusPack coreStatus;
    SingleCoreStatus singleCoreStatus;
    singleCoreStatus.l1Status.mAL1 = 3;
    singleCoreStatus.l1Status.nBL1 = 4;
    singleCoreStatus.l1Status.kAL1 = 512;
    singleCoreStatus.l1Status.kBL1 = 512;
    singleCoreStatus.l0Status.kL0 = 512;
    auto ret = tilingAlgo.AdjustNBuffer33L1Factors(coreStatus, singleCoreStatus);
    EXPECT_EQ(ret, false);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33MultiCoreDisableSplitK)
{
    matmul_tiling::MultiCoreMatmulTiling tiling;
    tiling.SetDim(24);
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(384, 512, 192);
    tiling.SetShape(384, 512, 192);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    optiling::TCubeTiling tilingData;
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, -1);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33MultiCoreNormal)
{
    matmul_tiling::MultiCoreMatmulTiling tiling;
    tiling.SetDim(24);
    tiling.EnableMultiCoreSplitK(true);
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(384, 512, 192);
    tiling.SetShape(384, 512, 192);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    optiling::TCubeTiling tilingData;
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_EQ(tilingData.get_singleCoreM(), 384);
    EXPECT_EQ(tilingData.get_singleCoreN(), 256);
    EXPECT_EQ(tilingData.get_singleCoreK(), 96);
    EXPECT_EQ(tilingData.get_baseM(), 128);
    EXPECT_EQ(tilingData.get_baseN(), 256);
    EXPECT_EQ(tilingData.get_baseK(), 32);
    EXPECT_EQ(tilingData.get_stepM(), 3);
    EXPECT_EQ(tilingData.get_stepN(), 1);
    EXPECT_EQ(tilingData.get_stepKa(), 3);
    EXPECT_EQ(tilingData.get_stepKb(), 3);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33MultiCoreMExceedsCoreNum)
{
    matmul_tiling::MultiCoreMatmulTiling tiling;
    tiling.SetDim(4);
    tiling.EnableMultiCoreSplitK(true);
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(2048, 512, 32);
    tiling.SetShape(2048, 512, 32);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    optiling::TCubeTiling tilingData;
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_EQ(tilingData.get_usedCoreNum(), 4);
    EXPECT_EQ(tilingData.get_singleCoreM(), 384);
    EXPECT_EQ(tilingData.get_singleCoreN(), 512);
    EXPECT_EQ(tilingData.get_singleCoreK(), 32);
    EXPECT_EQ(tilingData.get_baseM(), 128);
    EXPECT_EQ(tilingData.get_baseN(), 256);
    EXPECT_EQ(tilingData.get_baseK(), 32);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33MultiCoreKExceedsCoreNum)
{
    matmul_tiling::MultiCoreMatmulTiling tiling;
    tiling.SetDim(4);
    tiling.EnableMultiCoreSplitK(true);
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(384, 512, 1024);
    tiling.SetShape(394, 512, 1024);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    optiling::TCubeTiling tilingData;
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_EQ(tilingData.get_usedCoreNum(), 4);
    EXPECT_EQ(tilingData.get_singleCoreM(), 384);
    EXPECT_EQ(tilingData.get_singleCoreN(), 512);
    EXPECT_EQ(tilingData.get_singleCoreK(), 96);
    EXPECT_EQ(tilingData.get_baseM(), 128);
    EXPECT_EQ(tilingData.get_baseN(), 256);
    EXPECT_EQ(tilingData.get_baseK(), 32);
}

TEST_F(TestTiling, MatmulApiTilingNBuffer33MultiCoreCheckUsedCoreNum)
{
    matmul_tiling::MultiCoreMatmulTiling tiling;
    tiling.SetDim(4);
    tiling.EnableMultiCoreSplitK(true);
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(1024, 64, 64);
    tiling.SetShape(1024, 64, 64);
    tiling.SetSingleShape(128, 64, 64);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    tiling.SetMatmulConfigParams(1, false, ScheduleType::N_BUFFER_33, MatrixTraverse::NOSET, false);
    optiling::TCubeTiling tilingData;
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_EQ(tilingData.get_usedCoreNum(), 4);
}

TEST_F(TestTiling, MatmulApiTilingCheckUsedCoreNum)
{
    matmul_tiling::MultiCoreMatmulTiling tiling;
    tiling.SetDim(24);
    tiling.EnableMultiCoreSplitK(true);
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType ::DT_FLOAT);
    tiling.SetOrgShape(2048, 256, 128);
    tiling.SetShape(2048, 256, 128);
    tiling.SetSingleShape(128, 256, 128);
    tiling.SetBufferSpace(-1, -1, -1); // will use all buffer space if not explicitly specified
    optiling::TCubeTiling tilingData;
    auto ret = tiling.GetTiling(tilingData);

    EXPECT_EQ(ret, 0);
    EXPECT_EQ(tilingData.get_usedCoreNum(), 24);
}

TEST_F(TestTiling, TestTilingHostLog)
{
    EXPECT_NE(logInstance->logHandle, nullptr);
    EXPECT_NE(logInstance->CheckLogLevel, nullptr);
    EXPECT_NE(logInstance->DlogRecord, nullptr);
    signal(SIGABRT, SIG_IGN);
    logInstance->CheckLogLibFuncApi(nullptr, nullptr);
    signal(SIGABRT, SIG_DFL);
    UnifiedLog::LoggingSingleton logInstanceOnce;
}
#endif
