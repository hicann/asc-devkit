/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file asc_aicore_dump_impl.h
 * \brief
 */
#ifndef IMPL_UTILS_DEBUG_ASC_AICORE_DUMP_IMPL_H
#define IMPL_UTILS_DEBUG_ASC_AICORE_DUMP_IMPL_H

#ifndef ASCENDC_CPU_DEBUG
#include "impl/utils/sys_macros_impl.h"
#include "impl/utils/common_types.h"
#include "impl/utils/debug/asc_debug_types.h"
#include "impl/utils/debug/asc_debug_utils.h"

#if __NPU_ARCH__ == 2201
#include "impl/utils/debug/npu_arch_2201/asc_aicore_dump_utils.h"
#elif __NPU_ARCH__ == 3510
#include "impl/utils/debug/npu_arch_3510/asc_aicore_dump_utils.h"
#elif __NPU_ARCH__ == 5102
#include "impl/utils/debug/npu_arch_5102/asc_aicore_dump_utils.h"
#elif __NPU_ARCH__ == 5162
#include "impl/utils/debug/npu_arch_5162/asc_aicore_dump_utils.h"
#endif

namespace __asc_aicore {

template <AscendC::Hardware hardware, typename T, typename U>
__aicore__ inline void set_dump_tlv_info(
    U src, __gm__ DumpTensorTlv* dumpTlv, uint32_t alignDumpLen, uint32_t desc, uint32_t dump_size)
{
    dumpTlv->type = static_cast<uint32_t>(DumpType::DUMP_TENSOR);
    dumpTlv->length = sizeof(DumpTensorTlv) - sizeof(uint32_t[2]) + alignDumpLen;
    dumpTlv->tensorAddr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src));
    dumpTlv->dataType = static_cast<uint32_t>(get_dump_datatype<T>());
    dumpTlv->desc = desc;
    dumpTlv->blockIdx = static_cast<uint32_t>(asc_debug_get_block_idx());
    dumpTlv->bufferId = static_cast<uint32_t>(0U);
    dumpTlv->position = static_cast<uint16_t>(hardware);
    dumpTlv->dim = static_cast<uint32_t>(0U);
    for (uint32_t i = 0; i < K_MAX_SHAPE_DIM; ++i) {
        dumpTlv->shape[i] = static_cast<uint32_t>(0U);
    }
    dumpTlv->resv1 = static_cast<uint32_t>(0U);
    dumpTlv->dumpSize = dump_size * sizeof(T);
    asc_entire_dcci(reinterpret_cast<__gm__ uint64_t*>(dumpTlv));
}

template <typename Tlv>
__aicore__ inline void set_dump_shape_info(__gm__ Tlv* dumpTlv, const uint32_t shapeDim, const uint32_t* shape)
{
    if (shapeDim <= 0 || shapeDim > K_MAX_SHAPE_DIM || shape == nullptr) {
        return;
    }
    dumpTlv->dim = static_cast<uint32_t>(shapeDim);
    for (uint32_t i = 0; i < K_MAX_SHAPE_DIM; ++i) {
        dumpTlv->shape[i] = i < shapeDim ? static_cast<uint32_t>(shape[i]) : static_cast<uint32_t>(1U);
    }
    asc_entire_dcci(reinterpret_cast<__gm__ uint64_t*>(dumpTlv));
}

