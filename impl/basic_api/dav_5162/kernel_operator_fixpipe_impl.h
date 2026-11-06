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
 * \file kernel_operator_fixpipe_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_5162/kernel_operator_fixpipe_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H

#include "../../../include/basic_api/kernel_tpipe.h"
#include "../../../include/basic_api/kernel_operator_block_sync_intf.h"
#include "../kernel_process_lock.h"
#include "../../../include/basic_api/kernel_struct_fixpipe.h"
#include "../../../include/basic_api/kernel_struct_macro.h"

namespace AscendC {

__aicore__ inline void SetFixPipeClipReluImpl(uint64_t config) { set_m_clip_relu(config); }

template <typename T1, typename T2>
__aicore__ inline void SetFixPipeClipReluImpl(const FixpipeClipReluParams<T1, T2>& params)
{
    uint64_t config = 0;
    config |= ((uint64_t)params.preValue & 0xffff);
    set_m_clip_relu(config);
}

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
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeConfig");
}

__aicore__ inline void SetFixPipeConfigImpl(uint64_t config) { set_m_fpc(config); }

template <typename T, bool setRelu = false>
__aicore__ inline void SetFixPipeConfigImpl(const LocalTensor<T>& preTensor, bool isUnitFlag = false)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixPipeConfig");
}

__aicore__ inline void SetFixpipeNz2ndFlagImpl(uint16_t ndNum, uint16_t srcNdStride, uint32_t dstNdStride)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFixpipeNz2ndFlag");
}

__aicore__ inline void SetFixpipePreQuantFlagImpl(uint64_t config) { set_m_quant_pre(config); }

template <typename T>
__aicore__ inline void SetFixpipePreQuantFlagImpl(const FixPipePreQuantParams<T>& params)
{
    uint64_t config = 0;
    constexpr uint32_t POS_SHIFT_SCALE = 16;
    constexpr uint32_t POS_MUL_SCALE = 21;
    constexpr uint32_t POS_OFFSET_CFG = 37;
    constexpr uint32_t POS_CLIP_MAX_VAL_PRE = 48;

    config = ((((uint64_t)params.offset0 >> 0x9) & 0x7F) | (((params.offset0 >> 0x1f) & 0x1) << 0x7));
    if (params.isSft) {
        config |= (((uint64_t)params.shiftScale & 0x1F) << POS_SHIFT_SCALE);
        config |= ((params.mulScale) * 0x7FF << POS_MUL_SCALE);
    } else {
        config |= ((uint64_t)GetScalarBitcodeValue(params.scalarValue) & 0xFFFFE000);
    }
    config |= (((uint64_t)params.offset0 & 0x1FF) |
               (((uint64_t)params.offset1 & 0xF) | ((((uint64_t)params.offset1 >> 0x7) & 0x1) << 0x4)) |
               (((uint64_t)params.offset2 & 0xFF) | ((((uint64_t)params.offset2 >> 0xf) & 0x1) << 0x8)))
              << POS_OFFSET_CFG;
    config |= ((uint64_t)GetScalarBitcodeValue(params.clipMaxValPre) & 0xFFFF) << POS_CLIP_MAX_VAL_PRE;
    set_m_quant_pre(config);
}

template <const FixpipeConfig& config>
__aicore__ inline void SetDeqScalarDepOnMode(QuantMode_t mode, uint64_t deqScalar, uint8_t shiftVal)
{
    if (shiftVal == 0) {
        set_m_quant_pre(deqScalar);
    } else if (mode == QuantMode_t::DEQF16) {
        // fix point 1/2^16
        constexpr uint16_t shift = 23;
        constexpr uint64_t mask = 0xFFULL << shift;
        constexpr uint64_t floatOne = 0x3F800000ULL;
        uint64_t newExponent = (127 - shiftVal) & 0xFF;
        uint64_t newScalar = (floatOne & ~mask) | (newExponent << shift);
        set_m_quant_pre(newScalar);
    } else if (mode == QuantMode_t::REQ8) {
        // fix point by modify related  bits
        constexpr uint16_t shift = 23;
        constexpr uint64_t mask = 0xFFULL << shift;
        uint64_t originalBits = (deqScalar & mask) >> shift;
        uint64_t newBits = (originalBits - (uint64_t)shiftVal) & 0xFF;
        uint64_t newScalar = (deqScalar & ~mask) | (newBits << shift);
        set_m_quant_pre(newScalar);
    }
}

