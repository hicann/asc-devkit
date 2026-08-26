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
 * \file kernel_tpipe.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_tpipe.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_H__
#endif

#ifndef ASCENDC_KERNEL_QUEUE_H
#define ASCENDC_KERNEL_QUEUE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/common_types.h"
#include "kernel_common.h"
#include "../../impl/basic_api/kernel_event.h"
#include "kernel_tensor.h"
#include "../../impl/basic_api/kernel_tensor_base.h"
#include "../../impl/basic_api/kernel_tpipe_base.h"
#include "../../impl/basic_api/utils/kernel_utils_ceil_oom_que.h"
#include "../../impl/basic_api/utils/kernel_utils_constants.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#include "stub_fun.h"
#endif

namespace AscendC {
namespace Std {
template <typename... Tps>
class tuple;
}

struct DataCopyParams;

template <TPosition src, TPosition dst, int32_t depth, auto mask = 0>
class TQueBind {
public:
    __aicore__ inline TQueBind();
    __aicore__ inline void FreeBuffer(TBufHandle buf);
    __aicore__ inline TBuffAddr GetBufferAddr(TBufHandle buf);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    template <typename T>
    __aicore__ inline __sync_noalias__ LocalTensor<T> AllocTensor();
    template <typename T>
    __aicore__ inline __sync_noalias__ void AllocTensor(LocalTensor<T>& tensor);
#else
    template <typename T>
    __aicore__ inline __sync_alias__ LocalTensor<T> AllocTensor();
    template <typename T>
    __aicore__ inline __sync_alias__ void AllocTensor(LocalTensor<T>& tensor);
#endif
    template <typename T>
    __aicore__ inline void FreeTensor(LocalTensor<T>& tensor);
    template <typename T>
    __aicore__ inline bool EnQue(const LocalTensor<T>& tensor);
    __aicore__ inline bool EnQue(TBufHandle buf);
    template <TPosition srcUserPos, TPosition dstUserPos, typename T>
    __aicore__ inline bool EnQue(const LocalTensor<T>& tensor);
    template <typename T>
    __aicore__ inline void DeQue(LocalTensor<T>& tensor);
    template <typename T>
    __aicore__ inline LocalTensor<T> DeQue();
    __aicore__ inline TBufHandle DeQue();
    template <TPosition srcUserPos, TPosition dstUserPos, typename T>
    __aicore__ inline LocalTensor<T> DeQue();
    __aicore__ inline bool VacantInQue();
    __aicore__ inline bool HasTensorInQue();
    __aicore__ inline int32_t GetTensorCountInQue();
    __aicore__ inline bool HasIdleBuffer();
    __aicore__ inline void FreeAllEvent();
    template <typename T>
    __aicore__ inline TBufState GetState(const LocalTensor<T>& tensor) const;
    __aicore__ inline void InitStartBufHandle(TBufHandle startBufhandle, uint8_t num, uint32_t len);
    template <typename T>
    __aicore__ inline void InitBufHandle(
        T* bufPool, uint32_t index, TBufHandle bufhandle, uint32_t curPoolAddr, uint32_t len);

protected:
    static constexpr TQueConfig config = GetTQueConfig(mask);
    static constexpr bool nd2nz = config.nd2nz;
    static constexpr bool nz2nd = config.nz2nd;
    static constexpr bool scmBlockGroup = config.scmBlockGroup;
    static constexpr bool enableLoopQueue = config.enableLoopQueue;
    static constexpr TPosition srcPosition = src;
    static constexpr TPosition dstPosition = dst;
    static constexpr Hardware srcHardType = GetPhyType(src);
    static constexpr Hardware dstHardType = GetPhyType(dst);
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    static constexpr HardEvent enQueEvt = GetQueEvt(srcPosition, dstPosition, true);
    static constexpr HardEvent freeBufEvt = GetQueEvt(srcPosition, dstPosition, false);
#else
    static constexpr HardEvent enQueEvt = GetQueEvt(srcHardType, dstHardType, true, nd2nz, nz2nd);
    static constexpr HardEvent freeBufEvt = GetQueEvt(srcHardType, dstHardType, false, nd2nz, nz2nd);
#endif
    static constexpr int32_t queDepth = depth;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
    static constexpr bool enableGlobalManageQue = EnableGlobalManageQue<GetBufferPos(src, dst)>(config);
    static constexpr pipe_t srcPipe = GetPipe(srcHardType, dstHardType, true);
    static constexpr pipe_t dstPipe = GetPipe(srcHardType, dstHardType, false);
    static constexpr int32_t maxBlockNum = 32;
    static constexpr uint8_t shiftBits = CalculatesShiftedBit(config.bufferLen);
    static constexpr uint8_t maxBufferBlock = GlobalManageQueConfig<GetBufferPos(src, dst)>::maxBufferBlock;
    static constexpr uint8_t bufIdOffset = GlobalManageQueConfig<GetBufferPos(src, dst)>::bufIdOffset;
#endif
    union {
        uint64_t value;
        struct {
            uint8_t bufNum = 0;
            uint8_t usedCount;
            uint16_t head;
            uint16_t tail;
            uint8_t bufUsedCount;
            uint8_t bufCursor;
        };

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
        struct {
            uint32_t freeMask;
            uint8_t staticHead;
            uint8_t staticEnqueHead;
            uint8_t staticUsedCount;
            uint8_t staticBufUsedCount;
        };
#endif
    };
    typename TBufHandleAux<depth>::T que_;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    typename BufInfoAux<enableGlobalManageQue, config>::type bufStart;
#else
    struct TBufType* bufStart;
#endif
    DEBUG_CODE(uint32_t bufLen);
    friend class TPipe;
    template <TPosition pos, int32_t d, auto m>
    friend class TQue;
    template <TPosition pos, uint32_t bufIDSize>
    friend class TBufPool;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    template <TPosition bufPos, uint32_t bufIDSize>
    friend class TBufPoolExtImpl;
#endif
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    uint64_t bufPoolHandle{0U};
#endif
private:
    __aicore__ inline void SetTBufPoolHandle(uint64_t bufPoolHandle);
    template <typename T>
    __aicore__ inline LocalTensor<T> Buf2Tensor(TBufHandle buf);
    __aicore__ inline TBufState GetState(const TBufHandle& handle) const;
    static constexpr bool isTQue = true;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    __aicore__ inline uint64_t GetNext(const int32_t len = 1);
#endif
    __aicore__ inline TBufHandle AllocBuffer();
    template <TPosition srcUserPos, TPosition dstUserPos>
    __aicore__ inline bool EnQue(TBufHandle buf);
    template <TPosition srcUserPos, TPosition dstUserPos>
    __aicore__ inline TBufHandle DeQue();
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    template <typename IMPL, typename A, typename B, typename L0cT, class C, const auto& MM_CFG, typename>
    friend class Impl::Detail::CubeOutBuffer;
    template <typename T>
    __aicore__ inline __sync_noalias__ LocalTensor<T> AllocTensor(int32_t num);
    template <typename T>
    friend __aicore__ inline uint64_t GetTQueHeadAddr(const T& que);
#endif
    template <typename T>
    friend __aicore__ constexpr bool UseBufIdSync();
};

// Template Args:
// pos - position for queue, such as VECIN/VECOUT/A1...
// mask - the 0th bit is nd2nz, 1 means data trans from nd format to nz format
//        the 1st bit is nz2nd, 1 means data trans from nz format to nd format
template <TPosition pos, int32_t depth, auto mask = 0>
class TQue : public TQueBind<GetBufferLogicPos(pos, true), GetBufferLogicPos(pos, false), depth, mask> {
public:
    __aicore__ inline TQue() = default;

private:
    friend class TPipe;
    template <TPosition bufPos, uint32_t bufIDSize>
    friend class TBufPool;
    static constexpr bool isTQue = true;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    template <TPosition bufPos, uint32_t bufIDSize>
    friend class TBufPoolExtImpl;
    template <typename T>
    friend __aicore__ inline uint64_t GetTQueHeadAddr(const T& que);
#endif
};

template <TPosition pos = TPosition::LCM>
class TBuf : public TQueBind<pos, pos, 0, 0> {
public:
    __aicore__ inline TBuf() = default;
    template <typename T>
    __aicore__ inline LocalTensor<T> Get();
    template <typename T>
    __aicore__ inline LocalTensor<T> Get(uint32_t len);
    template <typename T>
    __aicore__ inline LocalTensor<T> GetWithOffset(uint32_t size, uint32_t bufOffset);
    // inheritance function from Class TQueBind
    template <typename T>
    __aicore__ inline void EnQue(const LocalTensor<T>& tensor);
    template <typename T>
    __aicore__ inline LocalTensor<T> DeQue();
    template <typename T>
    __aicore__ inline LocalTensor<T> AllocTensor();
    template <typename T>
    __aicore__ inline void FreeTensor(LocalTensor<T>& tensor);
    template <typename T>
    __aicore__ inline TBufState GetState(const LocalTensor<T>& tensor) const;
    __aicore__ inline bool EnQue(TBufHandle buf);
    __aicore__ inline TBufHandle DeQue();
    __aicore__ inline void FreeBuffer(TBufHandle buf);
    __aicore__ inline TBuffAddr GetBufferAddr(TBufHandle buf);
    __aicore__ inline void InitStartBufHandle(TBufHandle startBufhandle, uint8_t num, uint32_t len);

private:
    __aicore__ inline TBufHandle Get();
    __aicore__ inline TBufHandle Get(uint32_t len);
    __aicore__ inline uint32_t GetBufLen() const;
    __aicore__ inline void SetTpipeBuf(TBufType* bufStartIn, uint32_t bufLenIn);
    template <TPosition posPopBuffer>
    friend __aicore__ inline bool PopStackBuffer(TBuf<posPopBuffer>& popBuffer, TBufType& bufStart);
    __aicore__ inline TBufHandle AllocBuffer();

private:
    struct TBufType* bufStart;
    uint32_t bufLen;
    uint32_t offset;
    friend class TPipe;
    template <TPosition bufPos, uint32_t bufIDSize>
    friend class TBufPool;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    template <TPosition bufPos, uint32_t bufIDSize>
    friend class TBufPoolExtImpl;
#endif
    static constexpr bool isTQue = false;
};
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
template <TPosition pos, uint32_t bufIDSize = defaultBufIDSize>
class TBufPool : public TBufPoolExtImpl<pos, bufIDSize> {
public:
    __aicore__ inline TBufPool() = default;
    __aicore__ inline ~TBufPool();
};
#else
template <TPosition pos, uint32_t bufIDSize = defaultBufIDSize>
class TBufPool {
public:
    static constexpr TPosition poolPos = pos;

public:
    __aicore__ inline TBufPool();
    __aicore__ inline ~TBufPool();
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

private:
    friend class TPipe;
    template <TPosition src, TPosition dst, int32_t depth, auto mask>
    friend class TQueBind;
    template <TPosition bufPos, int32_t depth, auto mask>
    friend class TQue;
    template <TPosition bufPos>
    friend class TBuf;
    static constexpr bool isTbufPool = true;
};
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#define EXTERN_IMPL_BUFPOOL(EXT_BUFPOOL, POSITION, BUFID_SIZE)                           \
public:                                                                                  \
    static constexpr AscendC::TPosition poolPos = POSITION;                              \
    static constexpr int32_t bufSize = BUFID_SIZE;                                       \
    static constexpr bool isTbufPool = true;                                             \
    __aicore__ inline ~EXT_BUFPOOL() { Reset(); }                                        \
    __aicore__ inline void Reset()                                                       \
    {                                                                                    \
        auto ptr = this->tBufPoolImpl.buf_;                                              \
        if constexpr (GetPhyType(poolPos) == Hardware::UB) {                             \
            PipeBarrier<PIPE_MTE2>();                                                    \
            PipeBarrier<PIPE_MTE3>();                                                    \
            PipeBarrier<PIPE_V>();                                                       \
        } else {                                                                         \
            uint8_t i = 0;                                                               \
            do {                                                                         \
                if (ptr->freeBufEvtID != INVALID_TEVENTID) {                             \
                    WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);                    \
                    ptr->freeBufEvtID = INVALID_TEVENTID;                                \
                }                                                                        \
                i++;                                                                     \
                ptr++;                                                                   \
            } while (i < this->tBufPoolImpl.curBufSize_);                                \
            GetBuffImpl<PIPE_MTE1, true>(31);                                            \
            ReleaseBuffImpl<PIPE_MTE1, true>(31);                                        \
            GetBuffImpl<PIPE_MTE2, false>(31);                                           \
            ReleaseBuffImpl<PIPE_MTE2, false>(31);                                       \
        }                                                                                \
        tBufPoolImpl.curBufSize_ = 0;                                                    \
        tBufPoolImpl.maxAddr_ = tBufPoolImpl.startAddr_;                                 \
    }                                                                                    \
    __aicore__ inline void Init()                                                        \
    {                                                                                    \
        constexpr auto pool = AscendC::GetPhyType(poolPos);                              \
        static_assert(                                                                   \
            (pool == AscendC::Hardware::L1 || pool == AscendC::Hardware::UB),            \
            "TbufPool Position should be L1 Buffer(A1/B1/C1)/UB(VECIN/VECOUT/VECCALC)"); \
        ResetPool();                                                                     \
        tBufPoolImpl.bufIdPool_ = 0;                                                     \
        tBufPoolImpl.availableIdMask_ = 0;                                               \
        tBufPoolImpl.isReset_ = true;                                                    \
    }                                                                                    \
    __aicore__ inline AscendC::TBufHandle GetBufHandle(uint8_t offset)                   \
    {                                                                                    \
        return reinterpret_cast<AscendC::TBufHandle>(this->tBufPoolImpl.buf_ + offset);  \
    }                                                                                    \
    __aicore__ inline void SetCurAddr(uint32_t curAddr)                                  \
    {                                                                                    \
        this->tBufPoolImpl.maxAddr_ = curAddr;                                           \
        return;                                                                          \
    }                                                                                    \
    __aicore__ inline uint32_t GetCurAddr() { return this->tBufPoolImpl.maxAddr_; }      \
    __aicore__ inline void SetCurBufSize(uint8_t curBufSize)                             \
    {                                                                                    \
        this->tBufPoolImpl.curBufSize_ = curBufSize;                                     \
        return;                                                                          \
    }                                                                                    \
    __aicore__ inline uint8_t GetCurBufSize() { return this->tBufPoolImpl.curBufSize_; } \
                                                                                         \
