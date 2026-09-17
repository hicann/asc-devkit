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
#include "graph/tensor.h"
#include <dlfcn.h>
#include <string>
#define private public
#define protected public
#include "base/registry/op_impl_space_registry_v2.h"
#include "adv_api/matmul/matmul_tilingdata.h"
#include "platform_ascendc.h"
#include "platform/platform_info.h"
#include "platform/soc_spec.h"
#include "utils/context/context_builder.h"
#include "utils/context/context_builder_impl.h"

static int64_t GetTensorSizeByStorageShapeStub(const gert::StorageShape& storageShape, const ge::DataType& dtype)
{
    (void)storageShape;
    (void)dtype;
    return 10000000000LL;
}

class TestTiling : public testing::Test {
protected:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    void TearDown() { GlobalMockObject::verify(); }
};

TEST_F(TestTiling, TestAddInputTdMallocFailed)
{
    gert::StorageShape input_shape = {{1024, 5120}, {1024, 5120}};
    gert::StorageShape output_shape = {{1024, 5120}, {1024, 5120}};
    std::string file_path = "./test_data.bin";
    auto param = gert::TilingData::CreateCap(4096);
    auto workspace_size_holder = gert::ContinuousVector::Create<size_t>(4096);
    auto ws_size = reinterpret_cast<gert::ContinuousVector*>(workspace_size_holder.get());

    MOCKER_CPP(&context_ascendc::DataUtils::GetTensorSizeByStorageShape)
        .stubs()
        .will(invoke(GetTensorSizeByStorageShapeStub));

    auto holder = context_ascendc::ContextBuilder()
                      .NodeIoNum(1, 1)
                      .IrInstanceNum({1})
                      .SetOpNameType("tmpName", "tmpType")
                      .AddInputTd(0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, input_shape, file_path)
                      .AddOutputTd(0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, output_shape)
                      .TilingData(param.get())
                      .Workspace(ws_size)
                      .BuildTilingContext();

    EXPECT_EQ(holder, nullptr);
    GlobalMockObject::verify();
}

TEST_F(TestTiling, GetShapeSize)
{
    EXPECT_EQ(ge::Shape().GetShapeSize(), 0);
    EXPECT_EQ(ge::Shape({1, 1}).GetShapeSize(), 1);
    EXPECT_EQ(ge::Shape({0, 1}).GetShapeSize(), 0);
    EXPECT_EQ(ge::Shape({1, 0}).GetShapeSize(), 0);
    EXPECT_EQ(ge::Shape({-1, -1}).GetShapeSize(), -1);
    EXPECT_EQ(ge::Shape({1, -1}).GetShapeSize(), -1);
    EXPECT_EQ(ge::Shape({-1, 1}).GetShapeSize(), -1);
    EXPECT_EQ(ge::Shape({INT64_MAX, INT64_MAX}).GetShapeSize(), 0);
    EXPECT_EQ(ge::Shape({INT64_MIN, INT64_MIN}).GetShapeSize(), 0);
    EXPECT_EQ(ge::Shape({INT64_MAX, INT64_MIN}).GetShapeSize(), 0);
    EXPECT_EQ(ge::Shape({INT64_MIN, INT64_MAX}).GetShapeSize(), 0);
}

