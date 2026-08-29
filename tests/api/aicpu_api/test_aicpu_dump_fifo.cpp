/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 *
 * Unit tests for the FIFO-based AscendC::printf() flow implemented in
 * impl/aicpu_api/aicpu_dump.cpp (the "rts provides AicpuGetDfxInfo" path).
 *
 * NOTE: This file provides a strong definition of the weak symbol
 * AicpuGetDfxInfo (see below), so AscendC::printf() dispatches to
 * PrintfFifo() in the single test binary. The legacy linear-buffer flow is
 * covered by test_aicpu_dump.cpp, which clears g_utAicpuGetDfxInfoHook so
 * AicpuGetDfxInfo returns -1 and PrintfFifo falls back to PrintfLegacy.
 */
#include <gtest/gtest.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mutex>
#include <vector>

#include "securec.h"
#include "aicpu_api/aicpu_api.h"
#include <mockcpp/mockcpp.hpp>

/*
 * Test-side replica of the FIFO protocol structures declared in
 * impl/aicpu_api/aicpu_dump.cpp. Definitions are kept identical (same
 * packing / field layout) so the memory we prepare is interpreted
 * correctly by the implementation under test.
 */
namespace AscendC {

enum DumpType : uint32_t {
    DUMP_DEFAULT = 0U,
    DUMP_SCALAR,
    DUMP_TENSOR,
    DUMP_SHAPE,
    DUMP_ASSERT,
    DUMP_META,
    DUMP_TIME_STAMP,
    DUMP_SIMT,
    DUMP_BUFI, /* 8: write info marker */
    DUMP_BUFO, /* 9: read  info marker */
    DUMP_SKIP,
    DUMP_AICPU = 11U
};

constexpr uint32_t ATTR_ID_MEM_INFO = 1U;
constexpr uint32_t PRINT_TYPE_WRITING = 0xF0A55A0FU;
constexpr uint32_t PRINT_TYPE_READY = 11U; /* DUMP_AICPU */
constexpr uint32_t FIFO_MAGIC = 0xAE86U;

#pragma pack(push, 1)
struct AicpuPrintfMemInfo {
    uint64_t printfMemAddr;
    uint32_t printfMemSize;
    uint32_t resv0;
};

union AicpuDfxAttrValue {
    uint8_t resv[64U] = {0U};
    AicpuPrintfMemInfo printfMemInfo;
};

struct AicpuDfxAttrInfo {
    uint32_t attrId = 0U;
    AicpuDfxAttrValue value;
};

struct AicpuDfxInfo {
    uint64_t attrs = 0U;
    uint64_t numAttrs = 0U;
};
#pragma pack(pop)

struct BlockRingBufInfo {
    uint32_t length = 0U;
    uint32_t resv0 = 0U;
    uint32_t resv1 = 0U;
    uint32_t ringBufLen = 0U;
    uint16_t magic = 0U;
    uint16_t flag = 0U;
    uint32_t resv2 = 0U;
    uint64_t ringBufAddr = 0U;
    uint32_t resvMem[6];
};

struct RingBufWriteInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFI);
    uint32_t length = 0U;
    uint64_t bufOffset = 0U;
    uint64_t packIdx = 0U;
};

struct RingBufReadInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFO);
    uint32_t length = 0U;
    uint64_t bufOffset = 0U;
    uint64_t resv = 0U;
};

} // namespace AscendC

struct DumpConfig {
    void* dumpBufAddr;
    size_t dumpBufSize;
    size_t dumpOffset;
};

