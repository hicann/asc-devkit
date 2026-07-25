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
 * \file kernel_tbufpool_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_tbufpool_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TBUFPOOL_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_TBUFPOOL_IMPL_H
#define ASCENDC_MODULE_TBUFPOOL_IMPL_H

namespace AscendC {
// begin impl of tBufPool
template <TPosition pos, uint32_t bufIDSize>
__aicore__ inline TBufPool<pos, bufIDSize>::TBufPool()
{
    Init();
}

template <TPosition pos, uint32_t bufIDSize>
__aicore__ inline TBufPool<pos, bufIDSize>::~TBufPool()
{
    auto ptr = this->tBufPoolImpl.buf_;
    for (uint8_t i = 0; i < this->tBufPoolImpl.curBufSize_; i++, ptr++) {
        if (ptr->freeBufEvtID != INVALID_TEVENTID) {
            WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
            ptr->freeBufEvtID = INVALID_TEVENTID;
        }
    }
    ResetPool();
};

template <TPosition pos, uint32_t bufIDSize>
__aicore__ inline void TBufPool<pos, bufIDSize>::ResetPool()
{
    tBufPoolImpl.curBufSize_ = 0;
    tBufPoolImpl.startAddr_ = 0;
    tBufPoolImpl.maxAddr_ = 0;
    tBufPoolImpl.maxLen_ = 0;
}

template <TPosition pos, uint32_t bufIDSize>
__aicore__ inline void TBufPool<pos, bufIDSize>::Init()
{
    constexpr auto pool = GetPhyType(pos);
    static_assert(
        (pool == Hardware::L1 || pool == Hardware::UB),
        "TbufPool Position should be one of L1 Buffer(A1/B1/C1)/UB(VECIN/VECOUT/VECCALC)");
    ResetPool();
    tBufPoolImpl.isReset_ = true;
}

template <TPosition pos, uint32_t bufIDSize>
__aicore__ inline void TBufPool<pos, bufIDSize>::Reset()
{
    auto ptr = this->tBufPoolImpl.buf_;
    for (uint8_t i = 0; i < this->tBufPoolImpl.curBufSize_; i++, ptr++) {
        if (ptr->freeBufEvtID != INVALID_TEVENTID) {
            WaitFlagImpl(ptr->freeBufEvt, ptr->freeBufEvtID);
            ptr->freeBufEvtID = INVALID_TEVENTID;
        }
    }
    ResetPool();
    tBufPoolImpl.isReset_ = true;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    AscendCUpdateTbufPoolStatus(reinterpret_cast<uint64_t>(&tBufPoolImpl), tBufPoolImpl.isReset_);
#endif
}

template <TPosition pos, uint32_t bufIDSize>
template <class T>
__aicore__ inline bool TBufPool<pos, bufIDSize>::InitBuffer(T& que, uint8_t num, uint32_t len)
{
    static_assert((T::isTQue), "TBufPool::InitBuffer(T& que, uint8_t num, uint32_t len) not supports T as TBuf");
    ASCENDC_DEBUG_ASSERT(
        (len > 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len));
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    que.value = num;
    que.bufStart = this->tBufPoolImpl.buf_ + this->tBufPoolImpl.curBufSize_;
    DEBUG_CODE(que.bufLen = num * len);
    ASCENDC_DEBUG_ASSERT(
        (this->tBufPoolImpl.maxAddr_ + num * len <= this->tBufPoolImpl.startAddr_ + this->tBufPoolImpl.maxLen_),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Buffer Init length exceeds limit of BufPool. Max Length of BufPool is %u",
            this->tBufPoolImpl.maxLen_));
    auto curPoolAddr = this->tBufPoolImpl.maxAddr_;
    auto ptr = que.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    Hardware pool = GetBufferPos(T::srcPosition, T::dstPosition);
    ASCENDC_DEBUG_ASSERT(
        (pool == GetPhyType(pos)), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer pos should be same as pos of TbufPool"));
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_DEBUG_ASSERT(
        (num * len <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", num * len, bufferInitLen.at(pool)));
    auto bufPos = GetPosition(T::srcPosition, T::dstPosition);
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(bufPos));
    AscendCBufInit(static_cast<uint8_t>(bufPos), 0, num, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
    que.SetTBufPoolHandle(reinterpret_cast<uint64_t>(&tBufPoolImpl));
    ASCENDC_DEBUG_ASSERT(
        (curPoolAddr + num * len <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool)));
#endif
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
    this->tBufPoolImpl.maxAddr_ = curPoolAddr;
    this->tBufPoolImpl.curBufSize_ += num;
    ASCENDC_DEBUG_ASSERT(
        (this->tBufPoolImpl.curBufSize_ <= QBUFPOOL_MAX_LEN),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "buffer size is %d, limits is %d", this->tBufPoolImpl.curBufSize_, QBUFPOOL_MAX_LEN));
    return true;
}

template <TPosition pos, uint32_t bufIDSize>
template <TPosition bufPos>
__aicore__ inline bool TBufPool<pos, bufIDSize>::InitBuffer(TBuf<bufPos>& buf, uint32_t len)
{
    ASCENDC_DEBUG_ASSERT(
        (len > 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len));
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    constexpr int32_t bufHandleSize = 1;
    buf.bufStart = this->tBufPoolImpl.buf_ + this->tBufPoolImpl.curBufSize_;
    buf.bufLen = len;
    buf.offset = 0;
    ASCENDC_DEBUG_ASSERT(
        (this->tBufPoolImpl.maxAddr_ + len <= this->tBufPoolImpl.startAddr_ + this->tBufPoolImpl.maxLen_),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Buffer Init length exceeds limit of BufPool. Max Length of BufPool is %u",
            this->tBufPoolImpl.maxLen_));
    constexpr auto pool = GetPhyType(bufPos);
    ASCENDC_DEBUG_ASSERT(
        (GetPhyType(bufPos) == GetPhyType(pos)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer pos should be same as pos of TBufPool"));
    auto curPoolAddr = this->tBufPoolImpl.maxAddr_;
    auto ptr = buf.bufStart;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_DEBUG_ASSERT(
        (len <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "len is %u, exceeds the limit %d", len, bufferInitLen.at(pool)));
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(bufPos));
    AscendCBufInit(static_cast<uint8_t>(bufPos), 1, 1, reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len);
    buf.SetTBufPoolHandle(reinterpret_cast<uint64_t>(&tBufPoolImpl));
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
    this->tBufPoolImpl.maxAddr_ = curPoolAddr;
    this->tBufPoolImpl.curBufSize_ += bufHandleSize;
    ASCENDC_DEBUG_ASSERT(
        (this->tBufPoolImpl.curBufSize_ <= QBUFPOOL_MAX_LEN),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "current total buffer num is %d, exceeds the limit %d", this->tBufPoolImpl.curBufSize_,
            QBUFPOOL_MAX_LEN));
    return true;
}

