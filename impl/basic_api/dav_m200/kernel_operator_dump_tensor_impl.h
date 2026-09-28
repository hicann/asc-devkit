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
 * \file kernel_operator_dump_tensor_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m200/kernel_operator_dump_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H

#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_tensor.h"
#include "kernel_operator_common_impl.h"
#include "../kernel_tpipe_impl.h"
#include "kernel_operator_data_copy_impl.h"
#include "../kernel_pop_stack_buffer.h"
#include "../../utils/debug/asc_aicore_printf_impl.h"

namespace AscendC {
/* **************************************************************************************************
 * DumpTensorImpl                                             *
 * ************************************************************************************************* */
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_dumpWorkspaceReserved;

template <typename T>
__aicore__ constexpr inline Internal::DumpTensorDataType GetTensorDataType();

template <typename T>
__aicore__ inline uint32_t GetDataType(T data)
{
    return static_cast<uint32_t>(GetTensorDataType<T>());
}

template <typename T>
__aicore__ inline void InitTmpTensor(LocalTensor<T>& tmp, uint8_t quePos)
{
    TBuffAddr tbufTmpLocal;
    tbufTmpLocal.logicPos = quePos;
    tmp.SetAddr(tbufTmpLocal);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    tmp.address_.absAddr = reinterpret_cast<uint8_t*>(ConstDefiner::Instance().cpuUB);
#else
    tmp.address_.bufferAddr = get_imm(0);
#endif
    tmp.address_.dataLen = ONE_DUMP_BACKUP_SIZE;
}

__aicore__ inline int64_t GetBlockNum();
__aicore__ inline void InitDumpImpl(bool mixFlag, uint32_t gmLen) {}

/***********************************每个core内存分配示意图*************************************************

|------------------------------------------------core_0-------- --------------------------------------|
|---已使用addr、 MAGIC---|---bMsg_1---|---data_1---|---bMsg_2---|---data_2---|///未使用///|---backup---|
           |________________________________________________________________^

**********************************************************************************************************/

template <template <typename> class Tensor, typename T>
__aicore__ inline void DumpTensorRingBufImpl(
    const Tensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim);

template <typename T>
__aicore__ inline void DumpTensorLocal2GMImpl(
    const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE);
    if (g_sysPrintFifoSpace != nullptr) {
        DumpTensorRingBufImpl(src, desc, dumpSize, shape, shapeDim);
    }
    set_ctrl(ctrlValue);
}

template <typename T>
__aicore__ inline void DumpTensorLocal2GMImpl(const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    DumpTensorLocal2GMImpl(src, desc, dumpSize, nullptr, 0);
}

/***********************************每个core内存分配示意图*************************************************

|------------------------------------------------core_0-----------------------------------------------|
|---已使用addr、 MAGIC---|---bMsg_1---|---data_1---|---bMsg_2---|---data_2---|///未使用///|---backup---|
           |________________________________________________________________^

**********************************************************************************************************/

template <typename T>
__aicore__ inline void DumpTensorGM2GMImpl(
    const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE);
    if (g_sysPrintFifoSpace != nullptr) {
        DumpTensorRingBufImpl(src, desc, dumpSize, shape, shapeDim);
    }
    set_ctrl(ctrlValue);
}

template <typename T>
__aicore__ inline void DumpTensorGM2GMImpl(const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize)
{
    DumpTensorGM2GMImpl(src, desc, dumpSize, nullptr, 0);
}

__aicore__ inline void WriteRingBufShapeInfo(const ShapeInfo& shapeInfo);

__aicore__ inline void DumpShapeImpl(const ShapeInfo& shapeInfo)
{
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE);
    if (g_sysPrintFifoSpace != nullptr) {
        WriteRingBufShapeInfo(shapeInfo);
    }
}

__aicore__ inline uint32_t GetStringLength(__gm__ const char* s)
{
    uint32_t i = 0;
    while (*(s + i) != '\0') {
        i++;
    }
    return i + 1;
}

template <class... Args>
__aicore__ inline void PrintfImpl(DumpType printType, __gm__ const char* fmt, Args&&... args)
{
    __asc_aicore::printf_impl(fmt, args...);
}

__aicore__ inline void EnablePrintf()
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0) || defined(ASCENDC_TIME_STAMP_ON)
    static const struct BinaryMetaAscFeature __asc_feature_print__
        __attribute__((used, section(".ascend.meta"))) = {4, 4, 1};
#endif // defined(ASCENDC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
}

__aicore__ __gm__ inline RingBufReadInfo* GetRingBufReadInfo(__gm__ BlockRingBufInfo* blockRingBufInfo)
{
    __gm__ uint8_t* blockHead = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo);

    return reinterpret_cast<__gm__ RingBufReadInfo*>(blockHead + sizeof(BlockRingBufInfo));
}

