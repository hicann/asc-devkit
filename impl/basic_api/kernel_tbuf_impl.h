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
 * \file kernel_tbuf_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_tbuf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TBUF_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_TBUF_IMPL_H
#define ASCENDC_MODULE_TBUF_IMPL_H

namespace AscendC {
// begin impl of tbuf
template <TPosition pos>
template <typename T>
__aicore__ inline __sync_alias__ LocalTensor<T> TBuf<pos>::Get(uint32_t len)
{
    using PrimType = PrimT<T>;
    uint32_t dataLen;
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
        dataLen = len / INT4_TWO;
    } else {
        dataLen = len * sizeof(PrimType);
    }
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_DEBUG_ASSERT(
        (len > 0), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer length is %u, which should be larger than 0", len));
    ASCENDC_DEBUG_ASSERT(
        (dataLen % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "buffer length is %u, which should be times of 32 Bytes \n", len));
    ASCENDC_DEBUG_ASSERT(
        (dataLen <= bufLen), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "len is %u, max buffer len is %u", dataLen, bufLen));
#endif
    auto ptr = this->bufStart;
    ptr->dataLen = dataLen;
    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(pos);
    addr.bufferHandle = reinterpret_cast<TBufHandle>(ptr);
    addr.bufferAddr = ptr->address;
    addr.dataLen = ptr->dataLen;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(GetPhyType(pos))].absAddr;
    addr.absAddr = absAddr + addr.bufferAddr;
    AscendCBufGet(addr.logicPos, static_cast<uint8_t>(GetPhyType(pos)), reinterpret_cast<uint64_t>(addr.absAddr), len);
    if (this->bufPoolHandle != 0U) {
        AscendCUpdateTbufPoolStatus(this->bufPoolHandle, false);
        AscendCTBufPoolResetCheck(
            static_cast<uint8_t>(GetPhyType(pos)), reinterpret_cast<uint64_t>(absAddr + ptr->address),
            static_cast<uint64_t>(ptr->dataLen), this->bufPoolHandle);
    }
#endif
    LocalTensor<T> output;
    output.SetAddr(addr);
    return output;
}

template <TPosition pos>
template <typename T>
__aicore__ inline __sync_alias__ LocalTensor<T> TBuf<pos>::Get()
{
    using PrimType = PrimT<T>;
    if constexpr (IsSameType<PrimType, int4b_t>::value) {
        return Get<T>(bufLen * INT4_TWO);
    } else {
        return Get<T>(bufLen / sizeof(PrimType));
    }
}

template <TPosition pos>
template <typename T>
__aicore__ inline __sync_alias__ LocalTensor<T> TBuf<pos>::GetWithOffset(uint32_t size, uint32_t bufOffset)
{
    auto ptr = this->bufStart;
    ptr->dataLen = size * sizeof(T);
    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(pos);
    addr.bufferHandle = reinterpret_cast<TBufHandle>(ptr);
    addr.bufferAddr = ptr->address + bufOffset;
    addr.dataLen = ptr->dataLen;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto absAddr = GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(pos));
    addr.absAddr = absAddr + addr.bufferAddr;
#endif
    LocalTensor<T> output;
    output.SetAddr(addr);
    return output;
}

template <TPosition pos>
__aicore__ inline void TBuf<pos>::SetTpipeBuf(TBufType* bufStartIn, uint32_t bufLenIn)
{
    this->bufStart = bufStartIn;
    this->bufLen = bufLenIn;
    this->offset = 0;
}

template <TPosition pos>
template <typename T>
__aicore__ inline void TBuf<pos>::EnQue(const LocalTensor<T>& input)
{
    (void)(0);
}

template <TPosition pos>
template <typename T>
__aicore__ inline LocalTensor<T> TBuf<pos>::DeQue()
{
    return Get<T>();
}

template <TPosition pos>
template <typename T>
__aicore__ inline __sync_alias__ LocalTensor<T> TBuf<pos>::AllocTensor()
{
    return Get<T>();
}

template <TPosition pos>
template <typename T>
__aicore__ inline void TBuf<pos>::FreeTensor(LocalTensor<T>& input)
{
    (void)(0);
}

template <TPosition pos>
template <typename T>
__aicore__ inline TBufState TBuf<pos>::GetState(const LocalTensor<T>& input) const
{
    TBufHandle handle = input.GetBufferHandle();
    if (handle == nullptr) {
        return TBufState::FREE;
    }
    auto ptr = reinterpret_cast<TBufType*>(handle);
    return ptr->state;
}

template <TPosition pos>
__aicore__ inline bool TBuf<pos>::EnQue(TBufHandle buf)
{
    return true;
}

template <TPosition pos>
__aicore__ inline TBufHandle TBuf<pos>::DeQue()
{
    return Get();
}

template <TPosition pos>
__aicore__ inline TBufHandle TBuf<pos>::AllocBuffer()
{
    return Get();
}

template <TPosition pos>
__aicore__ inline void TBuf<pos>::FreeBuffer(TBufHandle buf)
{
    (void)(0);
}

template <TPosition pos>
__aicore__ inline TBuffAddr TBuf<pos>::GetBufferAddr(TBufHandle buf)
{
    auto ptr = reinterpret_cast<TBufType*>(buf);
    TBuffAddr addr;
    addr.logicPos = static_cast<uint8_t>(pos);
    addr.bufferHandle = buf;
    addr.bufferAddr = ptr->address;
    addr.dataLen = ptr->dataLen;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    auto absAddr = GetTPipePtr()->g_tpipeImpl.bufPoolBaseAddr_[static_cast<uint8_t>(GetPhyType(pos))].absAddr;
    addr.absAddr = absAddr + addr.bufferAddr;
#endif
    return addr;
}

template <TPosition pos>
__aicore__ inline void TBuf<pos>::InitStartBufHandle(TBufHandle startBufhandle, uint8_t num, uint32_t len)
{
    static_assert(!isTQue, "InitTBufAddr only support TBuf class");
    ASCENDC_DEBUG_ASSERT(
        (num == 1), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "param num must be one for TBuf, current num is %d", num));
    auto ptr = reinterpret_cast<TBufType*>(startBufhandle);
    this->bufStart = ptr;
    this->bufLen = len;
    this->offset = 0;
    return;
}

template <TPosition pos>
__aicore__ inline TBufHandle TBuf<pos>::Get(uint32_t len)
{
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ASCENDC_DEBUG_ASSERT(
        (len <= bufLen), KERNEL_LOG_INTERNAL(KERNEL_ERROR, "len is %u, max buffer len is %u", len, bufLen));
#endif
    this->bufStart->dataLen = len;
    return reinterpret_cast<TBufHandle>(this->bufStart);
}

template <TPosition pos>
__aicore__ inline TBufHandle TBuf<pos>::Get()
{
    return Get(bufLen);
}

template <TPosition pos>
__aicore__ inline uint32_t TBuf<pos>::GetBufLen() const
{
    return bufLen;
}

} // namespace AscendC
#endif // ASCENDC_MODULE_TBUF_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TBUF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_TBUF_IMPL_H__
#endif
