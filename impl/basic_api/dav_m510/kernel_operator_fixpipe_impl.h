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
 * \file kernel_operator_fixpipe_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m510/kernel_operator_fixpipe_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H

#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_operator_block_sync_intf.h"
#include "../kernel_process_lock.h"

namespace AscendC {
__aicore__ inline void SetFixPipeClipReluImpl(uint64_t config) { set_fix_clip_relu(config); }

template <typename T>
__aicore__ inline void SetFixPipeAddrImpl(const LocalTensor<T>& eleWiseTensor, uint16_t c0ChStride)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeAddr");
}

/* **************************************************************************************************
 * SPR                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void SetFixPipeConfigImpl(
    const LocalTensor<T>& reluPre, const LocalTensor<T>& quantPre, bool isUnitFlag = false)
{
    uint64_t config = 0;
    config = config | ((uint64_t)reluPre.GetPhyAddr() >> 6);         // align with 64bit, FPC[7:0], ReluPreAddr
    config = config | (((uint64_t)quantPre.GetPhyAddr() >> 7) << 8); // align with 128bit, FPC[15:8], QuantPreAddr.
    config = config | ((uint64_t)isUnitFlag << 63);                  // FPC[63], UnitFlag.
    set_fpc(config);
}

template <typename T, bool setRelu = false>
__aicore__ inline void SetFixPipeConfigImpl(const LocalTensor<T>& preTensor, bool isUnitFlag = false)
{
    uint64_t config = 0;
    if constexpr (setRelu) {
        config = config | ((uint64_t)preTensor.GetPhyAddr() >> 6); // align with 64bit, FPC[7:0], ReluPreAddr.
    } else {
        config = config | (((uint64_t)preTensor.GetPhyAddr() >> 7) << 8); // align with 128bit, FPC[15:8], QuantPreAddr.
    }
    config = config | ((uint64_t)isUnitFlag << 63); // FPC[63], UnitFlag.
    set_fpc(config);
}

__aicore__ inline void SetFixpipeNz2ndFlagImpl(uint16_t ndNum, uint16_t srcNdStride, uint32_t dstNdStride)
{
    // ND_PARA[63:32], dst nd stride in unit of element
    // ND_PARA[31:16], src nd stride in uint of C0_SIZE
    // ND_PARA[15:0], nd number.
    uint64_t config = (static_cast<uint64_t>(dstNdStride) << 32) | (static_cast<uint64_t>(srcNdStride) << 16) |
                      (static_cast<uint64_t>(ndNum));
    set_loop3_para(config);
}

__aicore__ inline void SetFixpipePreQuantFlagImpl(uint64_t config) { set_quant_pre(config); }

template <const FixpipeConfig& config>
__aicore__ inline void SetDeqScalarDepOnMode(QuantMode_t mode, uint64_t deqScalar, uint8_t shiftVal)
{
    if (shiftVal == 0) {
        set_quant_pre(deqScalar);
    } else if (mode == QuantMode_t::DEQF16) {
        // fix point 1/2^16
        constexpr uint16_t shift = 23;
        constexpr uint64_t mask = 0xFFULL << shift;
        constexpr uint64_t floatOne = 0x3F800000ULL;
        uint64_t newExponent = (127 - shiftVal) & 0xFF;
        uint64_t newScalar = (floatOne & ~mask) | (newExponent << shift);
        set_quant_pre(newScalar);
    } else if (mode == QuantMode_t::REQ8) {
        // fix point by modify related  bits
        constexpr uint16_t shift = 23;
        constexpr uint64_t mask = 0xFFULL << shift;
        uint64_t originalBits = (deqScalar & mask) >> shift;
        uint64_t newBits = (originalBits - (uint64_t)shiftVal) & 0xFF;
        uint64_t newScalar = (deqScalar & ~mask) | (newBits << shift);
        set_quant_pre(newScalar);
    }
}

__aicore__ inline void SetReluScalarOnMode(QuantMode_t mode, uint64_t reluScalar, uint8_t shiftVal)
{
#if !defined(ASCENDC_CPU_DEBUG)
    if (shiftVal == 0) {
        set_relu_alpha(reluScalar);
    } else if (mode == QuantMode_t::DEQF16 || mode == QuantMode_t::REQ8) {
        constexpr uint16_t shift = 23;
        constexpr uint64_t mask = 0xFFULL << shift;
        uint64_t originalBits = (reluScalar & mask) >> shift;
        uint64_t newBits = (originalBits - (uint64_t)shiftVal) & 0xFF;
        uint64_t newScalar = (reluScalar & ~mask) | (newBits << shift);
        set_relu_alpha(newScalar);
    }
#endif
}

/* **************************************************************************************************
 * Fixpipe                                             *
 * ************************************************************************************************* */
// tiling params
struct FixpipeTiling {
    uint16_t nIterNum = 0;
    uint16_t nSize = 0;
    bool isDb = false;
    uint16_t tailNSize = 0;
};

// fixpipe tiling calculating
__aicore__ inline FixpipeTiling GenFixpipeTiling(uint16_t n)
{
    FixpipeTiling tiling;
    // deqTensor/reluTensor in FB valid num is 1024
    uint16_t maxDeqNums = 1024;
    if (n <= maxDeqNums) {
        tiling.nIterNum = 1;
        tiling.nSize = n;
        tiling.isDb = false;
        tiling.tailNSize = 0;
    } else {
        tiling.isDb = true;
        uint16_t dbMaxDeqNums = maxDeqNums / 2;
        tiling.nIterNum = n / dbMaxDeqNums;
        tiling.nSize = dbMaxDeqNums;
        tiling.tailNSize = n % dbMaxDeqNums;
    }
    return tiling;
}

