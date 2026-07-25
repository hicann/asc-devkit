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
 * \file kernel_operator_fixpipe_v2_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_c220/kernel_operator_fixpipe_v2_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_V2_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_FIXPIPE_V2_IMPL_H
#define ASCENDC_MODULE_OPERATOR_FIXPIPE_V2_IMPL_H

#include "kernel_operator_set_spr_impl.h"
#include "../../../include/basic_api/kernel_struct_fixpipe.h"

namespace AscendC {
__aicore__ inline void SetFixPipeClipReluImpl(uint64_t config)
{
    (void)(config);
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeClipRelu");
}

template <typename T>
__aicore__ inline void SetFixPipeAddrImpl(const LocalTensor<T>& eleWise, uint16_t c0ChStride)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeAddr");
}

/* **************************************************************************************************
 * Fixpipe                                             *
 * ************************************************************************************************* */
template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeL0C2L1Param(__cbuf__ T* dst, __cc__ U* src, const FixpipeParamsV220& params)
{
    ASCENDC_DEBUG_ASSERT(
        (config.format != CO2Layout::ROW_MAJOR),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check format in Fixpipe, when src position is L0C Buffer(CO1) and dst position "
                          "is L1 Buffer(C1), format must be set as NZ \n"));
    ASCENDC_DEBUG_ASSERT(
        (!(params.isChannelSplit)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check isChannelSplit in Fixpipe, when src position is "
                          "L0C Buffer(CO1) and dst position is L1 Buffer(C1), isChannelSplit must be set as false \n"));

    ASCENDC_DEBUG_ASSERT(
        (SupportType<
            Tuple<U, T>, Tuple<float, int8_t>, Tuple<float, uint8_t>, Tuple<float, half>, Tuple<float, bfloat16_t>,
            Tuple<int32_t, int8_t>, Tuple<int32_t, uint8_t>, Tuple<int32_t, half>>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype in Fixpipe, when src position is L0C Buffer(CO1) and dst "
            "position is L1 Buffer(C1), support dtype combinations are src: float, dst: int8_t / uint8_t / half / "
            "bfloat16_t; "
            "src: int32_t, dst: int8_t / uint8_t / half"));
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeL0C2GMParam(__gm__ T* dst, __cc__ U* src, const FixpipeParamsV220& params)
{
    ASCENDC_DEBUG_ASSERT(
        (SupportType<
            Tuple<U, T>, Tuple<float, int8_t>, Tuple<float, uint8_t>, Tuple<float, half>, Tuple<float, bfloat16_t>,
            Tuple<float, float>, Tuple<int32_t, int8_t>, Tuple<int32_t, uint8_t>, Tuple<int32_t, half>,
            Tuple<int32_t, int32_t>>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check "
            "dtype in Fixpipe, when src position is L0C Buffer(CO1) and dst position is GM, support dtype "
            "combinations are src: "
            "float, dst: int8_t / uint8_t / half / bfloat16_t / float; src: int32_t, dst: int8_t / uint8_t / half / "
            "int32_t"));
    if constexpr (IsSameType<U, float>::value && IsSameType<T, float>::value) {
        ASCENDC_DEBUG_ASSERT(
            (params.quantPre == QuantMode_t::NoQuant),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check quantPre value in Fixpipe, when src is float, dst is float, supported value is "
                "NoQuant"));
    } else if constexpr (IsSameType<U, int32_t>::value && IsSameType<T, int32_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (params.quantPre == QuantMode_t::NoQuant),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check quantPre value in Fixpipe, when src is int32_t, dst is int32_t, supported value is "
                "NoQuant"));
    }
    if (params.isChannelSplit) {
        ASCENDC_DEBUG_ASSERT(
            (IsSameType<U, float>::value && IsSameType<T, float>::value),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "Failed to check "
                              "isChannelSplit value in Fixpipe, isChannelSplit can be set true only when src and dst "
                              "are both float \n"));
        ASCENDC_DEBUG_ASSERT(
            (config.format != CO2Layout::ROW_MAJOR),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "Failed to check format value in Fixpipe, "
                              "when isChannelSplit is set true, format must be set as NZ \n"));
    }
}

// tiling params
struct FixpipeTilingV220 {
    uint16_t nIterNum = 0;
    uint16_t nSize = 0;
    bool isDb = false;
    uint16_t tailNSize = 0;
};

