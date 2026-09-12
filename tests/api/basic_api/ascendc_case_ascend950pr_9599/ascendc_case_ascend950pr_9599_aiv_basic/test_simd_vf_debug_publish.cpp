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
#include <mockcpp/mockcpp.hpp>
#include <array>
#include <cstring>
#include <vector>
#include "stub_fun.h"
#include "impl/utils/sys_macros.h"
#include "impl/utils/common_types.h"

// Keep CPU stub types/core selection, but include the real device debug implementation.
// These device datatype names occur only in uninstantiated traits in the included header.
struct float8_e5m2_t;
struct float8_e4m3_t;
struct float8_e8m0_t;
struct float4_e2m1x2_t;
struct float4_e1m2x2_t;
namespace __cce_scalar {
using ::copy_ubuf_to_ubuf;
}
#pragma push_macro("ASCENDC_CPU_DEBUG")
#undef ASCENDC_CPU_DEBUG
#include "impl/utils/debug/asc_debug_utils.h"
#pragma pop_macro("ASCENDC_CPU_DEBUG")

namespace {
using namespace __asc_aicore;
constexpr uint32_t RING_BYTES = 256;
constexpr uint32_t BATCH_BYTES = 64;
struct GmBlock {
    DebugBlockHeadInfo head;
    DebugBlockReadInfo reader;
    alignas(8) std::array<uint8_t, RING_BYTES> data;
    DebugBlockWriteInfo writer;
};
static_assert(offsetof(GmBlock, reader) == sizeof(DebugBlockHeadInfo));
static_assert(offsetof(GmBlock, writer) == offsetof(GmBlock, data) + RING_BYTES);

struct Flush {
    const void* address;
    uint64_t writeOffset;
    uint64_t packages;
    uint32_t ubRead;
    uint16_t coreType;
};
GmBlock* activeGm;
BlockVFBufInfo* activeUb;
std::vector<Flush> flushes;
void RecordFlush(void* address, uint64_t entire, uint64_t destination)
{
    EXPECT_EQ(entire, cache_line_t::ENTIRE_DATA_CACHE);
    EXPECT_EQ(destination, dcci_dst_t::CACHELINE_OUT);
    flushes.push_back(
        {address, activeGm->writer.bufOffset, activeGm->writer.packIdx, activeUb->readLen, activeGm->head.flag});
}

class TestSimdVfDebugPublish : public testing::Test {
protected:
    void SetUp() override
    {
        previousCore = g_coreType;
        previousFifo = g_sysPrintFifoSpace;
        g_coreType = AscendC::AIV;
        gm = {};
        ub = {};
        gm.head.magic = 0xAE86;
        gm.head.length = sizeof(GmBlock);
        gm.head.ringBufLen = RING_BYTES;
        gm.head.ringBufAddr = reinterpret_cast<uint64_t>(gm.data.data());
        gm.head.debugBusAddr = 0x12345678;
        g_sysPrintFifoSpace = reinterpret_cast<uint8_t*>(&gm);
        activeGm = &gm;
        activeUb = &ub;
        flushes.clear();
        // Two complete scalar TLVs. The publisher must preserve the batch count.
        const uint32_t header[2] = {static_cast<uint32_t>(DumpType::DUMP_SCALAR), 24};
        std::memcpy(ub.buffer, header, sizeof(header));
        std::memcpy(ub.buffer + 32, header, sizeof(header));
        ub.buffer[24] = 7;
        ub.buffer[56] = 9;
        ub.writeLen = BATCH_BYTES;
        MOCKER(get_coreid).stubs().will(returnValue(int64_t(0)));
        MOCKER(dcci, void(void*, uint64_t, uint64_t)).stubs().will(invoke(RecordFlush));
    }
    void TearDown() override
    {
        GlobalMockObject::verify();
        g_sysPrintFifoSpace = previousFifo;
        g_coreType = previousCore;
    }
    void Publish() { asc_vf_debug_publish(&ub, ub.writeLen, ub.readLen); }
    void ExpectNoCommit()
    {
        EXPECT_EQ(gm.writer.bufOffset, 0U);
        EXPECT_EQ(gm.writer.packIdx, 0U);
    }
    GmBlock gm{};
    BlockVFBufInfo ub{};
    uint8_t* previousFifo = nullptr;
    int32_t previousCore = 0;
};

TEST_F(TestSimdVfDebugPublish, SetsAivTypeBeforePublishingBatch)
{
    ASSERT_EQ(gm.head.flag, 0U);
    Publish();
    EXPECT_EQ(gm.head.flag, 1U);
    EXPECT_EQ(gm.head.debugBusAddr, 0x12345678U);
    EXPECT_EQ(ub.flag, 0U);
    EXPECT_EQ(ub.readLen, BATCH_BYTES);
    EXPECT_EQ(gm.writer.bufOffset, BATCH_BYTES);
    EXPECT_EQ(gm.writer.packIdx, 2U);
    EXPECT_EQ(std::memcmp(ub.buffer, gm.data.data(), BATCH_BYTES), 0);
    ASSERT_EQ(flushes.size(), 3U);
    EXPECT_EQ(flushes[0].address, &gm.head);
    EXPECT_EQ(flushes[1].address, gm.data.data());
    EXPECT_EQ(flushes[2].address, &gm.writer);
    EXPECT_EQ(flushes[1].writeOffset, 0U);
    EXPECT_EQ(flushes[2].writeOffset, BATCH_BYTES);
    EXPECT_EQ(flushes[2].packages, 2U);
    for (const auto& flush : flushes) {
        EXPECT_EQ(flush.ubRead, 0U);
        EXPECT_EQ(flush.coreType, 1U);
    }
}

TEST_F(TestSimdVfDebugPublish, StandardGetterRetainsAicType)
{
    g_coreType = AscendC::AIC;
    gm.head.flag = 1;
    Publish();
    EXPECT_EQ(gm.head.flag, 0U);
    EXPECT_EQ(gm.writer.packIdx, 2U);
}

TEST_F(TestSimdVfDebugPublish, NullFifoStopsWithoutConsuming)
{
    g_sysPrintFifoSpace = nullptr;
    Publish();
    EXPECT_EQ(ub.flag, 1U);
    EXPECT_EQ(ub.readLen, 0U);
    ExpectNoCommit();
    EXPECT_TRUE(flushes.empty());
}

TEST_F(TestSimdVfDebugPublish, InvalidMagicStopsWithoutConsuming)
{
    gm.head.magic = 0;
    Publish();
    EXPECT_EQ(ub.flag, 1U);
    EXPECT_EQ(ub.readLen, 0U);
    ExpectNoCommit();
    EXPECT_TRUE(flushes.empty());
}

TEST_F(TestSimdVfDebugPublish, MalformedBatchStopsBeforeGmAccess)
{
    const uint32_t invalidLength = BATCH_BYTES;
    std::memcpy(ub.buffer + 4, &invalidLength, sizeof(invalidLength));
    Publish();
    EXPECT_EQ(ub.flag, 1U);
    EXPECT_EQ(ub.readLen, 0U);
    ExpectNoCommit();
    EXPECT_TRUE(flushes.empty());
}

TEST_F(TestSimdVfDebugPublish, PermanentCapacityFailureDoesNotPublish)
{
    gm.head.ringBufLen = 48;
    Publish();
    EXPECT_EQ(ub.flag, 1U);
    EXPECT_EQ(ub.readLen, BATCH_BYTES); // Existing batch-drop contract.
    ExpectNoCommit();
    ASSERT_EQ(flushes.size(), 1U);
    EXPECT_EQ(flushes[0].address, &gm.head);
}

TEST_F(TestSimdVfDebugPublish, TransientFailureCanRetryTheSameBatch)
{
    MOCKER(check_ringbuf_space).stubs().will(returnValue(false)).then(returnValue(true));
    Publish();
    EXPECT_EQ(ub.flag, 0U);
    EXPECT_EQ(ub.readLen, 0U);
    ExpectNoCommit();
    ASSERT_EQ(flushes.size(), 1U);
    Publish();
    EXPECT_EQ(ub.flag, 0U);
    EXPECT_EQ(ub.readLen, BATCH_BYTES);
    EXPECT_EQ(gm.writer.bufOffset, BATCH_BYTES);
    EXPECT_EQ(gm.writer.packIdx, 2U);
}
} // namespace