__aicore__ inline bool IsVectorQuantMode(QuantMode_t quantPre)
{
    return (
        quantPre == QuantMode_t::VDEQF16 || quantPre == QuantMode_t::VQF322B8_PRE || quantPre == QuantMode_t::VREQ8 ||
        quantPre == QuantMode_t::VQS322BF16_PRE || quantPre == QuantMode_t::VQF322F16_PRE ||
        quantPre == QuantMode_t::VQF322BF16_PRE || quantPre == QuantMode_t::VQF322HIF8_PRE ||
        quantPre == QuantMode_t::VQF322HIF8_PRE_HYBRID);
}

__aicore__ inline bool IsScalarQuantMode(QuantMode_t quantPre)
{
    return (
        quantPre == QuantMode_t::DEQF16 || quantPre == QuantMode_t::QF322B8_PRE || quantPre == QuantMode_t::REQ8 ||
        quantPre == QuantMode_t::QS322BF16_PRE || quantPre == QuantMode_t::QF322F16_PRE ||
        quantPre == QuantMode_t::QF322BF16_PRE || quantPre == QuantMode_t::QF322HIF8_PRE ||
        quantPre == QuantMode_t::QF322HIF8_PRE_HYBRID);
}

template <const FixpipeConfig& config>
__aicore__ inline void CopyReluTensorToFbuf(
    const FixpipeTiling& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex,
    const FixpipeParamsArch3510<config.format>& intriParams)
{
    constexpr uint32_t deqTensorAddrAlignValue = 128;
    uint16_t deqDataSize = DivCeil(calNSize * sizeof(uint64_t), deqTensorAddrAlignValue) * deqTensorAddrAlignValue;
    __fbuf__ uint64_t* deqTensorTempBuf =
        AscendCUtils::GetTemporaryFbBufferAddr<uint64_t>(0, deqDataSize / sizeof(uint64_t));
    uint32_t deqValueOffset = nIterIndex * fixpipeTiling.nSize;
    // memblock 1
    constexpr uint64_t memBlockType = 65536;
    deqTensorTempBuf = (__fbuf__ uint64_t*)((uint64_t)deqTensorTempBuf | memBlockType);
    // L1 -> FB
    constexpr uint16_t fbufBurstLenUnit = 64;
    uint16_t fbufBurstLen = deqDataSize / fbufBurstLenUnit; // copy from cbuf to fbuf, burst_len unit is 64Bytes
    copy_cbuf_to_fbuf(
        deqTensorTempBuf, (__cbuf__ uint64_t*)intriParams.vectorRelu + deqValueOffset, 1, fbufBurstLen / 2, 0, 0);
    uint64_t deqTensorAddr = ((uint64_t)deqTensorTempBuf) >> (uint64_t)6;
    set_fpc(deqTensorAddr);
    AscendCUtils::FreeTemporaryFbBuffer<uint64_t>(deqTensorTempBuf);
}

template <const FixpipeConfig& config>
__aicore__ inline void CopyDeqTensorToFbuf(
    __cbuf__ uint64_t* cbufWorkspace, const FixpipeTiling& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex,
    const FixpipeParamsArch3510<config.format>& intriParams)
{
    constexpr uint32_t deqTensorAddrAlignValue = 128;
    uint16_t deqDataSize = DivCeil(calNSize * sizeof(uint64_t), deqTensorAddrAlignValue) * deqTensorAddrAlignValue;
    __fbuf__ uint64_t* deqTensorTempBuf =
        AscendCUtils::GetTemporaryFbBufferAddr<uint64_t>(0, deqDataSize / sizeof(uint64_t));
    uint32_t deqValueOffset = nIterIndex * fixpipeTiling.nSize;

    // L1 -> FB
    constexpr uint16_t fbufBurstLenUnit = 64;
    uint16_t fbufBurstLen = deqDataSize / fbufBurstLenUnit; // copy from cbuf to fbuf, burst_len unit is 64Bytes
    copy_cbuf_to_fbuf(deqTensorTempBuf, cbufWorkspace + deqValueOffset, 1, fbufBurstLen, 0, 0);
    // FPC of fixpipe buffer for Quant_PRE is FPC[15:8], unit is 128Bytes
    uint64_t deqTensorAddr = (((uint64_t)deqTensorTempBuf) >> (uint64_t)7) << 8;
    if (intriParams.preReluMode == ReluMode::NORMAL_RELU || intriParams.preReluMode == ReluMode::NO_RELU) {
        set_fpc(deqTensorAddr);
        AscendCUtils::FreeTemporaryFbBuffer<uint64_t>(deqTensorTempBuf);
    }
    if (intriParams.preReluMode == ReluMode::VECTOR_RELU) {
        __fbuf__ uint64_t* reluTensorTempBuf =
            AscendCUtils::GetTemporaryFbBufferAddr<uint64_t>(0, deqDataSize / sizeof(uint64_t));
        constexpr uint64_t memBlockType = 65536;
        reluTensorTempBuf = (__fbuf__ uint64_t*)((uint64_t)reluTensorTempBuf | memBlockType);
        copy_cbuf_to_fbuf(
            reluTensorTempBuf, (__cbuf__ uint64_t*)intriParams.vectorRelu + deqValueOffset, 1, fbufBurstLen / 2, 0, 0);
        deqTensorAddr = deqTensorAddr | (((uint64_t)reluTensorTempBuf) << (uint64_t)50) >> 56;
        AscendCUtils::FreeTemporaryFbBuffer<uint64_t>(reluTensorTempBuf);
        set_fpc(deqTensorAddr);
        AscendCUtils::FreeTemporaryFbBuffer<uint64_t>(deqTensorTempBuf);
    } else if (intriParams.preReluMode == ReluMode::SCALAR_RELU) {
        SetReluScalarOnMode(intriParams.quantPre, intriParams.reluScalar, intriParams.fixShiftVal);
        set_fpc(deqTensorAddr);
        AscendCUtils::FreeTemporaryFbBuffer<uint64_t>(deqTensorTempBuf);
    }
}