extern "C" {
extern DumpConfig g_aicpuDumpConfig;

/* Test hook: controls what the fake "rts" AicpuGetDfxInfo returns. */
int32_t (*g_utAicpuGetDfxInfoHook)(uint64_t* infoAddr) = nullptr;

/*
 * Strong definition of the weak symbol declared in aicpu_dump.cpp.
 * Making it resolve here forces AscendC::printf() onto the FIFO path in
 * this test binary, and lets every test control the rts provider output.
 */
int32_t AicpuGetDfxInfo(uint64_t* infoAddr)
{
    if (g_utAicpuGetDfxInfoHook != nullptr) {
        return g_utAicpuGetDfxInfoHook(infoAddr);
    }
    return -1;
}

/*
 * malloc() injection via the linker --wrap=malloc. The test binary is
 * linked with -Wl,--wrap=malloc, so every malloc() call inside the binary
 * (including the FifoTempBuf in aicpu_dump.cpp) is routed through
 * __wrap_malloc(). This lets us simulate allocation failure without
 * hooking malloc at runtime (which would re-enter the hook from within
 * mockcpp/libc and crash). __real_malloc() is the original glibc malloc.
 */
void* __real_malloc(size_t size);

/* One-shot: when set, the next malloc(size == g_utMallocFailSize) fails. */
bool g_utMallocFailOnSize = false;
size_t g_utMallocFailSize = 0U;

void* __wrap_malloc(size_t size)
{
    if (g_utMallocFailOnSize && size == g_utMallocFailSize) {
        g_utMallocFailOnSize = false; /* one-shot */
        return nullptr;
    }
    return __real_malloc(size);
}
}

namespace {

using namespace AscendC;

/* Unaligned-safe little-endian reads for ring buffer verification. */
uint32_t ReadU32(const uint8_t* p)
{
    uint32_t v = 0;
    (void)memcpy(&v, p, sizeof(v));
    return v;
}

uint64_t ReadU64(const uint8_t* p)
{
    uint64_t v = 0;
    (void)memcpy(&v, p, sizeof(v));
    return v;
}

/*
 * Fake rts FIFO memory layout (as produced by the device-side runtime):
 *   [BlockRingBufInfo][RingBufReadInfo][ ring data ][RingBufWriteInfo]
 * GetRingBufReadInfo  -> blockInfo + sizeof(BlockRingBufInfo)
 * GetRingBufWriteInfo -> ringBufAddr + ringBufLen
 */
class FifoEnv {
public:
    FifoEnv(uint32_t ringLen, uint64_t oriBufOff, uint64_t readOff, uint32_t magic = FIFO_MAGIC)
    {
        const size_t headSize = sizeof(BlockRingBufInfo) + sizeof(RingBufReadInfo);
        mem_.resize(headSize + ringLen + sizeof(RingBufWriteInfo) + 64U, 0U);
        blockInfo_ = reinterpret_cast<BlockRingBufInfo*>(mem_.data());
        readInfo_ = reinterpret_cast<RingBufReadInfo*>(mem_.data() + sizeof(BlockRingBufInfo));
        uint8_t* ringData = mem_.data() + headSize;
        writeInfo_ = reinterpret_cast<RingBufWriteInfo*>(ringData + ringLen);
        ringBase_ = ringData;

        blockInfo_->length = static_cast<uint32_t>(mem_.size());
        blockInfo_->ringBufLen = ringLen;
        blockInfo_->magic = magic;
        blockInfo_->flag = 3U; /* aicpu */
        blockInfo_->ringBufAddr = reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(ringData));
        readInfo_->type = static_cast<uint32_t>(DumpType::DUMP_BUFO);
        readInfo_->bufOffset = readOff;
        writeInfo_->type = static_cast<uint32_t>(DumpType::DUMP_BUFI);
        writeInfo_->bufOffset = oriBufOff;
        writeInfo_->packIdx = 0U;
    }

    BlockRingBufInfo* Block() const { return blockInfo_; }
    RingBufWriteInfo* WriteInfo() const { return writeInfo_; }
    RingBufReadInfo* ReadInfo() const { return readInfo_; }
    uint8_t* Ring() const { return ringBase_; }

private:
    std::vector<uint8_t> mem_;
    BlockRingBufInfo* blockInfo_ = nullptr;
    RingBufReadInfo* readInfo_ = nullptr;
    RingBufWriteInfo* writeInfo_ = nullptr;
    uint8_t* ringBase_ = nullptr;
};

/* Fake rts dfx-info chain: AicpuDfxInfo -> AicpuDfxAttrInfo(mem info). */
struct DfxInfoChain {
    AicpuDfxInfo info;
    AicpuDfxAttrInfo attr0;
};

static DfxInfoChain g_dfxChain;

