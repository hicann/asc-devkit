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
 * \file kernel_tpipe_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_tpipe_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_TPIPE_INTERFACE_IMPL_H
#define ASCENDC_MODULE_TPIPE_INTERFACE_IMPL_H
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "dav_3510/kernel_tpipe_impl_c310.h"
#include "dav_3510/kernel_tpipe_impl_c310_vec.h"
#include "kernel_tquesync_impl.h"
#else
#include <type_traits>
#include "kernel_tquebind_impl.h"
#include "kernel_tquesync_impl.h"
#include "kernel_tbufpool_impl.h"
#include "kernel_tbuf_impl.h"
#include "../../include/basic_api/kernel_struct_data_copy.h"
#include "kernel_utils.h"
#include "../../include/basic_api/kernel_operator_sys_var_intf.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include <map>
#include <random>
#include "stub_def.h"
#include "stub_fun.h"
#endif

namespace AscendC {
template <TPosition pos>
__aicore__ inline uint64_t TransUBAddr(uint64_t addr)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    addr = addr - reinterpret_cast<uint64_t>(ConstDefiner::Instance().hardwareCpuBufferMap.at(positionHardMap.at(pos)));
#endif
    return addr;
}

#if defined(__NPU_ARCH__)
__aicore__ inline void PrintTimeStamp(uint32_t descId);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
inline uint8_t* GetBaseAddrCpu(int8_t logicPos)
{
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_DEBUG_ASSERT(
        (positionHardMap.find((TPosition)logicPos) != positionHardMap.end()),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "illegal logicPos %d ", int32_t(logicPos)));
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_DEBUG_ASSERT(
        (hardType != Hardware::GM), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "hardware position can not be gm"));
    return ConstDefiner::Instance().GetHardwareBaseAddr(hardType);
}
#endif

// begin impl of tpipe
__aicore__ inline TPipe::TPipe()
{
    InitSocState();
    Init();
}

__aicore__ inline TPipe::~TPipe()
{
    if (g_tpipeImpl.isDestroy) {
        return;
    }
    Destroy();
};

__aicore__ inline void TPipe::Init()
{
    // disabled on CPU mode for compatiablilty
#if defined(ASCENDC_DEBUG) && !defined(ASCENDC_CPU_DEBUG)
    ASCENDC_DEBUG_ASSERT(
        GetTPipePtr() == nullptr,
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "TPipe has already been constructed at %p!\n", (void*)g_lastTpipeInitPos));
    g_lastTpipeInitPos = get_pc();
#endif
    ResetPool();
    // for matmul macro, set flag M_MTE1 at the begining of operator, and also wait flag at the end.
    // matmul macro only use M_MTE1 event id 0 1 currently.
#if __NPU_ARCH__ == 2201
    if ASCEND_IS_AIC {
        auto enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
        ASCENDC_DEBUG_ASSERT((enQueEvtID == 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "enQueEvtID should be 0"));
        SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
        enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
        ASCENDC_DEBUG_ASSERT((enQueEvtID == 1), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "enQueEvtID should be 1"));
        SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
        // For load Bias
        enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
        ASCENDC_DEBUG_ASSERT((enQueEvtID == 2), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "enQueEvtID should be 2"));

        SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
    }
#elif (                                                                                             \
    __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || \
    __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    auto enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
    ASCENDC_DEBUG_ASSERT((enQueEvtID == 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "enQueEvtID should be 0"));
    SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));

    enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
    ASCENDC_DEBUG_ASSERT((enQueEvtID == 1), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "enQueEvtID should be 1"));
    SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
    // For load Bias
    enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
    ASCENDC_DEBUG_ASSERT((enQueEvtID == 2), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "enQueEvtID should be 2"));
    SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
#endif

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++) {
        if (bufferInitLen.find((Hardware)i) != bufferInitLen.end()) {
            SetBufferCtx((Hardware)i, &g_tpipeImpl.bufPoolBaseAddr_[i]);
        }
    }
    AscendCBufAbsAddr(
        uint8_t(Hardware::UB), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuUB)),
        bufferInitLen.at(Hardware::UB));
    AscendCBufAbsAddr(
        uint8_t(Hardware::L1), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL1)),
        bufferInitLen.at(Hardware::L1));
    AscendCBufAbsAddr(
        uint8_t(Hardware::L0A), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0A)),
        bufferInitLen.at(Hardware::L0A));
    AscendCBufAbsAddr(
        uint8_t(Hardware::L0B), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0B)),
        bufferInitLen.at(Hardware::L0B));
    AscendCBufAbsAddr(
        uint8_t(Hardware::L0C), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuL0C)),
        bufferInitLen.at(Hardware::L0C));
    AscendCBufAbsAddr(
        uint8_t(Hardware::BIAS), static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuBIAS)),
        bufferInitLen.at(Hardware::BIAS));
    AscendCBufAbsAddr(
        uint8_t(Hardware::FIXBUF),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(ConstDefiner::Instance().cpuFIXBUF)),
        bufferInitLen.at(Hardware::FIXBUF));
#endif
#if __NPU_ARCH__ == 2201
#if defined(__DAV_CUBE__)
    g_cubeTPipePtr = this;
#elif defined(__DAV_VEC__)
    g_vecTPipePtr = this;
#else
    g_tPipePtr = this;