__aicore__ __gm__ inline RingBufWriteInfo* GetRingBufWriteInfo(__gm__ BlockRingBufInfo* blockRingBufInfo)
{
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo->ringBufAddr);

    return reinterpret_cast<__gm__ RingBufWriteInfo*>(ringBufAddr + blockRingBufInfo->ringBufLen);
}

template <uint64_t timeOutCycle = 1000 * 1000> // 20ms
__aicore__ inline void RingBufferWaitRtsSync()
{
    volatile uint64_t counter = 0;
    while (counter < timeOutCycle) {
        counter++;
        __asm__ __volatile__("");
    }
}

__aicore__ inline bool WaitRingBufBeginRead(volatile __gm__ RingBufReadInfo* readInfo)
{
    constexpr uint32_t maxCounter = 15;
    volatile uint32_t counter = 0;
    while (readInfo->bufOffset == 0) {
        if (counter >= maxCounter) { // max wait 300ms, rts read gm per 200ms
            return false;
        }
        RingBufferWaitRtsSync(); // wait 20 ms
        ++counter;
        dcci((__gm__ uint64_t*)(readInfo), cache_line_t::ENTIRE_DATA_CACHE);
    }
    return true;
}

__aicore__ inline void SkipRingBufDirectly(volatile __gm__ RingBufWriteInfo* writeInfo)
{
    writeInfo->bufOffset = 0;
    dcci((__gm__ uint64_t*)(writeInfo), cache_line_t::ENTIRE_DATA_CACHE);
    return;
}

__aicore__ inline void SkipRingBufWithInfo(
    volatile __gm__ RingBufWriteInfo* writeInfo, __gm__ uint8_t* ringBufAddr, const uint32_t& ringBufLen)
{
    __gm__ SkipTlvInfo* skipInfo = reinterpret_cast<__gm__ SkipTlvInfo*>(ringBufAddr + writeInfo->bufOffset);
    skipInfo->type = static_cast<uint32_t>(DumpType::DUMP_SKIP);
    skipInfo->length = ringBufLen - writeInfo->bufOffset - sizeof(SkipTlvInfo);
    writeInfo->bufOffset = 0;
    writeInfo->packIdx += 1;
    dcci((__gm__ uint64_t*)(skipInfo), cache_line_t::ENTIRE_DATA_CACHE);
    dcci((__gm__ uint64_t*)(writeInfo), cache_line_t::ENTIRE_DATA_CACHE);
    return;
}

__aicore__ inline bool RingBufferWait(
    volatile __gm__ RingBufReadInfo* readInfo, volatile __gm__ RingBufWriteInfo* writeInfo, const uint32_t& tlvLen)
{
    constexpr uint32_t maxCounter = 15;
    volatile uint32_t counter = 0;
    while (writeInfo->bufOffset < readInfo->bufOffset && writeInfo->bufOffset + tlvLen >= readInfo->bufOffset) {
        if (counter >= maxCounter) { // max wait 300ms, rts read gm per 200ms
            return false;
        }
        RingBufferWaitRtsSync(); // wait 20 ms
        ++counter;
        dcci((__gm__ uint64_t*)(readInfo), cache_line_t::ENTIRE_DATA_CACHE);
    }
    return true;
}

__aicore__ inline bool CheckAndWaitRingBufSpace(__gm__ BlockRingBufInfo* blockRingBufInfo, const uint32_t& tlvLen)
{
    constexpr uint32_t minTlvLen = sizeof(SkipTlvInfo);

    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo->ringBufAddr);
    uint32_t ringBufLen = blockRingBufInfo->ringBufLen;

    volatile __gm__ RingBufReadInfo* readInfo = GetRingBufReadInfo(blockRingBufInfo);
    volatile __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    if (minTlvLen >= ringBufLen || tlvLen > ringBufLen) {
        return false;
    } else if (writeInfo->bufOffset + minTlvLen >= ringBufLen) {
        if (!WaitRingBufBeginRead(readInfo)) { // check read is begin
            return false;
        }
        SkipRingBufDirectly(writeInfo);
    } else if (writeInfo->bufOffset + tlvLen > ringBufLen) {
        if (!WaitRingBufBeginRead(readInfo)) { // check read is begin
            return false;
        }
        SkipRingBufWithInfo(writeInfo, ringBufAddr, ringBufLen);
    }
    if (writeInfo->packIdx > 0 && writeInfo->bufOffset < readInfo->bufOffset &&
        writeInfo->bufOffset + tlvLen >= readInfo->bufOffset) {
        return RingBufferWait(readInfo, writeInfo, tlvLen);
    }
    return true;
}

