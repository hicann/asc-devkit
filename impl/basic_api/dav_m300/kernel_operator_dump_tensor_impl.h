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
    "impl/basic_api/dav_m300/kernel_operator_dump_tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DUMP_TENSOR_IMPL_H

#include "../kernel_tpipe_impl.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_data_copy_impl.h"
#include "../../../include/basic_api/kernel_operator_sys_var_intf.h"
#include "../kernel_pop_stack_buffer.h"
#include "../../../include/basic_api/kernel_struct_fixpipe.h"
#if __NPU_ARCH__ == 1001
#include "../dav_c100/kernel_operator_fixpipe_impl.h"
#elif __NPU_ARCH__ == 2002
#include "../dav_m200/kernel_operator_fixpipe_impl.h"
#elif __NPU_ARCH__ == 2201
#include "../dav_c220/kernel_operator_fixpipe_impl.h"
#elif __NPU_ARCH__ == 3002
#include "kernel_operator_fixpipe_impl.h"
#endif
#include "../../utils/debug/asc_aicore_printf_impl.h"

namespace AscendC {
__BLOCK_LOCAL__ __inline__ __gm__ uint8_t* g_dumpWorkspaceReserved;

template <typename T>
__aicore__ inline uint32_t GetDataType(T data)
{
    uint32_t type;

    if (IsSameType<T, bool>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_BOOL);
    } else if (IsSameType<T, uint8_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT8);
    } else if (IsSameType<T, int8_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT8);
    } else if (IsSameType<T, int16_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT16);
    } else if (IsSameType<T, uint16_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT16);
    } else if (IsSameType<T, int32_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT32);
    } else if (IsSameType<T, uint32_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT32);
    } else if (IsSameType<T, uint64_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_UINT64);
    } else if (IsSameType<T, int64_t>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_INT64);
    } else if (IsSameType<T, float>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT);
    } else if (IsSameType<T, half>::value) {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_FLOAT16);
    } else {
        return static_cast<uint32_t>(Internal::DumpTensorDataType::ACL_MAX);
    }
}

#ifndef ASCENDC_CPU_DEBUG
__aicore__ inline void InitDumpImpl(bool mixFlag, uint32_t gmLen) {}
__aicore__ inline DataCopyParams GetDataCopyParamImpl(uint32_t offset)
{
    DataCopyParams repeatParams;
    repeatParams.blockCount = 1;
    repeatParams.blockLen = offset / ONE_BLK_SIZE;
    repeatParams.srcStride = 0;
    repeatParams.dstStride = 0;
    return repeatParams;
}

/**
 * The ring buffer (g_sysPrintFifoSpace) read/write flow below is ported from dav_m200.
 * Only this outer flow follows m200, the underlying data copy (UB2GM / L12GM / L0C2GM)
 * still uses the original m300 implementation.
 */
template <typename T>
__aicore__ constexpr inline Internal::DumpTensorDataType GetTensorDataType()
{
    if constexpr (IsSameType<T, bool>::value) {
        return Internal::DumpTensorDataType::ACL_BOOL;
    } else if constexpr (IsSameType<T, uint8_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT8;
    } else if constexpr (IsSameType<T, int8_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT8;
    } else if constexpr (IsSameType<T, int16_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT16;
    } else if constexpr (IsSameType<T, uint16_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT16;
    } else if constexpr (IsSameType<T, int32_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT32;
    } else if constexpr (IsSameType<T, uint32_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT32;
    } else if constexpr (IsSameType<T, uint64_t>::value) {
        return Internal::DumpTensorDataType::ACL_UINT64;
    } else if constexpr (IsSameType<T, int64_t>::value) {
        return Internal::DumpTensorDataType::ACL_INT64;
    } else if constexpr (IsSameType<T, float>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT;
    } else if constexpr (IsSameType<T, half>::value) {
        return Internal::DumpTensorDataType::ACL_FLOAT16;
    } else {
        return Internal::DumpTensorDataType::ACL_MAX;
    }
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

__aicore__ inline void EnablePrintf()
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0) || defined(ASCENDC_TIME_STAMP_ON)
    static const struct BinaryMetaAscFeature __asc_feature_print__
        __attribute__((used, section(".ascend.meta"))) = {4, 4, 1};
#endif // defined(ASCENDC_DUMP) || defined(ASCENDC_TIME_STAMP_ON)
}

__aicore__ inline __gm__ RingBufReadInfo* GetRingBufReadInfo(__gm__ BlockRingBufInfo* blockRingBufInfo)
{
    __gm__ uint8_t* blockHead = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo);

    return reinterpret_cast<__gm__ RingBufReadInfo*>(blockHead + sizeof(BlockRingBufInfo));
}

