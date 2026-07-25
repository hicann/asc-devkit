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
 * \file kernel_tpipe_impl_c310.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_3510/kernel_tpipe_impl_c310.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_C310_H__
#endif
#ifndef ASCENDC_MODULE_TPIPE_IMPL_C310_H
#define ASCENDC_MODULE_TPIPE_IMPL_C310_H

#include "../common_types.h"
#include "kernel_operator_common_impl.h"
#include "../kernel_check.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../kernel_event.h"
#include "../kernel_log.h"
#include "../kernel_macros.h"
#include "../../../include/basic_api/kernel_operator_block_sync_intf.h"
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_H__
#endif
#include "../../../include/basic_api/kernel_struct_data_copy.h"
#include "../../../include/basic_api/kernel_tensor.h"
#include "../kernel_tensor_base.h"
#include "../kernel_tensor_impl.h"
#include "../../../include/basic_api/kernel_tpipe.h"
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_H__
#endif
#include "../kernel_tpipe_base.h"
#include "../kernel_utils.h"
#include "../kernel_utils_base.h"
#include "../../../include/utils/std/tuple.h"
#include "../utils/kernel_utils_ceil_oom_que.h"
#include "../utils/kernel_utils_constants.h"
#include "../utils/kernel_utils_mode_cpu.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include <map>
#include <random>
#include "stub_def.h"
#include "stub_fun.h"
#endif

namespace AscendC {
namespace Std {
template <typename... Tps>
class tuple;
}
} // namespace AscendC

namespace AscendC {
__aicore__ inline void PrintTimeStamp(uint32_t descId);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
inline uint8_t* GetBaseAddrCpu(int8_t logicPos)
{
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_ASSERT((positionHardMap.find((TPosition)logicPos) != positionHardMap.end()), {
        KERNEL_LOG(KERNEL_ERROR, "illegal logicPos %d ", int32_t(logicPos));
    });
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_ASSERT((hardType != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "hardware position can not be gm"); });
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
    if ASCEND_IS_AIC {
        auto enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
        ASCENDC_ASSERT((enQueEvtID == 0), { KERNEL_LOG(KERNEL_ERROR, "enQueEvtID should be 0"); });
        SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
        enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
        ASCENDC_ASSERT((enQueEvtID == 1), { KERNEL_LOG(KERNEL_ERROR, "enQueEvtID should be 1"); });
        SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
        // For load Bias
        enQueEvtID = this->AllocEventID<HardEvent::M_MTE1>();
        ASCENDC_ASSERT((enQueEvtID == 2), { KERNEL_LOG(KERNEL_ERROR, "enQueEvtID should be 2"); });

        SetFlag<HardEvent::M_MTE1>(static_cast<event_t>(enQueEvtID));
    }
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++) {
        SetBufferCtx((Hardware)i, &g_tpipeImpl.bufPoolBaseAddr_[i]);
    }
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
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
#ifdef SPLIT_CORE_CUBE
    g_cubeTPipePtr = this;
