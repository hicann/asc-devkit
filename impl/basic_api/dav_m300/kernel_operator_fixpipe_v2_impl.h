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
    "impl/basic_api/dav_m300/kernel_operator_fixpipe_v2_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_V2_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_FIXPIPE_V2_IMPL_H
#define ASCENDC_MODULE_OPERATOR_FIXPIPE_V2_IMPL_H

#include "kernel_operator_set_spr_impl.h"
#include "../../../include/basic_api/kernel_struct_fixpipe.h"

namespace AscendC {
__aicore__ inline void SetFixPipeClipReluImpl(uint64_t config) { set_fix_clip_relu(config); }

template <typename T>
__aicore__ inline void SetFixPipeAddrImpl(const LocalTensor<T>& eleWise, uint16_t c0ChStride)
{
    static_assert(
        SupportType<T, half>(), "Failed to check dtype in SetFixPipeAddr, current api support dtype is : half");
    const Hardware eleWiseHWPos = GetPhyType((TPosition)eleWise.GetPosition());
    if (eleWiseHWPos != Hardware::L1)
        ASCENDC_CHECK_TPOSITION(
            false, "eleWise", "L1 Buffer(A1/B1/C1)", "SetFixPipeAddr",
            GetPositionDisplay(static_cast<TPosition>(eleWise.GetPosition())));
    uint64_t config = 0;
    config = config | c0ChStride; // ELT_SRC_PARA[15:0], C0 channel stride Size.
    config = config | ((static_cast<uint64_t>(eleWise.GetPhyAddr()) >> 5)
                       << 16); // L1 address, ELT_SRC_PARA[31:16], align with 32bit.
    set_fixp_addr(config);
#if ASCENDC_CPU_DEBUG
    uint64_t eltBaseAddr = static_cast<uint64_t>(eleWise.GetPhyAddr());
    SetEleSrcPara(eltBaseAddr);
#endif
}

/* **************************************************************************************************
 * Fixpipe                                             *
 * ************************************************************************************************* */
const uint32_t L0C_SRC_ALIGN = 16 * sizeof(float); // src must align with 16 elements, each of them is F32 / S32

__aicore__ inline bool IsVectorQuantMode(QuantMode_t quantPre)
{
    return (
        quantPre == QuantMode_t::VDEQF16 || quantPre == QuantMode_t::VQF322B8_PRE || quantPre == QuantMode_t::VREQ8);
}

__aicore__ inline bool IsScalarQuantMode(QuantMode_t quantPre)
{
    return (quantPre == QuantMode_t::DEQF16 || quantPre == QuantMode_t::QF322B8_PRE || quantPre == QuantMode_t::REQ8);
}

__aicore__ inline void SetLoop3Para(const FixpipeParamsM300& intriParams)
{
    ASCENDC_ASSERT((intriParams.ndNum > 0), { KERNEL_LOG(KERNEL_ERROR, "ndNum must be larger than 0"); });
    uint64_t loop3Para = static_cast<uint64_t>(intriParams.dstNdStride) << 32; // LOOP3_PARA[63:32]
    loop3Para |= static_cast<uint64_t>(intriParams.srcNdStride) << 16;         // LOOP3_PARA[31:16]
    loop3Para |= static_cast<uint64_t>(intriParams.ndNum);                     // LOOP3_PARA[15:0]
    set_loop3_para(loop3Para);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void CheckCommonFixpipeParam(__cc__ U* src, const FixpipeParamsM300& params)
{
#if ASCENDC_CPU_DEBUG
    uint64_t srcAbsAddr = src - (U*)(GetTPipePtr()->GetBaseAddr(int8_t(TPosition::CO1)));
    ASCENDC_ASSERT(srcAbsAddr % L0C_SRC_ALIGN == 0, {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check srcLocal start address "
                          "alignment in Fixpipe, its start address must align with 64B");
    });
    if (params.isChannelSplit) {
        ASCENDC_ASSERT((params.nSize <= 4095 && params.nSize >= 1 && params.nSize % 8 == 0), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check nSize value in Fixpipe, when isChannelSplit is true, its valid range is 1 ~ 4095 and "
                "must be divisible by 8, current value is %u",
                params.nSize);
        });
    } else if (config.format == CO2Layout::ROW_MAJOR) {
        ASCENDC_ASSERT((params.nSize <= 4095 && params.nSize >= 1), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check nSize "
                "value in Fixpipe, when isChannelSplit is false and format is NZ2ND, its valid range is 1 ~ 4095, "
                "current "
                "value is %u",
                params.nSize);
        });
    } else {
        ASCENDC_ASSERT((params.nSize <= 4095 && params.nSize >= 1 && params.nSize % 16 == 0), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check nSize value in Fixpipe, when isChannelSplit is false and format is NZ, its valid "
                "range "
                "is 1 ~ 4095 and must be divisible by 16, current value is %u",
                params.nSize);
        });
    }
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        ASCENDC_ASSERT((params.mSize <= 8192 && params.mSize >= 1), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check mSize "
                "value in Fixpipe, when format is ROW_MAJOR, its valid range is 1 ~ 8192, current value is %u",
                params.mSize);
        });
    } else {
        ASCENDC_ASSERT((params.mSize <= 65535 && params.mSize >= 1), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check mSize "
                "value in Fixpipe, when format is NZ, its valid range is 1 ~ 65535, current value is %u",
                params.mSize);
        });
    }

    ASCENDC_ASSERT((params.dstStride != 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check dstStride value in Fixpipe, "
            "its valid range is 1 ~ 4294967295, current value is %u",
            params.dstStride);
    });
    if (params.ndNum > 1) {
        ASCENDC_ASSERT((params.srcNdStride <= 512 && params.srcNdStride >= 1), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to "
                "check srcNdStride value in Fixpipe, when ndNum is > 1, its valid range is 1 ~ 512, current value is "
                "%u",
                params.srcNdStride);
        });
        ASCENDC_ASSERT((params.dstNdStride != 0), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check dstNdStride value in "
                "Fixpipe, when ndNum is > 1, its valid range is 1 ~ 65535, current value is %u",
                params.dstNdStride);
        });
    }

    if constexpr (IsSameType<U, float>::value && SupportType<T, int8_t, uint8_t>()) {
        ASCENDC_ASSERT((params.quantPre == QuantMode_t::QF322B8_PRE || params.quantPre == QuantMode_t::VQF322B8_PRE), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check quantPre value in Fixpipe, when src is float, dst is "
                              "int8_t / uint8_t, supported values are QF322B8_PRE and VQF322B8_PRE");
        });
    } else if constexpr (IsSameType<U, float>::value && IsSameType<T, half>::value) {
        ASCENDC_ASSERT((params.quantPre == QuantMode_t::F322F16), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check quantPre "
                              "value in Fixpipe, when src is float, dst is half, supported value is F322F16");
        });
    } else if constexpr (IsSameType<U, float>::value && IsSameType<T, bfloat16_t>::value) {
        ASCENDC_ASSERT((params.quantPre == QuantMode_t::F322BF16), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check quantPre"
                              " value in Fixpipe, when src is float, dst is bfloat16_t, supported value is F322BF16");
        });
    } else if constexpr (IsSameType<U, int32_t>::value && SupportType<T, int8_t, uint8_t>()) {
        ASCENDC_ASSERT((params.quantPre == QuantMode_t::REQ8 || params.quantPre == QuantMode_t::VREQ8), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check quantPre value in Fixpipe, when src is int32_t, dst is "
                              "int8_t / uint8_t, supported values are REQ8 and VREQ8");
        });
    } else if constexpr (IsSameType<U, int32_t>::value && IsSameType<T, half>::value) {
        ASCENDC_ASSERT((params.quantPre == QuantMode_t::DEQF16 || params.quantPre == QuantMode_t::VDEQF16), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check quantPre value in Fixpipe, when src is int32_t, dst is half, "
                              "supported values are DEQF16 and VDEQF16");
        });
    }