__aicore__ inline void SetReluScalarOnMode(QuantMode_t mode, uint64_t reluScalar, uint8_t shiftVal)
{
#if !defined(ASCENDC_CPU_DEBUG)
    if (shiftVal == 0) {
        set_m_relu_alpha(reluScalar);
    } else if (mode == QuantMode_t::DEQF16 || mode == QuantMode_t::REQ8) {
        constexpr uint16_t shift = 23;
        constexpr uint64_t mask = 0xFFULL << shift;
        uint64_t originalBits = (reluScalar & mask) >> shift;
        uint64_t newBits = (originalBits - (uint64_t)shiftVal) & 0xFF;
        uint64_t newScalar = (reluScalar & ~mask) | (newBits << shift);
        set_m_relu_alpha(newScalar);
    }
#endif
}

__aicore__ inline void SetFixpipePostQuantImpl(const FixPipePostQuantParams& params)
{
    uint64_t config = 0;
    constexpr uint32_t POS_OFFSET2_CFG = 32;
    constexpr uint32_t POS_OSHIFT_SCALE_OFFSET = 40;

    config |= (((uint64_t)params.offset0 & 0xF) | (((uint64_t)params.offset0 & 0x80) >> 0x3));
    config |= (((uint64_t)params.offset1 & 0xFF) | (((uint64_t)params.offset1 & 0x8000) >> 0x7));
    config |= ((uint64_t)params.offset2 & 0x1FF);
    config |= ((uint64_t)GetScalarBitcodeValue(params.scalarValue) & 0xFFFFE000);
    config |= ((((uint64_t)params.offset2 >> 0x9) & 0x7F) | ((((uint64_t)params.offset2 >> 0x1F) & 0x1) << 0x7))
              << POS_OFFSET2_CFG;
    config |= ((uint64_t)params.shiftScale & 0x1F) << POS_OSHIFT_SCALE_OFFSET;
    set_m_quant_post(config);
}

template <typename T>
__aicore__ inline void SetFixpipeEltAntiqImpl(const FixPipeEltAntiqParams<T>& params)
{
    uint64_t config = 0;
    constexpr uint32_t POS_MUL_SCALE = 5;
    constexpr uint32_t POS_B8_ANTI_QUANT_OFFSET = 20;
    constexpr uint32_t POS_S16_ANTI_QUANT_OFFSET = 28;
    constexpr uint32_t POS_S16_ANTI_QUANT_CFG = 44;

    config = ((((uint64_t)params.offset0 >> 0x9) & 0x7F) | (((params.offset0 >> 0x1f) & 0x1) << 0x7));
    if (params.isSft) {
        config |= ((params.shiftScale) & 0x1F);
        config |= ((params.mulScale) * 0x7FF << POS_MUL_SCALE);
    } else {
        config |= (uint64_t)GetScalarBitcodeValue(params.scalarValue) & 0xFFFF;
    }
    config |= (((uint64_t)params.s4Offset & 0x8) | ((((uint64_t)params.s4Offset >> 0x7) & 0x1) << 0x3)) << 0x10;
    config |= ((uint64_t)params.b8Offset & 0xFF) << POS_B8_ANTI_QUANT_OFFSET;
    config |= ((uint64_t)params.s16Offset & 0xFFFF) << POS_S16_ANTI_QUANT_OFFSET;
    config |= ((uint64_t)params.eltAntiqCfg & 0xF) << POS_S16_ANTI_QUANT_CFG;

    set_m_elt_antiq_para(config);
}