static void ResetDfxChain()
{
    (void)memset(&g_dfxChain, 0, sizeof(g_dfxChain));
    g_dfxChain.info.attrs = reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(&g_dfxChain.attr0));
    g_dfxChain.info.numAttrs = 1U;
    g_dfxChain.attr0.attrId = ATTR_ID_MEM_INFO;
}

static void SetDfxMemInfo(uint64_t memAddr, uint32_t memSize)
{
    ResetDfxChain();
    g_dfxChain.attr0.value.printfMemInfo.printfMemAddr = memAddr;
    g_dfxChain.attr0.value.printfMemInfo.printfMemSize = memSize;
}

/* Hook: rts returns a valid dfx-info chain. */
static int32_t HookDfxInfoOk(uint64_t* infoAddr)
{
    *infoAddr = reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(&g_dfxChain.info));
    return 0;
}

/* Hook: rts reports an error. */
static int32_t HookDfxInfoFail(uint64_t* infoAddr)
{
    (void)infoAddr;
    return -2;
}

/* Hook: rts writes a null dfx-info address. */
static int32_t HookDfxInfoNullAddr(uint64_t* infoAddr)
{
    *infoAddr = 0U;
    return 0;
}

/* Hook: dfx-info chain with no attrs array. */
static int32_t HookDfxInfoNoAttrs(uint64_t* infoAddr)
{
    ResetDfxChain();
    g_dfxChain.info.attrs = 0U;
    *infoAddr = reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(&g_dfxChain.info));
    return 0;
}

/* Hook: dfx-info chain with numAttrs == 0. */
static int32_t HookDfxInfoNoNumAttrs(uint64_t* infoAddr)
{
    ResetDfxChain();
    g_dfxChain.info.numAttrs = 0U;
    *infoAddr = reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(&g_dfxChain.info));
    return 0;
}

/* Hook: dfx-info chain whose attrs do not contain a mem-info attr. */
static int32_t HookDfxInfoWrongAttrId(uint64_t* infoAddr)
{
    ResetDfxChain();
    g_dfxChain.attr0.attrId = 2U; /* != ATTR_ID_MEM_INFO */
    *infoAddr = reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(&g_dfxChain.info));
    return 0;
}

/* Hook: mem-info attr with printfMemAddr == 0. */
static int32_t HookDfxInfoZeroMemAddr(uint64_t* infoAddr)
{
    ResetDfxChain();
    g_dfxChain.attr0.value.printfMemInfo.printfMemAddr = 0U;
    *infoAddr = reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(&g_dfxChain.info));
    return 0;
}

/* Legacy linear buffer used by the PrintfLegacy fallback path. */
static char g_legacyBuffer[256];

static void PrepareLegacyBuffer()
{
    (void)memset(g_legacyBuffer, 0, sizeof(g_legacyBuffer));
    /* First sizeof(size_t) bytes of the buffer hold the current offset. */
    *reinterpret_cast<size_t*>(g_legacyBuffer) = sizeof(size_t);
    g_aicpuDumpConfig.dumpBufAddr = g_legacyBuffer;
    g_aicpuDumpConfig.dumpBufSize = sizeof(g_legacyBuffer);
    g_aicpuDumpConfig.dumpOffset = sizeof(size_t);
}

static void ExpectLegacyBufferContent(const char* expected)
{
    const size_t off = *reinterpret_cast<size_t*>(g_legacyBuffer);
    EXPECT_EQ(off, sizeof(size_t) + strlen(expected));
    EXPECT_STREQ(g_legacyBuffer + sizeof(size_t), expected);
}

} // namespace

class TEST_AICPU_DUMP_FIFO : public testing::Test {
protected:
    void SetUp() override
    {
        GlobalMockObject::reset();
        g_utAicpuGetDfxInfoHook = nullptr;
        g_utMallocFailOnSize = false;
        g_utMallocFailSize = 0U;
        g_aicpuDumpConfig.dumpBufAddr = nullptr;
        g_aicpuDumpConfig.dumpBufSize = 0U;
        g_aicpuDumpConfig.dumpOffset = 0U;
    }