template <AscendC::Hardware hardware, typename T, typename U, typename Tlv>
__aicore__ inline uint32_t set_dump_tlv_data(U src, __gm__ Tlv* dumpTlv, uint32_t alignDumpLen, uint32_t dump_size)
{
    __gm__ T* dumpDstAddr = reinterpret_cast<__gm__ T*>(dumpTlv + 1);

    if (dumpDstAddr == nullptr) {
        return 1;
    }
    if (hardware == AscendC::Hardware::GM && src == nullptr) {
        return 1;
    }

    sync_all();
    uint32_t ret = 0;
    uint32_t dumpLen = 0;
    if constexpr (hardware == AscendC::Hardware::GM) {
        dumpLen = dump_size * sizeof(T);
        ret = mem_copy_gm_to_gm(
            reinterpret_cast<__gm__ uint8_t*>(dumpDstAddr), reinterpret_cast<__gm__ const uint8_t*>(src), dumpLen);
    } else if constexpr (hardware == AscendC::Hardware::UB) {
        dumpLen = alignDumpLen / ASC_ONE_DATABLOCK_SIZE;
        ret = mem_copy_ub_to_gm_impl(dumpDstAddr, src, static_cast<uint16_t>(dumpLen));
    } else if constexpr (hardware == AscendC::Hardware::L1) {
        ret = mem_copy_l1buf_to_gm_impl(dumpDstAddr, src, alignDumpLen);
    } else if constexpr (hardware == AscendC::Hardware::L0A) {
        ret = mem_copy_abuf_to_gm_impl(dumpDstAddr, src, alignDumpLen);
    } else if constexpr (hardware == AscendC::Hardware::L0B) {
        ret = mem_copy_bbuf_to_gm_impl(dumpDstAddr, src, alignDumpLen);
    } else if constexpr (hardware == AscendC::Hardware::L0C) {
        ret = mem_copy_cbuf_to_gm_impl(dumpDstAddr, src, alignDumpLen);
    } else if constexpr (hardware == AscendC::Hardware::BIAS) {
        ret = mem_copy_biasbuf_to_gm_impl(dumpDstAddr, src, alignDumpLen);
    } else if constexpr (hardware == AscendC::Hardware::FIXBUF) {
        ret = mem_copy_fbuf_to_gm_impl(dumpDstAddr, src, alignDumpLen);
    }
    sync_all();
    return ret;
}

template <AscendC::Hardware hardware>
__aicore__ constexpr inline bool is_super_tensor_hardware_supported()
{
    return hardware == AscendC::Hardware::GM || hardware == AscendC::Hardware::UB ||
           hardware == AscendC::Hardware::L1 || hardware == AscendC::Hardware::L0C ||
           hardware == AscendC::Hardware::BIAS || hardware == AscendC::Hardware::FIXBUF;
}

template <AscendC::Hardware hardware>
__aicore__ constexpr inline uint32_t get_super_tensor_payload_alignment()
{
    if constexpr (hardware == AscendC::Hardware::L0C) {
        return 16U * 16U * sizeof(uint32_t);
    }
    return ASC_ONE_DATABLOCK_SIZE;
}

template <AscendC::Hardware hardware>
__aicore__ inline uint32_t get_super_tensor_payload_capacity(uint32_t ringBufLen, uint32_t headerLen)
{
    constexpr uint32_t payloadAlignment = get_super_tensor_payload_alignment<hardware>();
    if (ringBufLen <= headerLen) {
        return 0U;
    }
    return ((ringBufLen - headerLen) / payloadAlignment) * payloadAlignment;
}

template <AscendC::Hardware hardware, typename T, typename U>
__aicore__ inline void set_super_tensor_tlv_info(
    U src, __gm__ DumpSuperTensorTlv* dumpTlv, uint32_t alignDumpLen, uint32_t desc, uint64_t tensorLength,
    uint32_t dumpSize, const uint32_t* shape, uint32_t shapeDim)
{
    dumpTlv->type = static_cast<uint32_t>(DumpType::DUMP_SUPER_TENSOR);
    dumpTlv->length = sizeof(DumpSuperTensorTlv) - sizeof(uint32_t[2]) + alignDumpLen;
    dumpTlv->tensorAddr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src));
    dumpTlv->dataType = static_cast<uint32_t>(get_dump_datatype<T>());
    dumpTlv->desc = desc;
    dumpTlv->bufferId = 0U;
    dumpTlv->position = static_cast<uint16_t>(hardware);
    dumpTlv->blockIdx = static_cast<uint16_t>(asc_debug_get_block_idx());
    dumpTlv->dim = 0U;
    for (uint32_t i = 0; i < K_MAX_SHAPE_DIM; ++i) {
        dumpTlv->shape[i] = 0U;
    }
    dumpTlv->resv = 0U;
    dumpTlv->tensorLength = tensorLength;
    dumpTlv->tensorOffset = 0U;
    dumpTlv->dumpSize = dumpSize;
    set_dump_shape_info(dumpTlv, shapeDim, shape);
    asc_entire_dcci(reinterpret_cast<__gm__ uint64_t*>(dumpTlv));
}