#endif
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeL0C2UBParam(__ubuf__ T* dst, __cc__ U* src, const FixpipeParamsM300& params)
{
    CheckCommonFixpipeParam<T, U, config>(src, params);
    ASCENDC_CHECK_TENSOR_PTR_ALIGN(
        dst, TPosition::VECCALC, ONE_BLK_SIZE, "dstLocal", "Fixpipe when dst position is UB(VECIN/VECCALC/VECOUT)");
    static_assert(
        (SupportType<
            Tuple<U, T>, Tuple<float, int8_t>, Tuple<float, uint8_t>, Tuple<float, half>, Tuple<float, bfloat16_t>,
            Tuple<float, float>, Tuple<int32_t, int8_t>, Tuple<int32_t, uint8_t>, Tuple<int32_t, half>,
            Tuple<int32_t, int32_t>>()),
        "Failed to check dtype in Fixpipe, when src position is "
        "L0C Buffer(CO1) and dst position is GM, support dtype combinations are src: float, dst: int8_t / uint8_t / "
        "half / "
        "bfloat16_t / float; src: int32_t, dst: int8_t / uint8_t / half / int32_t");
    if constexpr (IsSameType<U, float>::value && IsSameType<T, float>::value) {
        ASCENDC_ASSERT((params.quantPre == QuantMode_t::NoQuant), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check quantPre "
                              "value in Fixpipe, when src is float, dst is float, supported value is NoQuant");
        });
    } else if constexpr (IsSameType<U, int32_t>::value && IsSameType<T, int32_t>::value) {
        ASCENDC_ASSERT((params.quantPre == QuantMode_t::NoQuant), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check quantPre "
                              "value in Fixpipe, when src is int32_t, dst is int32_t, supported value is NoQuant");
        });
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

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeL0C2L1Param(__cbuf__ T* dst, __cc__ U* src, const FixpipeParamsM300& params)
{
    CheckCommonFixpipeParam<T, U, config>(src, params);
    ASCENDC_CHECK_TENSOR_PTR_ALIGN(
        dst, TPosition::C1, ONE_BLK_SIZE, "dstLocal", "Fixpipe when dst position is L1 Buffer(C1)");
    ASCENDC_DEBUG_ASSERT(
        (!(params.isChannelSplit)),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check isChannelSplit in Fixpipe, when src position is "
                          "L0C Buffer(CO1) and dst position is L1 Buffer(A1), isChannelSplit must be set as false \n"));
    static_assert(
        (SupportType<
            Tuple<U, T>, Tuple<float, int8_t>, Tuple<float, uint8_t>, Tuple<float, half>, Tuple<float, bfloat16_t>,
            Tuple<float, float>, Tuple<int32_t, int32_t>, Tuple<float, int4b_t>, Tuple<int32_t, int4b_t>,
            Tuple<int32_t, int8_t>, Tuple<int32_t, uint8_t>, Tuple<int32_t, half>>()),
        "Failed to check dtype in Fixpipe, when src position is L0C Buffer(CO1) and dst position is L1 Buffer(A1), "
        "support dtype combinations are src: float, dst: int8_t / uint8_t / half / bfloat16_t; src: int32_t, dst: "
        "int8_t / uint8_t / half");
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeL0C2GMParam(__gm__ T* dst, __cc__ U* src, const FixpipeParamsM300& params)
{
    CheckCommonFixpipeParam<T, U, config>(src, params);

    static_assert(
        (SupportType<
            Tuple<U, T>, Tuple<float, int8_t>, Tuple<float, uint8_t>, Tuple<float, half>, Tuple<float, bfloat16_t>,
            Tuple<float, float>, Tuple<int32_t, int8_t>, Tuple<int32_t, uint8_t>, Tuple<int32_t, half>,
            Tuple<int32_t, int32_t>>()),
        "Failed to check dtype in Fixpipe, when src position is L0C Buffer(CO1) "
        "and dst position is GM, support dtype combinations are src: float, dst: int8_t / uint8_t / half / bfloat16_t "
        "/ float; src: int32_t, dst: int8_t / uint8_t / half / int32_t");
    if constexpr (IsSameType<U, float>::value && IsSameType<T, float>::value) {
        ASCENDC_ASSERT((params.quantPre == QuantMode_t::NoQuant), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check quantPre "
                              "value in Fixpipe, when src is float, dst is float, supported value is NoQuant");
        });
    } else if constexpr (IsSameType<U, int32_t>::value && IsSameType<T, int32_t>::value) {
        ASCENDC_ASSERT((params.quantPre == QuantMode_t::NoQuant), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check quantPre "
                              "value in Fixpipe, when src is int32_t, dst is int32_t, supported value is NoQuant");
        });
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