__aicore__ __gm__ inline uint8_t* GetRingBufTlv(__gm__ BlockRingBufInfo* blockRingBufInfo)
{
    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo->ringBufAddr);
    return ringBufAddr + writeInfo->bufOffset;
}

__aicore__ __gm__ inline BlockRingBufInfo* GetBlockRingBufInfo()
{
    uint32_t blockIdx = (GetBlockIdxImpl() & 0x00FF) % DUMP_CORE_COUNT;
    uint32_t blockLength = reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace)->length;
    __gm__ BlockRingBufInfo* ringBufInfo =
        reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace + blockLength * blockIdx);
    return ringBufInfo->magic == 0xAE86 ? ringBufInfo : nullptr;
}

__aicore__ inline void UpdateWriteInfo(__gm__ RingBufWriteInfo* writeInfo, const uint32_t& tlvLen)
{
    writeInfo->bufOffset += tlvLen;
    writeInfo->packIdx += 1;
    dcci((__gm__ uint64_t*)(writeInfo), cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline void MemCopyGm2Gm(__gm__ uint8_t* dst, __gm__ const uint8_t* src, const uint32_t& len)
{
    if (dst == nullptr || src == nullptr) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        *(dst + i) = *(src + i);
    }
    dcci((__gm__ uint64_t*)(dst), cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline void SetStringArg(
    __gm__ uint8_t* paramAddr, uint32_t paramIdx, __gm__ const char* s, uint32_t& offset)
{
    __gm__ uint64_t* stringAddr = reinterpret_cast<__gm__ uint64_t*>(paramAddr) + paramIdx;
    __gm__ uint64_t* dstStrAddr = reinterpret_cast<__gm__ uint64_t*>(paramAddr + offset);

    // write string value offset
    *((__gm__ uint64_t*)stringAddr) = static_cast<uint64_t>(offset - ONE_PARAM_SIZE * paramIdx);
    dcci((__gm__ uint64_t*)stringAddr, cache_line_t::ENTIRE_DATA_CACHE);

    // write string content
    __gm__ uint8_t* d = reinterpret_cast<__gm__ uint8_t*>(dstStrAddr);
    uint32_t strLen = GetStringLength(s);
    MemCopyGm2Gm(d, reinterpret_cast<__gm__ const uint8_t*>(s), strLen);
    offset += strLen;
}

template <typename T>
__aicore__ inline void SetValueArg(__gm__ uint8_t* paramAddr, uint32_t paramIdx, T scalar)
{
    static_assert(!SupportType<T, double>(), "printf unsupport double type");

    __gm__ uint64_t* scalarAddr = (__gm__ uint64_t*)paramAddr + paramIdx;
    *scalarAddr = 0;

    if constexpr (SupportType<T, half, float>()) {
        *((__gm__ float*)scalarAddr) = static_cast<float>(scalar);
    } else if constexpr (std::is_signed<T>::value) {
        *((__gm__ int64_t*)scalarAddr) = static_cast<int64_t>(scalar);
    } else if constexpr (std::is_unsigned<T>::value) {
        *((__gm__ uint64_t*)scalarAddr) = static_cast<uint64_t>(scalar);
    } else if constexpr (std::is_pointer<T>::value) {
        *((__gm__ uint64_t*)scalarAddr) = (uintptr_t)scalar;
    } else if constexpr (std::is_enum<T>::value) {
        *((__gm__ uint64_t*)scalarAddr) = static_cast<uint64_t>(scalar);
    }

    dcci((__gm__ uint64_t*)scalarAddr, cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline void SetArgsData(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset) { return; }

template <typename... Args>
__aicore__ inline void SetArgsData(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, Args&&... args);

template <typename... Args>
__aicore__ inline void SetArgsDataImpl(
    __gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, __gm__ const char* s, Args&&... args)
{
    SetStringArg(paramAddr, paramIdx, s, offset);
    SetArgsData(paramAddr, paramIdx + 1, offset, args...);
}

template <typename T, typename... Args>
__aicore__ inline void SetArgsDataImpl(
    __gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, T scalar, Args&&... args)
{
    SetValueArg(paramAddr, paramIdx, scalar);
    SetArgsData(paramAddr, paramIdx + 1, offset, args...);
}

template <typename... Args>
__aicore__ inline void SetArgsData(__gm__ uint8_t* paramAddr, uint32_t paramIdx, uint32_t& offset, Args&&... args)
{
    SetArgsDataImpl(paramAddr, paramIdx, offset, args...);
}

template <typename... Args>
__aicore__ inline void WriteRingBufTlvData(__gm__ PrintTlvInfoHead* printTlv, __gm__ const char* fmt, Args&&... args)
{
    const uint32_t& strLen = GetStringLength(fmt);
    __gm__ uint8_t* paramAddr = reinterpret_cast<__gm__ uint8_t*>(printTlv + 1);
    __gm__ uint8_t* fmtAddr = paramAddr + printTlv->fmtOffset - sizeof(uint64_t);
    __gm__ uint8_t* strParamAddr = reinterpret_cast<__gm__ uint8_t*>(fmtAddr) + strLen;
    MemCopyGm2Gm(fmtAddr, reinterpret_cast<__gm__ const uint8_t*>(fmt), strLen);
    uint32_t strParamOffset = printTlv->fmtOffset + strLen;
    SetArgsData(paramAddr, 0, strParamOffset, args...);
}

template <typename T>
__aicore__ inline Hardware CheckDumpTensorPosition(const LocalTensor<T>& src)
{
    Hardware position = GetPhyType(static_cast<TPosition>(src.GetPosition()));
    if (position != Hardware::UB && position != Hardware::L1 && position != Hardware::L0C) {
        return Hardware::MAX;
    }
    return position;
}

template <typename T>
__aicore__ constexpr inline Internal::DumpTensorDataType GetTensorDataType()
{
    if constexpr (IsSameType<T, bool>::value) {
        return Internal::DumpTensorDataType::ACL_BOOL;
    } else if (IsSameType<T, uint8_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT8;
    } else if (IsSameType<T, int8_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT8;
    } else if (IsSameType<T, int16_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT16;
    } else if (IsSameType<T, uint16_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT16;
    } else if (IsSameType<T, int32_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT32;
    } else if (IsSameType<T, uint32_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT32;
    } else if (IsSameType<T, uint64_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT64;
    } else if (IsSameType<T, int64_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT64;
    } else if (IsSameType<T, float>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT;
    } else if (IsSameType<T, half>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT16;
    } else {
        return Internal::DumpTensorDataType::ACL_MAX;
    }
}

template <template <typename> class Tensor, typename T>
__aicore__ inline void WriteRingBufTlvHead(
    const Tensor<T>& src, __gm__ DumpTensorTlvInfoHead* dumpTensorTlv, const uint32_t& alignDumpDataLen,
    const uint32_t& desc, const uint32_t& dumpSize)
{
    Hardware position;
    if constexpr (IsSameType<Tensor<T>, LocalTensor<T>>::value) {
        position = GetPhyType(static_cast<TPosition>(src.GetPosition()));
    } else if (IsSameType<Tensor<T>, GlobalTensor<T>>::value) {
        position = Hardware::GM;
    }
    dumpTensorTlv->type = static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    dumpTensorTlv->length = sizeof(DumpTensorTlvInfoHead) - sizeof(uint32_t[2]) + alignDumpDataLen;
    dumpTensorTlv->tensorAddr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr()));
    dumpTensorTlv->dataType = static_cast<uint32_t>(GetTensorDataType<T>());
    dumpTensorTlv->desc = desc;
    dumpTensorTlv->bufferId = static_cast<uint32_t>(0U);
    dumpTensorTlv->position = static_cast<uint16_t>(position);
    dumpTensorTlv->blockIdx = static_cast<uint16_t>(GetBlockIdxImpl());
    dumpTensorTlv->dim = static_cast<uint32_t>(0U);
    for (uint32_t i = 0; i < K_MAX_SHAPE_DIM; ++i) {
        dumpTensorTlv->shape[i] = static_cast<uint32_t>(0U);
    }
    dumpTensorTlv->resv1 = static_cast<uint32_t>(0U);
    dumpTensorTlv->dumpSize = dumpSize * sizeof(T);
    dcci((__gm__ uint64_t*)(dumpTensorTlv), cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline void WriteRingBufTlvShape(
    __gm__ DumpTensorTlvInfoHead* dumpTensorTlv, const uint32_t shapeDim, const uint32_t* shape)
{
    if (shapeDim <= 0 || shapeDim >= K_MAX_SHAPE_DIM || shape == nullptr) {
        return;
    }
    dumpTensorTlv->dim = static_cast<uint32_t>(shapeDim);
    for (uint32_t i = 0; i < K_MAX_SHAPE_DIM; ++i) {
        dumpTensorTlv->shape[i] = i < shapeDim ? static_cast<uint32_t>(shape[i]) : static_cast<uint32_t>(1U);
    }
    dcci((__gm__ uint64_t*)(dumpTensorTlv), cache_line_t::ENTIRE_DATA_CACHE);
}

__aicore__ inline void ClearGmData(__gm__ uint8_t* dst, uint32_t len)
{
    if (dst == nullptr) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        *(dst + i) = 0x00;
    }
    dcci((__gm__ uint64_t*)(dst), cache_line_t::ENTIRE_DATA_CACHE);
}

template <typename T>
__aicore__ inline void SetDumpDataL12GM(__gm__ uint8_t* dst, const LocalTensor<T>& src, uint32_t alignDumpDataLen)
{
    DataCopyParams param = {1, static_cast<uint16_t>(alignDumpDataLen / ONE_BLK_SIZE), 0, 0};
    __gm__ uint8_t* gmBackAddr = dst + alignDumpDataLen;
    LocalTensor<uint8_t> tmp;
    InitTmpTensor(tmp, static_cast<uint8_t>(TPosition::VECIN));
    DataCopyUB2GMImpl(
        reinterpret_cast<__gm__ uint8_t*>(gmBackAddr), reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()),
        param); // backup data in UB
    PipeBarrier<PIPE_ALL>();
    DataCopyL12UBImpl(
        reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()), reinterpret_cast<__cbuf__ uint8_t*>(src.GetPhyAddr()),
        param); // L1 to UB
    PipeBarrier<PIPE_ALL>();
    DataCopyUB2GMImpl(
        reinterpret_cast<__gm__ uint8_t*>(dst), reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()),
        param); // UB to GM
    PipeBarrier<PIPE_ALL>();
    DataCopyGM2UBImpl(
        reinterpret_cast<__ubuf__ uint32_t*>(tmp.GetPhyAddr()), reinterpret_cast<__gm__ uint32_t*>(gmBackAddr),
        param); // recover data in UB
    PipeBarrier<PIPE_ALL>();
    ClearGmData(gmBackAddr, alignDumpDataLen);
}

template <typename T>
__aicore__ inline void SetDumpDataL0C2GM(__gm__ uint8_t* dst, const LocalTensor<T>& src, uint32_t alignDumpDataLen)
{
    constexpr uint32_t blockSize = 16 * 16 * sizeof(uint32_t);
    uint32_t copyLen = AlignUp(alignDumpDataLen, blockSize);
    DataCopyParams param = {1, static_cast<uint16_t>(copyLen / ONE_BLK_SIZE), 0, 0};
    __gm__ uint8_t* gmBackAddr = dst + copyLen;
    DataCopyEnhancedParams enhancedParams;
    enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX;
    DataCopyParams backupParamsL0C;
    backupParamsL0C.blockCount = 1;
    backupParamsL0C.blockLen = copyLen / blockSize;
    LocalTensor<uint8_t> tmp;
    InitTmpTensor(tmp, static_cast<uint8_t>(TPosition::VECIN));
    DataCopyUB2GMImpl(
        reinterpret_cast<__gm__ uint8_t*>(gmBackAddr), reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()),
        param); // backup data in UB
    PipeBarrier<PIPE_ALL>();
    DataCopyL0C2UBImpl(
        reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()), reinterpret_cast<__cc__ int32_t*>(src.GetPhyAddr()),
        backupParamsL0C,
        enhancedParams); // L0C to UB
    PipeBarrier<PIPE_ALL>();
    DataCopyUB2GMImpl(
        reinterpret_cast<__gm__ uint8_t*>(dst), reinterpret_cast<__ubuf__ uint8_t*>(tmp.GetPhyAddr()),
        param); // UB to GM
    PipeBarrier<PIPE_ALL>();
    DataCopyGM2UBImpl(
        reinterpret_cast<__ubuf__ uint32_t*>(tmp.GetPhyAddr()), reinterpret_cast<__gm__ uint32_t*>(gmBackAddr),
        param); // recover data in UB
    PipeBarrier<PIPE_ALL>();
    uint32_t tailLen = copyLen * 2 - alignDumpDataLen;
    ClearGmData(dst + alignDumpDataLen, tailLen);
}