TEST_F(TestTiling, TestPlatformAscendCReserveLocalMemory)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .will(returnValue(false));
    constexpr uint64_t mockUBSize = 128 * 1024;
    MOCKER_CPP(
        &fe::PlatFormInfos::GetLocalMemSize,
        void(fe::PlatFormInfos::*)(const fe::LocalMemType& mem_type, uint64_t& size))
        .stubs()
        .with(mockcpp::any(), outBound(mockUBSize))
        .will(returnValue(false));

    MOCKER_CPP(
        &platform_ascendc::PlatformAscendC::GetSocVersion,
        platform_ascendc::SocVersion(platform_ascendc::PlatformAscendC::*)(void) const)
        .stubs()
        .will(returnValue(platform_ascendc::SocVersion::ASCEND310P));
    uint64_t ubSize, l1Size;
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1Size);
    EXPECT_EQ(ubSize, mockUBSize);
    EXPECT_EQ(l1Size, mockUBSize);

    plat.ReserveLocalMemory(platform_ascendc::ReservedSize::RESERVED_SIZE_16K);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1Size);
    EXPECT_EQ(ubSize, mockUBSize - 16 * 1024);
    EXPECT_EQ(l1Size, mockUBSize);

    plat.ReserveLocalMemory(platform_ascendc::ReservedSize::RESERVED_SIZE_32K);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1Size);
    EXPECT_EQ(ubSize, mockUBSize - 32 * 1024);
    EXPECT_EQ(l1Size, mockUBSize);

    plat.ReserveLocalMemory(platform_ascendc::ReservedSize::RESERVED_SIZE_8K);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1Size);
    EXPECT_EQ(ubSize, mockUBSize - 8 * 1024);
    EXPECT_EQ(l1Size, mockUBSize);
    plat.ReserveLocalMemory(static_cast<platform_ascendc::ReservedSize>(5));
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002)
TEST_F(TestTiling, TestPlatformAscendC)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    EXPECT_EQ(plat.GetCoreNumVector(), 8);
    EXPECT_EQ(plat.GetCoreNumVector() + plat.GetCoreNumAic(), 18);

    uint64_t bt_size, fb_size;
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::BT, bt_size);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::FB, fb_size);

    EXPECT_EQ(bt_size, 0);
    EXPECT_EQ(fb_size, 0);
}
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
extern void platfrom_stub_set_num_aic(const char* num);
extern void platfrom_stub_set_num_aiv(const char* num);
extern void platfrom_stub_set_num_cub(const char* num);
extern void platfrom_stub_set_ctl(const char* num);
extern void platfrom_stub_set_chip_version(const char* num);
extern void platfrom_stub_set_num(uint32_t num);
extern void platfrom_stub_set_npuarch(const char* num);
TEST_F(TestTiling, TestPlatformAscendC)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    uint64_t ub_size, l1_size, bt_size, fb_size, l0;
    uint64_t l2_bw, hbm_bw, bw;
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ub_size);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1_size);
    EXPECT_EQ(ub_size, 196352);
    EXPECT_EQ(l1_size, 524032);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L0_A, l0);
    EXPECT_EQ(l0, 65536);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L0_B, l0);
    EXPECT_EQ(l0, 65536);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L0_C, l0);
    EXPECT_EQ(l0, 65536 * 2);
    plat.GetCoreMemBw(platform_ascendc::CoreMemType::L2, l2_bw);
    plat.GetCoreMemBw(platform_ascendc::CoreMemType::HBM, hbm_bw);
    EXPECT_EQ(l2_bw, 110);
    EXPECT_EQ(hbm_bw, 32);
    plat.GetCoreMemBw(platform_ascendc::CoreMemType::UB, bw);
    EXPECT_EQ(plat.GetVecRegLen(), 0);
    EXPECT_EQ(plat.GetCoreNum(), 48);
    EXPECT_EQ(plat.GetCoreNumAic(), 24);
    EXPECT_EQ(plat.GetCoreNumAiv(), 48);
    platfrom_stub_set_num_cub("20");
    EXPECT_EQ(plat.GetCoreNumAic(), 20);
    platfrom_stub_set_num_aiv("40");
    EXPECT_EQ(plat.GetCoreNumAiv(), 40);
    platfrom_stub_set_ctl("AICore");
    EXPECT_EQ(plat.GetCoreNumAic(), 24);
    EXPECT_EQ(plat.GetCoreNumAiv(), 24);
    platfrom_stub_set_num_aic("20");
    EXPECT_EQ(plat.GetCoreNumAic(), 20);
    EXPECT_EQ(plat.GetCoreNumAiv(), 20);
    EXPECT_EQ(bw, 0);
    EXPECT_EQ(plat.CalcTschBlockDim(1, 0, 1), 1);
    EXPECT_EQ(plat.CalcTschNumBlocks(1, 1, 0), 1);
    EXPECT_EQ(plat.CalcTschBlockDim(2, 1, 1), 2);
    EXPECT_EQ(plat.CalcTschNumBlocks(2, 2, 1), 2);
    EXPECT_EQ(plat.CalcTschBlockDim(2, 1, 2), 1);
    // invalid case, return 0
    EXPECT_EQ(plat.CalcTschBlockDim(3, 1, 2), 0);
    EXPECT_EQ(plat.CalcTschNumBlocks(6, 1, 3), 2);
    EXPECT_EQ(plat.CalcTschBlockDim(38, 20, 40), 19);
    EXPECT_EQ(plat.CalcTschNumBlocks(39, 20, 40), 20);
    EXPECT_EQ(plat.CalcTschBlockDim(40, 20, 40), 20);
    // invalid case, return 0
    EXPECT_EQ(plat.CalcTschBlockDim(41, 20, 40), 0);
    EXPECT_EQ(plat.CalcTschNumBlocks(41, 20, 40), 0);
    EXPECT_EQ(plat.GetCurNpuArch(), NpuArch::DAV_2201);
    EXPECT_EQ(plat.GetLibApiWorkSpaceSize(), 16 * 1024 * 1024);
    EXPECT_EQ(plat.GetResCubeGroupWorkSpaceSize(), 1 * 1024 * 1024);
    EXPECT_EQ(plat.GetResGroupBarrierWorkSpaceSize(), 1 * 1024 * 1024);
    platfrom_stub_set_chip_version("Ascend910");
    platfrom_stub_set_npuarch("1001");
    EXPECT_EQ(plat.GetLibApiWorkSpaceSize(), 2 * 1024 * 1024);
    EXPECT_EQ(plat.GetResCubeGroupWorkSpaceSize(), static_cast<uint32_t>(-1));
    EXPECT_EQ(plat.GetResGroupBarrierWorkSpaceSize(), static_cast<uint32_t>(-1));
    EXPECT_EQ(plat.GetSocVersion(), platform_ascendc::SocVersion::ASCEND910);
    EXPECT_EQ(plat.GetCoreNumVector(), 0);
    platfrom_stub_set_chip_version("Ascend950");
    platfrom_stub_set_npuarch("3510");
    EXPECT_EQ(plat.GetLibApiWorkSpaceSize(), 16 * 1024 * 1024);
    platfrom_stub_set_chip_version("MC62");
    platfrom_stub_set_npuarch("5102");
    EXPECT_EQ(plat.GetLibApiWorkSpaceSize(), 1024);
    platfrom_stub_set_chip_version("Ascend950");
    platfrom_stub_set_npuarch("3510");
    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .will(returnValue(true));
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::BT, bt_size);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::FB, fb_size);
    EXPECT_EQ(bt_size, 0);
    EXPECT_EQ(fb_size, 0);
}
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3002)
extern void platfrom_stub_set_num_aic(const char* num);
extern void platfrom_stub_set_num_aiv(const char* num);
extern void platfrom_stub_set_num_cub(const char* num);
extern void platfrom_stub_set_ctl(const char* num);
extern void platfrom_stub_set_chip_version(const char* num);
extern void platfrom_stub_set_num(uint32_t num);
TEST_F(TestTiling, TestPlatformAscendC)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    uint64_t ub_size, l1_size, l0;
    uint64_t l2_bw, hbm_bw, bw;
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ub_size);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L1, l1_size);
    EXPECT_EQ(ub_size, 248 * 1024);
    EXPECT_EQ(l1_size, 1024 * 1024);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L0_A, l0);
    EXPECT_EQ(l0, 65536);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L0_B, l0);
    EXPECT_EQ(l0, 65536);
    plat.GetCoreMemSize(platform_ascendc::CoreMemType::L0_C, l0);
    EXPECT_EQ(l0, 65536 * 2);
    plat.GetCoreMemBw(platform_ascendc::CoreMemType::L2, l2_bw);
    plat.GetCoreMemBw(platform_ascendc::CoreMemType::HBM, hbm_bw);
    EXPECT_EQ(l2_bw, 256);
    EXPECT_EQ(hbm_bw, 17);
    plat.GetCoreMemBw(platform_ascendc::CoreMemType::UB, bw);
    EXPECT_EQ(plat.GetVecRegLen(), 0);
    EXPECT_EQ(plat.GetCoreNum(), 1);
    EXPECT_EQ(plat.GetCoreNumAic(), 1);
    EXPECT_EQ(plat.GetCoreNumAiv(), 1);
    platfrom_stub_set_num_cub("1");
    EXPECT_EQ(plat.GetCoreNumAic(), 1);
    platfrom_stub_set_num_aiv("1");
    EXPECT_EQ(plat.GetCoreNumAiv(), 1);
    platfrom_stub_set_ctl("AICore");
    EXPECT_EQ(plat.GetCoreNumAic(), 1);
    EXPECT_EQ(plat.GetCoreNumAiv(), 1);
    platfrom_stub_set_num_aic("2");
    EXPECT_EQ(plat.GetCoreNumAic(), 2);
    EXPECT_EQ(plat.GetCoreNumAiv(), 2);
    EXPECT_EQ(bw, 0);
    EXPECT_EQ(plat.CalcTschBlockDim(1, 0, 1), 1);
    EXPECT_EQ(plat.CalcTschBlockDim(1, 1, 0), 1);
    EXPECT_EQ(plat.CalcTschBlockDim(2, 1, 1), 2);
    EXPECT_EQ(plat.CalcTschBlockDim(2, 2, 1), 2);
    EXPECT_EQ(plat.CalcTschBlockDim(2, 1, 2), 1);
    // invalid case, return 0
    EXPECT_EQ(plat.CalcTschBlockDim(3, 1, 2), 0);
    EXPECT_EQ(plat.CalcTschBlockDim(6, 1, 3), 2);
    EXPECT_EQ(plat.CalcTschBlockDim(38, 20, 40), 19);
    EXPECT_EQ(plat.CalcTschBlockDim(39, 20, 40), 20);
    EXPECT_EQ(plat.CalcTschBlockDim(40, 20, 40), 20);
    // invalid case, return 0
    EXPECT_EQ(plat.CalcTschBlockDim(41, 20, 40), 0);

    EXPECT_EQ(plat.GetLibApiWorkSpaceSize(), 2097152);
    EXPECT_EQ(plat.GetCoreNumVector(), 0);
}
#endif