// adaptation due to unit differences;
template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline FixpipeParamsM300 AdaptFixpipeParams(const FixpipeParamsM300& intriParams)
{
    FixpipeParamsM300 params(intriParams);
    if constexpr (config.format == CO2Layout::NZ) {
        params.dstStride = intriParams.dstStride * ONE_BLK_SIZE / sizeof(T);
    }

    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        constexpr uint16_t oriUnit = 1024;
        constexpr uint16_t multiples = oriUnit / (BLOCK_CUBE * sizeof(U));
        params.srcNdStride = params.srcNdStride * multiples;
    }
    return params;
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
__aicore__ inline void FixpipeL0C2L1Impl(__cbuf__ T* dst, __cc__ T* src, const FixpipeParamsM300& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        false,
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype in Fixpipe, when src position is L0C Buffer(CO1) and dst position is "
            "L1 Buffer(A1), "
            "support dtype combinations are src: float, dst: int8_t / uint8_t / half / bfloat16_t; src: int32_t, dst: "
            "int8_t / uint8_t / half\n"));
}

template <typename T, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(
    __cbuf__ T* dst, __cc__ T* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsM300& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        false,
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype in Fixpipe, when src position is L0C Buffer(CO1) and dst position is "
            "L1 Buffer(A1), "
            "support dtype combinations are src: float, dst: int8_t / uint8_t / half / bfloat16_t; src: int32_t, dst: "
            "int8_t / uint8_t / half\n"));
}