// fixpipe tiling calculating
__aicore__ inline FixpipeTilingV220 GenFixpipeTilingV220(uint16_t n)
{
    FixpipeTilingV220 tiling;
    // deqTensor/reluTensor in FB valid num is 256
    uint16_t maxDeqNums = 256;
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

__aicore__ inline void CopyDeqTensorToFbuf(
    __cbuf__ uint64_t* cbufWorkspace, const FixpipeTilingV220& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex)
{
    if ASCEND_IS_AIV {
        return;
    }
    uint16_t deqDataSize = DivCeil(calNSize * sizeof(uint64_t), 128) * 128;
    __fbuf__ uint64_t* deqTensorTempBuf =
        AscendCUtils::GetTemporaryFbBufferAddr<uint64_t>(0, deqDataSize / sizeof(uint64_t));
    uint32_t deqValueOffset = nIterIndex * fixpipeTiling.nSize;
    // L1 -> FB
    uint16_t fbufBurstLen = deqDataSize / 128; // copy from cbuf to fbuf, burst_len unit is 128Bytes
    copy_cbuf_to_fbuf(deqTensorTempBuf, cbufWorkspace + deqValueOffset, 1, fbufBurstLen, 0, 0);
    // FPC of fixpipe buffer for Quant_PRE is FPC[15:8], unit is 128Bytes
    uint64_t deqTensorAddr = ((uint64_t)deqTensorTempBuf >> static_cast<uint64_t>(7)) << 8;
    set_fpc(deqTensorAddr);
    AscendCUtils::FreeTemporaryFbBuffer<uint64_t>(deqTensorTempBuf);
}

template <typename T, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(__cbuf__ T* dst, __cc__ T* src, const FixpipeParamsV220& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        false,
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype in Fixpipe, when src position is L0C Buffer(CO1) and dst position is L1 Buffer(C1), "
            "support dtype combinations are src: float, dst: int8_t / uint8_t / half / bfloat16_t; src: int32_t, dst: "
            "int8_t / uint8_t / half\n"));
}