TEST_F(TestTiling, TestKernelContextBuild)
{
    string active_type = "gelu";
    gert::Shape input1_shape = {2, 1, 1, 1, 1, 1, 1, 2, 2};
    int32_t input1_tensor_buffer[] = {0, 2, 3, 3, 1, 0, 0, 1};
    gert::TensorData input1_tensor_data{(void*)input1_tensor_buffer, nullptr};
    gert::Shape output_shape = {5, 3};
    int64_t output_tensor_buffer[15];
    gert::TensorData output_tensor_data{(void*)output_tensor_buffer, nullptr};
    std::vector<float> floatVecAttr(2, 1.f);
    std::vector<bool> boolVecAttr(2, true);
    std::vector<int64_t> intVecAttr(2, 1);
    std::vector<std::vector<int64_t>> intVecVecAttr{{1, 2}, {3, 4}};
    std::vector<string> stringVecAttr(2, active_type);
    context_ascendc::ContextBuilder kernelRunContextBuilder;
    auto kernelHolder =
        kernelRunContextBuilder
            .Inputs({reinterpret_cast<void*>(&input1_shape), reinterpret_cast<void*>(&input1_tensor_data)})
            .Outputs({reinterpret_cast<void*>(&output_shape), reinterpret_cast<void*>(&output_tensor_data)})
            .BuildKernelRunContext();
    auto context = kernelHolder->GetContext<gert::KernelContext>();
    EXPECT_NE(context, nullptr);
}

