/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#define ASCENDC_DUMP 1
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <vector>
#include "kernel_operator.h"
#include "utils/debug/asc_dump.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510) && !defined(ASCENDC_CPU_DEBUG)
#include "impl/utils/debug/npu_arch_3510/asc_aicore_dump_utils.h"
#define ASCENDC_TEST_HAS_SIMD_VF_DUMP_POSITION 1
#endif

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
__aicore__ inline uint32_t asc_debug_get_core_idx()
{
    if ASCEND_IS_AIV {
        return AscendC::GetBlockIdxImpl();
    } else {
        return AscendC::GetBlockIdxImpl() + AscendC::AIV_CORE_NUM;
    }
}
#endif

// #include "api_check/kernel_cpu_check.h"

using namespace AscendC;

int32_t RaiseStubForDumpTensor(int32_t input);

#if defined(ASCENDC_TEST_HAS_SIMD_VF_DUMP_POSITION)
namespace {
constexpr uint16_t TEST_SIMD_VF_BLOCK_IDX = 3;
constexpr uint32_t TEST_SIMD_VF_DUMP_DESC = 630;
constexpr uint32_t TEST_SIMD_VF_DUMP_SIZE = 1;

__asc_simd_vf::DumpTensorTlv* GetSimdVfDumpTlv(BlockVFBufInfo& blockInfo)
{
    __asc_simd_vf::get_printf_ubuf_addr(reinterpret_cast<uint64_t>(&blockInfo), TEST_SIMD_VF_BLOCK_IDX);
    return reinterpret_cast<__asc_simd_vf::DumpTensorTlv*>(blockInfo.buffer);
}
} // namespace
#endif

struct TestDumpTensorParams {
    uint32_t dataSize;
    uint32_t typeSize;
    uint32_t dstSize;
    uint64_t dumpSize;
    void (*CalFunc)(uint8_t*, uint8_t*, uint8_t*, uint32_t, uint32_t, uint64_t);
};

class TestDumpTensorSuite : public testing::Test, public testing::WithParamInterface<TestDumpTensorParams> {
protected:
    void SetUp() { MOCKER(raise, int32_t(*)(int32_t)).stubs().will(invoke(RaiseStubForDumpTensor)); }
    void TearDown() { GlobalMockObject::verify(); }
};

class DumpTensorRuntimeGuard {
public:
    DumpTensorRuntimeGuard()
        : coreType_(g_coreType),
          blockNum_(block_num),
          blockIdx_(block_idx),
          subBlockIdx_(sub_block_idx),
          taskRation_(g_taskRation),
          dumpWorkspace_(AscendC::g_dumpWorkspaceReserved),
          sysWorkspace_(GetSysWorkSpacePtr())
    {}

    ~DumpTensorRuntimeGuard()
    {
        SetGCoreType(coreType_);
        block_num = blockNum_;
        block_idx = blockIdx_;
        sub_block_idx = subBlockIdx_;
        g_taskRation = taskRation_;
        AscendC::g_dumpWorkspaceReserved = dumpWorkspace_;
        SetSysWorkSpacePtr(sysWorkspace_);
    }

private:
    int32_t coreType_;
    decltype(block_num) blockNum_;
    decltype(block_idx) blockIdx_;
    decltype(sub_block_idx) subBlockIdx_;
    decltype(g_taskRation) taskRation_;
    __gm__ uint8_t* dumpWorkspace_;
    __gm__ uint8_t* sysWorkspace_;
};

void SetAivDumpEnv(uint32_t blockNumValue, uint32_t blockIdxValue, uint32_t subBlockIdxValue, uint32_t taskRationValue)
{
    SetGCoreType(2);
    block_num = blockNumValue;
    block_idx = blockIdxValue;
    sub_block_idx = subBlockIdxValue;
    g_taskRation = taskRationValue;
}