template <typename T>
__aicore__ inline void WriteRingBufTlvData(
    const LocalTensor<T>& src, __gm__ DumpTensorTlvInfoHead* dumpTensorTlv, const uint32_t& alignDumpDataLen,
    const uint32_t& dumpSize)
{
    __gm__ T* dumpDataAddr = reinterpret_cast<__gm__ T*>(dumpTensorTlv + 1);

    PipeBarrier<PIPE_ALL>();

    if (dumpTensorTlv->position == static_cast<uint16_t>(Hardware::UB)) { // UB to GM
        DataCopyParams copyParams = {1, static_cast<uint16_t>(alignDumpDataLen / ONE_BLK_SIZE), 0, 0};
        DataCopyUB2GMImpl(dumpDataAddr, reinterpret_cast<__ubuf__ T*>(src.GetPhyAddr()), copyParams);
    } else if (dumpTensorTlv->position == static_cast<uint16_t>(Hardware::L1)) { // L1 to GM
        SetDumpDataL12GM(reinterpret_cast<__gm__ uint8_t*>(dumpDataAddr), src, alignDumpDataLen);
    } else if (dumpTensorTlv->position == static_cast<uint16_t>(Hardware::L0C)) { // L0C to GM
        SetDumpDataL0C2GM(reinterpret_cast<__gm__ uint8_t*>(dumpDataAddr), src, alignDumpDataLen);
    }

    PipeBarrier<PIPE_ALL>();

    dcci((__gm__ uint64_t*)dumpDataAddr, cache_line_t::ENTIRE_DATA_CACHE);
}