template <typename T, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(
    __cbuf__ T* dst, __cc__ T* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        false,
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype in Fixpipe, when src position is L0C Buffer(CO1) and dst position is L1 Buffer(C1), "
            "support dtype combinations are src: float, dst: int8_t / uint8_t / half / bfloat16_t; src: int32_t, dst: "
            "int8_t / uint8_t / half\n"));
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(__ubuf__ T* dst, __cc__ U* src, const FixpipeParamsV220& intriParams)
{
    (void)dst;
    (void)src;
    (void)intriParams;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    ReportNotSupport(false, "Fixpipe from L0C to UB");
#endif
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(
    __ubuf__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams)
{
    (void)dst;
    (void)src;
    (void)cbufWorkspace;
    (void)intriParams;
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    ReportNotSupport(false, "Fixpipe from L0C to UB");
#endif
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(__cbuf__ T* dst, __cc__ U* src, const FixpipeParamsV220& intriParams)
{
    CheckFixpipeL0C2L1Param<T, U, config>(dst, src, intriParams);
    /*
    make code for scalar quant mode:
    1. copy deq scalar u64 immediate
    2. code gen: move data from l0c to l1
    */
    if (intriParams.quantPre == QuantMode_t::DEQF16 || intriParams.quantPre == QuantMode_t::QF322B8_PRE ||
        intriParams.quantPre == QuantMode_t::REQ8) {
        // deq factor of uint64 bits describe: bits[31:13] is deq value of fp32,
        SetQuantPreImpl(intriParams.deqScalar);
    }
    FixpipeTilingV220 fixpipeTiling;
    // LOC -> L1
    FixpipeL0cToL1<T, U, config>(dst, src, intriParams, fixpipeTiling, intriParams.nSize);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(
    __cbuf__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams)
{
    CheckFixpipeL0C2L1Param<T, U, config>(dst, src, intriParams);
    /*
    make code for vector quant mode:
    1. generate tiling
    2. copy deq tensor from l1 to fb0
    3. code gen: move data from l0c to l1
    */
    FixpipeTilingV220 fixpipeTiling = GenFixpipeTilingV220(intriParams.nSize);
    if (intriParams.quantPre == QuantMode_t::VDEQF16 || intriParams.quantPre == QuantMode_t::VQF322B8_PRE ||
        intriParams.quantPre == QuantMode_t::VREQ8) {
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2L1ImplN<T, U, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.nSize, i);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb0
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2L1ImplN<T, U, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum);
        }
        return;
    }
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImpl(__gm__ T* dst, __cc__ U* src, const FixpipeParamsV220& intriParams)
{
    CheckFixpipeL0C2GMParam<T, U, config>(dst, src, intriParams);
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        uint64_t ndPara = static_cast<uint64_t>(intriParams.dstNdStride) << 32; // ND_PARA[47:32]
        ndPara |= static_cast<uint64_t>(intriParams.srcNdStride) << 16;         // ND_PARA[31:16]
        ndPara |= static_cast<uint64_t>(intriParams.ndNum);                     // ND_PARA[15:0]
        SetNdParaImpl(ndPara);
    }
    FixpipeTilingV220 fixpipeTiling;
    /*
    make code for scalar quant mode:
    1. copy deq scalar u64 immediate
    2. code gen: move data from l0c to gm
    */
    if (intriParams.quantPre == QuantMode_t::DEQF16 || intriParams.quantPre == QuantMode_t::QF322B8_PRE ||
        intriParams.quantPre == QuantMode_t::REQ8) {
        SetQuantPreImpl(intriParams.deqScalar);
    }
    PipeBarrier<PIPE_FIX>();
    // LOC -> GM
    FixpipeL0cToOut<T, U, config>(dst, src, intriParams, fixpipeTiling, intriParams.nSize);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImpl(
    __gm__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams)
{
    CheckFixpipeL0C2GMParam<T, U, config>(dst, src, intriParams);
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        uint64_t ndPara = static_cast<uint64_t>(intriParams.dstNdStride) << 32; // ND_PARA[47:32]
        ndPara |= static_cast<uint64_t>(intriParams.srcNdStride) << 16;         // ND_PARA[31:16]
        ndPara |= static_cast<uint64_t>(intriParams.ndNum);                     // ND_PARA[15:0]
        SetNdParaImpl(ndPara);
    }
    /*
    make code for vector quant mode:
    1. generate tiling
    2. copy deq tensor from gm to fb0 (l1 -> fb0)
    3. code gen: move data from l0c to gm
    */
    FixpipeTilingV220 fixpipeTiling = GenFixpipeTilingV220(intriParams.nSize);
    if (intriParams.quantPre == QuantMode_t::VDEQF16 || intriParams.quantPre == QuantMode_t::VQF322B8_PRE ||
        intriParams.quantPre == QuantMode_t::VREQ8) {
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2GMImplN<T, U, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.nSize, i);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb0
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2GMImplN<T, U, config>(
                dst, src, cbufWorkspace, intriParams, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum);
        }
        return;
    }
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1ImplN(
    __cbuf__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams,
    const FixpipeTilingV220& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex)
{
    // mov deq tensor from L1 to FB
    CopyDeqTensorToFbuf(cbufWorkspace, fixpipeTiling, calNSize, nIterIndex);
    PipeBarrier<PIPE_FIX>();
    // L0C->L1
    FixpipeL0cToL1<T, U, config>(dst, src, intriParams, fixpipeTiling, calNSize, nIterIndex);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImplN(
    __gm__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams,
    const FixpipeTilingV220& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex)
{
    // mov deq tensor from L1 to FB
    CopyDeqTensorToFbuf(cbufWorkspace, fixpipeTiling, calNSize, nIterIndex);
    PipeBarrier<PIPE_FIX>();
    // L0C->GM
    FixpipeL0cToOut<T, U, config>(dst, src, intriParams, fixpipeTiling, calNSize, nIterIndex);
}

// contains loop info and cal n size for each loop
// move data L0C->L1
template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0cToL1(
    __cbuf__ T* dst, __cc__ U* src, const FixpipeParamsV220& intriParams, const FixpipeTilingV220& fixpipeTiling,
    uint16_t calNSize, uint16_t nIterIndex = 0)
{
    uint16_t cburstNum = fixpipeTiling.nSize / 16;
    uint32_t srcOffset = cburstNum * nIterIndex * intriParams.srcStride * BLOCK_CUBE;
    uint32_t dstOffset = cburstNum * nIterIndex * intriParams.dstStride * 32 / sizeof(T);
    // LOC -> L1 only n direction need tiling, m no need tiling
    // 910b soc, dst_stride in unit of 32B, input dst_stride in unit of 32B.
    if ASCEND_IS_AIC {
        switch (intriParams.quantPre) {
            case QuantMode_t::F322F16:
                return copy_matrix_cc_to_cbuf(
                    (__cbuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::F322F16,
                    static_cast<uint8_t>(intriParams.reluEn), false, false);
            case QuantMode_t::F322BF16:
                return copy_matrix_cc_to_cbuf(
                    (__cbuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::F322BF16,
                    static_cast<uint8_t>(intriParams.reluEn), false, false);
            case QuantMode_t::DEQF16:
                return copy_matrix_cc_to_cbuf(
                    (__cbuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::DEQF16,
                    static_cast<uint8_t>(intriParams.reluEn), false, false);
            case QuantMode_t::VDEQF16:
                return copy_matrix_cc_to_cbuf(
                    (__cbuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::VDEQF16,
                    static_cast<uint8_t>(intriParams.reluEn), false, false);
            case QuantMode_t::QF322B8_PRE:
                return copy_matrix_cc_to_cbuf(
                    (__cbuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::QF322B8_PRE,
                    static_cast<uint8_t>(intriParams.reluEn), false, false);
            case QuantMode_t::VQF322B8_PRE:
                return copy_matrix_cc_to_cbuf(
                    (__cbuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::VQF322B8_PRE,
                    static_cast<uint8_t>(intriParams.reluEn), false, false);
            case QuantMode_t::REQ8:
                return copy_matrix_cc_to_cbuf(
                    (__cbuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::REQ8,
                    static_cast<uint8_t>(intriParams.reluEn), false, false);
            case QuantMode_t::VREQ8:
                return copy_matrix_cc_to_cbuf(
                    (__cbuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::VREQ8,
                    static_cast<uint8_t>(intriParams.reluEn), false, false);
            default:
                ASCENDC_DEBUG_ASSERT(
                    false, KERNEL_LOG_INTERNAL(KERNEL_ERROR, "Fixpipe doesn't support this quantize mode \n"));
        }
    }
}

__aicore__ inline uint64_t GetGMLength(
    const FixpipeParamsV220& intriParams, const uint16_t& calNSize, const uint16_t& dstEleSize, const bool& nz2ndEn)
{
    constexpr uint16_t dstStrideUnit = 32;
    constexpr uint16_t fractalNsize = 16;
    uint64_t cburstNum = calNSize / fractalNsize;
    uint64_t gmLen =
        (cburstNum - 1) * intriParams.dstStride * dstStrideUnit + intriParams.mSize * fractalNsize * dstEleSize;
    if (nz2ndEn) {
        gmLen = (static_cast<uint64_t>(intriParams.ndNum) - 1) * dstEleSize * intriParams.dstNdStride +
                (intriParams.mSize - 1) * intriParams.dstStride * dstEleSize + cburstNum * fractalNsize * dstEleSize;
    }
    return gmLen;
}

// contains loop info and cal n size for each loop
// move data L0C->GM
template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0cToOut(
    __gm__ T* dst, __cc__ U* src, const FixpipeParamsV220& intriParams, const FixpipeTilingV220& fixpipeTiling,
    uint16_t calNSize, uint16_t nIterIndex = 0)
{
    uint16_t cburstNum = fixpipeTiling.nSize / 16;
    uint32_t srcOffset = cburstNum * nIterIndex * intriParams.srcStride * BLOCK_CUBE;
    uint32_t dstOffset = 0;
    bool nz2ndEn = false;
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        dstOffset = nIterIndex * fixpipeTiling.nSize;
        nz2ndEn = true;
    } else {
        dstOffset = cburstNum * nIterIndex * intriParams.dstStride * 32 / sizeof(T);
    }

    if constexpr (g_gm_overflow_check) {
        uint64_t gmLen = GetGMLength(intriParams, calNSize, sizeof(T), nz2ndEn);
        AscendCUtils::CheckGmMemOverflow((__gm__ T*)(dst + dstOffset), false, gmLen); // isSrc is false
    }
    if ASCEND_IS_AIC {
        switch (intriParams.quantPre) {
            case QuantMode_t::NoQuant:
                return copy_matrix_cc_to_gm(
                    (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::NoQuant,
                    static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn);
            case QuantMode_t::F322F16:
                return copy_matrix_cc_to_gm(
                    (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::F322F16,
                    static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn);
            case QuantMode_t::F322BF16:
                return copy_matrix_cc_to_gm(
                    (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::F322BF16,
                    static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn);
            case QuantMode_t::DEQF16:
                return copy_matrix_cc_to_gm(
                    (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::DEQF16,
                    static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn);
            case QuantMode_t::VDEQF16:
                return copy_matrix_cc_to_gm(
                    (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::VDEQF16,
                    static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn);
            case QuantMode_t::QF322B8_PRE:
                return copy_matrix_cc_to_gm(
                    (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::QF322B8_PRE,
                    static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn);
            case QuantMode_t::VQF322B8_PRE:
                return copy_matrix_cc_to_gm(
                    (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::VQF322B8_PRE,
                    static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn);
            case QuantMode_t::REQ8:
                return copy_matrix_cc_to_gm(
                    (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::REQ8,
                    static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn);
            case QuantMode_t::VREQ8:
                return copy_matrix_cc_to_gm(
                    (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
                    intriParams.dstStride, intriParams.srcStride, intriParams.unitFlag, QuantMode_t::VREQ8,
                    static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn);
            default:
                ASCENDC_DEBUG_ASSERT(
                    false, KERNEL_LOG_INTERNAL(KERNEL_ERROR, "Fixpipe doesn't support this quantize mode \n"));
        }
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_FIXPIPE_V2_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_V2_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_V2_IMPL_H__
#endif