template <const FixpipeConfig& config>
__aicore__ inline void SetLoop3Para(const FixpipeParamsArch3510<config.format>& intriParams)
{
    // the loop3DstStride range has increased.
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        ASCENDC_ASSERT((intriParams.params.ndNum > 0), { KERNEL_LOG(KERNEL_ERROR, "ndNum must be larger than 0"); });
        // Loop3_dst_stride in uint of element
        uint64_t loop3Para = static_cast<uint64_t>(intriParams.params.dstNdStride) << 32; // LOOP3_PARA[63:32]
        // original src_nd_stride in uint of fractal_size(1024B = 16 * 16 * sizeof(SrcT))
        // src_nd_stride in unit of C0_SIZE, Loop3_src_stride
        loop3Para |= static_cast<uint64_t>(intriParams.params.srcNdStride) << 16; // LOOP3_PARA[31:16]
        loop3Para |= static_cast<uint64_t>(intriParams.params.ndNum);             // LOOP3_PARA[15:0]
        set_loop3_para(loop3Para);
    } else if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        ASCENDC_ASSERT((intriParams.params.dnNum > 0), { KERNEL_LOG(KERNEL_ERROR, "dnNum must be larger than 0"); });
        // Loop3_dst_stride in uint of element
        uint64_t loop3Para = static_cast<uint64_t>(intriParams.params.dstDnMatrixStride) << 32; // LOOP3_PARA[63:32]
        // src_nd_stride in unit of C0_SIZE, Loop3_src_stride
        loop3Para |= static_cast<uint64_t>(intriParams.params.srcNzMatrixStride) << 16; // LOOP3_PARA[31:16]
        loop3Para |= static_cast<uint64_t>(intriParams.params.dnNum);                   // LOOP3_PARA[15:0]
        set_loop3_para(loop3Para);
    }
}

// contains loop info and cal n size for each loop
// move data L0C->L1
template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0cToL1(
    __cbuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& intriParams,
    const FixpipeTiling& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex = 0)
{
    ASCENDC_DEBUG_ASSERT(
        (!(intriParams.isChannelSplit)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check isChannelSplit in Fixpipe, when src position is "
                          "L0C Buffer(CO1) and dst position is L1 Buffer(C1), isChannelSplit must be set as false \n"));

    uint16_t cburstNum = fixpipeTiling.nSize / BLOCK_CUBE;
    // Loop1_src_stride in unit of C0_size
    uint32_t srcOffset = cburstNum * nIterIndex * intriParams.srcStride * BLOCK_CUBE;
    uint32_t dstOffset = 0;
    // If NZ2ND/NZ2DN is enabled, it is the dst_D value in unit of element. Loop2_dst_stride
    // If NZ2ND/NZ2DN is disabled, it is in unit of element, Loop1_dst_stride
    bool nz2ndEn = false;
    bool nz2dnEn = false;
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        dstOffset = nIterIndex * fixpipeTiling.nSize;
        nz2ndEn = true;
    } else if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        dstOffset = nIterIndex * fixpipeTiling.nSize * intriParams.dstStride;
        nz2dnEn = true;
    } else {
        dstOffset = cburstNum * nIterIndex * intriParams.dstStride * DEFAULT_C0_SIZE / sizeof(DstT);
    }
    // LOC -> L1 only n direction need fixpipeTiling, m no need fixpipeTiling
    if (intriParams.reluEn == true && intriParams.preReluMode == ReluMode::NO_RELU) {
        return copy_matrix_cc_to_cbuf(
            (__cbuf__ DstT*)(dst + dstOffset), (__cc__ SrcT*)(src + srcOffset), 0, calNSize, intriParams.mSize,
            intriParams.dstStride, intriParams.srcStride, 0, static_cast<uint8_t>(intriParams.preClipReluMode),
            intriParams.unitFlag, static_cast<uint64_t>(intriParams.quantPre), static_cast<uint8_t>(intriParams.reluEn),
            false, nz2ndEn, static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0, false, false, false,
            false, false, nz2dnEn);
    } else {
        return copy_matrix_cc_to_cbuf(
            (__cbuf__ DstT*)(dst + dstOffset), (__cc__ SrcT*)(src + srcOffset), 0, calNSize, intriParams.mSize,
            intriParams.dstStride, intriParams.srcStride, 0, static_cast<uint8_t>(intriParams.preClipReluMode),
            intriParams.unitFlag, static_cast<uint64_t>(intriParams.quantPre),
            static_cast<uint8_t>(intriParams.preReluMode), false, nz2ndEn,
            static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0, false, false, false, false, false,
            nz2dnEn);
    }
}