    void TearDown() override
    {
        GlobalMockObject::verify();
        GlobalMockObject::reset();
    }

    /* Point the fake rts FIFO at the given env and enable the FIFO path. */
    void UseFifoEnv(const FifoEnv& env)
    {
        SetDfxMemInfo(reinterpret_cast<uint64_t>(reinterpret_cast<uintptr_t>(env.Block())), env.WriteInfo()->length);
        g_utAicpuGetDfxInfoHook = HookDfxInfoOk;
    }
};

/* ── FIFO happy path ───────────────────────────────────────────────── */

TEST_F(TEST_AICPU_DUMP_FIFO, FifoNormalWrite)
{
    FifoEnv env(1024, 0, 0);
    UseFifoEnv(env);

    AscendC::printf("hello %d", 42); /* "hello 42": strLen 8 -> tlvLen 40 */

    EXPECT_EQ(env.WriteInfo()->bufOffset, 40U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 1U);
    /* header at offset 0, fmt string at offset 24 */
    EXPECT_EQ(ReadU32(env.Ring()), PRINT_TYPE_READY);
    EXPECT_EQ(ReadU32(env.Ring() + 4), 32U); /* printfLength = tlvLen - 8 */
    EXPECT_EQ(ReadU32(env.Ring() + 8), 0U);  /* blockIdx */
    EXPECT_EQ(ReadU32(env.Ring() + 12), 0U); /* resv */
    EXPECT_EQ(ReadU64(env.Ring() + 16), 8U); /* fmtOffset */
    EXPECT_STREQ(reinterpret_cast<char*>(env.Ring() + 24), "hello 42");
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoSequentialWrites)
{
    FifoEnv env(1024, 0, 0);
    UseFifoEnv(env);

    AscendC::printf("ab");  /* tlvLen 32 */
    AscendC::printf("cde"); /* tlvLen 32 */

    EXPECT_EQ(env.WriteInfo()->bufOffset, 64U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 2U);
    EXPECT_EQ(ReadU32(env.Ring()), PRINT_TYPE_READY);
    EXPECT_EQ(ReadU32(env.Ring() + 32), PRINT_TYPE_READY);
    EXPECT_STREQ(reinterpret_cast<char*>(env.Ring() + 24), "ab");
    EXPECT_STREQ(reinterpret_cast<char*>(env.Ring() + 24 + 32), "cde");
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoEmptyString)
{
    FifoEnv env(1024, 0, 0);
    UseFifoEnv(env);

    AscendC::printf(""); /* vsnprintf returns 0 -> strLen 0 -> tlvLen 32 */

    EXPECT_EQ(env.WriteInfo()->bufOffset, 32U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 1U);
    EXPECT_EQ(ReadU32(env.Ring()), PRINT_TYPE_READY);
    EXPECT_EQ(env.Ring()[24], 0U); /* NUL terminator */
}

/* ── Fallback to legacy linear buffer ───────────────────────────────── */

TEST_F(TEST_AICPU_DUMP_FIFO, FifoFallbackToLegacyOnNoDfxInfo)
{
    PrepareLegacyBuffer();
    /* g_utAicpuGetDfxInfoHook == nullptr -> AicpuGetDfxInfo returns -1. */
    AscendC::printf("legacy %d", 7);

    ExpectLegacyBufferContent("legacy 7");
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoFallbackToLegacyOnHookError)
{
    PrepareLegacyBuffer();
    g_utAicpuGetDfxInfoHook = HookDfxInfoFail;
    AscendC::printf("fb %d", 1);

    ExpectLegacyBufferContent("fb 1");
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoFallbackToLegacyOnNullDfxAddr)
{
    PrepareLegacyBuffer();
    g_utAicpuGetDfxInfoHook = HookDfxInfoNullAddr;
    AscendC::printf("fb %d", 2);

    ExpectLegacyBufferContent("fb 2");
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoFallbackToLegacyOnNoAttrs)
{
    PrepareLegacyBuffer();
    g_utAicpuGetDfxInfoHook = HookDfxInfoNoAttrs;
    AscendC::printf("fb %d", 3);

    ExpectLegacyBufferContent("fb 3");
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoFallbackToLegacyOnNoNumAttrs)
{
    PrepareLegacyBuffer();
    g_utAicpuGetDfxInfoHook = HookDfxInfoNoNumAttrs;
    AscendC::printf("fb %d", 4);

    ExpectLegacyBufferContent("fb 4");
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoFallbackToLegacyOnWrongAttrId)
{
    PrepareLegacyBuffer();
    g_utAicpuGetDfxInfoHook = HookDfxInfoWrongAttrId;
    AscendC::printf("fb %d", 5);

    ExpectLegacyBufferContent("fb 5");
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoFallbackToLegacyOnZeroMemAddr)
{
    PrepareLegacyBuffer();
    g_utAicpuGetDfxInfoHook = HookDfxInfoZeroMemAddr;
    AscendC::printf("fb %d", 6);

    ExpectLegacyBufferContent("fb 6");
}

