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
 * \file kernel_tpipe_base.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_tpipe_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_BASE_H__
#endif
#ifndef ASCENDC_MODULE_TPIPE_BASE_H
#define ASCENDC_MODULE_TPIPE_BASE_H

namespace AscendC {
// begin base define of tquebind
template <int depth>
struct TBufHandleAux {
    using T = TBufHandle[depth];
};

template <>
struct TBufHandleAux<1> {
    using T = TBufHandle;
};
constexpr TEventID INVALID_TEVENTID = (static_cast<TEventID>(-1));
#if (__NPU_ARCH__ == 3510)
constexpr uint8_t INVALID_STATIC_ENQUE_HEAD = 0xff;
constexpr int32_t MIN_BUFFER_BLOCK_SIZE = 32 * 1024;
constexpr uint8_t TSCM_BUFID_MAX = 20;

template <Hardware hardType>
struct GlobalManageQueConfig {
    static constexpr uint8_t maxBufferBlock = 0;
    static constexpr uint8_t bufIdOffset = 0;
    static constexpr uint32_t totalSize = 0;
};

template <>
struct GlobalManageQueConfig<Hardware::L1> {
    static constexpr uint8_t maxBufferBlock = 16;
    static constexpr uint8_t bufIdOffset = 0;
    static constexpr uint32_t totalSize = TOTAL_L1_SIZE;
};

template <>
struct GlobalManageQueConfig<Hardware::L0A> {
    static constexpr uint8_t maxBufferBlock = 2;
    static constexpr uint8_t bufIdOffset = 28;
    static constexpr uint32_t totalSize = TOTAL_L0A_SIZE;
};

template <>
struct GlobalManageQueConfig<Hardware::L0B> {
    static constexpr uint8_t maxBufferBlock = 2;
    static constexpr uint8_t bufIdOffset = 30;
    static constexpr uint32_t totalSize = TOTAL_L0B_SIZE;
};

template <>
struct GlobalManageQueConfig<Hardware::L0C> {
    static constexpr uint8_t maxBufferBlock = 8;
    static constexpr uint8_t bufIdOffset = 20;
    static constexpr uint32_t totalSize = TOTAL_L0C_SIZE;
};

template <Hardware hardType>
__aicore__ constexpr bool EnableGlobalManageQue(const TQueConfig& config)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    return config.bufferLen > 0 && config.bufferNumber > 0 &&
           config.bufferLen * config.bufferNumber == GlobalManageQueConfig<hardType>::totalSize;
#else
    return false;
#endif
}

template <bool enableGlobalManageQue, const TQueConfig& config>
struct BufInfoAux {
    using type = struct TBufType*;
};

template <const TQueConfig& config>
struct BufInfoAux<true, config> {
    static constexpr uint8_t bufNum = config.bufferNumber;
    struct BufAux {
        struct TBufType bufInfo[bufNum];
    };
    using type = BufAux;
};
#endif

// begin base define of tpipe
struct TEventPool {
    uint64_t eventOccupy;
};

struct TPipeBufPool {
    uint32_t maxAddr;
};

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
struct BufPoolExtra {
    uint8_t* absAddr;
    uint32_t phySpace;
};
#endif

struct TShareBuf {
    enum class ShareHard : uint8_t { // Redefine to save resources
        L1 = 0,
        L0C = 1,
        UB = 2,
        MAX,
    };
    int32_t start[static_cast<uint8_t>(ShareHard::MAX)];
    int32_t maxAddr[static_cast<uint8_t>(ShareHard::MAX)];
    DEBUG_CODE(uint32_t length[static_cast<uint8_t>(ShareHard::MAX)]);
};

struct SpmInfo {
    uint64_t spmAddr;
    int32_t spmBuffSize;
    uint8_t spmBufType;
};