TEST_F(TestTiling, TestTilingContextBuildWithBinFile)
{
    string active_type = "gelu";
    gert::StorageShape x_shape = {{1024, 5120}, {1024, 5120}};
    gert::StorageShape expert_tokens_shape = {{16}, {16}};
    gert::StorageShape weight1_shape = {{16, 5120, 0}, {16, 5120, 0}};
    gert::StorageShape bias1_shape = {{16, 0}, {16, 0}};
    gert::StorageShape weight2_shape = {{16, 0, 5120}, {16, 0, 5120}};
    gert::StorageShape bias2_shape = {{16, 5120}, {16, 5120}};
    gert::StorageShape output_shape = {{1024, 5210}, {1024, 5210}};

    std::vector<int64_t> expert_tokens_const_value(16, 1);

    std::vector<float> x_const_value(1024 * 5120, 2.f);
    std::vector<float> bias2_value(16 * 5120, 3.f);
    auto param = gert::TilingData::CreateCap(4096);
    auto workspace_size_holer = gert::ContinuousVector::Create<size_t>(4096);
    auto ws_size = reinterpret_cast<gert::ContinuousVector*>(workspace_size_holer.get());
    auto holder =
        context_ascendc::ContextBuilder()
            .NodeIoNum(6, 1)
            .IrInstanceNum({1, 1, 1, 1, 1, 1})
            .SetOpNameType("tmpName", "tmpType")
            .AddInputTd(
                0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, x_shape, reinterpret_cast<void*>(x_const_value.data()))
            .AddInputTd(1, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, weight1_shape)
            .AddInputTd(2, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, weight2_shape)
            .AddInputTd(3, ge::DT_INT64, ge::FORMAT_ND, ge::FORMAT_ND, expert_tokens_shape, "./expert_tokens_data.bin")
            .AddInputTd(4, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, bias1_shape)
            .AddInputTd(
                5, ge::DT_BF16, ge::FORMAT_ND, ge::FORMAT_ND, bias2_shape, reinterpret_cast<void*>(bias2_value.data()))
            .AddOutputTd(0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, output_shape)
            .AddAttr("activation", active_type)
            .AddAttr("inner_precise", static_cast<int64_t>(1))
            .TilingData(param.get())
            .Workspace(ws_size)
            .BuildTilingContext();
    EXPECT_EQ(holder, nullptr);
    std::string longFileName(5000, 'F');
    auto holder1 =
        context_ascendc::ContextBuilder()
            .NodeIoNum(4, 1)
            .IrInstanceNum({1, 1, 1, 1})
            .SetOpNameType("tmpName", "tmpType")
            .AddInputTd(
                0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, x_shape, reinterpret_cast<void*>(x_const_value.data()))
            .AddInputTd(1, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, expert_tokens_shape, longFileName)
            .AddInputTd(2, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, expert_tokens_shape, "")
            .AddInputTd(3, ge::DT_INT64, ge::FORMAT_ND, ge::FORMAT_ND, expert_tokens_shape, "./expert_tokens_data.bin")
            .AddOutputTd(0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, output_shape)
            .AddAttr("activation", active_type)
            .AddAttr("inner_precise", static_cast<int64_t>(1))
            .TilingData(param.get())
            .Workspace(ws_size)
            .BuildTilingContext();
    EXPECT_EQ(holder1, nullptr);
}