// contains loop info and cal n size for each loop
// move data L0C->UB
template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0cToUB(
    __ubuf__ T* dst, __cc__ U* src, const FixpipeParamsM300& intriParams, const FixpipeTilingV220& fixpipeTiling,
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
    // LOC -> UB only n direction need tiling, m no need tiling
    return copy_matrix_cc_to_ub(
        (__ubuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
        intriParams.dstStride, intriParams.srcStride, 0, intriParams.unitFlag, intriParams.quantPre,
        static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn, false);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(__ubuf__ T* dst, __cc__ U* src, const FixpipeParamsM300& intriParams)
{
    const FixpipeParamsM300& params = AdaptFixpipeParams<T, U, config>(intriParams);
    CheckFixpipeL0C2UBParam<T, U, config>(dst, src, params);
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        SetLoop3Para(params);
    }
    /*
    make code for scalar quant mode:
    1. copy deq scalar u64 immediate
    2. code gen: move data from l0c to ub
    */
    if (IsScalarQuantMode(params.quantPre)) {
        SetQuantPreImpl(params.deqScalar);
    }
    PipeBarrier<PIPE_FIX>();
    // LOC -> UB
    FixpipeTilingV220 fixpipeTiling;
    FixpipeL0cToUB<T, U, config>(dst, src, params, fixpipeTiling, params.nSize, 0);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(
    __ubuf__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsM300& intriParams)
{
    const FixpipeParamsM300& params = AdaptFixpipeParams<T, U, config>(intriParams);
    CheckFixpipeL0C2UBParam<T, U, config>(dst, src, params);
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        SetLoop3Para(params);
    }
    /*
    make code for vector quant mode:
    1. generate tiling
    2. copy deq tensor from l1 to fb0
    3. code gen: move data from l0c to ub
    */
    FixpipeTilingV220 fixpipeTiling = GenFixpipeTilingV220(params.nSize);
    if (IsVectorQuantMode(params.quantPre)) {
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2UBImplN<T, U, config>(dst, src, cbufWorkspace, params, fixpipeTiling, fixpipeTiling.nSize, i);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb0
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2UBImplN<T, U, config>(
                dst, src, cbufWorkspace, params, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum);
        }
        return;
    }
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImplN(
    __ubuf__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsM300& intriParams,
    const FixpipeTilingV220& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex)
{
    // mov deq tensor from L1 to FB
    CopyDeqTensorToFbuf(cbufWorkspace, fixpipeTiling, calNSize, nIterIndex);
    PipeBarrier<PIPE_FIX>();
    // L0C->UB
    FixpipeL0cToUB<T, U, config>(dst, src, intriParams, fixpipeTiling, calNSize, nIterIndex);
}

// contains loop info and cal n size for each loop
// move data L0C->L1
template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0cToL1(
    __cbuf__ T* dst, __cc__ U* src, const FixpipeParamsM300& intriParams, const FixpipeTilingV220& fixpipeTiling,
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
    // LOC -> L1 only n direction need tiling, m no need tiling
    return copy_matrix_cc_to_cbuf(
        (__cbuf__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
        intriParams.dstStride, intriParams.srcStride, 0, intriParams.unitFlag, intriParams.quantPre,
        static_cast<uint8_t>(intriParams.reluEn), false, nz2ndEn, false);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(__cbuf__ T* dst, __cc__ U* src, const FixpipeParamsM300& intriParams)
{
    const FixpipeParamsM300& params = AdaptFixpipeParams<T, U, config>(intriParams);
    CheckFixpipeL0C2L1Param<T, U, config>(dst, src, intriParams);
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        SetLoop3Para(params);
    }
    /*
    make code for scalar quant mode:
    1. copy deq scalar u64 immediate
    2. code gen: move data from l0c to l1
    */
    if (IsScalarQuantMode(params.quantPre)) {
        // deq factor of uint64 bits describe: bits[31:13] is deq value of fp32,
        SetQuantPreImpl(params.deqScalar);
    }
    PipeBarrier<PIPE_FIX>();
    FixpipeTilingV220 fixpipeTiling;
    // LOC -> L1
    FixpipeL0cToL1<T, U, config>(dst, src, params, fixpipeTiling, params.nSize);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(
    __cbuf__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsM300& intriParams)
{
    const FixpipeParamsM300& params = AdaptFixpipeParams<T, U, config>(intriParams);
    CheckFixpipeL0C2L1Param<T, U, config>(dst, src, params);
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        SetLoop3Para(params);
    }
    /*
    make code for vector quant mode:
    1. generate tiling
    2. copy deq tensor from l1 to fb0
    3. code gen: move data from l0c to l1
    */
    FixpipeTilingV220 fixpipeTiling = GenFixpipeTilingV220(params.nSize);
    if (IsVectorQuantMode(params.quantPre)) {
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2L1ImplN<T, U, config>(dst, src, cbufWorkspace, params, fixpipeTiling, fixpipeTiling.nSize, i);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb0
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2L1ImplN<T, U, config>(
                dst, src, cbufWorkspace, params, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum);
        }
        return;
    }
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1ImplN(
    __cbuf__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsM300& intriParams,
    const FixpipeTilingV220& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex)
{
    // mov deq tensor from L1 to FB
    CopyDeqTensorToFbuf(cbufWorkspace, fixpipeTiling, calNSize, nIterIndex);
    PipeBarrier<PIPE_FIX>();
    // L0C->L1
    FixpipeL0cToL1<T, U, config>(dst, src, intriParams, fixpipeTiling, calNSize, nIterIndex);
}

template <typename T>
__aicore__ inline uint64_t GetGMLength(
    const FixpipeParamsM300& intriParams, const uint16_t& calNSize, const bool& nz2ndEn)
{
    const uint16_t dstEleSize = sizeof(T);
    constexpr uint16_t dstStrideUnit = 32;
    constexpr uint16_t fractalNsize = 16;
    uint64_t cburstNum = calNSize / fractalNsize;
    uint64_t gmLen;
    gmLen = (cburstNum - 1) * intriParams.dstStride * dstStrideUnit + intriParams.mSize * fractalNsize * dstEleSize;
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
    __gm__ T* dst, __cc__ U* src, const FixpipeParamsM300& intriParams, const FixpipeTilingV220& fixpipeTiling,
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
        uint64_t gmLen = GetGMLength<T>(intriParams, calNSize, nz2ndEn);
        AscendCUtils::CheckGmMemOverflow((__gm__ T*)(dst + dstOffset), false, gmLen); // isSrc is false
    }
    return copy_matrix_cc_to_gm(
        (__gm__ T*)(dst + dstOffset), (__cc__ U*)(src + srcOffset), 0, calNSize, intriParams.mSize,
        intriParams.dstStride, intriParams.srcStride, 0, intriParams.unitFlag, intriParams.quantPre,
        static_cast<uint8_t>(intriParams.reluEn), intriParams.isChannelSplit, nz2ndEn, false);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImpl(__gm__ T* dst, __cc__ U* src, const FixpipeParamsM300& intriParams)
{
    const FixpipeParamsM300& params = AdaptFixpipeParams<T, U, config>(intriParams);
    CheckFixpipeL0C2GMParam<T, U, config>(dst, src, params);
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        SetLoop3Para(params);
    }
    /*
    make code for scalar quant mode:
    1. copy deq scalar u64 immediate
    2. code gen: move data from l0c to gm
    */
    if (IsScalarQuantMode(params.quantPre)) {
        SetQuantPreImpl(params.deqScalar);
    }
    PipeBarrier<PIPE_FIX>();
    // LOC -> GM
    FixpipeTilingV220 fixpipeTiling;
    FixpipeL0cToOut<T, U, config>(dst, src, params, fixpipeTiling, params.nSize);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImpl(
    __gm__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsM300& intriParams)
{
    const FixpipeParamsM300& params = AdaptFixpipeParams<T, U, config>(intriParams);
    CheckFixpipeL0C2GMParam<T, U, config>(dst, src, params);
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        SetLoop3Para(params);
    }
    /*
    make code for vector quant mode:
    1. generate tiling
    2. copy deq tensor from gm to fb0 (l1 -> fb0)
    3. code gen: move data from l0c to gm
    */
    FixpipeTilingV220 fixpipeTiling = GenFixpipeTilingV220(params.nSize);
    if (IsVectorQuantMode(params.quantPre)) {
        for (uint16_t i = 0; i < fixpipeTiling.nIterNum; ++i) {
            FixpipeL0C2GMImplN<T, U, config>(dst, src, cbufWorkspace, params, fixpipeTiling, fixpipeTiling.nSize, i);
        }
        // deal with the tail, it also need copy deq/relu tensor from L1 to fb0
        if (fixpipeTiling.tailNSize > 0) {
            FixpipeL0C2GMImplN<T, U, config>(
                dst, src, cbufWorkspace, params, fixpipeTiling, fixpipeTiling.tailNSize, fixpipeTiling.nIterNum);
        }
        return;
    }
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImplN(
    __gm__ T* dst, __cc__ U* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsM300& intriParams,
    const FixpipeTilingV220& fixpipeTiling, uint16_t calNSize, uint16_t nIterIndex)
{
    // mov deq tensor from L1 to FB
    CopyDeqTensorToFbuf(cbufWorkspace, fixpipeTiling, calNSize, nIterIndex);
    PipeBarrier<PIPE_FIX>();
    // L0C->GM
    FixpipeL0cToOut<T, U, config>(dst, src, intriParams, fixpipeTiling, calNSize, nIterIndex);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_FIXPIPE_V2_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_V2_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_V2_IMPL_H__
#endif