// contains loop info and cal n size for each loop
// move data L0C->UB
template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0cToUB(
    __ubuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& intriParams,
    const FixpipeTiling& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex = 0)
{
    ASCENDC_DEBUG_ASSERT(
        (!(intriParams.isChannelSplit)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check isChannelSplit in Fixpipe, when src position is "
                          "L0C Buffer(CO1) and dst position is UB(VECIN/VECCALC/VECOUT), isChannelSplit must be set "
                          "as false \n"));

    // dual destination mode limit
    ASCENDC_ASSERT(
        !(intriParams.dualDstCtl != 0b00 && (config.format == CO2Layout::COLUMN_MAJOR ||
                                             intriParams.quantPre != QuantMode_t::NoQuant || intriParams.reluEn)),
        {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Dual destination mode can be enabled only when normal DMA or NZ2ND is enabled with any other "
                "Fixpipe functions bypassed.");
        });

    ASCENDC_ASSERT(!(intriParams.dualDstCtl == 0b01 && intriParams.mSize % 2 != 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "dual destination mode, split in M dimension, M/2 * N is written to each UB, M must be number of 2.");
    });

    ASCENDC_ASSERT(!(intriParams.dualDstCtl == 0b10 && calNSize % 32 != 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "dual destination mode, split in N dimension, M * N/2 is written to each UB, N must be number of 32.");
    });

    uint16_t cburstNum = fixpipeTiling.nSize / BLOCK_CUBE;
    uint32_t srcOffset = cburstNum * nIterIndex * intriParams.srcStride * BLOCK_CUBE;
    uint32_t dstOffset = 0;
    bool nz2ndEn = false;
    bool nz2dnEn = false;
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        dstOffset = nIterIndex * fixpipeTiling.nSize;
        nz2ndEn = true;
    } else if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        dstOffset = nIterIndex * fixpipeTiling.nSize * intriParams.dstStride;
        nz2dnEn = true;
    } else {
        dstOffset = cburstNum * nIterIndex * intriParams.dstStride * DEFAULT_C0_SIZE / sizeof(DstT);
    }
    // LOC -> UB only n direction need fixpipeTiling, m no need fixpipeTiling
    if (intriParams.reluEn == true && intriParams.preReluMode == ReluMode::NO_RELU) {
        return copy_matrix_cc_to_ub(
            (__ubuf__ DstT*)(dst + dstOffset), (__cc__ SrcT*)(src + srcOffset), calNSize, intriParams.mSize,
            intriParams.dstStride, intriParams.srcStride, intriParams.dualDstCtl, intriParams.subBlockId,
            static_cast<uint8_t>(intriParams.preClipReluMode), intriParams.unitFlag,
            static_cast<uint64_t>(intriParams.quantPre), static_cast<uint8_t>(intriParams.reluEn), false, nz2ndEn,
            static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0, false, false, false, false, false,
            nz2dnEn);
    } else {
        return copy_matrix_cc_to_ub(
            (__ubuf__ DstT*)(dst + dstOffset), (__cc__ SrcT*)(src + srcOffset), calNSize, intriParams.mSize,
            intriParams.dstStride, intriParams.srcStride, intriParams.dualDstCtl, intriParams.subBlockId,
            static_cast<uint8_t>(intriParams.preClipReluMode), intriParams.unitFlag,
            static_cast<uint64_t>(intriParams.quantPre), static_cast<uint8_t>(intriParams.preReluMode), false, nz2ndEn,
            static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0, false, false, false, false, false,
            nz2dnEn);
    }
}

template <const FixpipeConfig& config>
__aicore__ inline uint64_t GetGMLen(
    const FixpipeParamsArch3510<config.format>& intriParams, const uint16_t calNSize, const uint16_t dstEleSize)
{
    constexpr uint16_t fractalNsize = 16;
    uint64_t cburstNum = calNSize / fractalNsize;
    uint64_t gmLen =
        (cburstNum - 1) * intriParams.dstStride * dstEleSize + intriParams.mSize * fractalNsize * dstEleSize;
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        // dstStride is dst_D Loop2_dst_stride
        gmLen = (static_cast<uint64_t>(intriParams.params.ndNum) - 1) * dstEleSize * intriParams.params.dstNdStride +
                (intriParams.mSize - 1) * intriParams.dstStride * dstEleSize + cburstNum * fractalNsize * dstEleSize;
    } else if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        gmLen =
            (static_cast<uint64_t>(intriParams.params.dnNum) - 1) * dstEleSize * intriParams.params.dstDnMatrixStride +
            (intriParams.nSize - 1) * intriParams.dstStride * dstEleSize +
            intriParams.mSize / fractalNsize * fractalNsize * dstEleSize;
    }
    return gmLen;
}