__aicore__ inline void set_super_tensor_body_tlv_info(
    __gm__ DumpSuperTensorBodyTlv* dumpTlv, uint32_t alignDumpLen, uint64_t tensorLength, uint64_t tensorOffset,
    uint32_t dumpSize)
{
    dumpTlv->type = static_cast<uint32_t>(DumpType::DUMP_SUPER_TENSOR_BODY);
    dumpTlv->length = sizeof(DumpSuperTensorBodyTlv) - sizeof(uint32_t[2]) + alignDumpLen;
    dumpTlv->resv1 = 0U;
    dumpTlv->resv2 = 0U;
    dumpTlv->tensorLength = tensorLength;
    dumpTlv->tensorOffset = tensorOffset;
    dumpTlv->dumpSize = dumpSize;
    asc_entire_dcci(reinterpret_cast<__gm__ uint64_t*>(dumpTlv));
}

template <AscendC::Hardware hardware, typename T, typename U>
__aicore__ inline void asc_dump_super_tensor_impl(
    U src, uint32_t desc, uint64_t tensorLength, const uint32_t* shape, uint32_t shapeDim,
    __gm__ DebugBlockHeadInfo* blockInfo)
{
    constexpr uint32_t payloadAlignment = get_super_tensor_payload_alignment<hardware>();
    uint64_t tensorOffset = 0U;
    bool isFirst = true;
    while (tensorOffset < tensorLength) {
        const uint32_t headerLen = isFirst ? sizeof(DumpSuperTensorTlv) : sizeof(DumpSuperTensorBodyTlv);
        const uint32_t payloadCapacity = get_super_tensor_payload_capacity<hardware>(blockInfo->ringBufLen, headerLen);
        if (payloadCapacity == 0U) {
            return;
        }
        const uint64_t remaining = tensorLength - tensorOffset;
        const uint32_t dumpSize = remaining > payloadCapacity ? payloadCapacity : static_cast<uint32_t>(remaining);
        const uint32_t alignDumpLen = align_up(dumpSize, payloadAlignment);
        const uint32_t tlvLen = headerLen + alignDumpLen;
        if (!check_ringbuf_space(blockInfo, tlvLen)) {
            return;
        }

        __gm__ uint8_t* tlvAddr = get_ringbuf_tlv_addr(blockInfo);
        U chunkSrc = src + tensorOffset / sizeof(T);
        if (isFirst) {
            auto* dumpTlv = reinterpret_cast<__gm__ DumpSuperTensorTlv*>(tlvAddr);
            set_super_tensor_tlv_info<hardware, T>(
                src, dumpTlv, alignDumpLen, desc, tensorLength, dumpSize, shape, shapeDim);
            if (set_dump_tlv_data<hardware, T>(chunkSrc, dumpTlv, alignDumpLen, dumpSize / sizeof(T)) != 0U) {
                return;
            }
        } else {
            auto* dumpTlv = reinterpret_cast<__gm__ DumpSuperTensorBodyTlv*>(tlvAddr);
            set_super_tensor_body_tlv_info(dumpTlv, alignDumpLen, tensorLength, tensorOffset, dumpSize);
            if (set_dump_tlv_data<hardware, T>(chunkSrc, dumpTlv, alignDumpLen, dumpSize / sizeof(T)) != 0U) {
                return;
            }
        }

        __gm__ DebugBlockWriteInfo* writeInfo = get_block_write_info(blockInfo);
        update_write_info(writeInfo, tlvLen);
        tensorOffset += dumpSize;
        isFirst = false;
    }
}

