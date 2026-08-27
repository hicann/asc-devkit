/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file aicpu_dump.cpp
 * \brief
 */

#include <stdarg.h>
#include <mutex>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "securec.h"
#include "aicpu_api/aicpu_api.h"

struct DumpConfig {
    void* dumpBufAddr;
    size_t dumpBufSize;
    size_t dumpOffset;
};

std::mutex dump_mutex;
extern "C" {
DumpConfig g_aicpuDumpConfig = {reinterpret_cast<void*>(0x8), 0x100000, 0x0};
}

/* ── rts interface (weak symbol provided by rts) ────────────────────── */

extern "C" {
int32_t __attribute__((weak)) AicpuGetDfxInfo(uint64_t* infoAddr);
}

namespace AscendC {

/* ── Constants ──────────────────────────────────────────────────────── */

// Dump types 0-10 are inherited from the aicore dump framework and are kept
// here only for protocol compatibility; aicpu itself uses only DUMP_BUFI, DUMP_BUFI, DUMP_AICPU
// (= 8, 9, 11), which also doubles as the FIFO entry READY marker (PRINT_TYPE_READY).
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
constexpr uint32_t FIFO_ALIGN = 8U;
constexpr uint64_t FIFO_WAIT_INTERVAL_NS = 300000000ULL; /* 300ms */
constexpr uint32_t FIFO_MAX_WAIT_ITERS = 15U;

/* ── FIFO protocol structures ───────────────────────────────────────── */

#pragma pack(push, 1)
struct AicpuPrintfMemInfo {
    uint64_t printfMemAddr = 0U;
    uint32_t printfMemSize = 0U;
    uint32_t resv0 = 0U;
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
    uint32_t length = 0U;      // total size per block (include head and r/w info)
    uint32_t resv0 = 0U;       // resv
    uint32_t resv1 = 0U;       // resv
    uint32_t ringBufLen = 0U;  // fifo buff size (print tlv storage)
    uint16_t magic = 0U;       // magic number for info verify, expected to be 0xAE86
    uint16_t flag = 0U;        // 0: simd, 1: simt, 3: aicpu
    uint32_t resv2 = 0U;       // reserve
    uint64_t ringBufAddr = 0U; // start addr of fifo buff
    uint32_t resvMem[6];       // reserved
};

struct RingBufWriteInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFI); // DumpType = DUMP_BUFI
    uint32_t length = 0U;                                       // u64 + u64
    uint64_t bufOffset = 0U;                                    // the offset of write addr relative to ringBufAddr
    uint64_t packIdx = 0U;                                      // print pack counter
};

struct RingBufReadInfo {
    uint32_t type = static_cast<uint32_t>(DumpType::DUMP_BUFO); // DumpType = DUMP_BUFO
    uint32_t length = 0U;                                       // u64 + u64
    uint64_t bufOffset = 0U;                                    // the offset of read addr relative to ringBufAddr
    uint64_t resv = 0U;
};

struct PrintfInfo {
    uint32_t printfType = static_cast<uint32_t>(DumpType::DUMP_DEFAULT); // DumpType = DUMP_AICPU
    uint32_t printfLength = 0U;                                          // length of print info
    uint32_t blockIdx = 0U;                                              // threadIdx
    uint32_t resv = 0U;                                                  // reserved
    uint64_t fmtOffset = 0U; // offset of fmt string from the start of fmtOffset addr
};

/* ── Mutex for multi-thread safety ──────────────────────────────────── */

std::mutex g_aicpuFifoMutex;

/* ── set debug flag in .ascend.meta ──────────────────────────────────── */

struct AicpuTlvHead { // TLV Head include type and length
    uint16_t type;
    uint16_t len;
};

struct AicpuTlv {
    AicpuTlvHead head;
    uint32_t value; // feature: PRINT = 1, FFTS = 2, L2CACHE = 3
};

inline void enable_asc_aicpu_fifo_debug()
{
#if (!defined(ASCENDC_DUMP) || (ASCENDC_DUMP != 0))
    static const struct AicpuTlv __asc_aicpu_meta_section__ __attribute__((used, section(".ascend.meta"))) = {4, 4, 6};
#endif // (!defined(ASCENDC_DUMP) || (ASCENDC_DUMP != 0))
}

/* ── FIFO memory retrieval ──────────────────────────────────────────── */