#endif
#else
    g_tPipePtr = this;
#endif
    g_tpipeImpl.isDestroy = false;
}

template <class T, class First, class... Rest>
__aicore__ inline void TPipe::AllocAddrs(TBufType* ptr, const First& addr, const Rest&... addrs)
{
    static_assert(
        Std::is_tuple_v<First> && Std::tuple_size_v<First> == 2,
        "input Addrs must be Std::tuple type and tuple_size must be 2");
    constexpr bool useAltBufId = T::config.consumerSize > 1;
    ptr->state = TBufState::FREE;
    ptr->freeBufEvt = T::freeBufEvt;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    if constexpr (UseBufIdSync<T>()) {
        ptr->bufId = AllocMutexID();
        ptr->bufIdAlt = INVALID_TBUFID;
    } else
#endif
        if constexpr (T::queDepth == 0) {
        ptr->enQueEvtID = AllocEventID<T::enQueEvt>();
        ptr->freeBufEvtID = AllocEventID<T::freeBufEvt>();
        SetFlag<T::freeBufEvt>(ptr->freeBufEvtID);
    } else {
        ptr->enQueEvtID = INVALID_TEVENTID;
        ptr->freeBufEvtID = INVALID_TEVENTID;
    }
    // process specifically for better static_assert message
    if constexpr ((Std::tuple_size_v<First>) > 1) {
        ptr->address = Std::get<0>(addr);
        ptr->dataLen = Std::get<1>(addr);
    }

#ifdef ASCENDC_CPU_DEBUG
    int32_t maxLen = ptr->address + ptr->dataLen;
    Hardware pool = GetBufferPos(T::srcPosition, T::dstPosition);
    int32_t currentPoolSize = ConstDefiner::Instance().bufferInitLen.at(pool);
    ASCENDC_DEBUG_ASSERT(
        maxLen <= currentPoolSize,
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "current buffer access buffer at %d, exceeds the limit %d", maxLen, currentPoolSize));
#endif
    ptr->usertag = -1;
    if constexpr (sizeof...(addrs) > 0) {
        AllocAddrs<T>(++ptr, addrs...);
    }
}

template <class T, class U, class V, class... Addrs>
__aicore__ inline bool TPipe::InitBuffer(T& que, const Std::tuple<U, V>& addr0, const Addrs&... addrs)
{
    static_assert((T::isTQue), "TPipe::InitBuffer(T& que, Addrs ...addrs) not supports T as TBuf");
    constexpr uint32_t num = sizeof...(addrs) + 1;
    ASCENDC_DEBUG_ASSERT(
        (que.config.bufferNumber == 0 || que.config.bufferNumber == num),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer number is %u, which should be the same as TQueConfig::bufferNumber(%u)", num,
            que.config.bufferNumber));
    static_assert(T::dstPosition != TPosition::TSCM, "Init Buffer is not support Postion L1 Buffer(TSCM)");
    Hardware pool = GetBufferPos(T::srcPosition, T::dstPosition);
    que.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
    que.value = num;
    ASCENDC_DEBUG_ASSERT(
        (pool != Hardware::GM), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer pos can not be Hardware::GM"));
    ASCENDC_DEBUG_ASSERT(
        (pool != Hardware::MAX), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer pos can not be Hardware::MAX"));
    auto ptr = que.bufStart;
    AllocAddrs<T>(ptr, addr0, addrs...);
    this->g_tpipeImpl.curBufSize_ += num;
    // total buffer num created by InitBuffer must be <= 64
    ASCENDC_DEBUG_ASSERT(
        (this->g_tpipeImpl.curBufSize_ <= QBUF_MAX_LEN && this->g_tpipeImpl.curBufSize_ > 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Total buffer num managed by TPipe is %d, should be in range (0, %d]\n",
            this->g_tpipeImpl.curBufSize_, QBUF_MAX_LEN));
    return true;
}

template <class T>
__aicore__ inline bool TPipe::InitBuffer(T& que, uint8_t num, uint32_t len)
{
    static_assert((T::isTQue), "TPipe::InitBuffer(T& que, uint8_t num, uint32_t len) not supports T as TBuf");
    ASCENDC_DEBUG_ASSERT(
        (que.config.bufferNumber == 0 || que.config.bufferNumber == num),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer number is %u, which should be the same as TQueConfig::bufferNumber(%u)", num,
            que.config.bufferNumber));
    ASCENDC_DEBUG_ASSERT(
        (que.config.bufferLen == 0 || que.config.bufferLen == len),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer length is %u, which should be the same as TQueConfig::bufferLen(%u)", len,
            que.config.bufferLen));
    ASCENDC_CHECK_VALUE_RANGE(num, 1, QBUF_MAX_LEN, "num", "InitBuffer");
    if constexpr (T::dstPosition == TPosition::TSCM) {
        return TscmInitBuffer(que, num, len);
    }
    Hardware pool = GetBufferPos(T::srcPosition, T::dstPosition);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    auto currentPoolSize = bufferInitLen.at(pool);
    ASCENDC_DEBUG_ASSERT(
        (len > 0 && len <= currentPoolSize),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer length is %u, which should be larger than 0, and at least less than or equal to %u",
            len, currentPoolSize));
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    if constexpr (T::dstPosition == TPosition::C2) {
        len = (len + TWO_BLK_SIZE - MIN_BLOCK_LEN) / TWO_BLK_SIZE * TWO_BLK_SIZE;
    }