template <typename T>
void DataDumpTensor(
    __gm__ uint8_t* dstGm, __gm__ uint8_t* srcGm, __gm__ uint8_t* workGm, __gm__ uint32_t dataSize,
    __gm__ uint32_t dstSize, __gm__ uint64_t dumpSize)
{
    TPipe tpipe;
    uint32_t desc = 630;
    GlobalTensor<T> srcGlobal;
    GlobalTensor<T> dstGlobal;
    GlobalTensor<T> workGlobal;

    srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(srcGm), dataSize);
    dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dstSize);
    workGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(workGm), DUMP_WORKSPACE_SIZE * 2);

    TBuf<TPosition::VECCALC> tbuf;
    tpipe.InitBuffer(tbuf, dataSize * sizeof(T));
    LocalTensor<T> inputLocal = tbuf.Get<T>();
    DataCopyParams copyParams{1, dataSize * sizeof(T) / 32, 0, 0};
    DataCopy(inputLocal, srcGlobal, copyParams);
    pipe_barrier(PIPE_ALL);

    InitDump(false, workGm, dumpSize);
}

INSTANTIATE_TEST_CASE_P(
    TEST_DUMP_TENSOR, TestDumpTensorSuite,
    ::testing::Values(TestDumpTensorParams{64, 4, 96, DUMP_UINTSIZE, DataDumpTensor<uint32_t>}));

int32_t RaiseStubForDumpTensor(int32_t input) { return 0; }

TEST_P(TestDumpTensorSuite, TestDumpTensorCases)
{
    auto param = GetParam();
    int32_t coreTypeTmp = g_coreType;
    auto blockNumTmp = block_num;
    auto blockIdxTmp = block_idx;
    auto subBlockIdxTmp = sub_block_idx;
    auto taskRationTmp = g_taskRation;
    SetGCoreType(2);
    block_num = 1;
    block_idx = 0;
    sub_block_idx = 0;
    g_taskRation = 1;

    uint8_t srcGm[param.dataSize * param.typeSize];
    uint8_t dstGm[param.dstSize * param.typeSize];
    std::vector<uint8_t> workGm(param.dumpSize, 0);

    param.CalFunc(dstGm, srcGm, workGm.data(), param.dataSize, param.dstSize, param.dumpSize);

    SetGCoreType(coreTypeTmp);
    block_num = blockNumTmp;
    block_idx = blockIdxTmp;
    sub_block_idx = subBlockIdxTmp;
    g_taskRation = taskRationTmp;
}

TEST_F(TestDumpTensorSuite, InitDumpImplNullWorkspaceReturns)
{
    DumpTensorRuntimeGuard guard;
    SetAivDumpEnv(1, 0, 0, 1);

    AscendC::g_dumpWorkspaceReserved = nullptr;
    InitDumpImpl(false, DUMP_UINTSIZE);

    EXPECT_EQ(AscendC::g_dumpWorkspaceReserved, nullptr);
}

TEST_F(TestDumpTensorSuite, InitDumpImplSkipsOutOfRangeCore)
{
    DumpTensorRuntimeGuard guard;
    SetAivDumpEnv(1, 54, 0, 2);

    std::vector<uint8_t> workGm(DUMP_UINTSIZE, 0);
    AscendC::g_dumpWorkspaceReserved = workGm.data();
    EXPECT_EQ(asc_debug_get_core_idx(), DUMP_CORE_COUNT);

    InitDumpImpl(false, DUMP_UINTSIZE);

    auto* blockInfo = reinterpret_cast<uint32_t*>(workGm.data());
    EXPECT_EQ(blockInfo[BLOCK_INFO_MAGIC_POS], 0U);
}