#elif defined(SPLIT_CORE_VEC)
    g_vecTPipePtr = this;
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
    if ASCEND_IS_AIV {
        ptr->bufId = AllocMutexID();
        if constexpr (useAltBufId) {
            ptr->bufIdAlt = AllocMutexID();
        } else {
            ptr->bufIdAlt = INVALID_TBUFID;
        }
    } else {
        // c310 aic quedepth=0 will setflag
        if constexpr (T::queDepth == 0) {
            ptr->enQueEvtID = AllocEventID<T::enQueEvt>();
            ptr->freeBufEvtID = AllocEventID<T::freeBufEvt>();
            SetFlag<T::freeBufEvt>(ptr->freeBufEvtID);
        } else if constexpr (T::config.enableStaticEvtId) {
            ptr->bufId = AllocMutexID();
            ptr->bufIdAlt = INVALID_TBUFID;
        } else {
            ptr->enQueEvtID = INVALID_TEVENTID;
            ptr->freeBufEvtID = INVALID_TEVENTID;
        }
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
    ASCENDC_ASSERT(maxLen <= currentPoolSize, {
        KERNEL_LOG(KERNEL_ERROR, "current buffer access buffer at %d, exceeds the limit %d", maxLen, currentPoolSize);
    });
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
    ASCENDC_ASSERT((que.config.bufferNumber == 0 || que.config.bufferNumber == num), {
        KERNEL_LOG(
            KERNEL_ERROR, "buffer number is %u, which should be the same as TQueConfig::bufferNumber(%u)", num,
            que.config.bufferNumber);
    });
    static_assert(T::dstPosition != TPosition::TSCM, "Init Buffer is not supported Postion L1 Buffer(TSCM)");
    Hardware pool = GetBufferPos(T::srcPosition, T::dstPosition);
    que.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
    que.value = num;
    ASCENDC_ASSERT((pool != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::GM"); });
    ASCENDC_ASSERT((pool != Hardware::MAX), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::MAX"); });
    auto ptr = que.bufStart;
    AllocAddrs<T>(ptr, addr0, addrs...);
    this->g_tpipeImpl.curBufSize_ += num;
    // total buffer num created by InitBuffer must be <= 64
    ASCENDC_DEBUG_ASSERT(
        (this->g_tpipeImpl.curBufSize_ <= QBUF_MAX_LEN && this->g_tpipeImpl.curBufSize_ > 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Total buffer num managed by TPipe is %d, should be in range (0, %d]\n",
            this->g_tpipeImpl.curBufSize_, QBUF_MAX_LEN));
#ifdef ASCENDC_TIME_STAMP_ON
    PrintTimeStamp(static_cast<uint32_t>(TimeStampId::TIME_STAMP_BUFFER));
#endif
    return true;
}

template <class T>
__aicore__ inline bool TPipe::InitBuffer(T& que, uint8_t num, uint32_t len)
{
    static_assert((T::isTQue), "TPipe::InitBuffer(T& que, uint8_t num, uint32_t len) not supports T as TBuf");

    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    if constexpr (T::enableGlobalManageQue) {
        return true;
    } else {
        if constexpr (T::dstPosition == TPosition::TSCM) {
            return TscmInitBuffer(que, num, len);
        }
        constexpr bool useAltBufId = T::config.consumerSize > 1;
        len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
        ASCENDC_ASSERT((T::config.bufferLen == 0 || T::config.bufferLen == len), {
            KERNEL_LOG(
                KERNEL_ERROR, "init buffer len %d must > 0 and <= bufferLen %d if bufferLen is > 0",
                static_cast<int32_t>(len), static_cast<int32_t>(T::config.bufferLen));
        });
        ASCENDC_ASSERT((num > 0 && (T::config.bufferNumber == 0 || T::config.bufferNumber == num)), {
            KERNEL_LOG(
                KERNEL_ERROR, "init buffer num %d must > 0 and <= bufferNumber %d if bufferNumber is > 0",
                static_cast<int32_t>(num), static_cast<int32_t>(T::config.bufferNumber));
        });

        que.value = num;
        que.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
        DEBUG_CODE(que.bufLen = num * len);

        Hardware pool = GetBufferPos(T::srcPosition, T::dstPosition);
        ASCENDC_ASSERT((pool != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::GM"); });
        ASCENDC_ASSERT((pool != Hardware::MAX), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::MAX"); });
        auto curPoolAddr = this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
        auto ptr = que.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
        ASCENDC_ASSERT((num * len <= bufferInitLen.at(pool)), {
            KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", num * len, bufferInitLen.at(pool));
        });
        auto pos_ = GetPosition(T::srcPosition, T::dstPosition);
        auto absAddr = GetBaseAddr(static_cast<int8_t>(pos_));
        AscendCBufInit(static_cast<uint8_t>(pos_), 0, num, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
#endif
        for (int32_t i = 0; i < num; i++, ptr++) {
            ptr->state = TBufState::FREE;
            ptr->freeBufEvt = T::freeBufEvt;
            if ASCEND_IS_AIV {
                ptr->bufId = AllocMutexID();
                if constexpr (useAltBufId) {
                    ptr->bufIdAlt = AllocMutexID();
                } else {
                    ptr->bufIdAlt = INVALID_TBUFID;
                }
            } else {
                // c310 aic quedepth=0 will setflag
                if constexpr (T::queDepth == 0) {
                    ptr->enQueEvtID = AllocEventID<T::enQueEvt>();
                    ptr->freeBufEvtID = AllocEventID<T::freeBufEvt>();
                    SetFlag<T::freeBufEvt>(ptr->freeBufEvtID);
                } else if constexpr (T::config.enableStaticEvtId) {
                    ptr->bufId = AllocMutexID();
                    ptr->bufIdAlt = INVALID_TBUFID;
                } else {
                    ptr->enQueEvtID = INVALID_TEVENTID;
                    ptr->freeBufEvtID = INVALID_TEVENTID;
                }
            }
            ptr->address = curPoolAddr;
            ptr->dataLen = len;
            ptr->usertag = -1;
            curPoolAddr += len;
        }
        ASCENDC_ASSERT((curPoolAddr <= bufferInitLen.at(pool)), {
            KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool));
        });
        this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr = curPoolAddr;
        this->g_tpipeImpl.curBufSize_ += num;
        ASCENDC_ASSERT((this->g_tpipeImpl.curBufSize_ < QBUF_MAX_LEN), {
            KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, limits is %d", this->g_tpipeImpl.curBufSize_, QBUF_MAX_LEN);
        });
        ASCENDC_ASSERT(
            (this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr <=
             this->g_tpipeImpl.tscmBufferPtr_),
            {
                KERNEL_LOG(
                    KERNEL_ERROR, "tscm addr is %d, limits is %d", this->g_tpipeImpl.tscmBufferPtr_,
                    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr);
            });
        return true;
    }
}

template <TPosition pos>
__aicore__ inline bool TPipe::InitBuffer(TBuf<pos>& buf, uint32_t len)
{
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    buf.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
    buf.bufLen = len;
    buf.offset = 0;

    constexpr auto pool = GetPhyType(pos);
    ASCENDC_ASSERT((pool != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::GM"); });

    auto curPoolAddr = g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
    auto ptr = buf.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "len is %u, exceeds the limit %d", len, bufferInitLen.at(pool));
    });
    auto absAddr = GetBaseAddr(static_cast<int8_t>(pos));
    AscendCBufInit(static_cast<uint8_t>(pos), 1, 1, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
#endif
    ptr->state = TBufState::FREE;
    ptr->enQueEvtID = INVALID_TEVENTID;
    ptr->freeBufEvtID = INVALID_TEVENTID;
    ptr->address = curPoolAddr;
    ptr->dataLen = len;
    ptr->usertag = -1;
    curPoolAddr += len;
    ptr++;
    ASCENDC_ASSERT((curPoolAddr <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, exceeds the limit %d", curPoolAddr, bufferInitLen.at(pool));
    });
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr = curPoolAddr;
    this->g_tpipeImpl.curBufSize_ += 1;
    ASCENDC_ASSERT((this->g_tpipeImpl.curBufSize_ < QBUF_MAX_LEN), {
        KERNEL_LOG(
            KERNEL_ERROR, "current total buffer num is %d, exceeds the limit %d", this->g_tpipeImpl.curBufSize_,
            QBUF_MAX_LEN);
    });
    return true;
}

template <class T>
__aicore__ inline bool TPipe::InitBufPool(T& bufPool, uint32_t len)
{
    static_assert(
        (T::isTbufPool), "TPipe::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T as TbufPool");
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = AlignUp(len, ONE_BLK_SIZE);
    constexpr auto pool = GetPhyType(T::poolPos);
    constexpr uint32_t bufIdSize = T::bufSize;
    bufPool.tBufPoolImpl.startAddr_ = this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = len;
    uint32_t bufIdPool = 0;
    for (uint32_t i = 0; i < bufIdSize; i++) {
        MutexID id = AllocMutexID();
        bufIdPool |= 1 << id;
    }
    bufPool.tBufPoolImpl.bufIdPool_ = bufIdPool;
    bufPool.tBufPoolImpl.availableIdMask_ = bufIdPool;
    auto curPoolAddr = this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr;

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", len, bufferInitLen.at(pool));
    });
    auto pos = T::poolPos;
    auto absAddr = GetBaseAddr(static_cast<int8_t>(pos));
    AscendCTBufPoolInit(
        static_cast<uint8_t>(pos), reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len,
        reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
#endif
    curPoolAddr += len;
    ASCENDC_ASSERT((curPoolAddr <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool));
    });
    this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr = curPoolAddr;
    ASCENDC_ASSERT(
        (this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr <= this->g_tpipeImpl.tscmBufferPtr_), {
            KERNEL_LOG(
                KERNEL_ERROR, "tscm addr is %d, limits is %d", this->g_tpipeImpl.tscmBufferPtr_,
                this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L1)].maxAddr);
        });
    return true;
}