#endif
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    que.value = num;
    que.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
    DEBUG_CODE(que.bufLen = num * len);

    ASCENDC_DEBUG_ASSERT(
        (pool != Hardware::GM), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer pos can not be Hardware::GM"));
    ASCENDC_DEBUG_ASSERT(
        (pool != Hardware::MAX), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer pos can not be Hardware::MAX"));
    auto curPoolAddr = this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    if constexpr (T::dstPosition == TPosition::C2PIPE2GM) {
        curPoolAddr = 0;
    }
#endif
    auto ptr = que.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_DEBUG_ASSERT(
        (num * len <= currentPoolSize),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer size is %u, exceeds the limit %u", num * len, currentPoolSize));
    auto pos_ = GetPosition(T::srcPosition, T::dstPosition);
    auto absAddr = GetBaseAddr(static_cast<int8_t>(pos_));
    AscendCBufInit(static_cast<uint8_t>(pos_), 0, num, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
#endif
    for (int32_t i = 0; i < num; i++, ptr++) {
        ptr->state = TBufState::FREE;
        ptr->freeBufEvt = T::freeBufEvt;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
        if constexpr (UseBufIdSync<T>()) {
            ptr->bufId = AllocMutexID();
            ptr->bufIdAlt = INVALID_TBUFID;
        } else
#endif
            if constexpr (T::queDepth == 0) {
            ptr->enQueEvtID = AllocEventID<T::enQueEvt>();
            ptr->freeBufEvtID = AllocEventID<T::freeBufEvt>();
            SetFlag<T::freeBufEvt>(ptr->freeBufEvtID);
        } else {
            ptr->enQueEvtID = INVALID_TEVENTID;
            ptr->freeBufEvtID = INVALID_TEVENTID;
        }
        ptr->address = curPoolAddr;
        ptr->dataLen = len;
        ptr->usertag = -1;
        curPoolAddr += len;
    }
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
    ASCENDC_DEBUG_ASSERT(
        (curPoolAddr <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool)));
#endif
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr = curPoolAddr;
    this->g_tpipeImpl.curBufSize_ += num;
    // total buffer num created by InitBuffer must be <= 64
    ASCENDC_DEBUG_ASSERT(
        (this->g_tpipeImpl.curBufSize_ <= QBUF_MAX_LEN && this->g_tpipeImpl.curBufSize_ > 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Total buffer num managed by TPipe is %d, should be in range (0, %d]\n",
            this->g_tpipeImpl.curBufSize_, QBUF_MAX_LEN));
    ASCENDC_DEBUG_ASSERT(
        (this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr <= this->g_tpipeImpl.tscmBufferPtr_),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "tscm addr is %d, limits is %d",
            this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr, this->g_tpipeImpl.tscmBufferPtr_));
    return true;
}

template <TPosition pos>
__aicore__ inline bool TPipe::InitBuffer(TBuf<pos>& buf, uint32_t len)
{
    constexpr auto pool = GetPhyType(pos);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    auto currentPoolSize = bufferInitLen.at(pool);
    ASCENDC_DEBUG_ASSERT(
        (len > 0 && len <= currentPoolSize),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer length is %u, which should be larger than 0, and at least less than or equal to %u",
            len, currentPoolSize));
#endif

    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    if constexpr (pos == TPosition::C2) {
        len = (len + TWO_BLK_SIZE - MIN_BLOCK_LEN) / TWO_BLK_SIZE * TWO_BLK_SIZE;
    }
#endif
    constexpr int32_t bufHandleSize = 1;
    buf.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
    buf.bufLen = len;
    buf.offset = 0;

    ASCENDC_DEBUG_ASSERT(
        (pool != Hardware::GM), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer pos can not be Hardware::GM"));

    auto curPoolAddr = g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
    auto ptr = buf.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_DEBUG_ASSERT(
        (len <= currentPoolSize),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "len is %u, exceeds the limit %u", len, currentPoolSize));
    auto absAddr = GetBaseAddr(static_cast<int8_t>(pos));
    AscendCBufInit(static_cast<uint8_t>(pos), 1, 1, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
#endif
    for (uint8_t i = 0; i < bufHandleSize; i++, ptr++) {
        ptr->state = TBufState::FREE;
        ptr->enQueEvtID = INVALID_TEVENTID;
        ptr->freeBufEvtID = INVALID_TEVENTID;
        ptr->address = curPoolAddr;
        ptr->dataLen = len;
        ptr->usertag = -1;
        curPoolAddr += len;
    }
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
    ASCENDC_DEBUG_ASSERT(
        (curPoolAddr <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "curPoolAddr is %d, exceeds the limit %d", curPoolAddr, bufferInitLen.at(pool)));
#endif
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr = curPoolAddr;
    this->g_tpipeImpl.curBufSize_ += bufHandleSize;
    // total buffer num created by InitBuffer must be <= 64
    ASCENDC_DEBUG_ASSERT(
        (this->g_tpipeImpl.curBufSize_ <= QBUF_MAX_LEN && this->g_tpipeImpl.curBufSize_ > 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Total buffer num managed by TPipe is %d, should be in range (0, %d]\n",
            this->g_tpipeImpl.curBufSize_, QBUF_MAX_LEN));
    return true;
}

template <class T>
__aicore__ inline bool TPipe::InitBufPool(T& bufPool, uint32_t len)
{
    static_assert(
        (T::isTbufPool), "TPipe::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T as TbufPool");
    constexpr auto pool = GetPhyType(T::poolPos);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    auto currentPoolSize = bufferInitLen.at(pool);
    ASCENDC_DEBUG_ASSERT(
        (len > 0 && len <= currentPoolSize),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer length is %u, which should be larger than 0, and at least less than or equal to %u",
            len, currentPoolSize));
#endif
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    bufPool.tBufPoolImpl.startAddr_ = this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = len;
    auto curPoolAddr = this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_DEBUG_ASSERT(
        (len <= currentPoolSize),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer size is %u, exceeds the limit %u", len, currentPoolSize));
    auto pos = T::poolPos;
    auto absAddr = GetBaseAddr(static_cast<int8_t>(pos));
    AscendCTBufPoolInit(
        static_cast<uint8_t>(pos), reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len,
        reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
#endif
    curPoolAddr += len;
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
    ASCENDC_DEBUG_ASSERT(
        (curPoolAddr <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool)));
#endif
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr = curPoolAddr;
    ASCENDC_DEBUG_ASSERT(
        (this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr <= this->g_tpipeImpl.tscmBufferPtr_),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "tscm addr is %d, limits is %d", this->g_tpipeImpl.tscmBufferPtr_,
            this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr));
    return true;
}