// contains loop info and cal n size for each loop
// move data L0C->GM
template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0cToOut(
    __gm__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& intriParams,
    const FixpipeTiling& fixpipeTiling, uint16_t calNSize, const uint8_t cacheMode, uint16_t nIterIndex = 0)
{
    if (intriParams.isChannelSplit) {
        ASCENDC_DEBUG_ASSERT(
            (IsSameType<SrcT, float>::value && IsSameType<DstT, float>::value),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "Failed to check "
                              "isChannelSplit value in Fixpipe, isChannelSplit can be set true only when src and dst "
                              "are both float \n"));
        ASCENDC_DEBUG_ASSERT(
            (config.format == CO2Layout::NZ),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "Failed to check format value in Fixpipe, "
                              "when isChannelSplit is set true, format must be set as NZ \n"));
    }
    uint16_t cburstNum = fixpipeTiling.nSize / BLOCK_CUBE;
    uint32_t srcOffset = cburstNum * nIterIndex * intriParams.srcStride * BLOCK_CUBE;
    uint32_t dstOffset = 0;
    bool nz2ndEn = false;
    bool nz2dnEn = false;
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        dstOffset = nIterIndex * fixpipeTiling.nSize;
        nz2ndEn = true;
    } else if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        dstOffset = nIterIndex * fixpipeTiling.nSize * intriParams.dstStride;
        nz2dnEn = true;
    } else {
        dstOffset = cburstNum * nIterIndex * intriParams.dstStride * DEFAULT_C0_SIZE / sizeof(DstT);
    }
    if constexpr (g_gm_overflow_check) {
        bool isSrc = false;
        uint16_t dstEleSize = sizeof(DstT);
        uint64_t gmLen = GetGMLen<config>(intriParams, calNSize, dstEleSize);
        AscendCUtils::CheckGmMemOverflow((__gm__ DstT*)(dst + dstOffset), isSrc, gmLen);
    }
    // LOC -> GM only n direction need fixpipeTiling, m no need fixpipeTiling
    if (intriParams.reluEn == true && intriParams.preReluMode == ReluMode::NO_RELU) {
        return copy_matrix_cc_to_gm(
            (__gm__ DstT*)(dst + dstOffset), (__cc__ SrcT*)(src + srcOffset), 0, calNSize, intriParams.mSize,
            intriParams.dstStride, intriParams.srcStride, cacheMode, static_cast<uint8_t>(intriParams.preClipReluMode),
            intriParams.unitFlag, static_cast<uint64_t>(intriParams.quantPre), static_cast<uint8_t>(intriParams.reluEn),
            intriParams.isChannelSplit, nz2ndEn, static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0,
            false, false, false, false, false, nz2dnEn);
    } else {
        return copy_matrix_cc_to_gm(
            (__gm__ DstT*)(dst + dstOffset), (__cc__ SrcT*)(src + srcOffset), 0, calNSize, intriParams.mSize,
            intriParams.dstStride, intriParams.srcStride, cacheMode, static_cast<uint8_t>(intriParams.preClipReluMode),
            intriParams.unitFlag, static_cast<uint64_t>(intriParams.quantPre),
            static_cast<uint8_t>(intriParams.preReluMode), intriParams.isChannelSplit, nz2ndEn,
            static_cast<uint64_t>(QuantMode_post::NoConv), 0, false, false, 0, false, false, false, false, false,
            nz2dnEn);
    }
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1ImplN(
    __cbuf__ DstT* dst, __cc__ SrcT* src, __cbuf__ uint64_t* cbufWorkspace,
    const FixpipeParamsArch3510<config.format>& intriParams, const FixpipeTiling& fixpipeTiling, uint16_t calNSize,
    uint16_t nIterIndex)
{
    // mov deq tensor from L1 to FB
    CopyDeqTensorToFbuf<config>(cbufWorkspace, fixpipeTiling, calNSize, nIterIndex, intriParams);
    PipeBarrier<PIPE_FIX>();
    // L0C->L1
    FixpipeL0cToL1<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, calNSize, nIterIndex);
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1ImplR(
    __cbuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& intriParams,
    const FixpipeTiling& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex)
{
    // mov deq tensor from L1 to FB
    CopyReluTensorToFbuf<config>(fixpipeTiling, calNSize, nIterIndex, intriParams);
    PipeBarrier<PIPE_FIX>();
    // L0C->L1
    FixpipeL0cToL1<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, calNSize, nIterIndex);
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImplN(
    __ubuf__ DstT* dst, __cc__ SrcT* src, __cbuf__ uint64_t* cbufWorkspace,
    const FixpipeParamsArch3510<config.format>& intriParams, const FixpipeTiling& fixpipeTiling, uint16_t calNSize,
    uint16_t nIterIndex)
{
    // mov deq tensor from L1 to FB
    CopyDeqTensorToFbuf<config>(cbufWorkspace, fixpipeTiling, calNSize, nIterIndex, intriParams);
    PipeBarrier<PIPE_FIX>();
    // L0C->UB
    FixpipeL0cToUB<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, calNSize, nIterIndex);
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImplR(
    __ubuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& intriParams,
    const FixpipeTiling& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex)
{
    // mov deq tensor from L1 to FB
    CopyReluTensorToFbuf<config>(fixpipeTiling, calNSize, nIterIndex, intriParams);
    PipeBarrier<PIPE_FIX>();
    // L0C->UB
    FixpipeL0cToUB<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, calNSize, nIterIndex);
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImplN(
    __gm__ DstT* dst, __cc__ SrcT* src, __cbuf__ uint64_t* cbufWorkspace,
    const FixpipeParamsArch3510<config.format>& intriParams, const FixpipeTiling& fixpipeTiling, uint16_t calNSize,
    uint16_t nIterIndex, const uint8_t cacheMode)
{
    // mov deq tensor from L1 to FB
    CopyDeqTensorToFbuf<config>(cbufWorkspace, fixpipeTiling, calNSize, nIterIndex, intriParams);
    PipeBarrier<PIPE_FIX>();
    // L0C->GM
    FixpipeL0cToOut<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, calNSize, cacheMode, nIterIndex);
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImplR(
    __gm__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& intriParams,
    const FixpipeTiling& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex, const uint8_t cacheMode)
{
    // mov deq tensor from L1 to FB
    CopyReluTensorToFbuf<config>(fixpipeTiling, calNSize, nIterIndex, intriParams);
    PipeBarrier<PIPE_FIX>();
    // L0C->GM
    FixpipeL0cToOut<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, calNSize, cacheMode, nIterIndex);
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeQuantParams(const FixpipeParamsArch3510<config.format>& params)
{
    if (params.quantPre == QuantMode_t::NoQuant) {
        ASCENDC_ASSERT((SupportType<Tuple<SrcT, DstT>, Tuple<float, float>, Tuple<int32_t, int32_t>>()), {
            KERNEL_LOG(KERNEL_ERROR, "Failed to check quantPre value in Fixpipe");
        });
    } else if (
        params.quantPre == QuantMode_t::F322F16 || params.quantPre == QuantMode_t::QF322F16_PRE ||
        params.quantPre == QuantMode_t::VQF322F16_PRE) {
        ASCENDC_ASSERT((SupportType<Tuple<SrcT, DstT>, Tuple<float, half>>()), {
            KERNEL_LOG(KERNEL_ERROR, "Failed to check quantPre value in Fixpipe");
        });
    } else if (
        params.quantPre == QuantMode_t::F322BF16 || params.quantPre == QuantMode_t::QF322BF16_PRE ||
        params.quantPre == QuantMode_t::VQF322BF16_PRE) {
        ASCENDC_ASSERT((SupportType<Tuple<SrcT, DstT>, Tuple<float, bfloat16_t>>()), {
            KERNEL_LOG(KERNEL_ERROR, "Failed to check quantPre value in Fixpipe");
        });
    } else if (params.quantPre == QuantMode_t::DEQF16 || params.quantPre == QuantMode_t::VDEQF16) {
        ASCENDC_ASSERT((SupportType<Tuple<SrcT, DstT>, Tuple<int32_t, half>>()), {
            KERNEL_LOG(KERNEL_ERROR, "Failed to check quantPre value in Fixpipe");
        });
    } else if (params.quantPre == QuantMode_t::QF322B8_PRE || params.quantPre == QuantMode_t::VQF322B8_PRE) {
        ASCENDC_ASSERT((SupportType<Tuple<SrcT, DstT>, Tuple<float, int8_t>, Tuple<float, uint8_t>>()), {
            KERNEL_LOG(KERNEL_ERROR, "Failed to check quantPre value in Fixpipe");
        });
    } else if (params.quantPre == QuantMode_t::REQ8 || params.quantPre == QuantMode_t::VREQ8) {
        ASCENDC_ASSERT((SupportType<Tuple<SrcT, DstT>, Tuple<int32_t, int8_t>, Tuple<int32_t, uint8_t>>()), {
            KERNEL_LOG(KERNEL_ERROR, "Failed to check quantPre value in Fixpipe");
        });
    } else if (
        params.quantPre == QuantMode_t::QF322HIF8_PRE || params.quantPre == QuantMode_t::VQF322HIF8_PRE ||
        params.quantPre == QuantMode_t::QF322HIF8_PRE_HYBRID || params.quantPre == QuantMode_t::VQF322HIF8_PRE_HYBRID) {
        ASCENDC_ASSERT((SupportType<Tuple<SrcT, DstT>, Tuple<float, hifloat8_t>>()), {
            KERNEL_LOG(KERNEL_ERROR, "Failed to check quantPre value in Fixpipe");
        });
    } else if (params.quantPre == QuantMode_t::QS322BF16_PRE || params.quantPre == QuantMode_t::VQS322BF16_PRE) {
        ASCENDC_ASSERT((SupportType<Tuple<SrcT, DstT>, Tuple<int32_t, bfloat16_t>>()), {
            KERNEL_LOG(KERNEL_ERROR, "Failed to check quantPre value in Fixpipe");
        });
    }
}