template <class T, class U>
__aicore__ inline bool TPipe::InitBufPool(T& bufPool, uint32_t len, U& shareBuf)
{
    static_assert(
        (T::isTbufPool && U::isTbufPool),
        "TPipe::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T and U as TBufPool");
    ASCENDC_ASSERT((len > 0), { KERNEL_LOG(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len); });
    len = AlignUp(len, ONE_BLK_SIZE);
    constexpr auto pool = GetPhyType(T::poolPos);
    ASCENDC_ASSERT((pool == GetPhyType(U::poolPos)), {
        KERNEL_LOG(KERNEL_ERROR, "Hardware type of input bufPool should be same as shareBuf");
    });
    static_assert((T::bufSize <= U::bufSize), "U bufIDSize must be > T bufIDSize");

    bufPool.tBufPoolImpl.startAddr_ = shareBuf.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = shareBuf.tBufPoolImpl.maxLen_;
    bufPool.tBufPoolImpl.bufIdPool_ = shareBuf.tBufPoolImpl.bufIdPool_ & shareBuf.tBufPoolImpl.availableIdMask_;
    bufPool.tBufPoolImpl.availableIdMask_ = bufPool.tBufPoolImpl.bufIdPool_;
    ASCENDC_ASSERT((len <= shareBuf.tBufPoolImpl.maxLen_), {
        KERNEL_LOG(
            KERNEL_ERROR, "Length of input bufPool should be shorter than len of shareBuf, which is %u",
            shareBuf.tBufPoolImpl.maxLen_);
    });
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", len, bufferInitLen.at(pool));
    });
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
    ASCENDC_ASSERT(
        (evt < HardEvent::MAX), { KERNEL_LOG(KERNEL_ERROR, "illegal event %d", static_cast<int32_t>(evt)); });
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    auto lastId = sff0(ptr->eventOccupy);
    ASCENDC_ASSERT((lastId < QUE_MAX_EVENT && lastId >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "current id is %ld, max buffer number in same queue position is %d", lastId, QUE_MAX_EVENT);
    });
    ptr->eventOccupy = sbitset1(ptr->eventOccupy, lastId);
    return lastId;
}