TEST_F(TestTiling, TestTilingContextBuildWithConstValue)
{
    string active_type = "gelu";
    gert::StorageShape x_shape = {{1024, 5120}, {1024, 5120}};
    gert::StorageShape expert_tokens_shape = {{16}, {16}};
    gert::StorageShape weight1_shape = {{16, 5120, 1}, {16, 5120, 1}};
    gert::StorageShape bias1_shape = {{16, 1}, {16, 1}};
    gert::StorageShape weight2_shape = {{16, 1, 5120}, {16, 1, 5120}};
    gert::StorageShape bias2_shape = {{16, 5120}, {16, 5120}};
    gert::StorageShape output_shape = {{1024, 5210}, {1024, 5210}};

    std::vector<int64_t> expert_tokens_const_value(16, 1);
    std::vector<float> x_const_value(1024 * 5120, 2.f);
    std::vector<float> bias2_value(16 * 5120, 3.f);
    std::vector<float> floatVecAttr(2, 1.f);
    std::vector<bool> boolVecAttr(2, true);
    std::vector<int64_t> intVecAttr(2, 1);
    std::vector<std::vector<int64_t>> intVecVecAttr{{1, 2}, {3, 4}};
    std::vector<string> stringVecAttr(2, active_type);
    auto param = gert::TilingData::CreateCap(4096);
    auto workspace_size_holer = gert::ContinuousVector::Create<size_t>(4096);
    auto ws_size = reinterpret_cast<gert::ContinuousVector*>(workspace_size_holer.get());
    auto holder =
        context_ascendc::ContextBuilder()
            .NodeIoNum(6, 1)
            .IrInstanceNum({1, 1, 1, 1, 1, 1})
            .AddInputTd(
                0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, x_shape, reinterpret_cast<void*>(x_const_value.data()))
            .AddInputTd(1, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, weight1_shape)
            .AddInputTd(2, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, weight2_shape)
            .AddInputTd(
                3, ge::DT_INT64, ge::FORMAT_ND, ge::FORMAT_ND, expert_tokens_shape,
                reinterpret_cast<void*>(expert_tokens_const_value.data()))
            .AddInputTd(4, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, bias1_shape)
            .AddInputTd(
                5, ge::DT_BF16, ge::FORMAT_ND, ge::FORMAT_ND, bias2_shape, reinterpret_cast<void*>(bias2_value.data()))
            .AddOutputTd(0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, output_shape)
            .AddAttr("activation", active_type)
            .AddAttr("inner_precise", static_cast<int64_t>(1))
            .AddAttr("boolAttr", true)
            .AddAttr("floatAttr", 1.f)
            .AddAttr("floatVecAttr", floatVecAttr)
            .AddAttr("stringVecAttr", stringVecAttr)
            .AddAttr("intVecVecAttr", intVecVecAttr)
            .TilingData(param.get())
            .Workspace(ws_size)
            .AddPlatformInfo("Ascend910B")
            .BuildTilingContext();
    EXPECT_NE(holder, nullptr);
    gert::TilingContext* tilingContext = holder->GetContext<gert::TilingContext>();
    EXPECT_EQ(*tilingContext->GetInputShape(1), weight1_shape);
    EXPECT_EQ(*tilingContext->GetOutputShape(0), output_shape);
    auto info_input_0 = tilingContext->GetInputDesc(0);
    EXPECT_NE(info_input_0, nullptr);
    EXPECT_EQ(info_input_0->GetDataType(), ge::DT_FLOAT16);
    auto info_output_0 = tilingContext->GetOutputDesc(0);
    EXPECT_NE(info_output_0, nullptr);
    EXPECT_EQ(info_output_0->GetDataType(), ge::DT_FLOAT16);
}

TEST_F(TestTiling, TestTilingContextBuildWithConstValue350)
{
    string active_type = "gelu";
    gert::StorageShape x_shape = {{1024, 5120}, {1024, 5120}};
    gert::StorageShape expert_tokens_shape = {{16}, {16}};
    gert::StorageShape weight1_shape = {{16, 5120, 1}, {16, 5120, 1}};
    gert::StorageShape bias1_shape = {{16, 1}, {16, 1}};
    gert::StorageShape weight2_shape = {{16, 1, 5120}, {16, 1, 5120}};
    gert::StorageShape bias2_shape = {{16, 5120}, {16, 5120}};
    gert::StorageShape output_shape = {{1024, 5210}, {1024, 5210}};

    std::vector<int64_t> expert_tokens_const_value(16, 1);
    std::vector<float> x_const_value(1024 * 5120, 2.f);
    std::vector<float> bias2_value(16 * 5120, 3.f);
    std::vector<float> floatVecAttr(2, 1.f);
    std::vector<bool> boolVecAttr(2, true);
    std::vector<int64_t> intVecAttr(2, 1);
    std::vector<std::vector<int64_t>> intVecVecAttr{{1, 2}, {3, 4}};
    std::vector<string> stringVecAttr(2, active_type);
    auto param = gert::TilingData::CreateCap(4096);
    auto workspace_size_holer = gert::ContinuousVector::Create<size_t>(4096);
    auto ws_size = reinterpret_cast<gert::ContinuousVector*>(workspace_size_holer.get());
    auto holder =
        context_ascendc::ContextBuilder()
            .NodeIoNum(6, 1)
            .IrInstanceNum({1, 1, 1, 1, 1, 1})
            .AddInputTd(
                0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, x_shape, reinterpret_cast<void*>(x_const_value.data()))
            .AddInputTd(1, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, weight1_shape)
            .AddInputTd(2, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, weight2_shape)
            .AddInputTd(
                3, ge::DT_INT64, ge::FORMAT_ND, ge::FORMAT_ND, expert_tokens_shape,
                reinterpret_cast<void*>(expert_tokens_const_value.data()))
            .AddInputTd(4, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, bias1_shape)
            .AddInputTd(
                5, ge::DT_BF16, ge::FORMAT_ND, ge::FORMAT_ND, bias2_shape, reinterpret_cast<void*>(bias2_value.data()))
            .AddOutputTd(0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, output_shape)
            .AddAttr("activation", active_type)
            .AddAttr("inner_precise", static_cast<int64_t>(1))
            .AddAttr("boolAttr", true)
            .AddAttr("floatAttr", 1.f)
            .AddAttr("floatVecAttr", floatVecAttr)
            .AddAttr("stringVecAttr", stringVecAttr)
            .AddAttr("intVecVecAttr", intVecVecAttr)
            .TilingData(param.get())
            .Workspace(ws_size)
            .AddPlatformInfo("Ascend350")
            .BuildTilingContext();
    EXPECT_NE(holder, nullptr);
    gert::TilingContext* tilingContext = holder->GetContext<gert::TilingContext>();
    EXPECT_EQ(*tilingContext->GetInputShape(1), weight1_shape);
    EXPECT_EQ(*tilingContext->GetOutputShape(0), output_shape);
    auto info_input_0 = tilingContext->GetInputDesc(0);
    EXPECT_NE(info_input_0, nullptr);
    EXPECT_EQ(info_input_0->GetDataType(), ge::DT_FLOAT16);
    auto info_output_0 = tilingContext->GetOutputDesc(0);
    EXPECT_NE(info_output_0, nullptr);
    EXPECT_EQ(info_output_0->GetDataType(), ge::DT_FLOAT16);
}