inline int32_t GetAscAicpuPrintfMemInfo(AicpuPrintfMemInfo& info)
{
    if (AicpuGetDfxInfo == nullptr) {
        return -1;
    }

    uint64_t dfxInfoAddr = 0U;
    if (AicpuGetDfxInfo(&dfxInfoAddr) != 0) {
        return -1;
    }

    auto* dfxInfo = reinterpret_cast<AicpuDfxInfo*>(dfxInfoAddr);
    if (dfxInfo == nullptr || dfxInfo->attrs == 0U || dfxInfo->numAttrs == 0U) {
        return -1;
    }

    auto* attrBase = reinterpret_cast<AicpuDfxAttrInfo*>(dfxInfo->attrs);
    for (uint64_t i = 0U; i < dfxInfo->numAttrs; ++i) {
        if (attrBase[i].attrId == ATTR_ID_MEM_INFO) {
            info = attrBase[i].value.printfMemInfo;
            if (info.printfMemAddr != 0U) {
                return 0;
            }
        }
    }
    return -1;
}

inline BlockRingBufInfo* GetAicpuDebugBlockInfo(AicpuPrintfMemInfo& memInfo)
{
    if (memInfo.printfMemAddr == 0U) {
        return nullptr;
    }
    auto* blockInfo = reinterpret_cast<BlockRingBufInfo*>(memInfo.printfMemAddr);
    if (blockInfo->magic != 0xAE86U) {
        return nullptr;
    }
    return blockInfo;
}

inline RingBufReadInfo* GetRingBufReadInfo(BlockRingBufInfo* blockInfo)
{
    auto* base = reinterpret_cast<uint8_t*>(blockInfo);
    return reinterpret_cast<RingBufReadInfo*>(base + sizeof(BlockRingBufInfo));
}

inline RingBufWriteInfo* GetRingBufWriteInfo(BlockRingBufInfo* blockInfo)
{
    auto* ringBase = reinterpret_cast<uint8_t*>(blockInfo->ringBufAddr);
    return reinterpret_cast<RingBufWriteInfo*>(ringBase + blockInfo->ringBufLen);
}

inline uint32_t CalcFormattedLen(const char* fmt, va_list args)
{
    va_list argsCopy;
    va_copy(argsCopy, args);
    int32_t len = vsnprintf(nullptr, 0, fmt, argsCopy);
    va_end(argsCopy);
    return (len > 0) ? static_cast<uint32_t>(len) : 0U;
}

inline uint32_t CalcTlvLen(uint32_t strLen)
{
    uint32_t total = sizeof(PrintfInfo) + strLen + 1U; /* +1 for NUL */
    total = (total + FIFO_ALIGN - 1U) & ~(FIFO_ALIGN - 1U);
    return total;
}

inline uint64_t GetFreeRingBufLen(uint64_t rOff, uint64_t wOff, uint32_t ringBufLen)
{
    return (wOff - rOff < ringBufLen) ? (ringBufLen - (wOff - rOff)) : 0;
}

inline bool AicpuRingbufWait(
    RingBufReadInfo* readInfo, RingBufWriteInfo* writeInfo, uint32_t tlvLen, uint32_t ringBufLen)
{
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(FIFO_WAIT_INTERVAL_NS / 1000000000ULL);
    ts.tv_nsec = static_cast<long>(FIFO_WAIT_INTERVAL_NS % 1000000000ULL);

    for (uint32_t i = 0U; i < FIFO_MAX_WAIT_ITERS; ++i) {
        if (GetFreeRingBufLen(readInfo->bufOffset, writeInfo->bufOffset, ringBufLen) > tlvLen) {
            return true;
        }
        /* Sleep 300ms to let rts reader make progress. */
        (void)nanosleep(&ts, nullptr);
    }
    return false;
}

inline bool CheckAicpuRingBuf(BlockRingBufInfo* blockInfo, uint32_t tlvLen)
{
    const uint32_t ringBufLen = blockInfo->ringBufLen;
    RingBufReadInfo* readInfo = GetRingBufReadInfo(blockInfo);
    RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockInfo);

    if (tlvLen > ringBufLen) {
        return false;
    }
    if (!AicpuRingbufWait(readInfo, writeInfo, tlvLen, ringBufLen)) {
        return false;
    }

    return true;
}

/* ── Temp buffer for split-write (allocated on demand, freed at program exit) ── */

class FifoTempBuf {
public:
    ~FifoTempBuf()
    {
        if (buf_ != nullptr) {
            free(buf_);
            buf_ = nullptr;
        }
    }

    uint8_t* Get(uint32_t size)
    {
        if (buf_ == nullptr && size > 0) {
            buf_ = static_cast<uint8_t*>(malloc(size));
        }
        return buf_;
    }

private:
    uint8_t* buf_ = nullptr;
};

static FifoTempBuf g_fifoTempBuf;

/* ── WriteInfo update helpers ──────────────────────────────────────── */