template <HardEvent evt>
__aicore__ inline void TPipe::ReleaseEventID(TEventID id)
{
    ASCENDC_ASSERT((id >= 0 && id < QUE_MAX_EVENT), {
        KERNEL_LOG(
            KERNEL_ERROR, "current id is %d, which should be larger than 0, and smaller than %d",
            static_cast<int32_t>(id), QUE_MAX_EVENT);
    });
    ASCENDC_ASSERT((evt != HardEvent::MAX), { KERNEL_LOG(KERNEL_ERROR, "evt cannot be HardEvent::MAX"); });
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    ptr->eventOccupy = sbitset0(ptr->eventOccupy, id);
    return;
}

__aicore__ inline TEventID TPipe::FetchEventID(HardEvent evt)
{
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    auto lastId = sff0(ptr->eventOccupy);
    ASCENDC_ASSERT((lastId < QUE_MAX_EVENT && lastId >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "current id is %ld, max buffer number in same queue position is %d", lastId, QUE_MAX_EVENT);
    });
    return lastId;
}

template <HardEvent evt>
__aicore__ inline TEventID TPipe::FetchEventID()
{
    auto ptr = this->g_tpipeImpl.eventPool_ + EventToIndex(evt);
    auto lastId = sff0(ptr->eventOccupy);
    ASCENDC_ASSERT((lastId < QUE_MAX_EVENT && lastId >= 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "current id is %ld, max buffer number in same queue position is %d", lastId, QUE_MAX_EVENT);
    });
    return lastId;
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
    ASCENDC_ASSERT((pool != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "buffer pos can not be Hardware::GM"); });
    ASCENDC_ASSERT(((offset + len) <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "offset is %d, len is %d, exceeds the limit %d", offset, len, bufferInitLen.at(pool));
    });
    auto absAddr = this->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(pool)].absAddr;
    addr.absAddr = absAddr + addr.bufferAddr;
#endif
    return addr;
}

// NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. Please do not use it!
template <TPosition pos, typename T>
__aicore__ inline __sync_noalias__ LocalTensor<T> TPipe::GetAbsAddr(int32_t offset, int32_t size) const
{
    TBuffAddr addr = GetAbsAddr<pos>(offset, static_cast<int32_t>((size * sizeof(T))));
    LocalTensor<T> tensor;
    tensor.SetAddr(addr);
    return tensor;
}

__aicore__ inline void InitShareBufStart(
    TPipe* tpipe, uint32_t mode, uint32_t* shareLens, uint32_t lens, uint8_t subBlockIdx)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_ASSERT((lens == static_cast<uint32_t>(TShareBuf::ShareHard::MAX)), {
        KERNEL_LOG(
            KERNEL_ERROR, "lens is %d, which should be %d", lens, static_cast<uint32_t>(TShareBuf::ShareHard::MAX));
    });
#else
    (void)(lens);