template <typename T>
__aicore__ inline void WriteRingBufTlvData(
    const GlobalTensor<T>& src, __gm__ DumpTensorTlvInfoHead* dumpTensorTlv, const uint32_t& dumpSize)
{
    PipeBarrier<PIPE_ALL>();
    __gm__ uint8_t* dst = reinterpret_cast<__gm__ uint8_t*>(dumpTensorTlv + 1);
    MemCopyGm2Gm(dst, reinterpret_cast<__gm__ const uint8_t*>(src.GetPhyAddr()), dumpSize * sizeof(T));
}

#ifndef ASCENDC_CPU_DEBUG
template <typename T>
__aicore__ inline void WriteSuperTensorData(
    const GlobalTensor<T>& src, __gm__ uint8_t* dst, uint32_t alignDumpLen, uint32_t dumpSize)
{
    PipeBarrier<PIPE_ALL>();
    MemCopyGm2Gm(dst, reinterpret_cast<__gm__ const uint8_t*>(src.GetPhyAddr()), dumpSize);
}

template <typename T>
__aicore__ inline void WriteSuperTensorData(
    const LocalTensor<T>& src, __gm__ uint8_t* dst, uint32_t alignDumpLen, uint32_t dumpSize)
{
    PipeBarrier<PIPE_ALL>();
    Hardware position = CheckDumpTensorPosition(src);
    if (position == Hardware::UB) {
        DataCopyParams params = {1, static_cast<uint16_t>(alignDumpLen / ONE_BLK_SIZE), 0, 0};
        DataCopyUB2GMImpl(dst, reinterpret_cast<__ubuf__ uint8_t*>(src.GetPhyAddr()), params);
    } else if (position == Hardware::L1) {
        SetDumpDataL12GM(dst, src, alignDumpLen);
    } else if (position == Hardware::L0C) {
        SetDumpDataL0C2GM(dst, src, alignDumpLen);
    }
    PipeBarrier<PIPE_ALL>();
    dcci(reinterpret_cast<__gm__ uint64_t*>(dst), cache_line_t::ENTIRE_DATA_CACHE);
}