protected:                                                                               \
    AscendC::TBufPoolImpl<bufSize> tBufPoolImpl;                                         \
                                                                                         \
private:                                                                                 \
    __aicore__ inline void ResetPool()                                                   \
    {                                                                                    \
        tBufPoolImpl.curBufSize_ = 0;                                                    \
        tBufPoolImpl.startAddr_ = 0;                                                     \
        tBufPoolImpl.maxAddr_ = 0;                                                       \
        tBufPoolImpl.maxLen_ = 0;                                                        \
    }                                                                                    \
                                                                                         \
private:                                                                                 \
    friend class AscendC::TPipe;                                                         \
    template <AscendC::TPosition src, AscendC::TPosition dst, int32_t depth, auto mask>  \
    friend class AscendC::TQueBind;                                                      \
    template <AscendC::TPosition bufPos, int32_t depth, auto mask>                       \
    friend class AscendC::TQue;                                                          \
    template <AscendC::TPosition bufPos>                                                 \
    friend class AscendC::TBuf

#else

#define EXTERN_IMPL_BUFPOOL(EXT_BUFPOOL, POSITION, BUFID_SIZE)                           \
public:                                                                                  \
    static constexpr AscendC::TPosition poolPos = POSITION;                              \
    static constexpr int bufIDSize = BUFID_SIZE;                                         \
    static constexpr bool isTbufPool = true;                                             \
    __aicore__ inline ~EXT_BUFPOOL() { Reset(); }                                        \
    __aicore__ inline void Reset()                                                       \
    {                                                                                    \
        auto ptr = this->tBufPoolImpl.buf_;                                              \
        for (uint8_t i = 0; i < this->tBufPoolImpl.curBufSize_; i++, ptr++) {            \
            if (ptr->freeBufEvtID != AscendC::INVALID_TEVENTID) {                        \
                AscendC::WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);               \
                ptr->freeBufEvtID = AscendC::INVALID_TEVENTID;                           \
            }                                                                            \
        }                                                                                \
        ResetPool();                                                                     \
    }                                                                                    \
    __aicore__ inline void Init()                                                        \
    {                                                                                    \
        constexpr auto pool = AscendC::GetPhyType(poolPos);                              \
        static_assert(                                                                   \
            (pool == AscendC::Hardware::L1 || pool == AscendC::Hardware::UB),            \
            "TbufPool Position should be L1 Buffer(A1/B1/C1)/UB(VECIN/VECOUT/VECCALC)"); \
        ResetPool();                                                                     \
        tBufPoolImpl.isReset_ = true;                                                    \
    }                                                                                    \
    __aicore__ inline AscendC::TBufHandle GetBufHandle(uint8_t offset)                   \
    {                                                                                    \
        return reinterpret_cast<AscendC::TBufHandle>(this->tBufPoolImpl.buf_ + offset);  \
    }                                                                                    \
    __aicore__ inline void SetCurAddr(uint32_t curAddr)                                  \
    {                                                                                    \
        this->tBufPoolImpl.maxAddr_ = curAddr;                                           \
        return;                                                                          \
    }                                                                                    \
    __aicore__ inline uint32_t GetCurAddr() { return this->tBufPoolImpl.maxAddr_; }      \
    __aicore__ inline void SetCurBufSize(uint8_t curBufSize)                             \
    {                                                                                    \
        this->tBufPoolImpl.curBufSize_ = curBufSize;                                     \
        return;                                                                          \
    }                                                                                    \
    __aicore__ inline uint8_t GetCurBufSize() { return this->tBufPoolImpl.curBufSize_; } \
                                                                                         \