/* ── FIFO early-return / drop paths ─────────────────────────────────── */

TEST_F(TEST_AICPU_DUMP_FIFO, FifoBadMagic)
{
    FifoEnv env(128, 0, 0, 0x1234U);
    UseFifoEnv(env);

    AscendC::printf("hello");

    EXPECT_EQ(env.WriteInfo()->bufOffset, 0U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 0U);
    EXPECT_EQ(env.Ring()[0], 0U);
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoNullRingBase)
{
    FifoEnv env(128, 0, 0);
    env.Block()->ringBufAddr = 0U;
    UseFifoEnv(env);

    AscendC::printf("hello");

    EXPECT_EQ(env.WriteInfo()->bufOffset, 0U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 0U);
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoZeroRingLen)
{
    FifoEnv env(128, 0, 0);
    env.Block()->ringBufLen = 0U;
    UseFifoEnv(env);

    AscendC::printf("hello");

    EXPECT_EQ(env.WriteInfo()->bufOffset, 0U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 0U);
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoDropWhenTlvTooLarge)
{
    FifoEnv env(16, 0, 0); /* ringBufLen 16 < tlvLen 32 */
    UseFifoEnv(env);

    AscendC::printf("hello");

    EXPECT_EQ(env.WriteInfo()->bufOffset, 0U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 0U);
    EXPECT_EQ(env.Ring()[0], 0U);
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoDropOnWaitTimeout)
{
    /* rOff - wOff >= ringBufLen -> free space 0, wait times out after 15 iters. */
    MOCKER(nanosleep).stubs().will(returnValue(0));
    FifoEnv env(128, 256, 0);
    UseFifoEnv(env);

    AscendC::printf("hello");

    EXPECT_EQ(env.WriteInfo()->bufOffset, 256U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 0U);
    for (size_t i = 0; i < 64; ++i) {
        EXPECT_EQ(env.Ring()[i], 0U);
    }
}

/* ── Split writes (ring tail/head boundary) ─────────────────────────── */

TEST_F(TEST_AICPU_DUMP_FIFO, FifoSplitHeaderWrite)
{
    /* wOff = 48, 48 + sizeof(PrintfInfo) = 72 > 64 -> byte-by-byte header. */
    FifoEnv env(64, 48, 32);
    UseFifoEnv(env);

    AscendC::printf("hello"); /* tlvLen 32 */

    EXPECT_EQ(env.WriteInfo()->bufOffset, 80U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 1U);
    /* header spans [48..64) + [0..8); type flipped to READY at [48..52) */
    EXPECT_EQ(ReadU32(env.Ring() + 48), PRINT_TYPE_READY);
    EXPECT_EQ(ReadU32(env.Ring() + 52), 24U); /* printfLength = tlvLen - 8 */
    EXPECT_EQ(ReadU32(env.Ring() + 56), 0U);  /* blockIdx */
    EXPECT_EQ(ReadU32(env.Ring() + 60), 0U);  /* resv */
    EXPECT_EQ(ReadU64(env.Ring()), 8U);       /* fmtOffset, wrapped to head */
    EXPECT_STREQ(reinterpret_cast<char*>(env.Ring() + 8), "hello");
}