#endif

    ASCENDC_ASSERT((subBlockIdx == 0 || subBlockIdx == 1), {
        KERNEL_LOG(KERNEL_ERROR, "subBlockIdx is %d, which should only be 0/1", subBlockIdx);
    });
    tpipe->AuxShareBufStart(mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::L1), Hardware::L1, subBlockIdx);
    tpipe->AuxShareBufStart(
        mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::L0C), Hardware::L0C, subBlockIdx);
#if (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    tpipe->AuxShareBufStart(mode, shareLens, static_cast<uint8_t>(TShareBuf::ShareHard::UB), Hardware::UB, subBlockIdx);
#endif
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0A)].maxAddr = 0;
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::L0B)].maxAddr = 0;
    // v100 Shouldn't Use Bias Table
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::BIAS)].maxAddr = 0;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
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
#if (__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002)
    tpipe->g_tpipeImpl.bufPool_[static_cast<uint8_t>(Hardware::UB)].maxAddr =
        tpipe->g_tpipeImpl.shareBufPool_.maxAddr[static_cast<uint8_t>(TShareBuf::ShareHard::UB)];
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
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
    (void)(bufferSize);
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "only support platform ascend910, ascend310p"); });
}

template <typename T>
__aicore__ inline void TPipe::WriteSpmBuffer(
    const LocalTensor<T>& writeLocal, const DataCopyParams& copyParams, int32_t writeOffset)
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
            reinterpret_cast<__ubuf__ T*>(writeLocal.GetPhyAddr()), copyParams);
        event_t eventIDMTE3ToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE2));
        SetFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
        WaitFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_ASSERT((writeOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "writeOffset is %d, which must be 32B aligned", writeOffset);
        });
        DataCopyUB2L1Impl(
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(writeLocal.GetPhyAddr()), copyParams);
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
    const LocalTensor<T>& readLocal, const DataCopyParams& copyParams, int32_t readOffset)
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
            reinterpret_cast<__ubuf__ T*>(readLocal.GetPhyAddr()),
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, copyParams);
        SetFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);
        WaitFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);

        SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
        WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_ASSERT((readOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "readOffset is %d, which must be 32B aligned", readOffset);
        });
        event_t eventIDVToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE1));
        event_t eventIDMTE1ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_V));
        event_t eventIDMTE1ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_MTE3));
        SetFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        WaitFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        DataCopyL12UBImpl(
            reinterpret_cast<__ubuf__ T*>(readLocal.GetPhyAddr()),
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, copyParams);

        SetFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);
        WaitFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);

        SetFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
        WaitFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
    }
}

template <typename T>
__aicore__ inline void TPipe::WriteSpmBuffer(
    const LocalTensor<T>& writeLocal, const int32_t writeSize, int32_t writeOffset)
{
    /*
     * before write, the local may come from MTE2/V, so need insert MTE3 wait V/MTE2
     * after write, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    int computeSize = writeSize != 0 ? writeSize : GetShapeSize(writeLocal.GetShapeInfo());
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
            reinterpret_cast<__ubuf__ T*>(writeLocal.GetPhyAddr()), repeatParams);
        event_t eventIDMTE3ToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE2));
        SetFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
        WaitFlag<HardEvent::MTE3_MTE2>(eventIDMTE3ToMTE2);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_ASSERT((writeOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "writeOffset is %d, which must be 32B aligned", writeOffset);
        });
        ASCENDC_ASSERT((writeSize % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "writeSize is %d, which must be 32B aligned", writeSize);
        });
        DataCopyUB2L1Impl(
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + writeOffset,
            reinterpret_cast<__ubuf__ T*>(writeLocal.GetPhyAddr()), repeatParams);
        event_t eventIDMTE3ToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE1));
        SetFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
        WaitFlag<HardEvent::MTE3_MTE1>(eventIDMTE3ToMTE1);
    }

    SetFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
    WaitFlag<HardEvent::MTE3_V>(eventIDMTE3ToV);
}

template <typename T>
__aicore__ inline void TPipe::ReadSpmBuffer(const LocalTensor<T>& readLocal, const int32_t readSize, int32_t readOffset)
{
    /*
     * before read, the local may be calculate, so need insert MTE wait V
     * after read, the local may used to compute or copy out, need insert V/MTE2 wait MTE3
     */
    int computeSize = readSize != 0 ? readSize : GetShapeSize(readLocal.GetShapeInfo());
    struct DataCopyParams repeatParams;
    repeatParams.blockLen = computeSize / AscendCUtils::GetC0Count(sizeof(T));
    if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::GM)) {
        event_t eventIDVToMTE2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE2));
        event_t eventIDMTE2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
        event_t eventIDMTE2ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE3));
        SetFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        WaitFlag<HardEvent::V_MTE2>(eventIDVToMTE2);
        DataCopyGM2UBImpl(
            reinterpret_cast<__ubuf__ T*>(readLocal.GetPhyAddr()),
            reinterpret_cast<__gm__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, repeatParams);

        SetFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);
        WaitFlag<HardEvent::MTE2_V>(eventIDMTE2ToV);

        SetFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
        WaitFlag<HardEvent::MTE2_MTE3>(eventIDMTE2ToMTE3);
    } else if (g_tpipeImpl.spmInfo_.spmBufType == static_cast<uint8_t>(Hardware::L1)) {
        ASCENDC_ASSERT((readOffset % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "readOffset is %d, which must be 32B aligned", readOffset);
        });
        ASCENDC_ASSERT((readSize % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(KERNEL_ERROR, "readSize is %d, which must be 32B aligned", readSize);
        });
        event_t eventIDVToMTE1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE1));
        event_t eventIDMTE1ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_V));
        event_t eventIDMTE1ToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE1_MTE3));
        SetFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        WaitFlag<HardEvent::V_MTE1>(eventIDVToMTE1);
        DataCopyL12UBImpl(
            reinterpret_cast<__ubuf__ T*>(readLocal.GetPhyAddr()),
            reinterpret_cast<__cbuf__ T*>(g_tpipeImpl.spmInfo_.spmAddr) + readOffset, repeatParams);

        SetFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);
        WaitFlag<HardEvent::MTE1_V>(eventIDMTE1ToV);

        SetFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
        WaitFlag<HardEvent::MTE1_MTE3>(eventIDMTE1ToMTE3);
    }
}