struct TPipeImpl {
    struct TEventPool eventPool_[EVENT_NUM];
    struct TPipeBufPool bufPool_[static_cast<uint8_t>(Hardware::MAX)];
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    BufPoolExtra bufPoolBaseAddr_[static_cast<uint8_t>(Hardware::MAX)];
#endif
    struct TBufType buf_[QBUF_MAX_LEN];
    TShareBuf shareBufPool_;
    SpmInfo spmInfo_;
    // the tscm buffer addr
    uint32_t tscmBufferPtr_;
#if (__NPU_ARCH__ == 3510)
    TBufId bufIdPool_;
    TBufId tscmBufIdPool_;
    TBufId sharedEvtId_;
    int8_t crossSyncId_;
#endif
    uint8_t curBufSize_;
    bool isDestroy;
};

constexpr uint32_t defaultBufIDSize = 4;

template <uint32_t bufIDSize = defaultBufIDSize>
struct TBufPoolImpl {
    struct TBufType buf_[bufIDSize];
    uint32_t startAddr_;
    uint32_t maxAddr_;
    uint32_t maxLen_;
    uint8_t curBufSize_;
#if (__NPU_ARCH__ == 3510)
    uint32_t bufIdPool_;
    uint32_t availableIdMask_;
#endif
    uint8_t isReset_;
};

class TPipeBase {
public:
    __aicore__ inline void InitShareBufStart(uint32_t mode, uint32_t* shareLens, uint32_t lens, uint8_t subBlockIdx);
    __aicore__ inline void InitShareBufEnd();

protected:
    TPipeImpl g_tpipeImpl;
    __aicore__ inline void AuxShareBufStart(
        uint32_t mode, uint32_t* shareLens, uint8_t pos, Hardware hard, uint8_t subBlockIdx);
};

__aicore__ inline void TPipeBase::InitShareBufStart(
    uint32_t mode, uint32_t* shareLens, uint32_t lens, uint8_t subBlockIdx)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_DEBUG_ASSERT(
        (lens == static_cast<uint32_t>(TShareBuf::ShareHard::MAX)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "lens is %u, which should be %u", lens, static_cast<uint32_t>(TShareBuf::ShareHard::MAX)));
#else
    (void)(lens);
#endif

    ASCENDC_DEBUG_ASSERT(
        (subBlockIdx == 0 || subBlockIdx == 1),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "subBlockIdx is %d, which should only be 0/1", subBlockIdx));
    AuxShareBufStart(mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::L1), Hardware::L1, subBlockIdx);
    AuxShareBufStart(mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::L0C), Hardware::L0C, subBlockIdx);
#if (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    AuxShareBufStart(mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::UB), Hardware::UB, subBlockIdx);
#endif
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0A)].maxAddr = 0;
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0B)].maxAddr = 0;
    // v100 Shouldn't Use Bias Table
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::BIAS)].maxAddr = 0;
#if (__NPU_ARCH__ == 3510)
    Internal::g_sharedEvtId = Internal::g_bufId;
#endif

    return;
}

__aicore__ inline void TPipeBase::InitShareBufEnd()
{
    // debug methods need to be added.
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr =
        g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::L1)];
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0C)].maxAddr =
        g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::L0C)];
#if (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::UB)].maxAddr =
        g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::UB)];
#endif

#if (__NPU_ARCH__ == 3510)
    Internal::g_bufId = Internal::g_sharedEvtId;
#endif

    return;
}