template <template <typename> class Tensor, typename T>
__aicore__ inline void WriteSuperTensorHead(
    const Tensor<T>& src, __gm__ __asc_aicore::DumpSuperTensorTlv* dumpTlv, Hardware position, uint32_t desc,
    uint64_t tensorLength, uint32_t dumpSize, uint32_t alignDumpLen, const uint32_t* shape, uint32_t shapeDim)
{
    dumpTlv->type = static_cast<uint32_t>(__asc_aicore::DumpType::DUMP_SUPER_TENSOR);
    dumpTlv->length = sizeof(__asc_aicore::DumpSuperTensorTlv) - sizeof(uint32_t[2]) + alignDumpLen;
    dumpTlv->tensorAddr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr()));
    dumpTlv->dataType = static_cast<uint32_t>(GetTensorDataType<T>());
    dumpTlv->desc = desc;
    dumpTlv->bufferId = 0U;
    dumpTlv->position = static_cast<uint16_t>(position);
    dumpTlv->blockIdx = static_cast<uint16_t>(GetBlockIdxImpl());
    dumpTlv->dim = shape != nullptr && shapeDim <= K_MAX_SHAPE_DIM ? shapeDim : 0U;
    for (uint32_t i = 0; i < K_MAX_SHAPE_DIM; ++i) {
        dumpTlv->shape[i] = dumpTlv->dim == 0U ? 0U : (i < dumpTlv->dim ? shape[i] : 1U);
    }
    dumpTlv->resv = 0U;
    dumpTlv->tensorLength = tensorLength;
    dumpTlv->tensorOffset = 0U;
    dumpTlv->dumpSize = dumpSize;
    dcci(reinterpret_cast<__gm__ uint64_t*>(dumpTlv), cache_line_t::ENTIRE_DATA_CACHE);
}

