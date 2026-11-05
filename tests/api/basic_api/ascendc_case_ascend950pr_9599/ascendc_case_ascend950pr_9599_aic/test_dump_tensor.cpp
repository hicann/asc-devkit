/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <array>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "stub_fun.h"
#include "impl/utils/sys_macros_impl.h"
#include "impl/utils/common_types.h"

// The CPU printf implementation and the device debug types both declare a
// DumpType enum. Rename the former so both can be included here.
#define DumpType CpuPrintfDumpType
#include "impl/utils/debug/asc_aicore_printf_impl.h"
#undef DumpType
struct float4_e2m1x2_t;
namespace __cce_scalar {
using ::copy_ubuf_to_ubuf;
}
using namespace __asc_aicore;
#pragma push_macro("ASCENDC_CPU_DEBUG")
#undef ASCENDC_CPU_DEBUG
#include "impl/utils/debug/asc_debug_utils.h"
#pragma pop_macro("ASCENDC_CPU_DEBUG")
#include "kernel_tensor.h"

// CPU address spaces collapse to the same pointer type, so the public asc_dump
// overloads stop differing from one another. Rename those unused overloads to
// keep the header includable; the sender and payload helpers stay unchanged.
#define DUMP_TEST_JOIN_IMPL(a, b) a##b
#define DUMP_TEST_JOIN(a, b) DUMP_TEST_JOIN_IMPL(a, b)
#define asc_dump DUMP_TEST_JOIN(UnusedDumpOverload, __COUNTER__)
#pragma push_macro("ASCENDC_CPU_DEBUG")
#undef ASCENDC_CPU_DEBUG
#include "impl/utils/debug/asc_aicore_dump_impl.h"
#pragma pop_macro("ASCENDC_CPU_DEBUG")
#undef asc_dump
#undef DUMP_TEST_JOIN
#undef DUMP_TEST_JOIN_IMPL

namespace {
using namespace __asc_aicore;
using AscendC::Hardware;

uint32_t copiedBytes;
uint32_t* copiedDestination;
uint32_t* copiedSource;
uint32_t RecordPayload(uint32_t* dst, uint32_t* src, const uint32_t& bytes)
{
    copiedBytes = bytes;
    copiedDestination = dst;
    copiedSource = src;
    return 0U;
}

struct DumpPayloadCase {
    uint32_t count;
    uint32_t payloadBytes;
};

class TestDumpTensorPayload : public testing::TestWithParam<DumpPayloadCase> {
protected:
    void SetUp() override
    {
        copiedBytes = 0U;
        previousCore = g_coreType;
        g_coreType = AscendC::AIC;
        fifo.head.ringBufLen = fifo.data.size();
        fifo.head.ringBufAddr = reinterpret_cast<uint64_t>(fifo.data.data());
        MOCKER(get_block_info).stubs().will(returnValue(&fifo.head));
    }
    void TearDown() override
    {
        GlobalMockObject::verify();
        g_coreType = previousCore;
    }
    struct Fifo {
        DebugBlockHeadInfo head{};
        DebugBlockReadInfo reader{};
        alignas(8) std::array<uint8_t, 4096U> data{};
        DebugBlockWriteInfo writer{};
    } fifo;
    alignas(32) std::array<uint32_t, 512U> source{};
    int32_t previousCore{};
};

TEST_P(TestDumpTensorPayload, OrdinaryL0CReservesTheWholeCopy)
{
    const auto test = GetParam();
    // Check the byte length handed to the architecture implementation.
    MOCKER(mem_copy_cbuf_to_gm_impl<uint32_t>).expects(once()).will(invoke(RecordPayload));
    MOCKER(sync_all).stubs();
    asc_dump_impl<Hardware::L0C, uint32_t>(source.data(), 17U, test.count, nullptr, 0U);

    const auto& tlv = *reinterpret_cast<const DumpTensorTlv*>(fifo.data.data());
    EXPECT_EQ(copiedBytes, test.payloadBytes);
    EXPECT_EQ(copiedDestination, reinterpret_cast<const uint32_t*>(&tlv + 1));
    EXPECT_EQ(copiedSource, source.data());
    EXPECT_EQ(tlv.type, static_cast<uint32_t>(DumpType::DUMP_TENSOR));
    EXPECT_EQ(tlv.dumpSize, test.count * sizeof(uint32_t));
    EXPECT_EQ(tlv.length + 8U, sizeof(tlv) + test.payloadBytes);
    EXPECT_EQ(fifo.writer.bufOffset, sizeof(tlv) + test.payloadBytes);
    EXPECT_EQ(fifo.writer.packIdx, 1U);
}

INSTANTIATE_TEST_SUITE_P(
    L0CBoundaries, TestDumpTensorPayload,
    testing::Values(DumpPayloadCase{8U, 1024U}, DumpPayloadCase{256U, 1024U}, DumpPayloadCase{264U, 2048U}));

TEST_F(TestDumpTensorPayload, GmKeeps32ByteReservationAndActualLength)
{
    MOCKER(sync_all).stubs();
    asc_dump_impl<Hardware::GM, uint32_t>(source.data(), 17U, 1U, nullptr, 0U);
    const auto& tlv = *reinterpret_cast<const DumpTensorTlv*>(fifo.data.data());
    EXPECT_EQ(tlv.dumpSize, sizeof(uint32_t));
    EXPECT_EQ(tlv.length + 8U, sizeof(tlv) + 32U);
    EXPECT_EQ(fifo.writer.bufOffset, sizeof(tlv) + 32U);
}

TEST_F(TestDumpTensorPayload, SuperTensorCapacityUsesTheSameCopyGranularity)
{
    EXPECT_EQ(get_super_tensor_payload_capacity<Hardware::L0C>(4096U, 128U), 3072U);
    EXPECT_EQ(get_super_tensor_payload_capacity<Hardware::GM>(4096U, 128U), 3968U);
    // Ring buffers below one payload alignment are a formula boundary rather than
    // a supported configuration: DumpTensor documents 1 KB as the smallest FIFO.
    EXPECT_EQ(get_super_tensor_payload_capacity<Hardware::L0C>(1024U, 128U), 0U);
    EXPECT_EQ(get_super_tensor_payload_capacity<Hardware::L0C>(128U, 128U), 0U);
}

TEST_F(TestDumpTensorPayload, SuperTensorTailKeepsActualLengthAndOffset)
{
    DumpSuperTensorBodyTlv tlv{};
    set_super_tensor_body_tlv_info(&tlv, 1024U, 6148U, 6144U, 4U);
    EXPECT_EQ(tlv.type, static_cast<uint32_t>(DumpType::DUMP_SUPER_TENSOR_BODY));
    EXPECT_EQ(tlv.length + 8U, sizeof(tlv) + 1024U);
    EXPECT_EQ(tlv.tensorLength, 6148U);
    EXPECT_EQ(tlv.tensorOffset, 6144U);
    EXPECT_EQ(tlv.dumpSize, 4U);
}
} // namespace