__aicore__ inline void TPipeBase::AuxShareBufStart(
    uint32_t mode, uint32_t* shareLens, uint8_t pos, Hardware hard, uint8_t subBlockIdx)
{
    uint8_t hardU8 = static_cast<uint8_t>(hard);
    if (unlikely(g_tpipeImpl.shareBufPool_.start[pos] == -1)) { // The address has not been initialized.
        // Record the maximum allocated address.
        g_tpipeImpl.shareBufPool_.start[pos] = this->g_tpipeImpl.bufPool_[hardU8].maxAddr;
        g_tpipeImpl.shareBufPool_.maxAddr[pos] = g_tpipeImpl.shareBufPool_.start[pos] + shareLens[pos];
        DEBUG_CODE(g_tpipeImpl.shareBufPool_.length[pos] = shareLens[pos]);
    } else {
        DEBUG_CODE(
            g_tpipeImpl.shareBufPool_.length[pos] = g_tpipeImpl.shareBufPool_.length[pos] > shareLens[pos] ?
                                                        g_tpipeImpl.shareBufPool_.length[pos] :
                                                        shareLens[pos]);
        // Record the maximum allocated address.
        g_tpipeImpl.shareBufPool_.maxAddr[pos] = this->g_tpipeImpl.bufPool_[hardU8].maxAddr;
        g_tpipeImpl.bufPool_[hardU8].maxAddr = g_tpipeImpl.shareBufPool_.start[pos]; // Reset resource start position.
    }

    if (mode == 1 && subBlockIdx == 1) {
        this->g_tpipeImpl.bufPool_[hardU8].maxAddr += shareLens[pos] / HALF_FACTOR; // Reset resource start position.
    }

#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
    ASCENDC_DEBUG_ASSERT(
        (g_tpipeImpl.shareBufPool_.length[pos] >= shareLens[pos]),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "share buf addr is %u, exceeds the limit %u", shareLens[pos],
            g_tpipeImpl.shareBufPool_.length[pos]));
#endif
}

namespace Impl {
namespace Detail {
template <typename IMPL, typename A, typename B, typename L0cT, class C, const auto& MM_CFG, typename>
class CubeOutBuffer;
}
} // namespace Impl
template <TPosition pos>
class TBuf;

template <TPosition pos, uint32_t bufIDSize>
class TBufPoolExtImpl {
public:
    static constexpr TPosition poolPos = pos;
    // Get Queue Free Event
    static constexpr TPosition srcPos = GetBufferLogicPos(pos, true);
    static constexpr TPosition dstPos = GetBufferLogicPos(pos, false);
    static constexpr Hardware srcHardType = GetPhyType(srcPos);
    static constexpr Hardware dstHardType = GetPhyType(dstPos);
    static constexpr HardEvent freeBufEvt = GetQueEvt(srcHardType, dstHardType, false, 0, 0);
    static constexpr int32_t bufSize = bufIDSize;

    __aicore__ inline TBufPoolExtImpl();
    __aicore__ inline ~TBufPoolExtImpl() = default;
    template <class T>
    __aicore__ inline bool InitBuffer(T& que, uint8_t num, uint32_t len);
    template <TPosition bufPos>
    __aicore__ inline bool InitBuffer(TBuf<bufPos>& buf, uint32_t len);
    template <class T, class U>
    __aicore__ inline bool InitBufPool(T& bufPool, uint32_t len, U& shareBuf);
    template <class T>
    __aicore__ inline bool InitBufPool(T& bufPool, uint32_t len);
    __aicore__ inline void Reset();

protected:
    TBufPoolImpl<bufIDSize> tBufPoolImpl;

private:
    __aicore__ inline void Init();
    __aicore__ inline void ResetPool();
    friend class TPipe;
    template <class T>
    __aicore__ inline bool InitConstBuffer(T& que, uint8_t num, uint32_t len);
    template <TPosition src, TPosition dst, int32_t depth, auto mask>
    friend class TQueBind;
    template <TPosition bufPos, int32_t depth, auto mask>
    friend class TQue;
    template <TPosition bufPos>
    friend class TBuf;
    static constexpr bool isTbufPool = true;
};

namespace Internal {
__aicore__ inline void ResetTPipePtr()
{
#ifdef SPLIT_CORE_CUBE
    g_cubeTPipePtr = nullptr;
#elif defined(SPLIT_CORE_VEC)
    g_vecTPipePtr = nullptr;
#else
    g_tPipePtr = nullptr;
#endif
}
} // namespace Internal

} // namespace AscendC
#endif // ASCENDC_MODULE_TPIPE_BASE_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_BASE_H__
#endif