__aicore__ inline __gm__ RingBufWriteInfo* GetRingBufWriteInfo(__gm__ BlockRingBufInfo* blockRingBufInfo)
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
        dcci((__gm__ uint64_t*)(readInfo), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    }
    return true;
}

__aicore__ inline void SkipRingBufDirectly(volatile __gm__ RingBufWriteInfo* writeInfo)
{
    writeInfo->bufOffset = 0;
    dcci((__gm__ uint64_t*)(writeInfo), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
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
    dcci((__gm__ uint64_t*)(skipInfo), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    dcci((__gm__ uint64_t*)(writeInfo), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
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
        dcci((__gm__ uint64_t*)(readInfo), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
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

__aicore__ inline __gm__ uint8_t* GetRingBufTlv(__gm__ BlockRingBufInfo* blockRingBufInfo)
{
    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);
    __gm__ uint8_t* ringBufAddr = reinterpret_cast<__gm__ uint8_t*>(blockRingBufInfo->ringBufAddr);
    return ringBufAddr + writeInfo->bufOffset;
}

__aicore__ inline __gm__ BlockRingBufInfo* GetBlockRingBufInfo()
{
    uint32_t blockIdx = (GetBlockIdxImpl() & 0x00FF) % DUMP_CORE_COUNT;
    uint32_t blockLength = reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace)->length;
    __gm__ BlockRingBufInfo* ringBufInfo =
        reinterpret_cast<__gm__ BlockRingBufInfo*>(g_sysPrintFifoSpace + blockLength * blockIdx);
    if (ringBufInfo->magic != 0xAE86) {
        return nullptr;
    }
    ringBufInfo->flag = 0;
    dcci(reinterpret_cast<__gm__ uint64_t*>(ringBufInfo), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    return ringBufInfo;
}

__aicore__ inline void UpdateWriteInfo(__gm__ RingBufWriteInfo* writeInfo, const uint32_t& tlvLen)
{
    writeInfo->bufOffset += tlvLen;
    writeInfo->packIdx += 1;
    dcci((__gm__ uint64_t*)(writeInfo), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

__aicore__ inline void MemCopyGm2Gm(__gm__ uint8_t* dst, __gm__ const uint8_t* src, const uint32_t& len)
{
    if (dst == nullptr || src == nullptr) {
        return;
    }
    for (uint32_t i = 0; i < len; i++) {
        *(dst + i) = *(src + i);
    }
    dcci((__gm__ uint64_t*)(dst), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

template <template <typename> class Tensor, typename T>
__aicore__ inline void WriteRingBufTlvHead(
    const Tensor<T>& src, __gm__ DumpTensorTlvInfoHead* dumpTensorTlv, const uint32_t& alignDumpDataLen,
    const uint32_t& desc, const uint32_t& dumpSize)
{
    Hardware position = Hardware::GM;
    if constexpr (IsSameType<Tensor<T>, LocalTensor<T>>::value) {
        position = GetPhyType(static_cast<TPosition>(src.GetPosition()));
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
    dcci((__gm__ uint64_t*)(dumpTensorTlv), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
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
    dcci((__gm__ uint64_t*)(dumpTensorTlv), cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

// The cce instruction of L0C to GM copy is kept the same as the original m300 implementation
// (different from m200 which goes through UB). Only the destination is changed from the workspace
// BlockInfo to the ring buffer TLV data address.
template <typename T>
__aicore__ inline void DumpTensorL0C2GMImpl(const LocalTensor<T>& src, __gm__ uint8_t* dumpAddr, uint32_t dumpSize)
{
    // L0C to GM
    uint16_t align = (dumpSize % DEFAULT_BLOCK_SIZE == 0) ? 0 : 1;
    uint16_t countBlks = align + dumpSize / DEFAULT_BLOCK_SIZE;

    uint16_t burstLen =
        static_cast<uint16_t>(SRC_BURST_LEN_SIZE_ELE * SRC_BURST_LEN_SIZE_ELE * sizeof(float) / ONE_BLK_SIZE);
    uint16_t n = countBlks * BLOCK_CUBE;
    uint16_t m = (burstLen * ONE_BLK_SIZE / B32_BYTE_SIZE) / BLOCK_CUBE;

    copy_matrix_cc_to_gm(
        (__gm__ float*)(dumpAddr), (__cc__ float*)(src.GetPhyAddr()), 0, n, m, static_cast<uint16_t>(m * BLOCK_CUBE), m,
        0, 0, static_cast<uint64_t>(QuantMode_t::NoQuant), static_cast<uint8_t>(false), false, false,
        static_cast<uint64_t>(QuantMode_post::NoConv), 0, 0, 0, 0, false);
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
    dcci((__gm__ uint64_t*)shapeTlv, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);

    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    UpdateWriteInfo(writeInfo, tlvLen);
}

template <typename T>
__aicore__ inline void WriteRingBufTlvData(
    const LocalTensor<T>& src, __gm__ DumpTensorTlvInfoHead* dumpTensorTlv, const uint32_t& alignDumpDataLen,
    const uint32_t& dumpSize)
{
    __gm__ T* dumpDataAddr = reinterpret_cast<__gm__ T*>(dumpTensorTlv + 1);

    PipeBarrier<PIPE_ALL>();

    if (dumpTensorTlv->position == static_cast<uint16_t>(Hardware::UB)) { // UB to GM
        DataCopyParams copyParams = GetDataCopyParamImpl(alignDumpDataLen);
        DataCopyUB2GMImpl(dumpDataAddr, reinterpret_cast<__ubuf__ T*>(src.GetPhyAddr()), copyParams);
    } else if (dumpTensorTlv->position == static_cast<uint16_t>(Hardware::L1)) { // L1 to GM
        // keep the original m300 L1 -> GM implementation (cbuf -> gm directly)
        DataCopyParams copyParams = GetDataCopyParamImpl(alignDumpDataLen);
        DataCopyL12GMImpl(dumpDataAddr, reinterpret_cast<__cbuf__ T*>(src.GetPhyAddr()), copyParams);
    } else if (dumpTensorTlv->position == static_cast<uint16_t>(Hardware::L0C)) { // L0C to GM
        // keep the original m300 L0C -> GM implementation (copy_matrix_cc_to_gm)
        DumpTensorL0C2GMImpl(src, reinterpret_cast<__gm__ uint8_t*>(dumpDataAddr), dumpSize);
    }

    PipeBarrier<PIPE_ALL>();

    dcci((__gm__ uint64_t*)dumpDataAddr, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
}

template <typename T>
__aicore__ inline void WriteRingBufTlvData(
    const GlobalTensor<T>& src, __gm__ DumpTensorTlvInfoHead* dumpTensorTlv, const uint32_t& dumpSize)
{
    PipeBarrier<PIPE_ALL>();
    __gm__ uint8_t* dst = reinterpret_cast<__gm__ uint8_t*>(dumpTensorTlv + 1);
    MemCopyGm2Gm(dst, reinterpret_cast<__gm__ const uint8_t*>(src.GetPhyAddr()), dumpSize * sizeof(T));
}

template <template <typename> class Tensor, typename T>
__aicore__ static __attribute__((noinline)) void DumpTensorRingBufImpl(
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
    uint32_t alignDumpDataLen = AlignUp(dumpSize * sizeof(T), ONE_BLK_SIZE);
    uint32_t tlvLen = sizeof(DumpTensorTlvInfoHead) + alignDumpDataLen;
    uint32_t maxResvLen = sizeof(DumpTensorTlvInfoHead) + AlignUp(alignDumpDataLen, blockSize) * 2; // resv tmp space
    if (!CheckAndWaitRingBufSpace(blockRingBufInfo, maxResvLen)) { // Reserved Backup Capacity
        return;
    }

    __gm__ DumpTensorTlvInfoHead* dumpTensorTlv =
        reinterpret_cast<__gm__ DumpTensorTlvInfoHead*>(GetRingBufTlv(blockRingBufInfo));

    WriteRingBufTlvHead(src, dumpTensorTlv, alignDumpDataLen, desc, dumpSize);
    WriteRingBufTlvShape(dumpTensorTlv, shapeDim, shape);
    if constexpr (IsSameType<Tensor<T>, LocalTensor<T>>::value) {
        WriteRingBufTlvData(src, dumpTensorTlv, alignDumpDataLen, dumpSize);
    } else if constexpr (IsSameType<Tensor<T>, GlobalTensor<T>>::value) {
        WriteRingBufTlvData(src, dumpTensorTlv, dumpSize);
    }

    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    UpdateWriteInfo(writeInfo, tlvLen);
#endif // ASCENDC_DUMP
}

#ifdef ASCENDC_TIME_STAMP_ON
// 3002 has no __get_entry_sys_cnt() builtin, so the kernel entry cycle is captured on the first
// PrintTimeStamp call of each block and then reused as the baseline of the following TLVs.
__BLOCK_LOCAL__ __inline__ uint8_t g_timeStampEntryFlag = 0;
__BLOCK_LOCAL__ __inline__ uint64_t g_timeStampEntryCycle = 0;
#endif

__aicore__ inline void WriteRingBufTimeStampInfo(uint32_t descId, uint64_t cycle, uint64_t entry)
{
    using TimeStampTlvInfo = __asc_aicore::TimeStampTlv;
    __gm__ BlockRingBufInfo* blockRingBufInfo = GetBlockRingBufInfo();
    if (blockRingBufInfo == nullptr) {
        return;
    }
    uint32_t tlvLen = sizeof(TimeStampTlvInfo);
    if (!CheckAndWaitRingBufSpace(blockRingBufInfo, tlvLen)) {
        return;
    }
    __gm__ TimeStampTlvInfo* timeStampTlv = reinterpret_cast<__gm__ TimeStampTlvInfo*>(GetRingBufTlv(blockRingBufInfo));
    timeStampTlv->type = static_cast<uint32_t>(DumpType::DUMP_TIME_STAMP);
    timeStampTlv->length = tlvLen - sizeof(uint32_t[2]);
    timeStampTlv->descId = descId;
    timeStampTlv->blockIdx = static_cast<uint16_t>(GetBlockIdxImpl());
    timeStampTlv->resv = static_cast<uint16_t>(0U);
    timeStampTlv->cycle = cycle;
    timeStampTlv->pc = static_cast<uint64_t>(__asc_aicore::asc_debug_get_program_counter());
    timeStampTlv->entry = entry;
    timeStampTlv->resvMem[0] = static_cast<uint32_t>(0U);
    timeStampTlv->resvMem[1] = static_cast<uint32_t>(0U);
    dcci((__gm__ uint64_t*)timeStampTlv, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);

    __gm__ RingBufWriteInfo* writeInfo = GetRingBufWriteInfo(blockRingBufInfo);

    UpdateWriteInfo(writeInfo, tlvLen);
}

__aicore__ inline void DumpTimeStampRingBufImpl(uint32_t descId)
{
#ifdef ASCENDC_TIME_STAMP_ON
    EnablePrintf();
    const uint64_t cycle = static_cast<uint64_t>(__asc_aicore::asc_debug_get_system_cycle());
    if (g_timeStampEntryFlag == 0) {
        g_timeStampEntryFlag = 1;
        g_timeStampEntryCycle = cycle;
        WriteRingBufTimeStampInfo(static_cast<uint32_t>(TimeStampId::TIME_STAMP_WRAP_FIRST), cycle, cycle);
    }
    WriteRingBufTimeStampInfo(descId, cycle, g_timeStampEntryCycle);
#endif
}

__aicore__ static __attribute__((noinline)) void DumpTimeStampImpl(uint32_t descId)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        DumpTimeStampRingBufImpl(descId);
    }
    set_ctrl(ctrlValue);
}
#else
#include <cstdio>

template <template <typename> class Tensor, typename T>
__aicore__ static __attribute__((noinline)) void DumpTensorRingBufImpl(
    const Tensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
    assert(false && "DumpTensorRingBuf is not supported in cpu mode.");
}

__aicore__ static __attribute__((noinline)) void DumpTimeStampImpl(uint32_t descId)
{
    assert(false && "DumpTimeStampImpl is not supported in cpu mode.");
}

__aicore__ inline void WriteRingBufShapeInfo(const ShapeInfo& shapeInfo) { return; }
#endif

__aicore__ static __attribute__((noinline)) void DumpShapeImpl(const ShapeInfo& shapeInfo)
{
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
    if (g_sysPrintFifoSpace != nullptr) {
        WriteRingBufShapeInfo(shapeInfo);
    }
}

template <typename T>
__aicore__ inline void DumpTensorLocal2GMImpl(
    const LocalTensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
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

template <typename T>
__aicore__ inline void DumpTensorGM2GMImpl(
    const GlobalTensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    dcci((__gm__ uint64_t*)g_sysPrintFifoSpace, cache_line_t::ENTIRE_DATA_CACHE, dcci_dst_t::CACHELINE_OUT);
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

template <class... Args>
__aicore__ inline void PrintfImpl(DumpType printType, __gm__ const char* fmt, Args&&... args)
{
    __asc_aicore::printf_impl(fmt, args...);
}

__aicore__ inline void AscendCTimeStamp(uint32_t descId, uint64_t pcPtr = 0)
{
#ifdef ASCENDC_TIME_STAMP_ON
    DumpTimeStampImpl(descId);
#endif
}

__aicore__ inline void InitDump(bool mixFlag, uint32_t gmLen) {}
__aicore__ inline void InitDump(bool mixFlag, GM_ADDR dumpStartAddr, uint32_t gmLen) {}
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DUMP_TENSOR_IMPL_H__
#endif