template <TPosition pos>
__aicore__ inline uint64_t TPipe::GetQueueEndAddress()
{
    Hardware hardType = GetPhyType(pos);
    ASCENDC_ASSERT((hardType == Hardware::UB), { KERNEL_LOG(KERNEL_ERROR, "hardType should be UB"); });
    return this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(hardType)].maxAddr;
}

__aicore__ inline void TPipe::DestroyWithoutPipeAll()
{
// disabled on CPU mode for compatiablilty
#if defined(ASCENDC_DEBUG) && !defined(ASCENDC_CPU_DEBUG)
    Internal::ResetTPipePtr(); // Reset global tpipe ptr to nullptr for next Initialazation.
#endif
    if ASCEND_IS_AIC {
        g_tpipeImpl.isDestroy = true;
        auto ptr = this->g_tpipeImpl.buf_;
        for (uint8_t i = 0; i < this->g_tpipeImpl.curBufSize_; i++, ptr++) {
            if (ptr->freeBufEvtID != INVALID_TEVENTID && ptr->state == TBufState::FREE) {
                WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
                ptr->freeBufEvtID = INVALID_TEVENTID;
            }
        }
        WaitFlag<HardEvent::M_MTE1>(0);
        ReleaseEventID<HardEvent::M_MTE1>(0);
        WaitFlag<HardEvent::M_MTE1>(1);
        ReleaseEventID<HardEvent::M_MTE1>(1);
        // For Bias
        WaitFlag<HardEvent::M_MTE1>(2);
        ReleaseEventID<HardEvent::M_MTE1>(2);
    }
    Internal::g_bufId = 0;
}

__aicore__ inline void TPipe::Destroy()
{
    DestroyWithoutPipeAll();
#ifndef __ASCENDC_ENABLE_SUPER_KERNEL__
    pipe_barrier(PIPE_ALL);
#endif
}

__aicore__ inline void TPipe::Reset()
{
    if ASCEND_IS_AIC {
        auto ptr = this->g_tpipeImpl.buf_;
        for (uint8_t i = 0; i < this->g_tpipeImpl.curBufSize_; i++, ptr++) {
            if (ptr->freeBufEvtID != INVALID_TEVENTID && ptr->state == TBufState::FREE) {
                WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
                ptr->freeBufEvtID = INVALID_TEVENTID;
            }
        }
    } else {
        GetBuffImpl<PIPE_V, true>(0);
        ReleaseBuffImpl<PIPE_V, true>(0);
        GetBuffImpl<PIPE_MTE3, false>(0);
        ReleaseBuffImpl<PIPE_MTE3, false>(0);
        PipeBarrierInternal<PIPE_MTE2>();
        PipeBarrierInternal<PIPE_MTE3>();
    }
    InitSocState();
    ResetPool();
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++) {
        SetBufferCtx((Hardware)i, &g_tpipeImpl.bufPoolBaseAddr_[i]);
    }