template <typename DstT, typename SrcT, const FixpipeConfig& config, bool IsGm = false>
__aicore__ inline void CheckFixpipeParams(__cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& params)
{
    static_assert(
        SupportType<
            Tuple<SrcT, DstT>, Tuple<float, bfloat16_t>, Tuple<float, half>, Tuple<float, fp8_e4m3fn_t>,
            Tuple<float, hifloat8_t>, Tuple<float, int8_t>, Tuple<float, uint8_t>, Tuple<float, float>,
            Tuple<int32_t, bfloat16_t>, Tuple<int32_t, half>, Tuple<int32_t, int8_t>, Tuple<int32_t, uint8_t>,
            Tuple<int32_t, int32_t>>(),
        "Failed to check dtype in Fixpipe");
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    CheckFixpipeQuantParams<DstT, SrcT, config>(params);
    if (params.preClipReluMode == ClipReluMode::CLIP_RELU) {
        ASCENDC_ASSERT((params.preReluMode == ReluMode::NORMAL_RELU), {
            KERNEL_LOG(KERNEL_ERROR, "When preClipReluMode is CLIP_RELU, preReluMode must be NORMAL_RELU");
        });
    }
    if (params.preReluMode == ReluMode::SCALAR_RELU || params.preReluMode == ReluMode::VECTOR_RELU) {
        ASCENDC_ASSERT(!(params.quantPre == QuantMode_t::NoQuant), {
            KERNEL_LOG(KERNEL_ERROR, "When preReluMode is SCALAR_RELU or VECTOR_RELU, quantPre can not be NoQuant");
        });
    }
    if (params.preReluMode == ReluMode::SCALAR_RELU || params.preReluMode == ReluMode::VECTOR_RELU ||
        params.preReluMode == ReluMode::NORMAL_RELU) {
        ASCENDC_ASSERT(
            !(params.reluEn == true), { KERNEL_LOG(KERNEL_ERROR, "When preReluMode is enable, reluEn must be true"); });
    }
    constexpr uint32_t L0C_SRC_ALIGN = 16 * sizeof(float); // src must align with 16 elements, each of them is F32 / S32
    uint64_t srcAbsAddr = src - (SrcT*)(GetTPipePtr()->GetBaseAddr(int8_t(TPosition::CO1)));
    ASCENDC_ASSERT((srcAbsAddr % L0C_SRC_ALIGN == 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check src start "
                          "address alignment in Fixpipe");
    });

    if (params.isChannelSplit) {
        ASCENDC_ASSERT((params.nSize <= UINT12_MAX && params.nSize >= 1 && params.nSize % 8 == 0), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check nSize value in Fixpipe, when isChannelSplit is true, its valid "
                "range is 1 ~ 4095 and must be divisible by 8, current value is %u",
                params.nSize);
        });
    } else if (config.format == CO2Layout::ROW_MAJOR || config.format == CO2Layout::COLUMN_MAJOR) {
        ASCENDC_CHECK_VALUE_RANGE(
            params.nSize, 1, UINT12_MAX, "nSize", "Fixpipe when isChannelSplit is false and format is NZ2ND");
        if constexpr (!IsGm) {
            ASCENDC_ASSERT((params.nSize * sizeof(DstT) % 32 == 0), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "Failed to check nSize value in Fixpipe, when NZ2ND/NZ2DN, "
                    "its valid value * sizeof(DstT) must be divisible by 32B, current value is %u",
                    params.nSize);
            });
            ASCENDC_ASSERT((params.mSize * sizeof(DstT) % 32 == 0), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "Failed to check mSize value in Fixpipe, when NZ2ND/NZ2DN, "
                    "its valid value * sizeof(DstT) must be divisible by 32B, current value is %u",
                    params.mSize);
            });
        }
    } else {
        ASCENDC_ASSERT((params.nSize <= UINT12_MAX && params.nSize >= 1 && params.nSize % 16 == 0), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check nSize value in Fixpipe, when isChannelSplit is false and format "
                "is NZ, its valid range is 1 ~ 4095 and must be divisible by 16, current value is %u",
                params.nSize);
        });
    }
    ASCENDC_CHECK_VALUE_RANGE(params.mSize, 1, UINT16_MAX, "mSize", "Fixpipe");

    ASCENDC_ASSERT((params.dstStride != 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check dstStride value in Fixpipe, "
            "its valid range is 1 ~ 4294967295, current value is %u",
            params.dstStride);
    });
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        if (params.params.ndNum > 1) {
            ASCENDC_CHECK_VALUE_RANGE(
                params.params.srcNdStride, 0, UINT16_MAX, "srcNdStride", "Fixpipe when ndNum is > 1");
            ASCENDC_CHECK_VALUE_RANGE(
                params.params.dstNdStride, 1, UINT32_MAX, "dstNdStride", "Fixpipe when ndNum is > 1");
        }
    } else if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        if (params.params.dnNum > 1) {
            ASCENDC_CHECK_VALUE_RANGE(
                params.params.dstDnMatrixStride, 1, UINT32_MAX, "dstNdStride", "Fixpipe when dnNum is > 1");
        }
    }