__aicore__ inline void SetFixpipeEltwiseAddrImpl(uint64_t config) { set_m_elt_src_para(config); }

__aicore__ inline void SetFixpipeEltwiseAddrImpl(const FixpipeEltwiseAddrParams& params)
{
    uint64_t config = 0;
    constexpr uint32_t POS_ELT_SRC_ADDR = 16;
    constexpr uint32_t POS_SRC_STRIDE1 = 32;
    constexpr uint32_t POS_SRC_STRIDE2 = 48;

    config |= ((uint64_t)params.c0Indicator & 0x1);
    config |= ((uint64_t)params.eltSrcAddr & 0xFFFF) << POS_ELT_SRC_ADDR;
    config |= ((uint64_t)params.srcStride1 & 0xFFFF) << POS_SRC_STRIDE1;
    config |= ((uint64_t)params.srcStride2 & 0xFFFF) << POS_SRC_STRIDE2;
    set_m_elt_src_para(config);
}

/* **************************************************************************************************
 * Fixpipe                                             *
 * ************************************************************************************************* */
/* pack nd num and nd stride into FIXP_NZ_PARA for nd2nd/nd2dn trans */
template <const FixpipeConfig& config>
__aicore__ inline void SetFixpipeNzParaImpl(const FixpipeParamsV516& params)
{
    constexpr uint32_t POS_FIXP_NZ_ND_NUM = 0;
    constexpr uint32_t POS_FIXP_NZ_SRC_ND_STRIDE = 16;
    constexpr uint32_t POS_FIXP_NZ_DST_ND_STRIDE = 32;

    if constexpr (config.format != CO2Layout::NZ) {
        uint64_t nzPara = 0;
        nzPara |= (((uint64_t)params.ndNum & 0xffff) << POS_FIXP_NZ_ND_NUM);
        nzPara |= (((uint64_t)params.srcNdStride & 0xffff) << POS_FIXP_NZ_SRC_ND_STRIDE);
        nzPara |= (((uint64_t)params.dstNdStride & 0xffff) << POS_FIXP_NZ_DST_ND_STRIDE);
        set_fixp_nz_para(nzPara);
    }
}

template <const FixpipeConfig& config>
__aicore__ inline fixp_trans_mode_t GetFixpipeTransMode()
{
    if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        return fixp_trans_mode_t::NZ2ND;
    } else if constexpr (config.format == CO2Layout::COLUMN_MAJOR) {
        return fixp_trans_mode_t::NZ2DN;
    } else {
        return fixp_trans_mode_t::NORMAL_DMA;
    }
}

/* fix data L1 to L1 */
template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL1ToL1(
    __cbuf__ DstT* dst, __cbuf__ SrcT* src, const FixpipeParamsV516& params, uint16_t nIterIndex = 0)
{
    SetFixpipeNzParaImpl<config>(params);
    if constexpr (config.format == CO2Layout::NZ) {
        fix_cbuf_to_cbuf(
            (__cbuf__ DstT*)(dst), (__cbuf__ SrcT*)(src), params.mSize, params.nSize, params.dstStride,
            params.srcStride, 0, GetFixpipeTransMode<config>());
    } else {
        uint32_t loop2DstStride = params.dstStride * sizeof(DstT);
        fix_cbuf_to_cbuf(
            (__cbuf__ DstT*)(dst), (__cbuf__ SrcT*)(src), params.nSize, params.mSize, loop2DstStride, params.srcStride,
            0, GetFixpipeTransMode<config>());
    }
}