template <class T, class U>
__aicore__ inline bool TPipe::InitBufPool(T& bufPool, uint32_t len, U& shareBuf)
{
    static_assert(
        (T::isTbufPool && U::isTbufPool),
        "TPipe::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T and U as TBufPool");
    constexpr auto pool = GetPhyType(T::poolPos);
    ASCENDC_DEBUG_ASSERT(
        (pool == GetPhyType(U::poolPos)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "Hardware type of input bufPool should be same as shareBuf"));
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    auto currentPoolSize = bufferInitLen.at(pool);
    ASCENDC_DEBUG_ASSERT(
        (len > 0 && len <= currentPoolSize),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer length is %u, which should be larger than 0, and at least less than or equal to %u",
            len, currentPoolSize));
#endif
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;

    bufPool.tBufPoolImpl.startAddr_ = shareBuf.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = shareBuf.tBufPoolImpl.maxLen_;
    ASCENDC_DEBUG_ASSERT(
        (len <= shareBuf.tBufPoolImpl.maxLen_),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Length of input bufPool should be shorter than len of shareBuf, which is %u",
            shareBuf.tBufPoolImpl.maxLen_));
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_DEBUG_ASSERT(
        (len <= currentPoolSize),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer size is %u, exceeds the limit %u", len, currentPoolSize));
    auto pos = T::poolPos;
    auto absAddr = GetBaseAddr(static_cast<int8_t>(pos));
    AscendCTBufPoolInit(
        static_cast<uint8_t>(pos), reinterpret_cast<uint64_t>(bufPool.tBufPoolImpl.startAddr_ + absAddr), len,
        reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
#endif
    return true;
}

template <HardEvent evt>
__aicore__ inline TEventID TPipe::AllocEventID()
{
    ASCENDC_DEBUG_ASSERT(
        (evt < HardEvent::MAX), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "illegal event %d", static_cast<int32_t>(evt)));
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    auto lastId = sff0(ptr->eventOccupy);
    ASCENDC_DEBUG_ASSERT(
        (lastId < QUE_MAX_EVENT && lastId >= 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "current id is %ld, max buffer number in same queue position is %d", lastId, QUE_MAX_EVENT));
    ptr->eventOccupy = sbitset1(ptr->eventOccupy, lastId);
    return lastId;
}

template <HardEvent evt>
__aicore__ inline void TPipe::ReleaseEventID(TEventID id)
{
    ASCENDC_DEBUG_ASSERT(
        (id >= 0 && id < QUE_MAX_EVENT),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "current id is %d, which should be larger than 0, and smaller than %d",
            static_cast<int32_t>(id), QUE_MAX_EVENT));
    ASCENDC_DEBUG_ASSERT((evt != HardEvent::MAX), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "evt cannot be HardEvent::MAX"));
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    ptr->eventOccupy = sbitset0(ptr->eventOccupy, id);
    return;
}

__aicore__ inline TEventID TPipe::FetchEventID(HardEvent evt)
{
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    auto lastId = sff0(ptr->eventOccupy);
    ASCENDC_DEBUG_ASSERT(
        (lastId < QUE_MAX_EVENT && lastId >= 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "current id is %ld, max buffer number in same queue position is %d", lastId, QUE_MAX_EVENT));
    return lastId;
}

template <HardEvent evt>
__aicore__ inline TEventID TPipe::FetchEventID()
{
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    auto lastId = sff0(ptr->eventOccupy);
    ASCENDC_DEBUG_ASSERT(
        (lastId < QUE_MAX_EVENT && lastId >= 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "current id is %ld, max buffer number in same queue position is %d", lastId, QUE_MAX_EVENT));
    return lastId;
}