#endif
}

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
template <typename T>
[[deprecated("NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. "
             "Please do not use it!")]] inline uint64_t
TPipe::GetAbsAddr(const LocalTensor<T>& tensor)
{
    // Translates the CPU address to the actual physical address.
    // Currently, only L1 or UB address translation is supported.
    int8_t logicPos = tensor.GetPosition();
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_ASSERT((positionHardMap.find((TPosition)logicPos) != positionHardMap.end()), {
        KERNEL_LOG(KERNEL_ERROR, "illegal logicPos %d ", static_cast<int32_t>(logicPos));
    });
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_ASSERT(((hardType == Hardware::UB) || (hardType == Hardware::L1)), {
        KERNEL_LOG(KERNEL_ERROR, "illegal hardType %d ", static_cast<int32_t>(hardType));
    });
    uint8_t* phyAddr = reinterpret_cast<uint8_t*>(tensor.GetPhyAddr());
    uint8_t* baseAddr = static_cast<uint8_t*>(g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint32_t>(hardType)].absAddr);
    ASCENDC_ASSERT((phyAddr >= baseAddr), {
        KERNEL_LOG(
            KERNEL_ERROR, "phyAddr is %p, baseAddr is %p, phyAddr should be larger than baseAddr", phyAddr, baseAddr);
    });
    uint64_t delta = phyAddr - baseAddr;
    if (hardType == Hardware::UB) {
        ASCENDC_ASSERT((delta < TMP_UB_OFFSET), {
            KERNEL_LOG(KERNEL_ERROR, "addr %lu exceed ub limits %lu ", delta, TMP_UB_OFFSET);
        });
    } else {
        ASCENDC_ASSERT((delta < TOTAL_L1_SIZE), {
            KERNEL_LOG(KERNEL_ERROR, "addr %lu exceed l1 limits %lu", delta, TOTAL_L1_SIZE);
        });
    }
    return delta;
}

// NOTICE: GetAbsAddr has been deprecated and will be removed in the next version. Please do not use it!
template <typename T>
inline uint64_t GetAbsAddr(TPipe* tpipe, const LocalTensor<T>& tensor)
{
    // Translates the CPU address to the actual physical address.
    // Currently, only L1 or UB address translation is supported.
    int8_t logicPos = tensor.GetPosition();
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_ASSERT((positionHardMap.find((TPosition)logicPos) != positionHardMap.end()), {
        KERNEL_LOG(KERNEL_ERROR, "illegal logicPos %d ", static_cast<int32_t>(logicPos));
    });
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_ASSERT(((hardType == Hardware::UB) || (hardType == Hardware::L1)), {
        KERNEL_LOG(KERNEL_ERROR, "illegal hardType %d ", static_cast<int32_t>(hardType));
    });
    uint8_t* phyAddr = reinterpret_cast<uint8_t*>(tensor.GetPhyAddr());
    uint8_t* baseAddr =
        static_cast<uint8_t*>(tpipe->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint32_t>(hardType)].absAddr);
    ASCENDC_ASSERT((phyAddr >= baseAddr), {
        KERNEL_LOG(
            KERNEL_ERROR, "phyAddr is %p, baseAddr is %p, phyAddr should be larger than baseAddr", phyAddr, baseAddr);
    });
    uint64_t delta = phyAddr - baseAddr;
    if (hardType == Hardware::UB) {
        ASCENDC_ASSERT((delta < TMP_UB_OFFSET), {
            KERNEL_LOG(KERNEL_ERROR, "addr %lu exceed ub limits %lu ", delta, TMP_UB_OFFSET);
        });
    } else {
        ASCENDC_ASSERT((delta < TOTAL_L1_SIZE), {
            KERNEL_LOG(KERNEL_ERROR, "addr %lu exceed l1 limits %lu", delta, TOTAL_L1_SIZE);
        });
    }
    return delta;
}

inline uint8_t* TPipe::GetBaseAddr(int8_t logicPos)
{
    auto positionHardMap = ConstDefiner::Instance().positionHardMap;
    ASCENDC_ASSERT((positionHardMap.find((TPosition)logicPos) != positionHardMap.end()), {
        KERNEL_LOG(KERNEL_ERROR, "illegal logicPos %d ", int32_t(logicPos));
    });
    Hardware hardType = positionHardMap.at((TPosition)logicPos);
    ASCENDC_ASSERT((hardType != Hardware::GM), { KERNEL_LOG(KERNEL_ERROR, "hardware position can not be gm"); });
    uint8_t* baseAddr = static_cast<uint8_t*>(g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint32_t>(hardType)].absAddr);
    return baseAddr;
}