inline void AdvanceWriteInfo(RingBufWriteInfo* writeInfo, uint64_t newBufOff)
{
    __atomic_store_n(reinterpret_cast<uint64_t*>(&writeInfo->packIdx), writeInfo->packIdx + 1U, __ATOMIC_SEQ_CST);
    __atomic_store_n(reinterpret_cast<uint64_t*>(&writeInfo->bufOffset), newBufOff, __ATOMIC_SEQ_CST);
}

inline void RollbackWriteInfo(RingBufWriteInfo* writeInfo, uint64_t oldBufOff)
{
    __atomic_store_n(reinterpret_cast<uint64_t*>(&writeInfo->packIdx), writeInfo->packIdx - 1U, __ATOMIC_SEQ_CST);
    __atomic_store_n(reinterpret_cast<uint64_t*>(&writeInfo->bufOffset), oldBufOff, __ATOMIC_SEQ_CST);
}

/* ── Legacy printf (linear buffer) ──────────────────────────────────── */

static void PrintfLegacy(const char* fmt, va_list otherArgs)
{
    std::lock_guard<std::mutex> lock(dump_mutex);
    void* aicpuDumpBuffer = g_aicpuDumpConfig.dumpBufAddr;
    size_t aicpuDumpBufSize = g_aicpuDumpConfig.dumpBufSize;
    size_t aicpuDumpOffSet = 0;
    if (aicpuDumpBuffer != nullptr) {
        aicpuDumpOffSet = *reinterpret_cast<size_t*>(aicpuDumpBuffer);
    } else {
        return;
    }
    if (aicpuDumpOffSet >= aicpuDumpBufSize) {
        return;
    }
    aicpuDumpOffSet += static_cast<size_t>(vsnprintf_s(
        reinterpret_cast<char*>(aicpuDumpBuffer) + aicpuDumpOffSet, aicpuDumpBufSize - aicpuDumpOffSet,
        aicpuDumpBufSize - aicpuDumpOffSet, fmt, otherArgs));
    *reinterpret_cast<size_t*>(g_aicpuDumpConfig.dumpBufAddr) = aicpuDumpOffSet;
}

/* ── FIFO write helpers ─────────────────────────────────────────────── */

struct RingBufWriteCtx {
    uint8_t* ringBase;
    uint32_t ringBufLen;
    uint32_t writingOff;
    uint32_t tlvLen;
    uint32_t strLen;
};

static void WritePrintInfoHeader(uint8_t* ringBase, uint32_t ringBufLen, uint64_t writeBufOffset, uint32_t tlvLen)
{
    uint32_t wOff = static_cast<uint32_t>(writeBufOffset % ringBufLen);
    PrintfInfo infoHead;
    infoHead.printfType = PRINT_TYPE_WRITING;
    infoHead.printfLength = tlvLen - sizeof(uint32_t[2]);
    infoHead.blockIdx = 0U;
    infoHead.resv = 0U;
    infoHead.fmtOffset = 8U;
    // Write printfType=WRITING to ringbuf BEFORE updating bufOffset.
    // This ensures rts sees WRITING (not stale READY) when it notices
    // bufOffset has advanced, causing it to stop reading this round.
    if (wOff + sizeof(PrintfInfo) <= ringBufLen) {
        (void)memcpy_s(ringBase + wOff, ringBufLen - wOff, &infoHead, sizeof(PrintfInfo));
    } else {
        // Split: header may span tail/head boundary, write byte-by-byte.
        const uint8_t* headBytes = reinterpret_cast<const uint8_t*>(&infoHead);
        for (uint32_t i = 0U; i < sizeof(PrintfInfo); ++i) {
            ringBase[(wOff + i) % ringBufLen] = headBytes[i];
        }
    }
}

static bool WriteFormattedString(const RingBufWriteCtx& ctx, const char* fmt, va_list otherArgs)
{
    uint32_t tailSpace = ctx.ringBufLen - ctx.writingOff;
    bool needSplit = (ctx.tlvLen - sizeof(PrintfInfo) > tailSpace);
    if (!needSplit) {
        // header already written, write formatted string directly
        char* strDest = reinterpret_cast<char*>(ctx.ringBase + ctx.writingOff);
        size_t destMax = ctx.ringBufLen - ctx.writingOff;
        size_t count = (ctx.strLen < destMax) ? ctx.strLen : (destMax - 1U);
        (void)vsnprintf_s(strDest, destMax, count, fmt, otherArgs);
        return true;
    }
    // Write formatted string to temp buffer after header.
    // vsnprintf_s to tempBuf first, then copy to ringBuf (string may span boundary)
    uint8_t* tempBuf = g_fifoTempBuf.Get(ctx.ringBufLen);
    if (tempBuf == nullptr) {
        return false;
    }
    char* strDest = reinterpret_cast<char*>(tempBuf);
    size_t destMax = static_cast<size_t>(ctx.ringBufLen);
    size_t count = (ctx.strLen < destMax) ? ctx.strLen : (destMax - 1U);
    (void)vsnprintf_s(strDest, destMax, count, fmt, otherArgs);
    // Copy string to ringbuf at the position after header (may span boundary)
    uint32_t strCopyLen = ctx.tlvLen - sizeof(PrintfInfo);
    uint32_t part1 = tailSpace;
    uint32_t part2 = strCopyLen - part1;
    (void)memcpy_s(ctx.ringBase + ctx.writingOff, tailSpace, tempBuf, part1);
    (void)memcpy_s(ctx.ringBase, ctx.ringBufLen, tempBuf + part1, part2);

    return true;
}