template <TPosition pos>
__aicore__ inline uint64_t TPipe::GetQueueEndAddress()
{
    Hardware hardType = GetPhyType(pos);
    ASCENDC_DEBUG_ASSERT((hardType == Hardware::UB), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "hardType should be UB"));
    return this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardType)].maxAddr;
}

__aicore__ inline void TPipe::DestroyWithoutPipeAll()
{
    // disabled on CPU mode for compatiablilty
#if defined(ASCENDC_DEBUG) && !defined(ASCENDC_CPU_DEBUG)
    Internal::ResetTPipePtr(); // Reset global tpipe ptr to nullptr for next Initialazation.
#endif
    g_tpipeImpl.isDestroy = true;
    auto ptr = this->g_tpipeImpl.buf_;
    for (uint8_t i = 0; i < this->g_tpipeImpl.curBufSize_; i++, ptr++) {
        if (ptr->freeBufEvtID != INVALID_TEVENTID && ptr->state == TBufState::FREE) {
            WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
            ptr->freeBufEvtID = INVALID_TEVENTID;
        }
    }
    // for matmul macro, release M_MTE1 0 1 event id.
#if __NPU_ARCH__ == 2201
    if ASCEND_IS_AIC {
        WaitFlag<HardEvent::M_MTE1>(0);
        ReleaseEventID<HardEvent::M_MTE1>(0);
        WaitFlag<HardEvent::M_MTE1>(1);
        ReleaseEventID<HardEvent::M_MTE1>(1);
        // For Bias
        WaitFlag<HardEvent::M_MTE1>(2);
        ReleaseEventID<HardEvent::M_MTE1>(2);
    }
#elif (                                                                                             \
    __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || \
    __NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    WaitFlag<HardEvent::M_MTE1>(0);
    ReleaseEventID<HardEvent::M_MTE1>(0);
    WaitFlag<HardEvent::M_MTE1>(1);
    ReleaseEventID<HardEvent::M_MTE1>(1);
    WaitFlag<HardEvent::M_MTE1>(2);
    ReleaseEventID<HardEvent::M_MTE1>(2);
#endif
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 5102
    Internal::g_bufId = 0;
#endif
}

__aicore__ inline void TPipe::Destroy()
{
    DestroyWithoutPipeAll();
#ifndef __ASCENDC_ENABLE_SUPER_KERNEL__
    pipe_barrier(PIPE_ALL);
#endif
#if __NPU_ARCH__ == 2002
    dcci((__gm__ int64_t*)0, cache_line_t::ENTIRE_DATA_CACHE);
#endif
}

__aicore__ inline void TPipe::Reset()
{
    auto ptr = this->g_tpipeImpl.buf_;
    for (uint8_t i = 0; i < this->g_tpipeImpl.curBufSize_; i++, ptr++) {
        if (ptr->freeBufEvtID != INVALID_TEVENTID && ptr->state == TBufState::FREE) {
            WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
            ptr->freeBufEvtID = INVALID_TEVENTID;
        }
    }
    InitSocState();
    ResetPool();
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++) {
        if (bufferInitLen.find((Hardware)i) != bufferInitLen.end()) {
            SetBufferCtx((Hardware)i, &g_tpipeImpl.bufPoolBaseAddr_[i]);
        }
    }
#endif
}

__aicore__ inline void InitShareBufStart(
    TPipe* tpipe, uint32_t mode, uint32_t* shareLens, uint32_t lens, uint8_t subBlockIdx)
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
    tpipe->AuxShareBufStart(mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::L1), Hardware::L1, subBlockIdx);
    tpipe->AuxShareBufStart(
        mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::L0C), Hardware::L0C, subBlockIdx);
#if (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 5102)
    tpipe->AuxShareBufStart(mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::UB), Hardware::UB, subBlockIdx);
#endif
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0A)].maxAddr = 0;
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0B)].maxAddr = 0;
    // v100 Shouldn't Use Bias Table
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::BIAS)].maxAddr = 0;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    Internal::g_sharedEvtId = Internal::g_bufId;
#endif
    return;
}

__aicore__ inline void InitShareBufEnd(TPipe* tpipe)
{
    // debug methods need to be added.
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr =
        tpipe->g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::L1)];
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0C)].maxAddr =
        tpipe->g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::L0C)];
#if (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002) || (__NPU_ARCH__ == 5102)
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::UB)].maxAddr =
        tpipe->g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::UB)];
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    Internal::g_bufId = Internal::g_sharedEvtId;
#endif
    return;
}

template <typename T>
__aicore__ inline void TPipe::InitSpmBuffer(const GlobalTensor<T>& workspace, const int32_t bufferSize)
{
    g_tpipeImpl.spmInfo_.spmBuffSize = bufferSize;
    g_tpipeImpl.spmInfo_.spmAddr = reinterpret_cast<uint64_t>(workspace.GetPhyAddr());
    g_tpipeImpl.spmInfo_.spmBufType = static_cast<uint8_t>(Hardware::GM);
}

__aicore__ inline void TPipe::InitSpmBuffer(const int32_t bufferSize)
{
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510)
    (void)(bufferSize);
    ASCENDC_DEBUG_ASSERT((false), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "not supported on current device"));
#else
    g_tpipeImpl.spmInfo_.spmBuffSize = bufferSize;
    TQueBind<TPosition::VECIN, TPosition::A1, 1> inQueue;
    constexpr auto pool = GetPhyType(TPosition::A1);
    g_tpipeImpl.spmInfo_.spmAddr = g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