#endif
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeL0C2UBParam(
    __ubuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& params)
{
    CheckFixpipeParams<DstT, SrcT, config>(src, params);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    uint64_t dstAbsAddr = dst - (DstT*)(GetTPipePtr()->GetBaseAddr(int8_t(TPosition::VECCALC)));
    ASCENDC_ASSERT((dstAbsAddr % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dst start "
                          "address alignment in Fixpipe");
    });
#endif
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeL0C2L1Param(
    __cbuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& params)
{
    CheckFixpipeParams<DstT, SrcT, config>(src, params);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    uint64_t dstAbsAddr = dst - (DstT*)(GetTPipePtr()->GetBaseAddr(int8_t(TPosition::C1)));
    ASCENDC_ASSERT((dstAbsAddr % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dst start "
                          "address alignment in Fixpipe");
    });
#endif
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeL0C2GMParam(
    __gm__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& params)
{
    CheckFixpipeParams<DstT, SrcT, config, true>(src, params);
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(
    __cbuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& intriParams)
{
    CheckFixpipeL0C2L1Param<DstT, SrcT, config>(dst, src, intriParams);
    // nz2nd/nz2dn need set LOOP3_PARA
    SetLoop3Para<config>(intriParams);

    // nz2dn mode need set CHANNEL_PARA extra
    if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        // Loop0_dst_stride in uint of CO_SIZE
        uint64_t channelPara = static_cast<uint64_t>(intriParams.params.srcNzC0Stride) << 48; // CHANNEL_PARA[63:48]
        set_channel_para(channelPara);
    }
    /*
    make code for scalar quant mode:
    1. copy deq scalar u64 immediate
    2. code gen: move data from l0c to l1
    */
    if (IsScalarQuantMode(intriParams.quantPre)) {
        // deq factor of uint64 bits describe: bits[31:13] is deq value of fp32,
        SetDeqScalarDepOnMode<config>(intriParams.quantPre, intriParams.deqScalar, intriParams.fixShiftVal);
    }
    if (intriParams.preReluMode == ReluMode::NORMAL_RELU || intriParams.preReluMode == ReluMode::NO_RELU) {
        PipeBarrier<PIPE_FIX>();
        // LOC -> GM
        FixpipeTiling fixpipeTiling;
        FixpipeL0cToL1<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, intriParams.nSize);
    } else if (intriParams.preReluMode == ReluMode::VECTOR_RELU) {
        FixpipeTiling fixpipeTiling = GenFixpipeTiling(intriParams.nSize);
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2L1ImplR<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, fixpipeTiling.nSize, i);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb1
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2L1ImplR<DstT, SrcT, config>(
                dst, src, intriParams, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum);
        }
        return;
    } else if (intriParams.preReluMode == ReluMode::SCALAR_RELU) {
        SetReluScalarOnMode(intriParams.quantPre, intriParams.reluScalar, intriParams.fixShiftVal);
        PipeBarrier<PIPE_FIX>();
        FixpipeTiling fixpipeTiling;
        // LOC -> L1
        FixpipeL0cToL1<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, intriParams.nSize);
    }
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(
    __cbuf__ DstT* dst, __cc__ SrcT* src, __cbuf__ uint64_t* cbufWorkspace,
    const FixpipeParamsArch3510<config.format>& intriParams)
{
    CheckFixpipeL0C2L1Param<DstT, SrcT, config>(dst, src, intriParams);
    // nz2nd/nz2dn need set LOOP3_PARA
    SetLoop3Para<config>(intriParams);

    // nz2dn mode need set CHANNEL_PARA extra
    if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        // Loop0_dst_stride in uint of CO_SIZE
        uint64_t channelPara = static_cast<uint64_t>(intriParams.params.srcNzC0Stride) << 48; // CHANNEL_PARA[63:48]
        set_channel_para(channelPara);
    }
    /*
    make code for vector quant mode:
    1. generate fixpipeTiling
    2. copy deq tensor from l1 to fb1 (l1 -> fb1)
    3. code gen: move data from l0c to l1
    */
    if (IsVectorQuantMode(intriParams.quantPre)) {
        FixpipeTiling fixpipeTiling = GenFixpipeTiling(intriParams.nSize);
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2L1ImplN<DstT, SrcT, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.nSize, i);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb1
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2L1ImplN<DstT, SrcT, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum);
        }
        return;
    }
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(
    __ubuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& intriParams)
{
    CheckFixpipeL0C2UBParam<DstT, SrcT, config>(dst, src, intriParams);
    // nz2nd/nz2dn need set LOOP3_PARA
    SetLoop3Para<config>(intriParams);
    // nz2dn mode need set CHANNEL_PARA extra
    if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        // Loop0_src_stride in uint of CO_SIZE
        uint64_t channelPara = static_cast<uint64_t>(intriParams.params.srcNzC0Stride) << 48; // CHANNEL_PARA[63:48]
        set_channel_para(channelPara);
    }
    /*
    make code for scalar quant mode:
    1. copy deq scalar u64 immediate
    2. code gen: move data from l0c to ub
    */
    if (IsScalarQuantMode(intriParams.quantPre)) {
        // deq factor of uint64 bits describe: bits[31:13] is deq value of fp32
        SetDeqScalarDepOnMode<config>(
            intriParams.quantPre, intriParams.deqScalar, intriParams.fixShiftVal); // float32->uint64_t
    }
    if (intriParams.preReluMode == ReluMode::NORMAL_RELU || intriParams.preReluMode == ReluMode::NO_RELU) {
        PipeBarrier<PIPE_FIX>();
        // LOC -> GM
        FixpipeTiling fixpipeTiling;
        FixpipeL0cToUB<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, intriParams.nSize);
    } else if (intriParams.preReluMode == ReluMode::VECTOR_RELU) {
        FixpipeTiling fixpipeTiling = GenFixpipeTiling(intriParams.nSize);
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2UBImplR<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, fixpipeTiling.nSize, i);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb1
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2UBImplR<DstT, SrcT, config>(
                dst, src, intriParams, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum);
        }
        return;
    } else if (intriParams.preReluMode == ReluMode::SCALAR_RELU) {
        SetReluScalarOnMode(intriParams.quantPre, intriParams.reluScalar, intriParams.fixShiftVal);
        PipeBarrier<PIPE_FIX>();
        // LOC->UB
        FixpipeTiling fixpipeTiling;
        FixpipeL0cToUB<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, intriParams.nSize);
    }
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(
    __ubuf__ DstT* dst, __cc__ SrcT* src, __cbuf__ uint64_t* cbufWorkspace,
    const FixpipeParamsArch3510<config.format>& intriParams)
{
    CheckFixpipeL0C2UBParam<DstT, SrcT, config>(dst, src, intriParams);
    // nz2nd/nz2dn need set LOOP3_PARA
    SetLoop3Para<config>(intriParams);
    // nz2dn mode need set CHANNEL_PARA extra
    if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        // Loop0_src_stride in uint of CO_SIZE
        uint64_t channelPara = static_cast<uint64_t>(intriParams.params.srcNzC0Stride) << 48; // CHANNEL_PARA[63:48]
        set_channel_para(channelPara);
    }
    /*
    make code for vector quant mode:
    1. generate fixpipeTiling
    2. copy deq tensor from l1 to fb1 (l1 -> fb1)
    3. code gen: move data from l0c to ub
    */
    FixpipeTiling fixpipeTiling = GenFixpipeTiling(intriParams.nSize);
    if (IsVectorQuantMode(intriParams.quantPre)) {
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2UBImplN<DstT, SrcT, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.nSize, i);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb1
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2UBImplN<DstT, SrcT, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum);
        }
        return;
    }
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImpl(
    __gm__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsArch3510<config.format>& intriParams,
    const uint8_t cacheMode = 0)
{
    CheckFixpipeL0C2GMParam<DstT, SrcT, config>(dst, src, intriParams);
    // nz2nd/nz2dn need set LOOP3_PARA
    SetLoop3Para<config>(intriParams);
    // nz2dn mode need set CHANNEL_PARA extra
    if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        uint64_t channelPara = static_cast<uint64_t>(intriParams.params.srcNzC0Stride) << 48; // CHANNEL_PARA[63:48]
        set_channel_para(channelPara);
    }
    /*
    make code for scalar quant mode:
    1. copy deq scalar u64 immediate
    2. code gen: move data from l0c to gm
    */
    if (IsScalarQuantMode(intriParams.quantPre)) {
        SetDeqScalarDepOnMode<config>(intriParams.quantPre, intriParams.deqScalar, intriParams.fixShiftVal);
    }
    if (intriParams.preReluMode == ReluMode::NORMAL_RELU || intriParams.preReluMode == ReluMode::NO_RELU) {
        PipeBarrier<PIPE_FIX>();
        // LOC -> GM
        FixpipeTiling fixpipeTiling;
        FixpipeL0cToOut<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, intriParams.nSize, cacheMode);
    } else if (intriParams.preReluMode == ReluMode::VECTOR_RELU) {
        FixpipeTiling fixpipeTiling = GenFixpipeTiling(intriParams.nSize);
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2GMImplR<DstT, SrcT, config>(
                dst, src, intriParams, fixpipeTiling, fixpipeTiling.nSize, i, cacheMode);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb1
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2GMImplR<DstT, SrcT, config>(
                dst, src, intriParams, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum, cacheMode);
        }
        return;
    } else if (intriParams.preReluMode == ReluMode::SCALAR_RELU) {
        SetReluScalarOnMode(intriParams.quantPre, intriParams.reluScalar, intriParams.fixShiftVal);
        PipeBarrier<PIPE_FIX>();
        // LOC -> GM
        FixpipeTiling fixpipeTiling;
        FixpipeL0cToOut<DstT, SrcT, config>(dst, src, intriParams, fixpipeTiling, intriParams.nSize, cacheMode);
    }
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImpl(
    __gm__ DstT* dst, __cc__ SrcT* src, __cbuf__ uint64_t* cbufWorkspace,
    const FixpipeParamsArch3510<config.format>& intriParams, const uint8_t cacheMode = 0)
{
    CheckFixpipeL0C2GMParam<DstT, SrcT, config>(dst, src, intriParams);
    // nz2nd/nz2dn need set LOOP3_PARA
    SetLoop3Para<config>(intriParams);
    // nz2dn mode need set CHANNEL_PARA extra
    if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        uint64_t channelPara = static_cast<uint64_t>(intriParams.params.srcNzC0Stride) << 48; // CHANNEL_PARA[63:48]
        set_channel_para(channelPara);
    }
    /*
    make code for vector quant mode:
    1. generate fixpipeTiling
    2. copy deq tensor from l1 to fb1 (l1 -> fb1)
    3. code gen: move data from l0c to gm
    */
    FixpipeTiling fixpipeTiling = GenFixpipeTiling(intriParams.nSize);
    if (IsVectorQuantMode(intriParams.quantPre)) {
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2GMImplN<DstT, SrcT, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.nSize, i, cacheMode);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb1
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2GMImplN<DstT, SrcT, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum,
                cacheMode);
        }
        return;
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__
#endif