TEST_F(TestDumpTensorSuite, InitDumpNullStartAddrReturns)
{
    DumpTensorRuntimeGuard guard;
    SetAivDumpEnv(1, 0, 0, 1);

    InitDump(false, static_cast<__gm__ uint8_t*>(nullptr), DUMP_UINTSIZE);

    EXPECT_EQ(AscendC::g_dumpWorkspaceReserved, nullptr);
}

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
TEST_F(TestDumpTensorSuite, SimdVfDumpCpuFallbacks)
{
    constexpr uint32_t desc = 630;
    constexpr uint32_t dumpSize = 1;
    uint32_t ubInput[1] = {0};
    vector_u32 regInput = {};

#ifndef NDEBUG
    EXPECT_DEATH(asc_dump_ubuf<uint32_t>(ubInput, desc, dumpSize), "asc_dump_ubuf is not supported in cpu mode");
    EXPECT_DEATH(
        __asc_simd_vf::asc_dump_ubuf<uint32_t>(ubInput, desc, dumpSize), "asc_dump_ubuf is not supported in cpu mode");
    EXPECT_DEATH(asc_dump_reg<uint32_t>(regInput, desc, dumpSize), "asc_dump_reg is not supported in cpu mode");
    EXPECT_DEATH(
        __asc_simd_vf::asc_dump_reg<uint32_t>(regInput, desc, dumpSize), "asc_dump_reg is not supported in cpu mode");
    EXPECT_DEATH(asc_dump<uint32_t>(ubInput, desc, dumpSize), "asc_dump is not supported in cpu mode");
    EXPECT_DEATH(__asc_simd_vf::asc_dump<uint32_t>(ubInput, desc, dumpSize), "asc_dump is not supported in cpu mode");
    EXPECT_DEATH(asc_dump<uint32_t>(regInput, desc, dumpSize), "asc_dump is not supported in cpu mode");
    EXPECT_DEATH(__asc_simd_vf::asc_dump<uint32_t>(regInput, desc, dumpSize), "asc_dump is not supported in cpu mode");
#else
    asc_dump_ubuf<uint32_t>(ubInput, desc, dumpSize);
    __asc_simd_vf::asc_dump_ubuf<uint32_t>(ubInput, desc, dumpSize);
    asc_dump_reg<uint32_t>(regInput, desc, dumpSize);
    __asc_simd_vf::asc_dump_reg<uint32_t>(regInput, desc, dumpSize);
    asc_dump<uint32_t>(ubInput, desc, dumpSize);
    __asc_simd_vf::asc_dump<uint32_t>(ubInput, desc, dumpSize);
    asc_dump<uint32_t>(regInput, desc, dumpSize);
    __asc_simd_vf::asc_dump<uint32_t>(regInput, desc, dumpSize);
#endif
}

TEST_F(TestDumpTensorSuite, AicoreDumpCpuFallbacksRemainAvailable)
{
    uint32_t input[1] = {0};

#ifndef NDEBUG
    EXPECT_DEATH(__asc_aicore::asc_dump_ubuf<uint32_t>(input, 0, 1), "asc_dump_ubuf is not supported in cpu mode");
    EXPECT_DEATH(__asc_aicore::asc_dump<uint32_t>(input, 0, 1), "asc_dump is not supported in cpu mode");
    EXPECT_DEATH(asc_dump_gm<uint32_t>(input, 0, 1), "asc_dump_gm is not supported in cpu mode");
#else
    __asc_aicore::asc_dump_ubuf<uint32_t>(input, 0, 1);
    __asc_aicore::asc_dump<uint32_t>(input, 0, 1);
    asc_dump_gm<uint32_t>(input, 0, 1);
#endif
}
#endif

#if defined(ASCENDC_TEST_HAS_SIMD_VF_DUMP_POSITION)
TEST_F(TestDumpTensorSuite, AscDumpUbufWritesUbPosition)
{
    BlockVFBufInfo blockInfo;
    auto* dumpTlv = GetSimdVfDumpTlv(blockInfo);
    uint32_t input[TEST_SIMD_VF_DUMP_SIZE] = {0};

    __asc_simd_vf::asc_dump_ubuf<uint32_t>(
        reinterpret_cast<__ubuf__ uint32_t*>(input), TEST_SIMD_VF_DUMP_DESC, TEST_SIMD_VF_DUMP_SIZE);

    EXPECT_EQ(dumpTlv->position, static_cast<uint16_t>(__asc_simd_vf::DumpTensorPosition::UB));
    EXPECT_EQ(dumpTlv->desc, TEST_SIMD_VF_DUMP_DESC);
    EXPECT_EQ(dumpTlv->blockIdx, TEST_SIMD_VF_BLOCK_IDX);
}

TEST_F(TestDumpTensorSuite, AscDumpRegWritesRegPosition)
{
    BlockVFBufInfo blockInfo;
    auto* dumpTlv = GetSimdVfDumpTlv(blockInfo);
    vector_u32 input = {};

    __asc_simd_vf::asc_dump_reg<uint32_t>(input, TEST_SIMD_VF_DUMP_DESC, TEST_SIMD_VF_DUMP_SIZE);

    EXPECT_EQ(dumpTlv->position, static_cast<uint16_t>(__asc_simd_vf::DumpTensorPosition::REG));
    EXPECT_EQ(dumpTlv->desc, TEST_SIMD_VF_DUMP_DESC);
    EXPECT_EQ(dumpTlv->blockIdx, TEST_SIMD_VF_BLOCK_IDX);
}
#endif