template <template <typename> class Tensor, typename T>
__aicore__ inline void DumpSuperTensorRingBufImpl(
    const Tensor<T>& src, uint32_t desc, uint64_t tensorLength, const uint32_t* shape, uint32_t shapeDim,
    __gm__ BlockRingBufInfo* blockRingBufInfo)
{
    Hardware position = Hardware::GM;
    if constexpr (IsSameType<Tensor<T>, LocalTensor<T>>::value) {
        position = CheckDumpTensorPosition(src);
    }
    const bool needBackup = position == Hardware::L1 || position == Hardware::L0C;
    const uint32_t alignment = position == Hardware::L0C ? 16U * 16U * sizeof(uint32_t) : ONE_BLK_SIZE;
    // The legacy and common headers describe the same FIFO ABI. Keep the 2002 core lookup.
    static_assert(sizeof(BlockRingBufInfo) == sizeof(__asc_aicore::DebugBlockHeadInfo));
    static_assert(sizeof(RingBufReadInfo) == sizeof(__asc_aicore::DebugBlockReadInfo));
    static_assert(sizeof(RingBufWriteInfo) == sizeof(__asc_aicore::DebugBlockWriteInfo));
    auto* blockInfo = reinterpret_cast<__gm__ __asc_aicore::DebugBlockHeadInfo*>(blockRingBufInfo);
    uint64_t tensorOffset = 0U;
    while (tensorOffset < tensorLength) {
        const bool isFirst = tensorOffset == 0U;
        const uint32_t headerLen =
            isFirst ? sizeof(__asc_aicore::DumpSuperTensorTlv) : sizeof(__asc_aicore::DumpSuperTensorBodyTlv);
        if (blockInfo->ringBufLen <= headerLen) {
            return;
        }
        uint32_t payloadCapacity = (blockInfo->ringBufLen - headerLen) / (needBackup ? 2U : 1U);
        // L1/L0C reuse the UB scratch tensor; UB DMA blockLen is a uint16_t.
        const uint32_t maxCopyLen = needBackup ? ONE_DUMP_BACKUP_SIZE : 0xFFFFU * ONE_BLK_SIZE;
        if (position != Hardware::GM && payloadCapacity > maxCopyLen) {
            payloadCapacity = maxCopyLen;
        }
        payloadCapacity = payloadCapacity / alignment * alignment;
        if (payloadCapacity == 0U) {
            return;
        }
        const uint64_t remaining = tensorLength - tensorOffset;
        const uint32_t dumpSize = remaining > payloadCapacity ? payloadCapacity : static_cast<uint32_t>(remaining);
        const uint32_t alignDumpLen = AlignUp(dumpSize, alignment);
        const uint32_t tlvLen = headerLen + alignDumpLen;
        const uint32_t reserveLen = tlvLen + (needBackup ? alignDumpLen : 0U);
        if (!__asc_aicore::check_ringbuf_space(blockInfo, reserveLen)) {
            return;
        }
        __gm__ uint8_t* tlvAddr = __asc_aicore::get_ringbuf_tlv_addr(blockInfo);
        if (isFirst) {
            auto* dumpTlv = reinterpret_cast<__gm__ __asc_aicore::DumpSuperTensorTlv*>(tlvAddr);
            WriteSuperTensorHead(src, dumpTlv, position, desc, tensorLength, dumpSize, alignDumpLen, shape, shapeDim);
        } else {
            auto* dumpTlv = reinterpret_cast<__gm__ __asc_aicore::DumpSuperTensorBodyTlv*>(tlvAddr);
            dumpTlv->type = static_cast<uint32_t>(__asc_aicore::DumpType::DUMP_SUPER_TENSOR_BODY);
            dumpTlv->length = tlvLen - sizeof(uint32_t[2]);
            dumpTlv->resv1 = 0U;
            dumpTlv->resv2 = 0U;
            dumpTlv->tensorLength = tensorLength;
            dumpTlv->tensorOffset = tensorOffset;
            dumpTlv->dumpSize = dumpSize;
            dcci(reinterpret_cast<__gm__ uint64_t*>(dumpTlv), cache_line_t::ENTIRE_DATA_CACHE);
        }
        auto chunkSrc = src[static_cast<uint32_t>(tensorOffset / sizeof(T))];
        WriteSuperTensorData(chunkSrc, tlvAddr + headerLen, alignDumpLen, dumpSize);
        // Publish only the TLV, after the borrowed UB has been restored.
        __asc_aicore::update_write_info(__asc_aicore::get_block_write_info(blockInfo), tlvLen);
        tensorOffset += dumpSize;
    }
}
#endif