TEST_F(TestTiling, TestTilingContextBuildFailed)
{
    string active_type = "gelu";
    gert::StorageShape x_shape = {{-1, 5120}, {-1, 5120}};
    std::vector<float> x_const_value(1024 * 5120, 2.f);
    auto param = gert::TilingData::CreateCap(4096);
    auto workspace_size_holer = gert::ContinuousVector::Create<size_t>(4096);
    auto ws_size = reinterpret_cast<gert::ContinuousVector*>(workspace_size_holer.get());
    auto holder =
        context_ascendc::ContextBuilder()
            .NodeIoNum(1, 1)
            .IrInstanceNum({1, 1})
            .CompileInfo(nullptr)
            .PlatformInfo(nullptr)
            .AddInputTd(
                0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, x_shape, reinterpret_cast<void*>(x_const_value.data()))
            .Workspace(ws_size)
            .BuildTilingContext();
    EXPECT_EQ(holder, nullptr);
}

void* DlopenStub(const char* name, int flag)
{
    static int data = 10;
    return (void*)(&data);
}

void* DlopenStub1(const char* name, int flag) { return (void*)(nullptr); }

void GetSocVersionStub(char*, const uint32_t) {}

void* DlsymStub(void* handle, const char* symbol)
{
    void (*rtGetSocVersion)(char*, const uint32_t);
    ;
    rtGetSocVersion = GetSocVersionStub;
    return (void*)(rtGetSocVersion);
}

void* DlsymStub1(void* handle, const char* symbol) { return (void*)(nullptr); }

void TbeLoadSoAndSaveToRegistryStub(const char*) {}

void* DlsymStub2(void* handle, const char* symbol)
{
    void (*registryFunc)(const char*);
    registryFunc = TbeLoadSoAndSaveToRegistryStub;
    return (void*)(registryFunc);
}

errno_t StrcpyStub(char* strDest, size_t destMax, const char* strSrc) { return 1; }

char* RealpathStub(const char* path, char* resolved_path)
{
    static char* tmp = "test";
    return tmp;
}

TEST_F(TestTiling, TestOpTilingRegistryLoadTilingLibrary)
{
    context_ascendc::OpTilingRegistry tilingRegObject;
    tilingRegObject.LoadTilingLibrary(nullptr);
    tilingRegObject.LoadTilingLibrary("testsopath");
    std::string tmpStr = "";
    constexpr int32_t pathMax = 256;
    for (int i = 0; i < pathMax + 1; i++) {
        tmpStr += "1";
    }
    tilingRegObject.LoadTilingLibrary(tmpStr.c_str());
    MOCKER(realpath, char* (*)(const char*, char*)).stubs().will(invoke(RealpathStub));
    MOCKER(dlopen, void* (*)(const char*, int)).stubs().will(invoke(DlopenStub1));
    tilingRegObject.LoadTilingLibrary("testsopath");
    GlobalMockObject::verify();

    MOCKER(realpath, char* (*)(const char*, char*)).stubs().will(invoke(RealpathStub));
    MOCKER(dlsym, void* (*)(void*, const char*)).stubs().will(invoke(DlsymStub1));
    tilingRegObject.LoadTilingLibrary("testsopath");
    GlobalMockObject::verify();

    MOCKER(realpath, char* (*)(const char*, char*)).stubs().will(invoke(RealpathStub));
    MOCKER(dlsym, void* (*)(void*, const char*)).stubs().will(invoke(DlsymStub2));
    tilingRegObject.LoadTilingLibrary("testsopath");
    GlobalMockObject::verify();
    EXPECT_EQ(tmpStr.size(), pathMax + 1);
}

// stub for AnyValue
namespace ge {
class AnyValue {};
} // namespace ge
namespace gert {
static DefaultOpImplSpaceRegistryV2& GetInstanceStubDefaultImpl()
{
    static DefaultOpImplSpaceRegistryV2 instance;
    return instance;
}

std::shared_ptr<OpImplSpaceRegistryV2> GetDefaultSpaceRegistryStub(
    DefaultOpImplSpaceRegistryV2* thisIns, gert::OppImplVersionTag opp_impl_version = gert::OppImplVersionTag::kOpp)
{
    static std::shared_ptr<OpImplSpaceRegistryV2> tmp = nullptr;
    return tmp;
}

std::shared_ptr<OpImplSpaceRegistryV2> GetDefaultSpaceRegistryStub1(
    DefaultOpImplSpaceRegistryV2* thisIns, gert::OppImplVersionTag opp_impl_version = gert::OppImplVersionTag::kOpp)
{
    static std::shared_ptr<OpImplSpaceRegistryV2> tmp(new gert::OpImplSpaceRegistryV2);
    return tmp;
}

const OpImplKernelRegistry::OpImplFunctionsV2* GetOpImplStubForOpImplKernelRegistry2(
    OpImplSpaceRegistryV2* thisIns, const char* opType)
{
    static OpImplKernelRegistry::OpImplFunctionsV2 tmp;
    static int data0 = 1;
    static int data1 = 1;
    static int data2 = 1;
    tmp.tiling = (OpImplKernelRegistry::TilingKernelFunc)(&data0);
    tmp.tiling_parse = (OpImplKernelRegistry::KernelFunc)(&data1);
    return &tmp;
}
} // namespace gert