TEST_F(TEST_AICPU_DUMP_FIFO, FifoSplitReleaseWrite)
{
    /* wOff = 62: header split (62+24 > 64) AND release split (62+4 > 64). */
    FifoEnv env(64, 62, 32);
    UseFifoEnv(env);

    AscendC::printf("hello"); /* tlvLen 32 */

    EXPECT_EQ(env.WriteInfo()->bufOffset, 94U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 1U);
    /* READY(11, little-endian) written byte-by-byte at [62..64)+[0..2) */
    EXPECT_EQ(env.Ring()[62], 0x0BU);
    EXPECT_EQ(env.Ring()[63], 0x00U);
    EXPECT_EQ(env.Ring()[0], 0x00U);
    EXPECT_EQ(env.Ring()[1], 0x00U);
    /* fmt string written at writingOff = 22 */
    EXPECT_STREQ(reinterpret_cast<char*>(env.Ring() + 22), "hello");
}

/*
 * Split string write: header fits at the tail, but the formatted string
 * would cross the ring boundary, so it is formatted into the temp buffer
 * and copied in two parts. Also covers the rollback when the temp buffer
 * allocation fails, and the temp-buffer cache reuse.
 *
 * NOTE: ringBufLen must be a multiple of 8 and bufOffset a multiple of 4
 * so that RingBufWriteInfo and the entry type-field stay naturally aligned
 * (the implementation flips them with 64/32-bit atomic stores).
 */
TEST_F(TEST_AICPU_DUMP_FIFO, FifoSplitStringWriteAndRollback)
{
    /* ringBufLen 128, oriBufOff 96 -> writingOff 120, tailSpace 8.
     * tlvLen for "hello 12345"(11 chars) = 40; needSplit: 16 > 8. */
    const uint32_t ringLen = 128U;
    const uint64_t oriBufOff = 96U;
    FifoEnv env(ringLen, oriBufOff, 16);
    UseFifoEnv(env);

    /* Part 1: temp-buffer allocation fails -> entry rolled back. */
    g_utMallocFailOnSize = true;
    g_utMallocFailSize = ringLen;
    AscendC::printf("hello %d", 12345);

    EXPECT_EQ(env.WriteInfo()->bufOffset, oriBufOff);
    EXPECT_EQ(env.WriteInfo()->packIdx, 0U);
    /* header was written (WRITING) but entry was NOT released */
    EXPECT_EQ(ReadU32(env.Ring() + 96), PRINT_TYPE_WRITING);
    EXPECT_FALSE(g_utMallocFailOnSize); /* one-shot flag was consumed */

    /* Part 2: allocation succeeds -> split write completes. */
    AscendC::printf("hello %d", 12345);

    EXPECT_EQ(env.WriteInfo()->bufOffset, oriBufOff + 40U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 1U);
    EXPECT_EQ(ReadU32(env.Ring() + 96), PRINT_TYPE_READY);
    /* part1 = 8 bytes at [120..128), part2 = 8 bytes at [0..8) */
    EXPECT_EQ(memcmp(env.Ring() + 120, "hello 12", 8), 0);
    EXPECT_EQ(env.Ring()[0], '3');
    EXPECT_EQ(env.Ring()[1], '4');
    EXPECT_EQ(env.Ring()[2], '5');
    EXPECT_EQ(env.Ring()[3], '\0');

    /* Part 3: cached temp buffer is reused. Reset the write position to
     * re-trigger the same split; Get() returns the cached buffer without
     * calling malloc, so the pending allocation-failure flag survives. */
    env.WriteInfo()->bufOffset = oriBufOff;
    g_utMallocFailOnSize = true;
    g_utMallocFailSize = ringLen;
    AscendC::printf("hello %d", 12345);

    EXPECT_EQ(env.WriteInfo()->bufOffset, oriBufOff + 40U);
    EXPECT_EQ(env.WriteInfo()->packIdx, 2U);
    EXPECT_EQ(ReadU32(env.Ring() + 96), PRINT_TYPE_READY);
    EXPECT_EQ(memcmp(env.Ring() + 120, "hello 12", 8), 0);
    EXPECT_TRUE(g_utMallocFailOnSize); /* no malloc issued, flag still set */
}