template <template <typename> class Tensor, typename T>
__aicore__ inline void DumpTensorRingBufImpl(
    const Tensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    EnablePrintf();
    if constexpr (GetTensorDataType<T>() == Internal::DumpTensorDataType::ACL_MAX) {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "dump tensor not support this data type"); });
        return;
    }
    if (dumpSize == 0) {
        return;
    }
    if constexpr (IsSameType<Tensor<T>, LocalTensor<T>>::value) {
        if (CheckDumpTensorPosition(src) == Hardware::MAX) {
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "dump tensor only support dump tensor from local to gm"); });
            return;
        }
    }
    __gm__ BlockRingBufInfo* blockRingBufInfo = GetBlockRingBufInfo();
    if (blockRingBufInfo == nullptr) {
        return;
    }
    constexpr uint32_t blockSize = 16 * 16 * sizeof(uint32_t);
    const uint64_t tensorLength = static_cast<uint64_t>(dumpSize) * sizeof(T);
    const uint64_t alignDumpLen64 = (tensorLength + ONE_BLK_SIZE - 1U) / ONE_BLK_SIZE * ONE_BLK_SIZE;
    const uint64_t maxResvLen64 =
        sizeof(DumpTensorTlvInfoHead) + (alignDumpLen64 + blockSize - 1U) / blockSize * blockSize * 2U;
    if (maxResvLen64 > blockRingBufInfo->ringBufLen) {
#ifndef ASCENDC_CPU_DEBUG
        DumpSuperTensorRingBufImpl(src, desc, tensorLength, shape, shapeDim, blockRingBufInfo);
#endif
        return;
    }
    const uint32_t alignDumpDataLen = static_cast<uint32_t>(alignDumpLen64);
    const uint32_t tlvLen = sizeof(DumpTensorTlvInfoHead) + alignDumpDataLen;
    const uint32_t maxResvLen = static_cast<uint32_t>(maxResvLen64);
    if (!CheckAndWaitRingBufSpace(blockRingBufInfo, maxResvLen)) { // Reserved Backup Capacity
        return;
    }

    __gm__ DumpTensorTlvInfoHead* dumpTensorTlv =
        reinterpret_cast<__gm__ DumpTensorTlvInfoHead*>(GetRingBufTlv(blockRingBufInfo));

    WriteRingBufTlvHead(src, dumpTensorTlv, alignDumpDataLen, desc, dumpSize);
    WriteRingBufTlvShape(dumpTensorTlv, shapeDim, shape);
    if constexpr (IsSameType<Tensor<T>, LocalTensor<T>>::value) {
        WriteRingBufTlvData(src, dumpTensorTlv, alignDumpDataLen, dumpSize);
    } else if (IsSameType<Tensor<T>, GlobalTensor<T>>::value) {
        WriteRingBufTlvData(src, dumpTensorTlv, dumpSize);
    }

    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    UpdateWriteInfo(writeInfo, tlvLen);
#endif // ASCENDC_DUMP
}

__aicore__ inline void WriteRingBufShapeInfo(const ShapeInfo& shapeInfo)
{
    __gm__ BlockRingBufInfo* blockRingBufInfo = GetBlockRingBufInfo();
    if (blockRingBufInfo == nullptr) {
        return;
    }
    uint32_t tlvLen = sizeof(DumpShapeTlvInfo);
    if (!CheckAndWaitRingBufSpace(blockRingBufInfo, tlvLen)) {
        return;
    }
    __gm__ DumpShapeTlvInfo* shapeTlv = reinterpret_cast<__gm__ DumpShapeTlvInfo*>(GetRingBufTlv(blockRingBufInfo));
    shapeTlv->type = static_cast<uint32_t>(DumpType::DUMP_SHAPE);
    shapeTlv->length = tlvLen - sizeof(uint32_t[2]);
    shapeTlv->dim = shapeInfo.shapeDim;
    for (uint32_t i = 0; i < K_MAX_SHAPE_DIM; ++i) {
        shapeTlv->shape[i] = shapeInfo.shape[i];
    }
    shapeTlv->resv = static_cast<uint32_t>(0U);
    ;
    dcci((__gm__ uint64_t*)shapeTlv, cache_line_t::ENTIRE_DATA_CACHE);

    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    UpdateWriteInfo(writeInfo, tlvLen);
}

__aicore__ inline void DumpTimeStampImpl(uint32_t descId) { return; }

__aicore__ inline void AscendCTimeStamp(uint32_t descId, uint64_t pcPtr = 0) { return; }
__aicore__ inline void InitDump(bool mixFlag, uint32_t gmLen) {}
__aicore__ inline void InitDump(bool mixFlag, GM_ADDR dumpStartAddr, uint32_t gmLen) {}
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