TEST_F(TestTiling, TestOpTilingRegistryGetTilingFunc)
{
    context_ascendc::OpTilingRegistry tilingRegObject;
    MOCKER_CPP(&gert::DefaultOpImplSpaceRegistryV2::GetSpaceRegistry)
        .stubs()
        .will(invoke(gert::GetDefaultSpaceRegistryStub));

    context_ascendc::TilingFunc funcPtr = tilingRegObject.GetTilingFunc("AddCustom");
    EXPECT_EQ(funcPtr, nullptr);
    GlobalMockObject::verify();

    MOCKER_CPP(&gert::DefaultOpImplSpaceRegistryV2::GetSpaceRegistry)
        .stubs()
        .will(invoke(gert::GetDefaultSpaceRegistryStub1));
    context_ascendc::TilingFunc funcPtr1 = tilingRegObject.GetTilingFunc("AddCustom");
    EXPECT_EQ(funcPtr1, nullptr);
    GlobalMockObject::verify();

    MOCKER_CPP(&gert::DefaultOpImplSpaceRegistryV2::GetSpaceRegistry)
        .stubs()
        .will(invoke(gert::GetDefaultSpaceRegistryStub1));
    MOCKER_CPP(&gert::OpImplSpaceRegistryV2::GetOpImpl)
        .stubs()
        .will(invoke(gert::GetOpImplStubForOpImplKernelRegistry2));
    context_ascendc::TilingFunc funcPtr2 = tilingRegObject.GetTilingFunc("AddCustom");
    EXPECT_NE(funcPtr2, nullptr);
    GlobalMockObject::verify();
}

TEST_F(TestTiling, TestTilingContextIoFailed)
{
    string active_type = "gelu";
    gert::StorageShape x_shape = {{-1, 5120}, {-1, 5120}};
    std::vector<float> x_const_value(1024 * 5120, 2.f);
    auto param = gert::TilingData::CreateCap(4096);
    auto workspace_size_holer = gert::ContinuousVector::Create<size_t>(4096);
    auto ws_size = reinterpret_cast<gert::ContinuousVector*>(workspace_size_holer.get());
    auto holder =
        context_ascendc::ContextBuilder()
            .AddInputTd(
                0, ge::DT_FLOAT16, ge::FORMAT_ND, ge::FORMAT_ND, x_shape, reinterpret_cast<void*>(x_const_value.data()))
            .Workspace(ws_size)
            .BuildTilingContext();
    EXPECT_EQ(holder, nullptr);
}

TEST_F(TestTiling, TestGetSocVersion)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .will(returnValue(false));

    platform_ascendc::SocVersion ret = plat.GetSocVersion();
    EXPECT_EQ(ret, platform_ascendc::SocVersion::RESERVED_VERSION);
}

TEST_F(TestTiling, TestGetNpuArchFailed)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .will(returnValue(false));

    auto ret = plat.GetCurNpuArch();
    EXPECT_EQ(ret, NpuArch::DAV_RESV);
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
TEST_F(TestTiling, TestGetNpuArchInvalidValue)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    platfrom_stub_set_npuarch("ABC");
    EXPECT_EQ(plat.GetCurNpuArch(), NpuArch::DAV_RESV);
    platfrom_stub_set_npuarch("2201");
}

TEST_F(TestTiling, TestGetNpuArchNeg)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    platfrom_stub_set_npuarch("-2");
    EXPECT_EQ(plat.GetCurNpuArch(), NpuArch::DAV_RESV);
    platfrom_stub_set_npuarch("2201");
}
#endif

TEST_F(TestTiling, TestCoreNum)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .will(returnValue(false));

    uint32_t ret1 = plat.GetCoreNumAic();
    uint32_t ret2 = plat.GetCoreNumAiv();
    EXPECT_EQ(ret1, 0);
    EXPECT_EQ(ret2, 0);
}

TEST_F(TestTiling, TestGetLibApiWorkSpaceSize)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .will(returnValue(false));

    uint32_t ret1 = plat.GetLibApiWorkSpaceSize();
    EXPECT_EQ(ret1, static_cast<uint32_t>(-1));
}
TEST_F(TestTiling, TestPlatformAscendCManager)
{
    void* handle;
    int a = 7;
    handle = &a;

    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .will(returnValue(false));

    auto ret2 = platform_ascendc::PlatformAscendCManager::GetInstance();
    EXPECT_TRUE(ret2 == nullptr);
}