template <AscendC::Hardware hardware, typename T, typename U>
__aicore__ inline void asc_dump_impl(
    U src, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    __gm__ DebugBlockHeadInfo* blockInfo = get_block_info();
    if (dump_size <= 0 || blockInfo == nullptr) {
        return;
    }
    constexpr uint32_t dataBlockSize = 32U;
    const uint64_t tensorLength = static_cast<uint64_t>(dump_size) * sizeof(T);
    const uint64_t alignDumpLen64 = ((tensorLength + dataBlockSize - 1U) / dataBlockSize) * dataBlockSize;
    const uint64_t tlvLen64 = sizeof(DumpTensorTlv) + alignDumpLen64;
    if (tlvLen64 > blockInfo->ringBufLen) {
#if __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 5102
        if constexpr (is_super_tensor_hardware_supported<hardware>()) {
            asc_dump_super_tensor_impl<hardware, T>(src, desc, tensorLength, shape, shapeDim, blockInfo);
        }
#endif
        return;
    }
    const uint32_t alignDumpLen = static_cast<uint32_t>(alignDumpLen64);
    const uint32_t tlvLen = static_cast<uint32_t>(tlvLen64);
    if (!check_ringbuf_space(blockInfo, tlvLen)) {
        return;
    }
    __gm__ DumpTensorTlv* dumpTlv = reinterpret_cast<__gm__ DumpTensorTlv*>(get_ringbuf_tlv_addr(blockInfo));

    set_dump_tlv_info<hardware, T>(src, dumpTlv, alignDumpLen, desc, dump_size);
    set_dump_shape_info(dumpTlv, shapeDim, shape);
    if (set_dump_tlv_data<hardware, T>(src, dumpTlv, alignDumpLen, dump_size) != 0) {
        return;
    }

    __gm__ DebugBlockWriteInfo* writeInfo = get_block_write_info(blockInfo);
    update_write_info(writeInfo, tlvLen);
#endif
}

__aicore__ inline void asc_dump_shape_impl(const uint32_t shapeDim, const uint32_t* shape)
{
#if !(defined(ASCENDC_DUMP) && ASCENDC_DUMP == 0)
    __gm__ DebugBlockHeadInfo* blockInfo = get_block_info();
    if (blockInfo == nullptr) {
        return;
    }
    uint32_t tlvLen = sizeof(DumpShapeTlv);
    if (!check_ringbuf_space(blockInfo, tlvLen)) {
        return;
    }
    __gm__ DumpShapeTlv* shapeTlv = reinterpret_cast<__gm__ DumpShapeTlv*>(get_ringbuf_tlv_addr(blockInfo));
    shapeTlv->type = static_cast<uint32_t>(DumpType::DUMP_SHAPE);
    shapeTlv->length = tlvLen - sizeof(uint32_t[2]);
    shapeTlv->dim = shapeDim;
    for (uint32_t i = 0; i < 8; ++i) {
        shapeTlv->shape[i] = i < shapeDim ? shape[i] : 1;
    }
    shapeTlv->resv = static_cast<uint32_t>(0U);
    asc_entire_dcci(reinterpret_cast<__gm__ uint64_t*>(shapeTlv));

    __gm__ DebugBlockWriteInfo* writeInfo = get_block_write_info(blockInfo);
    update_write_info(writeInfo, tlvLen);
#endif
}