#ifdef ASCENDC_CPU_DEBUG
    auto absAddr = GetBaseAddr(static_cast<int8_t>(TPosition::A1));
    g_tpipeImpl.spmInfo_.spmAddr = g_tpipeImpl.spmInfo_.spmAddr + reinterpret_cast<uint64_t>(absAddr);
#endif
    InitBuffer(inQueue, 1, bufferSize);
    g_tpipeImpl.spmInfo_.spmBufType = static_cast<uint8_t>(Hardware::L1);
#endif
}

template <typename T>
__aicore__ inline void TPipe::WriteSpmBuffer(
    const LocalTensor<T>& write, const DataCopyParams& copyParams, int32_t writeOffset)
{
    /*
     * before write, the local may come from MTE2/V, so need insert MTE3 wait V/MTE2
     * after write, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    event_t eventIDVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
    SetFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    WaitFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    event_t eventIDMTE2ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
    SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::GM)) {
        DataCopyUB2GMImpl(
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(write.GetPhyAddr()), copyParams);
        event_t eventIDMTE3ToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE2));
        SetFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
        WaitFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_DEBUG_ASSERT(
            (writeOffset % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "writeOffset is %d, which must be 32B aligned \n", writeOffset));
        DataCopyUB2L1Impl(
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(write.GetPhyAddr()), copyParams);
        event_t eventIDMTE3ToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE1));
        SetFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
        WaitFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
    }
    event_t eventIDMTE3ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_V));
    SetFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
    WaitFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
}

template <typename T>
__aicore__ inline void TPipe::ReadSpmBuffer(
    const LocalTensor<T>& read, const DataCopyParams& copyParams, int32_t readOffset)
{
    /*
     * before read, the local may be calculate, so need insert MTE wait V
     * after read, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::GM)) {
        event_t eventIDVToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE2));
        event_t eventIDMTE2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
        event_t eventIDMTE2ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
        SetFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        WaitFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        DataCopyGM2UBImpl(
            reinterpret_cast<__ubuf__ T*>(read.GetPhyAddr()),
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, copyParams);
        SetFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);
        WaitFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);

        SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
        WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_DEBUG_ASSERT(
            (readOffset % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "readOffset is %d, which must be 32B aligned \n", readOffset));
        event_t eventIDVToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE1));
        event_t eventIDMTE1ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_V));
        event_t eventIDMTE1ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_MTE3));
        SetFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        WaitFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        DataCopyL12UBImpl(
            reinterpret_cast<__ubuf__ T*>(read.GetPhyAddr()),
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, copyParams);

        SetFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);
        WaitFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);

        SetFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
        WaitFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
    }
}

template <typename T>
__aicore__ inline void TPipe::WriteSpmBuffer(const LocalTensor<T>& write, const int32_t writeSize, int32_t writeOffset)
{
    /*
     * before write, the local may come from MTE2/V, so need insert MTE3 wait V/MTE2
     * after write, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    int computeSize = writeSize != 0 ? writeSize : GetShapeSize(write.GetShapeInfo());
    struct DataCopyParams repeatParams;
    repeatParams.blockLen = computeSize / AscendCUtils::GetC0Count(sizeof(T));
    event_t eventIDVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
    event_t eventIDMTE2ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
    event_t eventIDMTE3ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_V));
    SetFlag<HardEvent::V_MTE3>(eventIDVToMTE3);
    WaitFlag<HardEvent::V_MTE3>(eventIDVToMTE3);

    SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::GM)) {
        DataCopyUB2GMImpl(
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(write.GetPhyAddr()), repeatParams);
        event_t eventIDMTE3ToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE2));
        SetFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
        WaitFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_DEBUG_ASSERT(
            (writeOffset % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "writeOffset is %d, which must be 32B aligned \n", writeOffset));
        ASCENDC_DEBUG_ASSERT(
            (writeSize % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "writeSize is %d, which must be 32B aligned \n", writeSize));
        DataCopyUB2L1Impl(
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(write.GetPhyAddr()), repeatParams);
        event_t eventIDMTE3ToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE1));
        SetFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
        WaitFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
    }

    SetFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
    WaitFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
}

template <typename T>
__aicore__ inline void TPipe::ReadSpmBuffer(const LocalTensor<T>& read, const int32_t readSize, int32_t readOffset)
{
    /*
     * before read, the local may be calculate, so need insert MTE wait V
     * after read, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    int computeSize = readSize != 0 ? readSize : GetShapeSize(read.GetShapeInfo());
    struct DataCopyParams repeatParams;
    repeatParams.blockLen = computeSize / AscendCUtils::GetC0Count(sizeof(T));
    if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::GM)) {
        event_t eventIDVToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE2));
        event_t eventIDMTE2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
        event_t eventIDMTE2ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
        SetFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        WaitFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        DataCopyGM2UBImpl(
            reinterpret_cast<__ubuf__ T*>(read.GetPhyAddr()),
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, repeatParams);

        SetFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);
        WaitFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);

        SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
        WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_DEBUG_ASSERT(
            (readOffset % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "readOffset is %d, which must be 32B aligned \n", readOffset));
        ASCENDC_DEBUG_ASSERT(
            (readSize % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "readSize is %d, which must be 32B aligned \n", readSize));
        event_t eventIDVToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE1));
        event_t eventIDMTE1ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_V));
        event_t eventIDMTE1ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_MTE3));
        SetFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        WaitFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        DataCopyL12UBImpl(
            reinterpret_cast<__ubuf__ T*>(read.GetPhyAddr()),
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, repeatParams);

        SetFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);
        WaitFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);

        SetFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
        WaitFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
    }
}

// NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. Please do not use it!
template <TPosition pos>
__aicore__ inline TBuffAddr TPipe::GetAbsAddr(int32_t offset, int32_t len) const
{
    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(pos);
    addr.bufferHandle = nullptr;
    addr.bufferAddr = offset;
    addr.dataLen = len;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    constexpr auto pool = GetPhyType(pos);
    ASCENDC_DEBUG_ASSERT(
        (pool != Hardware::GM), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer pos can not be Hardware::GM"));
    ASCENDC_DEBUG_ASSERT(
        ((offset + len) <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "offset is %d, len is %d, exceeds the limit %d", offset, len, bufferInitLen.at(pool)));
    auto absAddr = this->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(pool)].absAddr;
    addr.absAddr = absAddr + addr.bufferAddr;
#endif
    return addr;
}

// NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. Please do not use it!
template <TPosition pos, typename T>
__aicore__ inline __sync_alias__ LocalTensor<T> TPipe::GetAbsAddr(int32_t offset, int32_t size) const
{
    TBuffAddr addr = GetAbsAddr<pos>(offset, static_cast<int32_t>((size * sizeof(T))));
    LocalTensor<T> output;
    output.SetAddr(addr);
    return output;
}

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
template <typename T>
[[deprecated("NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. "
             "Please do not use it!")]] inline uint64_t
TPipe::GetAbsAddr(const LocalTensor<T>& input)
{
    // Translates the CPU address to the actual physical address.
    // Currently, only L1 or UB address translation is supported.
    int8_t logicPos = input.GetPosition();
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_DEBUG_ASSERT(
        (positionHardMap.find((TPosition)logicPos) != positionHardMap.end()),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "illegal logicPos %d ", static_cast<int32_t>(logicPos)));
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_DEBUG_ASSERT(
        ((hardType == Hardware::UB) || (hardType == Hardware::L1)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "illegal hardType %d ", static_cast<int32_t>(hardType)));
    uint8_t* phyAddr = reinterpret_cast<uint8_t*>(input.GetPhyAddr());
    uint8_t* baseAddr = static_cast<uint8_t*>(g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint32_t>(hardType)].absAddr);
    ASCENDC_DEBUG_ASSERT(
        (phyAddr >= baseAddr),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "phyAddr is %p, baseAddr is %p, phyAddr should be larger than baseAddr", phyAddr, baseAddr));
    uint64_t delta = phyAddr - baseAddr;
    if (hardType == Hardware::UB) {
        ASCENDC_DEBUG_ASSERT(
            (delta < TMP_UB_OFFSET),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "addr %lu exceed ub limits %lu ", delta, TMP_UB_OFFSET));
    } else {
        ASCENDC_DEBUG_ASSERT(
            (delta < TOTAL_L1_SIZE),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "addr %lu exceed l1 limits %lu", delta, TOTAL_L1_SIZE));
    }
    return delta;
}

// NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. Please do not use it!
template <typename T>
inline uint64_t GetAbsAddr(TPipe* tpipe, const LocalTensor<T>& input)
{
    // Translates the CPU address to the actual physical address.
    // Currently, only L1 or UB address translation is supported.
    int8_t logicPos = input.GetPosition();
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_DEBUG_ASSERT(
        (positionHardMap.find((TPosition)logicPos) != positionHardMap.end()),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "illegal logicPos %d ", static_cast<int32_t>(logicPos)));
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_DEBUG_ASSERT(
        ((hardType == Hardware::UB) || (hardType == Hardware::L1)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "illegal hardType %d ", static_cast<int32_t>(hardType)));
    uint8_t* phyAddr = reinterpret_cast<uint8_t*>(input.GetPhyAddr());
    uint8_t* baseAddr =
        static_cast<uint8_t*>(tpipe->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint32_t>(hardType)].absAddr);
    ASCENDC_DEBUG_ASSERT(
        (phyAddr >= baseAddr),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "phyAddr is %p, baseAddr is %p, phyAddr should be larger than baseAddr", phyAddr, baseAddr));
    uint64_t delta = phyAddr - baseAddr;
    if (hardType == Hardware::UB) {
        ASCENDC_DEBUG_ASSERT(
            (delta < TMP_UB_OFFSET),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "addr %lu exceed ub limits %lu ", delta, TMP_UB_OFFSET));
    } else {
        ASCENDC_DEBUG_ASSERT(
            (delta < TOTAL_L1_SIZE),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "addr %lu exceed l1 limits %lu", delta, TOTAL_L1_SIZE));
    }
    return delta;
}

inline uint8_t* TPipe::GetBaseAddr(int8_t logicPos)
{
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_DEBUG_ASSERT(
        (positionHardMap.find((TPosition)logicPos) != positionHardMap.end()),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "illegal logicPos %d ", int32_t(logicPos)));
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_DEBUG_ASSERT(
        (hardType != Hardware::GM), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "hardware position can not be gm"));
    uint8_t* baseAddr = static_cast<uint8_t*>(g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint32_t>(hardType)].absAddr);
    return baseAddr;
}

void inline TPipe::SetBufferCtx(Hardware hard, struct BufPoolExtra* bufPool)
{
    ASCENDC_DEBUG_ASSERT(
        (hard != Hardware::MAX), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "hard type can not be Hardware::MAX"));
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_DEBUG_ASSERT(
        (bufferInitLen.find(hard) != bufferInitLen.end()),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "illegal hard type %d", static_cast<int32_t>(hard)));
    uint8_t* ptr;
    if (hard == Hardware::GM) {
        ptr = ConstDefiner::Instance().cpuGM;
    } else {
        ptr = ConstDefiner::Instance().hardwareCpuBufferMap.at(hard);
    }
    {
        // init memory with random value
        std::default_random_engine e;
        int32_t* p = reinterpret_cast<int32_t*>(ptr);
        for (uint64_t i = 0; i < bufferInitLen.at(hard) / sizeof(int32_t); i++) {
            p[i] = e();
        }
    }
    bufPool->phySpace = bufferInitLen.at(hard);
    bufPool->absAddr = ptr;
    return;
}
#endif

__aicore__ inline void TPipe::InitSocState() const { AscendCUtils::InitSocStateImpl(); }

#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 5102
__aicore__ inline MutexID TPipe::AllocMutexID() { return ::AscendC::AllocMutexID(); }

__aicore__ inline void TPipe::ReleaseMutexID(MutexID id) { ::AscendC::ReleaseMutexID(id); }
#endif

__aicore__ inline void TPipe::ResetPool()
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 5102
    Internal::g_bufId = 0;
#endif
    g_tpipeImpl.tscmBufferPtr_ = TOTAL_L1_SIZE;
    g_tpipeImpl.curBufSize_ = 0;
    auto buf = g_tpipeImpl.bufPool_;
    for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++, buf++) {
        buf->maxAddr = 0;
    }
    auto evt = g_tpipeImpl.eventPool_;
    for (int32_t i = 0; i < EVENT_NUM; i++, evt++) {
        evt->eventOccupy = 0;
    }
    g_tpipeImpl.shareBufPool_.start[static_cast<uint8_t>(TShareBuf::ShareHard::L1)] = -1;
    g_tpipeImpl.shareBufPool_.start[static_cast<uint8_t>(TShareBuf::ShareHard::UB)] = -1;
    g_tpipeImpl.shareBufPool_.start[static_cast<uint8_t>(TShareBuf::ShareHard::L0C)] = -1;
}

template <class T>
__aicore__ inline bool TPipe::TscmInitBuffer(T& que, uint8_t num, uint32_t len)
{
    ASCENDC_DEBUG_ASSERT(
        (len > 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len));
    ASCENDC_ASSERT(
        ((num * len) < TOTAL_L1_SIZE),
        KERNEL_LOG(
            KERNEL_ERROR, "tscm buffer length is %u bytes, which is larger than total l1 size %u bytes", len * num,
            TOTAL_L1_SIZE));

    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    que.value = num;
    que.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
    DEBUG_CODE(que.bufLen = num * len);
    // Assign l1
    constexpr Hardware pool = Hardware::L1;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_DEBUG_ASSERT(
        (num * len <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer length %u is too large, the limit is %u", num * len, bufferInitLen.at(pool)));
#endif
    uint32_t curPoolAddr;
    if constexpr (T::scmBlockGroup) {
        curPoolAddr = g_tpipeImpl.tscmBufferPtr_ - num * len;
        g_tpipeImpl.tscmBufferPtr_ -= num * len;
    } else {
        curPoolAddr = g_tpipeImpl.tscmBufferPtr_ - (GetTaskRation() - GetSubBlockIdx()) * len * num;
        g_tpipeImpl.tscmBufferPtr_ -= GetTaskRation() * num * len;
    }

    auto ptr = que.bufStart;
    for (int32_t i = 0; i < num; i++, ptr++) {
        ptr->state = TBufState::FREE;
        ptr->freeBufEvt = T::freeBufEvt;
        ptr->enQueEvtID = INVALID_TEVENTID;
        ptr->freeBufEvtID = INVALID_TEVENTID;
        ptr->address = curPoolAddr;
        ptr->dataLen = len;
        ptr->usertag = -1;
        curPoolAddr += len;
    }

    ASCENDC_DEBUG_ASSERT(
        (this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr <= this->g_tpipeImpl.tscmBufferPtr_),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "tscm addr %d overlapped with maxAddr %d", this->g_tpipeImpl.tscmBufferPtr_,
            this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr));
    this->g_tpipeImpl.curBufSize_ += num;
    // total buffer num created by InitBuffer must be <= 64
    ASCENDC_DEBUG_ASSERT(
        (this->g_tpipeImpl.curBufSize_ <= QBUF_MAX_LEN && this->g_tpipeImpl.curBufSize_ > 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Total buffer num managed by TPipe is %d, should be in range (0, %d]\n",
            this->g_tpipeImpl.curBufSize_, QBUF_MAX_LEN));
    return true;
}
#endif // defined(__NPU_ARCH__)
} // namespace AscendC
#endif
#endif // ASCENDC_MODULE_TPIPE_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_H__
#endif