static void ReleaseEntry(uint8_t* ringBase, uint32_t ringBufLen, uint64_t oriBufOff)
{
    /* Ensure all data writes are visible before flipping the type */
    uint32_t readyType = PRINT_TYPE_READY;
    uint32_t wOff = static_cast<uint32_t>(oriBufOff % ringBufLen);
    if (wOff + sizeof(uint32_t) <= ringBufLen) {
        uint32_t* typeField = reinterpret_cast<uint32_t*>(ringBase + wOff);
        __atomic_store_n(typeField, readyType, __ATOMIC_SEQ_CST);
    } else {
        const uint8_t* src = reinterpret_cast<const uint8_t*>(&readyType);
        for (uint32_t i = 0U; i < sizeof(uint32_t); ++i) {
            ringBase[(wOff + i) % ringBufLen] = src[i];
        }
    }
}

/* ── FIFO printf ────────────────────────────────────────────────────── */

static void PrintfFifo(const char* fmt, va_list otherArgs)
{
    AicpuPrintfMemInfo memInfo;
    if (GetAscAicpuPrintfMemInfo(memInfo) != 0) {
        PrintfLegacy(fmt, otherArgs);
        return;
    }

    enable_asc_aicpu_fifo_debug();
    BlockRingBufInfo* blockInfo = GetAicpuDebugBlockInfo(memInfo);
    if (blockInfo == nullptr) {
        return;
    }

    const uint32_t ringBufLen = blockInfo->ringBufLen;
    uint8_t* ringBase = reinterpret_cast<uint8_t*>(blockInfo->ringBufAddr);
    if (ringBase == nullptr || ringBufLen == 0U) {
        return;
    }

    // 1. calculate write info length and check whether there is enough free space
    uint32_t strLen = CalcFormattedLen(fmt, otherArgs);
    uint32_t tlvLen = CalcTlvLen(strLen);
    /* Lock the whole reservation + write to guarantee atomicity, auto unlock wehn return. */
    std::lock_guard<std::mutex> lock(g_aicpuFifoMutex);

    if (!CheckAicpuRingBuf(blockInfo, tlvLen)) {
        return; /* drop on overflow / timeout */
    }
    // 2. prepare PrintInfo header and write WRITING to ringbuf before advancing buffOffset
    RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockInfo);
    uint64_t oriBufOff = writeInfo->bufOffset;
    uint64_t newBufOff = oriBufOff + tlvLen;
    WritePrintInfoHeader(ringBase, ringBufLen, oriBufOff, tlvLen);
    // 3. Advance bufOffset and packIdx together — rts sees WRITING and stops,
    //    and packIdx is consistent with bufOffset.
    AdvanceWriteInfo(writeInfo, newBufOff);
    // 4. Write the formatted string into the ringbuf.
    uint32_t writingOff = static_cast<uint32_t>(oriBufOff + sizeof(PrintfInfo)) % ringBufLen;
    RingBufWriteCtx ctx = {ringBase, ringBufLen, writingOff, tlvLen, strLen};
    if (!WriteFormattedString(ctx, fmt, otherArgs)) {
        RollbackWriteInfo(writeInfo, oriBufOff);
        return;
    }
    // 5. Release the entry to the rts reader
    ReleaseEntry(ringBase, ringBufLen, oriBufOff);
}

/* ── Public API: unified printf entry point ─────────────────────────── */

void printf(const char* fmt, ...)
{
    va_list otherArgs;
    va_start(otherArgs, fmt);

    if (AicpuGetDfxInfo == nullptr) {
        /* rts does not provide FIFO — use legacy linear buffer. */
        PrintfLegacy(fmt, otherArgs);
    } else {
        PrintfFifo(fmt, otherArgs);
    }

    va_end(otherArgs);
}

} // namespace AscendC