template <AscendC::Hardware hardware, typename T, typename U>
__aicore__ inline void asc_dump_local_impl(
    U input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    uint64_t ctrlValue = get_ctrl();
    set_atomic_none();
    enable_asc_diagnostics();
    if (g_sysPrintFifoSpace != nullptr) {
        asc_dump_impl<hardware, T>(input, desc, dump_size, shape, shapeDim);
    }
    set_ctrl(ctrlValue);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_gm(
    __gm__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    asc_dump_local_impl<AscendC::Hardware::GM, T>(input, desc, dump_size, shape, shapeDim);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_ubuf(
    __ubuf__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    asc_dump_local_impl<AscendC::Hardware::UB, T>(input, desc, dump_size, shape, shapeDim);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_abuf(
    __ca__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    asc_dump_local_impl<AscendC::Hardware::L0A, T>(input, desc, dump_size, shape, shapeDim);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_bbuf(
    __cb__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    asc_dump_local_impl<AscendC::Hardware::L0B, T>(input, desc, dump_size, shape, shapeDim);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_cbuf(
    __cc__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    asc_dump_local_impl<AscendC::Hardware::L0C, T>(input, desc, dump_size, shape, shapeDim);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_l1buf(
    __cbuf__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    asc_dump_local_impl<AscendC::Hardware::L1, T>(input, desc, dump_size, shape, shapeDim);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(
    __biasbuf__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    asc_dump_local_impl<AscendC::Hardware::BIAS, T>(input, desc, dump_size, shape, shapeDim);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(
    __fbuf__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    asc_dump_local_impl<AscendC::Hardware::FIXBUF, T>(input, desc, dump_size, shape, shapeDim);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_gm(__gm__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_local_impl<AscendC::Hardware::GM, T>(input, desc, dump_size, nullptr, 0);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_local_impl<AscendC::Hardware::UB, T>(input, desc, dump_size, nullptr, 0);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_abuf(__ca__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_local_impl<AscendC::Hardware::L0A, T>(input, desc, dump_size, nullptr, 0);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_bbuf(__cb__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_local_impl<AscendC::Hardware::L0B, T>(input, desc, dump_size, nullptr, 0);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_cbuf(__cc__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_local_impl<AscendC::Hardware::L0C, T>(input, desc, dump_size, nullptr, 0);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_l1buf(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_local_impl<AscendC::Hardware::L1, T>(input, desc, dump_size, nullptr, 0);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__gm__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_gm(input, desc, dump_size);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_ubuf(input, desc, dump_size);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__cc__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_cbuf(input, desc, dump_size);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump_l1buf(input, desc, dump_size);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__biasbuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump(input, desc, dump_size, nullptr, 0);
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__fbuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    asc_dump(input, desc, dump_size, nullptr, 0);
}
} // namespace __asc_aicore
#else
#include <cstdio>

namespace __asc_aicore {
template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_gm(__gm__ T* input, uint32_t desc, uint32_t dump_size)
{
    assert(false && "asc_dump_gm is not supported in cpu mode.");
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    assert(false && "asc_dump_ubuf is not supported in cpu mode.");
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_cbuf(__cc__ T* input, uint32_t desc, uint32_t dump_size)
{
    assert(false && "asc_dump_cbuf is not supported in cpu mode.");
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_l1buf(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)
{
    assert(false && "asc_dump_l1buf is not supported in cpu mode.");
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_gm(
    __gm__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    assert(false && "asc_dump_gm is not supported in cpu mode.");
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_ubuf(
    __ubuf__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    assert(false && "asc_dump_ubuf is not supported in cpu mode.");
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_cbuf(
    __cc__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    assert(false && "asc_dump_cbuf is not supported in cpu mode.");
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_l1buf(
    __cbuf__ T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    assert(false && "asc_dump_l1buf is not supported in cpu mode.");
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(T* input, uint32_t desc, uint32_t dump_size)
{
    assert(false && "asc_dump is not supported in cpu mode.");
}

template <typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(
    T* input, uint32_t desc, uint32_t dump_size, const uint32_t* shape, const uint32_t shapeDim)
{
    assert(false && "asc_dump is not supported in cpu mode.");
}

__aicore__ inline void asc_dump_shape_impl(const uint32_t shapeDim, const uint32_t* shape)
{
    assert(false && "asc_dump_shape_impl is not supported in cpu mode.");
}
} // namespace __asc_aicore

using namespace __asc_aicore;
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_DUMP_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_AICORE_DUMP_IMPL__
#endif

#endif // IMPL_UTILS_DEBUG_ASC_AICORE_DUMP_IMPL_H