void inline TPipe::SetBufferCtx(Hardware hard, struct BufPoolExtra* bufPool)
{
    ASCENDC_ASSERT((hard != Hardware::MAX), { KERNEL_LOG(KERNEL_ERROR, "hard type can not be Hardware::MAX"); });
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((bufferInitLen.find(hard) != bufferInitLen.end()), {
        KERNEL_LOG(KERNEL_ERROR, "illegal hard type %d", static_cast<int32_t>(hard));
    });
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

__aicore__ inline void TPipe::ResetPool()
{
    Internal::g_bufId = 0;
    g_tpipeImpl.tscmBufferPtr_ = TOTAL_L1_SIZE;
    g_tpipeImpl.curBufSize_ = 0;
    g_tpipeImpl.bufIdPool_ = 0;
    g_tpipeImpl.tscmBufIdPool_ = TSCM_BUFID_MAX;
    g_tpipeImpl.crossSyncId_ = Internal::TSCM_CROSS_SYNC_ID_MAX;
    auto buf = g_tpipeImpl.bufPool_;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++, buf++) {
        buf->maxAddr = 0;
    }
#else
    if ASCEND_IS_AIV {
        buf[static_cast<int32_t>(Hardware::UB)].maxAddr = GetDynamicMemStartPos<Hardware::UB>();
        buf[static_cast<int32_t>(Hardware::L1)].maxAddr = 0;
    } else {
        for (int32_t i = 0; i < static_cast<int32_t>(Hardware::MAX); i++, buf++) {
            buf->maxAddr = 0;
        }
    }
#endif
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
    ASCENDC_ASSERT(((num * len) < TOTAL_L1_SIZE), {
        KERNEL_LOG(
            KERNEL_ERROR, "tscm buffer length is %u bytes, which is larger than total l1 size %u bytes", len * num,
            TOTAL_L1_SIZE);
    });
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    que.value = num;
    que.bufStart = this->g_tpipeImpl.buf_ + this->g_tpipeImpl.curBufSize_;
    DEBUG_CODE(que.bufLen = num * len);
    // Assign l1
    constexpr Hardware pool = Hardware::L1;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_ASSERT((num * len <= bufferInitLen.at(pool)), {
        KERNEL_LOG(KERNEL_ERROR, "buffer length %d is too large, the limit is %d", num * len, bufferInitLen.at(pool));
    });
#endif
    uint32_t curPoolAddr;
    if constexpr (T::scmBlockGroup) {
        curPoolAddr = g_tpipeImpl.tscmBufferPtr_ - num * len;
        g_tpipeImpl.tscmBufferPtr_ -= num * len;
    } else {
        curPoolAddr = g_tpipeImpl.tscmBufferPtr_ - (TscmGetTaskRation() - GetSubBlockIdxImpl()) * len * num;
        g_tpipeImpl.tscmBufferPtr_ -= TscmGetTaskRation() * num * len;
    }

    auto ptr = que.bufStart;
    for (int32_t i = 0; i < num; i++, ptr++) {
        ptr->state = TBufState::FREE;
        ptr->freeBufEvt = T::freeBufEvt;
        if constexpr (T::srcHardType == Hardware::GM) {
            ptr->bufId = AllocTscmBufId();
            ptr->bufIdAlt = INVALID_TBUFID;
        } else {
            ptr->enQueEvtID = AllocCrossSyncId();
            ptr->freeBufEvtID = INVALID_TEVENTID;
        }
        ptr->address = curPoolAddr;
        ptr->dataLen = len;
        ptr->usertag = -1;
        curPoolAddr += len;
    }
    ASCENDC_ASSERT(
        (this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr <= this->g_tpipeImpl.tscmBufferPtr_), {
            KERNEL_LOG(
                KERNEL_ERROR, "tscm addr %d overlapped with maxAddr %d", this->g_tpipeImpl.tscmBufferPtr_,
                this->g_tpipeImpl.bufPool_[static_cast<uint8_t>(pool)].maxAddr);
        });
    this->g_tpipeImpl.curBufSize_ += num;
    ASCENDC_ASSERT((this->g_tpipeImpl.curBufSize_ <= QBUF_MAX_LEN), {
        KERNEL_LOG(
            KERNEL_ERROR, "max buffer num is %d, current buf size %d exceed this limits", QBUF_MAX_LEN,
            this->g_tpipeImpl.curBufSize_);
    });
    return true;
}
} // namespace AscendC
#endif // ASCENDC_MODULE_TPIPE_IMPL_C310_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_C310_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TPIPE_IMPL_C310_H__
#endif