/* fix data L1 to UB */
template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL1ToUb(
    __ubuf__ DstT* dst, __cbuf__ SrcT* src, const FixpipeParamsV516& params, uint16_t nIterIndex = 0)
{
    SetFixpipeNzParaImpl<config>(params);
    if constexpr (config.format == CO2Layout::NZ) {
        fix_cbuf_to_ubuf(
            (__ubuf__ DstT*)(dst), (__cbuf__ SrcT*)(src), params.mSize, params.nSize, params.dstStride,
            params.srcStride, 0, GetFixpipeTransMode<config>());
    } else {
        uint32_t loop2DstStride = params.dstStride * sizeof(DstT);
        fix_cbuf_to_ubuf(
            (__ubuf__ DstT*)(dst), (__cbuf__ SrcT*)(src), params.nSize, params.mSize, loop2DstStride, params.srcStride,
            0, GetFixpipeTransMode<config>());
    }
}

/* fix data L1 to Out */
template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL1ToOut(
    __gm__ DstT* dst, __cbuf__ SrcT* src, const FixpipeParamsV516& params, uint16_t nIterIndex = 0,
    const uint8_t sid = 5)
{
    SetFixpipeNzParaImpl<config>(params);
    if constexpr (config.format == CO2Layout::NZ) {
        fix_cbuf_to_gm(
            (__gm__ DstT*)(dst), (__cbuf__ SrcT*)(src), params.mSize, params.nSize, params.dstStride, params.srcStride,
            0, GetFixpipeTransMode<config>(), sid);
    } else {
        uint32_t loop2DstStride = params.dstStride * sizeof(DstT);
        fix_cbuf_to_gm(
            (__gm__ DstT*)(dst), (__cbuf__ SrcT*)(src), params.nSize, params.mSize, loop2DstStride, params.srcStride, 0,
            GetFixpipeTransMode<config>(), sid);
    }
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeParams(const FixpipeParamsV516& params)
{
    static_assert((sizeof(DstT) == 2) || (sizeof(DstT) == 1), "only b8 and b16 is valid!");
    static_assert(IsSameType<DstT, SrcT>::value, "src type and dst type mast be same!");
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL12L1Impl(__cbuf__ DstT* dst, __cbuf__ SrcT* src, const FixpipeParamsV516& params)
{
    CheckFixpipeParams<DstT, SrcT, config>(params);
    FixpipeL1ToL1<DstT, SrcT, config>(dst, src, params);
}

/* fix data L1 to UB */
template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL12UbImpl(__ubuf__ DstT* dst, __cbuf__ SrcT* src, const FixpipeParamsV516& params)
{
    CheckFixpipeParams<DstT, SrcT, config>(params);
    FixpipeL1ToUb<DstT, SrcT, config>(dst, src, params);
}

/* fix data L1 to Out */
template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL12GmImpl(
    __gm__ DstT* dst, __cbuf__ SrcT* src, const FixpipeParamsV516& params, const uint8_t cacheMode = 0,
    const uint8_t sid = 5)
{
    CheckFixpipeParams<DstT, SrcT, config>(params);
    FixpipeL1ToOut<DstT, SrcT, config>(dst, src, params, 0, sid);
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(__cbuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsV220& intriParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Fixpipe(L0C->L1) is not supported on current device"); });
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2L1Impl(
    __cbuf__ DstT* dst, __cc__ SrcT* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Fixpipe(L0C->L1) is not supported on current device"); });
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(__ubuf__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsV220& intriParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Fixpipe(L0C->UB) is not supported on current device"); });
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2UBImpl(
    __ubuf__ DstT* dst, __cc__ SrcT* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Fixpipe(L0C->UB) is not supported on current device"); });
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImpl(__gm__ DstT* dst, __cc__ SrcT* src, const FixpipeParamsV220& intriParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Fixpipe(L0C->GM) is not supported on current device"); });
}

template <typename DstT, typename SrcT, const FixpipeConfig& config>
__aicore__ inline void FixpipeL0C2GMImpl(
    __gm__ DstT* dst, __cc__ SrcT* src, __cbuf__ uint64_t* cbufWorkspace, const FixpipeParamsV220& intriParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Fixpipe(L0C->GM) is not supported on current device"); });
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_FIXPIPE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_FIXPIPE_IMPL_H__
#endif