protected:                                                                               \
    AscendC::TBufPoolImpl<bufIDSize> tBufPoolImpl;                                       \
                                                                                         \
private:                                                                                 \
    __aicore__ inline void ResetPool()                                                   \
    {                                                                                    \
        tBufPoolImpl.curBufSize_ = 0;                                                    \
        tBufPoolImpl.startAddr_ = 0;                                                     \
        tBufPoolImpl.maxAddr_ = 0;                                                       \
        tBufPoolImpl.maxLen_ = 0;                                                        \
    }                                                                                    \
                                                                                         \
private:                                                                                 \
    friend class AscendC::TPipe;                                                         \
    template <AscendC::TPosition src, AscendC::TPosition dst, int32_t depth, auto mask>  \
    friend class AscendC::TQueBind;                                                      \
    template <AscendC::TPosition bufPos, int32_t depth, auto mask>                       \
    friend class AscendC::TQue;                                                          \
    template <AscendC::TPosition bufPos>                                                 \
    friend class AscendC::TBuf

#endif

class TPipe : public TPipeBase {
public:
#if defined(__NPU_ARCH__)
    __aicore__ inline TPipe();
    __aicore__ inline ~TPipe();
#else
    __aicore__ inline TPipe() {}
    __aicore__ inline ~TPipe() {}
#endif
    __aicore__ inline void Init();
    template <class T>
    __aicore__ inline bool InitBuffer(T& que, uint8_t num, uint32_t len);
    template <class T, class U, class V, class... Addrs>
    __aicore__ inline bool InitBuffer(T& que, const Std::tuple<U, V>& addr0, const Addrs&... addrs);
    template <TPosition pos>
    __aicore__ inline bool InitBuffer(TBuf<pos>& buf, uint32_t len);
    template <class T>
    __aicore__ inline bool InitBufPool(T& bufPool, uint32_t len);
    template <class T, class U>
    __aicore__ inline bool InitBufPool(T& bufPool, uint32_t len, U& shareBuf);
    template <HardEvent evt>
    __aicore__ inline TEventID AllocEventID();
    template <HardEvent evt>
    __aicore__ inline void ReleaseEventID(TEventID id);
    template <HardEvent evt>
    __aicore__ inline TEventID FetchEventID();
    __aicore__ inline TEventID FetchEventID(HardEvent evt);
    // NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. Please do not use it!
    template <TPosition pos, typename T>
    __aicore__ inline LocalTensor<T> GetAbsAddr(int32_t offset, int32_t size) const;
    // NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. Please do not use it!
    template <TPosition pos>
    __aicore__ inline TBuffAddr GetAbsAddr(int32_t offset, int32_t len) const;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
    __aicore__ inline MutexID AllocMutexID();
    __aicore__ inline void ReleaseMutexID(MutexID id);
#endif
    /*
     * brief: these functions are used to use spm buffer;
     * demo case:
     * GlobalTensor<T> workTensor;
     * tpipe.InitSpmBuffer(workTensor, size);
     * LocalTensor<T> calcTensor = tpip.Get<T>(size);
     * // when local buffer is not enough, spill local to spm buffer;
     * tpipe.WriteSpmBuffer(calcTensor, size);
     * // ...
     * // read buffer from spm buffer into local
     * tpipe.ReadSpmBuffer(calcTensor, size);
     */
    template <typename T>
    __aicore__ inline void InitSpmBuffer(const GlobalTensor<T>& workspace, const int32_t bufferSize);
    __aicore__ inline void InitSpmBuffer(const int32_t bufferSize);
    template <typename T>
    __aicore__ inline void WriteSpmBuffer(
        const LocalTensor<T>& writeBuffer, const DataCopyParams& copyParams, int32_t writeOffset = 0);
    template <typename T>
    __aicore__ inline void ReadSpmBuffer(
        const LocalTensor<T>& readBuffer, const DataCopyParams& copyParams, int32_t readOffset = 0);
    template <typename T>
    __aicore__ inline void WriteSpmBuffer(
        const LocalTensor<T>& writeBuffer, const int32_t writeSize, int32_t writeOffset = 0);
    template <typename T>
    __aicore__ inline void ReadSpmBuffer(
        const LocalTensor<T>& readBuffer, const int32_t readSize, int32_t readOffset = 0);
    __aicore__ inline void DestroyWithoutPipeAll();
    __aicore__ inline void Destroy();
    __aicore__ inline void Reset();
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    // NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. Please do not use it!
    template <typename T>
    inline uint64_t GetAbsAddr(const LocalTensor<T>& tensor);
    inline uint8_t* GetBaseAddr(int8_t logicPos);
#endif

protected:
    template <TPosition src, TPosition dst, int32_t depth, auto mask>
    friend class TQueBind;
    template <TPosition pos, int32_t depth, auto mask>
    friend class TQue;
    template <TPosition pos>
    friend class TBuf;
    template <TPosition pos, uint32_t bufIDSize>
    friend class TBufPool;
    template <TPosition pos>
    friend __aicore__ inline bool PopStackBuffer(TBuf<pos>& popBuffer, TBufType& bufStart);
    template <typename T, TPosition pos>
    friend __aicore__ inline bool PopStackBuffer(LocalTensor<T>& popBuffer);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    void inline SetBufferCtx(Hardware hard, struct BufPoolExtra* bufPool);
#endif

private:
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    template <typename T>
    friend inline uint64_t GetAbsAddr(TPipe* tpipe, const LocalTensor<T>& tensor);
#endif
    friend __aicore__ inline void InitShareBufStart(
        TPipe* tpipe, uint32_t mode, uint32_t* shareLens, uint32_t lens, uint8_t subBlockIdx);
    friend __aicore__ inline void InitShareBufEnd(TPipe* tpipe);
    __aicore__ inline void InitSocState() const;
    __aicore__ inline void ResetPool();
    template <class T>
    __aicore__ inline bool TscmInitBuffer(T& que, uint8_t num, uint32_t len);
    template <class T, class First, class... Rest>
    __aicore__ inline void AllocAddrs(TBufType* ptr, const First& addr, const Rest&... addrs);
    /*
     * brief: these functions are used to get end and queueend addr.
     */
    template <TPosition pos>
    __aicore__ inline uint64_t GetQueueEndAddress();
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    __aicore__ inline TBufId AllocTscmBufId();
    __aicore__ inline int8_t AllocCrossSyncId();
#endif
};

template <TPosition pos, int32_t depth = 1, auto mask = 0>
using TSCM = TQueBind<pos, TPosition::TSCM, depth, mask>;
} // namespace AscendC

#include "../../impl/basic_api/kernel_tpipe_impl.h"
#endif // ASCENDC_KERNEL_QUEUE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_H__
#endif