TEST_F(TestTiling, TestGetVectorCoreNum)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .will(returnValue(false));
    MOCKER_CPP(
        &platform_ascendc::PlatformAscendC::GetSocVersion,
        platform_ascendc::SocVersion(platform_ascendc::PlatformAscendC::*)(void) const)
        .stubs()
        .will(returnValue(platform_ascendc::SocVersion::ASCEND310P));

    uint32_t ret1 = plat.GetCoreNumVector();
    EXPECT_EQ(ret1, static_cast<uint32_t>(0));
}

TEST_F(TestTiling, TestGetVecRegLen)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .will(returnValue(false));
    uint32_t ret = plat.GetVecRegLen();
    EXPECT_EQ(ret, static_cast<uint32_t>(0));
}

TEST_F(TestTiling, TestGetVecRegLenWhenSizeExceed)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    std::string largeValue = "5000000000";
    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .with(outBound(std::string("AICoreSpec")), outBound(std::string("vector_reg_width")), outBound(largeValue))
        .will(returnValue(true));

    uint32_t ret = plat.GetVecRegLen();
    EXPECT_EQ(ret, static_cast<uint32_t>(0));
}

TEST_F(TestTiling, TestGetVecRegLenWhenSizeStrIllegal)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    std::string largeValue = "25x";
    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .with(outBound(std::string("AICoreSpec")), outBound(std::string("vector_reg_width")), outBound(largeValue))
        .will(returnValue(true));

    uint32_t ret = plat.GetVecRegLen();
    EXPECT_EQ(ret, static_cast<uint32_t>(0));
}

TEST_F(TestTiling, TestGetVecRegLenNormal)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);

    std::string largeValue = "256";
    MOCKER_CPP(
        &fe::PlatFormInfos::GetPlatformResWithLock,
        bool(fe::PlatFormInfos::*)(const std::string&, const std::string&, std::string&))
        .stubs()
        .with(outBound(std::string("AICoreSpec")), outBound(std::string("vector_reg_width")), outBound(largeValue))
        .will(returnValue(true));

    uint32_t ret = plat.GetVecRegLen();
    EXPECT_EQ(ret, static_cast<uint32_t>(256));
}

TEST_F(TestTiling, TestPlatformCustomSocVersion)
{
    auto nested = [](char* version, const uint32_t maxLen) {};
    void (*rtGetSocVersion)(char* version, const uint32_t maxLen);
    rtGetSocVersion = nested;

    void* handle;
    int dummy = 0;
    handle = &dummy;
    MOCKER_CPP(&dlopen).stubs().will(returnValue(handle));
    MOCKER_CPP(&dlsym).stubs().will(returnValue((void*)nullptr)).then(returnValue((void*)rtGetSocVersion));
    MOCKER_CPP(&dlclose).stubs().will(returnValue(0));
    auto ret1 = platform_ascendc::PlatformAscendCManager::PlatformAscendCInit(nullptr);
    EXPECT_EQ(nullptr, ret1);

    auto ret2 = platform_ascendc::PlatformAscendCManager::PlatformAscendCInit(nullptr);
    EXPECT_NE(nullptr, ret2);

    auto ret3 = platform_ascendc::PlatformAscendCManager::PlatformAscendCInit("Ascend910B1");
    EXPECT_NE(nullptr, ret3);

    // PlatformAscendCManager::GetInstance() caches its outcome in a process-lifetime static that is
    // pinned by the first call in the whole process, so its result would otherwise depend on which
    // GetInstance-using test runs first. Force this first initialization to fail (return nullptr) so
    // the singleton is deterministically pinned to nullptr regardless of test execution order.
    MOCKER_CPP(&platform_ascendc::PlatformAscendCManager::PlatformAscendCInit)
        .stubs()
        .will(returnValue(static_cast<fe::PlatFormInfos*>(nullptr)));
    auto ret4 = platform_ascendc::PlatformAscendCManager::GetInstance("Ascend910B1");
    EXPECT_EQ(nullptr, ret4);
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
extern void platfrom_stub_set_chip_version(const char* num);
TEST_F(TestTiling, TestPlatformAscendCMC32DM11A)
{
    fe::PlatFormInfos platform_info;
    auto plat = platform_ascendc::PlatformAscendC(&platform_info);
    platfrom_stub_set_chip_version("MC32DM11A");
    EXPECT_EQ(plat.GetSocVersion(), platform_ascendc::SocVersion::MC32DM11A);
}
#endif

TEST_F(TestTiling, TestPlatformAscendCInitWithRuntimePlatformInfos)
{
    const char* customSocVersion = "Ascend910B1";

    MOCKER_CPP(
        &fe::PlatformInfoManager::InitRuntimePlatformInfos, uint32_t(fe::PlatformInfoManager::*)(const std::string&))
        .stubs()
        .will(returnValue(static_cast<uint32_t>(0)));

    MOCKER_CPP(
        &fe::PlatformInfoManager::GetRuntimePlatformInfosByDevice,
        uint32_t(fe::PlatformInfoManager::*)(const uint32_t&, fe::PlatFormInfos&, bool))
        .stubs()
        .will(returnValue(static_cast<uint32_t>(0)));

    auto ret = platform_ascendc::PlatformAscendCManager::PlatformAscendCInit(customSocVersion);
    EXPECT_NE(nullptr, ret);

    GlobalMockObject::verify();
}