template <TPosition pos, uint32_t bufIDSize>
template <class T>
__aicore__ inline bool TBufPool<pos, bufIDSize>::InitBufPool(T& bufPool, uint32_t len)
{
    static_assert(
        (T::isTbufPool), "TBufPool::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T as TbufPool");
    ASCENDC_DEBUG_ASSERT(
        (len > 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len));
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    constexpr auto pool = GetPhyType(T::poolPos);
    bufPool.tBufPoolImpl.startAddr_ = this->tBufPoolImpl.maxAddr_;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = len;
    ASCENDC_DEBUG_ASSERT(
        (this->tBufPoolImpl.maxAddr_ + len <= this->tBufPoolImpl.startAddr_ + this->tBufPoolImpl.maxLen_),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Buffer Init length exceeds limit of BufPool. Max Length of BufPool is %u",
            this->tBufPoolImpl.maxLen_));
    auto curPoolAddr = this->tBufPoolImpl.maxAddr_;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_DEBUG_ASSERT(
        (len <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", len, bufferInitLen.at(pool)));
    auto bufPos = T::poolPos;
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(bufPos));
    AscendCTBufPoolInit(
        static_cast<uint8_t>(bufPos), reinterpret_cast<uint64_t>(curPoolAddr + absAddr), len,
        reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
    AscendCRecordPoolHierarchy(
        reinterpret_cast<uint64_t>(&this->tBufPoolImpl), reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
#endif
    curPoolAddr += len;
#if defined(ASCENDC_CPU_DEBUG) && (ASCENDC_CPU_DEBUG == 1)
    ASCENDC_DEBUG_ASSERT(
        (curPoolAddr <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "curPoolAddr is %d, limits is %d", curPoolAddr, bufferInitLen.at(pool)));
#endif
    this->tBufPoolImpl.maxAddr_ = curPoolAddr;
    return true;
}

template <TPosition pos, uint32_t bufIDSize>
template <class T, class U>
__aicore__ inline bool TBufPool<pos, bufIDSize>::InitBufPool(T& bufPool, uint32_t len, U& shareBuf)
{
    static_assert(
        (T::isTbufPool && U::isTbufPool),
        "TBufPool::InitBufPool(T& bufPool, uint32_t len, U& shareBuf) only supports T and U as TBufPool");
    ASCENDC_DEBUG_ASSERT(
        (len > 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len));
    len = (len + ONE_BLK_SIZE - MIN_BLOCK_LEN) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    constexpr auto pool = GetPhyType(T::poolPos);
    constexpr auto sharedPool = GetPhyType(U::poolPos);
    ASCENDC_DEBUG_ASSERT(
        (pool == sharedPool),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "Position of input bufPool should be same as position of shareBuf"));
    bufPool.tBufPoolImpl.startAddr_ = shareBuf.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxAddr_ = bufPool.tBufPoolImpl.startAddr_;
    bufPool.tBufPoolImpl.maxLen_ = shareBuf.tBufPoolImpl.maxLen_;
    ASCENDC_DEBUG_ASSERT(
        (len <= shareBuf.tBufPoolImpl.maxLen_),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Length of input bufPool should be no longer than length of shareBuf, which is %u",
            shareBuf.tBufPoolImpl.maxLen_));
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto bufferInitLen = ConstDefiner::Instance().bufferInitLen;
    ASCENDC_DEBUG_ASSERT(
        (len <= bufferInitLen.at(pool)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer size is %d, exceeds the limit %d", len, bufferInitLen.at(pool)));
    auto bufPos = T::poolPos;
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(bufPos));
    AscendCTBufPoolInit(
        static_cast<uint8_t>(bufPos), reinterpret_cast<uint64_t>(bufPool.tBufPoolImpl.startAddr_ + absAddr), len,
        reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
    AscendCRecordPoolHierarchy(
        reinterpret_cast<uint64_t>(&this->tBufPoolImpl), reinterpret_cast<uint64_t>(&bufPool.tBufPoolImpl));
#endif
    return true;
}
} // namespace AscendC
#endif // ASCENDC_MODULE_TBUFPOOL_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TBUFPOOL_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TBUFPOOL_IMPL_H__
#endif
