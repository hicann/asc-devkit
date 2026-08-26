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
 * \file kernel_operator_macro_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_5161/kernel_operator_macro_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MACRO_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MACRO_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "../../../include/basic_api/kernel_operator_macro_bitmode_intf.h"

namespace AscendC {
__aicore__ inline void SetConvKernelCal(const ConvKernelBitModeParams& params) { set_kernel(params.GetConfig0()); }

__aicore__ inline void SetConvKernelCal(const ConvKernelParams& params)
{
    constexpr uint8_t POS_STRIDE_W = 0;
    constexpr uint8_t POS_STRIDE_H = 6;
    constexpr uint8_t POS_FILTER_W = 12;
    constexpr uint8_t POS_FILTER_H = 21;
    constexpr uint8_t POS_DILATION_W = 30;
    constexpr uint8_t POS_DILATION_H = 38;
    constexpr uint8_t POS_KERNEL_NUM = 46;

    uint64_t kernel = 0;
    kernel |= (uint64_t)(params.wStride) << POS_STRIDE_W;
    kernel |= (uint64_t)(params.hStride) << POS_STRIDE_H;
    kernel |= (uint64_t)(params.wKernel) << POS_FILTER_W;
    kernel |= (uint64_t)(params.hKernel) << POS_FILTER_H;
    kernel |= (uint64_t)(params.wDilation) << POS_DILATION_W;
    kernel |= (uint64_t)(params.hDilation) << POS_DILATION_H;
    kernel |= (uint64_t)(params.kernelNum) << POS_KERNEL_NUM;

    set_kernel(kernel);
}

__aicore__ inline void SetConvFMatrixCal(const ConvFMatrixBitModeParams& params) { set_m_fmatrix(params.GetConfig0()); }

__aicore__ inline void SetConvFMatrixCal(const ConvFMatrixParams& params)
{
    constexpr uint8_t POS_FM_W = 0;
    constexpr uint8_t POS_FM_H = 16;
    constexpr uint8_t POS_LFET_PAD = 32;
    constexpr uint8_t POS_RIGHT_PAD = 40;
    constexpr uint8_t POS_TOP_PAD = 48;
    constexpr uint8_t POS_BOT_PAD = 56;

    uint64_t fmatrix = 0;
    fmatrix |= (uint64_t)(params.wFmap) << POS_FM_W;
    fmatrix |= (uint64_t)(params.hFmap) << POS_FM_H;
    fmatrix |= (uint64_t)(params.leftPaddingSize) << POS_LFET_PAD;
    fmatrix |= (uint64_t)(params.rightPaddingSize) << POS_RIGHT_PAD;
    fmatrix |= (uint64_t)(params.topPaddingSize) << POS_TOP_PAD;
    fmatrix |= (uint64_t)(params.bottomPaddingSize) << POS_BOT_PAD;

    set_m_fmatrix(fmatrix);
}

__aicore__ inline void SetConvFMatrixStepPosCal(const ConvFMatrixStepPosBitModeParams& params)
{
    set_fm_step_pos(params.GetConfig0());
}

__aicore__ inline void SetConvFMatrixStepPosCal(const ConvFMatrixStepPosParams& params)
{
    constexpr uint8_t POS_W_START_POS = 0;
    constexpr uint8_t POS_H_START_POS = 16;
    constexpr uint8_t POS_W_STEP = 32;
    constexpr uint8_t POS_H_STEP = 48;

    uint64_t stepPos = 0;
    stepPos |= (uint64_t)(params.wStartPos) << POS_W_START_POS;
    stepPos |= (uint64_t)(params.hStartPos) << POS_H_START_POS;
    stepPos |= (uint64_t)(params.wStep) << POS_W_STEP;
    stepPos |= (uint64_t)(params.hStep) << POS_H_STEP;

    set_fm_step_pos(stepPos);
}

__aicore__ inline void SetConvFMatrixDual0Cal(const ConvFMatrixDual0BitModeParams& params)
{
    set_m_fmatrix_dual_0(params.GetConfig0());
}

__aicore__ inline void SetConvFMatrixDual0Cal(const ConvFMatrixDual0Params& params)
{
    constexpr uint8_t POS_SECOND_FM_ADDR = 0;
    constexpr uint8_t POS_SECOND_FM_H = 32;

    uint64_t dual0 = 0;
    dual0 |= (uint64_t)(params.secondAddr) << POS_SECOND_FM_ADDR;
    dual0 |= (uint64_t)(params.secondH) << POS_SECOND_FM_H;

    set_m_fmatrix_dual_0(dual0);
}

__aicore__ inline void SetConvFMatrixDual1Cal(const ConvFMatrixDual1BitModeParams& params)
{
    set_m_fmatrix_dual_1(params.GetConfig0());
}

__aicore__ inline void SetConvFMatrixDual1Cal(const ConvFMatrixDual1Params& params)
{
    constexpr uint8_t POS_FIRST_HWC0STRIDE = 0;
    constexpr uint8_t POS_SECOND_WC0STRIDE = 16;

    uint64_t dual1 = 0;
    dual1 |= (uint64_t)(params.firstStride) << POS_FIRST_HWC0STRIDE;
    dual1 |= (uint64_t)(params.secondStride) << POS_SECOND_WC0STRIDE;

    set_m_fmatrix_dual_1(dual1);
}

template <typename T>
__aicore__ inline void SetConvPaddingCal(const ConvPaddingBitModeParams<T>& params)
{
    set_m_padding(params.GetConfig0());
}

template <typename T>
__aicore__ inline void SetConvPaddingCal(const ConvPaddingParams<T>& params)
{
    constexpr uint8_t POS_PADDING_VALUE = 0;
    constexpr uint8_t POS_PADDING_CONTROL_MODE = 32;
    constexpr uint8_t POS_PADDING_VALUE_ADDR = 34;

    union {
        T t;
        uint16_t u;
    } cvt;
    cvt.t = params.paddingValue;

    uint64_t padding = 0;
    padding |= (uint64_t)(cvt.u) << POS_PADDING_VALUE;
    padding |= (uint64_t)(params.paddingMode) << POS_PADDING_CONTROL_MODE;
    padding |= (uint64_t)(params.paddingAddr) << POS_PADDING_VALUE_ADDR;

    set_m_padding(padding);
}

__aicore__ inline void SetFixPipeConfigCal(const FixPipeConfigBitModeParams& params) { set_m_fpc(params.GetConfig0()); }

__aicore__ inline void SetFixPipeConfigCal(const FixPipeConfigParams& params)
{
    constexpr uint8_t POS_RELU_PRE_ADDR = 0;
    constexpr uint8_t POS_QUANT_PRE_ADDR = 8;
    constexpr uint8_t POS_QUANT_POST_ADDR = 24;
    constexpr uint8_t POS_ANTIQ_PARA_ADDR = 32;
    constexpr uint8_t POS_FIXED_SHIFT_VAL = 40;
    constexpr uint8_t POS_RESTORATION_CTRL = 63;

    uint64_t fpc = 0;
    fpc |= (uint64_t)(params.preReluAddr) << POS_RELU_PRE_ADDR;
    fpc |= (uint64_t)(params.preQuantAddr) << POS_QUANT_PRE_ADDR;
    fpc |= (uint64_t)(params.postQuantAddr) << POS_QUANT_POST_ADDR;
    fpc |= (uint64_t)(params.antiquantAddr) << POS_ANTIQ_PARA_ADDR;
    fpc |= (uint64_t)(params.shiftValue) << POS_FIXED_SHIFT_VAL;
    fpc |= (uint64_t)(params.restorationCtrl) << POS_RESTORATION_CTRL;

    set_m_fpc(fpc);
}

__aicore__ inline void SetFixPipeReluAlphaCal(const FixPipeReluAlphaBitModeParams& params)
{
    set_m_relu_alpha(params.GetConfig0());
}

__aicore__ inline void SetFixPipeReluAlphaCal(const FixPipeReluAlphaParams& params)
{
    constexpr uint8_t POS_LUT = 0;
    constexpr uint8_t POS_PRE_VALUE = 13;

    uint64_t reluAlpha = 0;
    reluAlpha |= (uint64_t)(static_cast<uint8_t>(params.lut)) << POS_LUT;
    reluAlpha |= (uint64_t)(params.preValue) << POS_PRE_VALUE;

    set_m_relu_alpha(reluAlpha);
}

__aicore__ inline void SetFixpipePostQuantFlagCal(uint64_t config) { set_m_quant_post(config); }

__aicore__ inline void SetFixpipeEltAntiqCal(uint64_t config) { set_m_elt_antiq_para(config); }

__aicore__ inline void SetFixPipeAddrCal(const FixPipeAddrBitModeParams& params)
{
    set_m_elt_src_para(params.GetConfig0());
}

__aicore__ inline void SetFixPipeAddrCal(const FixPipeAddrParams& params)
{
    constexpr uint8_t POS_C0_INDICATOR = 0;
    constexpr uint8_t POS_ELT_SRC_ADDR = 16;
    constexpr uint8_t POS_SRC_STRIDE1 = 32;
    constexpr uint8_t POS_SRC_STRIDE2 = 48;

    uint64_t eltSrcPara = 0;
    eltSrcPara |= (uint64_t)(params.c0Indicator ? 1 : 0) << POS_C0_INDICATOR;
    eltSrcPara |= (uint64_t)(params.eltSrcAddr) << POS_ELT_SRC_ADDR;
    eltSrcPara |= (uint64_t)(params.srcStride1) << POS_SRC_STRIDE1;
    eltSrcPara |= (uint64_t)(params.srcStride2) << POS_SRC_STRIDE2;

    set_m_elt_src_para(eltSrcPara);
}

__aicore__ inline void SetConvStrideParaCal(const ConvStrideBitModeParams& params)
{
    set_cube_stride_para(params.GetConfig0());
}

__aicore__ inline void SetConvStrideParaCal(const ConvStrideParams& params)
{
    constexpr uint8_t POS_LOOP3_DST_STRIDE = 0;
    constexpr uint8_t POS_LOOP4_DST_STRIDE = 16;

    uint64_t stridePara = 0;
    stridePara |= (uint64_t)(params.loop3DstStride) << POS_LOOP3_DST_STRIDE;
    stridePara |= (uint64_t)(params.loop4DstStride) << POS_LOOP4_DST_STRIDE;

    set_cube_stride_para(stridePara);
}

__aicore__ inline void SetMatrixParaCal(const MatrixParaBitModeParams& params) { set_matrix_para(params.GetConfig0()); }

__aicore__ inline void SetMatrixParaCal(const MatrixParaParams& params)
{
    constexpr uint8_t POS_M = 0;
    constexpr uint8_t POS_K = 16;
    constexpr uint8_t POS_GROUP_K = 32;

    uint64_t matrixPara = 0;
    matrixPara |= (uint64_t)(params.m) << POS_M;
    matrixPara |= (uint64_t)(params.k) << POS_K;
    matrixPara |= (uint64_t)(params.groupK) << POS_GROUP_K;

    set_matrix_para(matrixPara);
}

__aicore__ inline void SetMmadStrideParaCal(const MmadStrideParaBitModeParams& params)
{
    set_cube_stride_para(params.GetConfig0());
}

__aicore__ inline void SetMmadStrideParaCal(const MmadStrideParaParams& params)
{
    constexpr uint8_t POS_LOOP4_SRC_STRIDE = 0;
    constexpr uint8_t POS_LOOP4_DST_STRIDE = 16;

    uint64_t stridePara = 0;
    stridePara |= (uint64_t)(params.loop4SrcStride) << POS_LOOP4_SRC_STRIDE;
    stridePara |= (uint64_t)(params.loop4DstStride) << POS_LOOP4_DST_STRIDE;

    set_cube_stride_para(stridePara);
}

__aicore__ inline void SetItaMaxAddrCal(const ItaMaxAddrBitModeParams& params)
{
    set_ita_max_addr(params.GetConfig0());
}

__aicore__ inline void SetItaMaxAddrCal(const ItaMaxAddrParaParams& params)
{
    constexpr uint8_t POS_HMAX_ADDR = 0;
    constexpr uint8_t POS_TMAX_ADDR = 16;
    constexpr uint8_t POS_DMAX_ADDR = 32;
    constexpr uint8_t POS_M0N_ADDR = 48;

    uint64_t itaMaxAddr = 0;
    itaMaxAddr |= (uint64_t)(params.hmaxAddr) << POS_HMAX_ADDR;
    itaMaxAddr |= (uint64_t)(params.tmaxAddr) << POS_TMAX_ADDR;
    itaMaxAddr |= (uint64_t)(params.dmaxAddr) << POS_DMAX_ADDR;
    itaMaxAddr |= (uint64_t)(params.m0nStride) << POS_M0N_ADDR;

    set_ita_max_addr(itaMaxAddr);
}

__aicore__ inline void SetMatrixMxParaCal(const MatrixMxParaBitModeParams& params)
{
    set_mx_buf_addr(params.GetConfig0());
}

__aicore__ inline void SetMatrixMxParaCal(const MatrixMxParaParams& params)
{
    constexpr uint8_t POS_A_MX_ADDR = 0;
    constexpr uint8_t POS_B_MX_ADDR = 32;

    uint64_t mxPara = 0;
    mxPara |= (uint64_t)(params.aMxAddr) << POS_A_MX_ADDR;
    mxPara |= (uint64_t)(params.bMxAddr) << POS_B_MX_ADDR;

    set_mx_buf_addr(mxPara);
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void ConvFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* fm, __cb__ Src1T* filter, const ConvFixBitModeParams& convFixParams)
{
    convolution(dst, fm, filter, (__cbuf__ int32_t*)get_imm(0), (uint64_t)0, convFixParams.GetConfig0());
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void ConvFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* fm, __cb__ Src1T* filter, const ConvFixParams& convFixParams)
{
    constexpr uint8_t POS_BIAS_MATRIX_INIT = 0;
    constexpr uint8_t POS_BIAS_MATRIX_BRC = 1;
    constexpr uint8_t POS_PRE_QUANT_MODE = 2;
    constexpr uint8_t POS_PRE_RELU_MODE = 7;
    constexpr uint8_t POS_POST_QUANT_MODE = 10;
    constexpr uint8_t POS_POST_RELU_MODE = 15;
    constexpr uint8_t POS_PRE_CLIP_RELU_MODE = 18;
    constexpr uint8_t POS_ELTWISE_OP = 20;
    constexpr uint8_t POS_ELTWISE_ANTIQ_EN = 23;
    constexpr uint8_t POS_ELTWISE_M_BRC = 24;
    constexpr uint8_t POS_MASK_CTRL = 28;
    constexpr uint8_t POS_DUAL_SRC_EN = 34;
    constexpr uint8_t POS_COMPACT_EN = 35;
    constexpr uint8_t POS_DEPEND_EN = 36;
    constexpr uint8_t POS_INSTR_ID = 37;
    constexpr uint8_t POS_POST_PROC_HALF = 39;
    constexpr uint8_t POS_NBRC_BIAS_CTRL = 40;
    constexpr uint8_t POS_CIN = 48;

    uint64_t config0 = 0;
    config0 |= (uint64_t)(convFixParams.biasMatrixInitVal ? 1 : 0) << POS_BIAS_MATRIX_INIT;
    config0 |= (uint64_t)(convFixParams.biasMatrixBroadcast ? 1 : 0) << POS_BIAS_MATRIX_BRC;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.preQuantMode)) << POS_PRE_QUANT_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.preReluMode)) << POS_PRE_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.postQuantMode)) << POS_POST_QUANT_MODE;
    config0 |= (uint64_t)(convFixParams.actPostMode) << POS_POST_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.preClipReluMode)) << POS_PRE_CLIP_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.eltwiseOp)) << POS_ELTWISE_OP;
    config0 |= (uint64_t)(convFixParams.eltwiseAntiqEnable ? 1 : 0) << POS_ELTWISE_ANTIQ_EN;
    config0 |= (uint64_t)(convFixParams.eltwiseBroadcastEnable ? 1 : 0) << POS_ELTWISE_M_BRC;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.lsbMask)) << POS_MASK_CTRL;
    config0 |= (uint64_t)(convFixParams.dualSrcEnable ? 1 : 0) << POS_DUAL_SRC_EN;
    config0 |= (uint64_t)(convFixParams.compactEnable ? 1 : 0) << POS_COMPACT_EN;
    config0 |= (uint64_t)(convFixParams.dependEnable ? 1 : 0) << POS_DEPEND_EN;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.instrId)) << POS_INSTR_ID;
    config0 |= (uint64_t)(convFixParams.postProcHalfEnable ? 1 : 0) << POS_POST_PROC_HALF;
    config0 |= (uint64_t)(convFixParams.nbrcBiasCtrl ? 1 : 0) << POS_NBRC_BIAS_CTRL;
    config0 |= (uint64_t)(convFixParams.cin) << POS_CIN;

    convolution(dst, fm, filter, (__cbuf__ int32_t*)get_imm(0), (uint64_t)0, config0);
}

template <typename DstT, typename Src0T, typename Src1T, typename BiasT>
__aicore__ inline void ConvFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* fm, __cb__ Src1T* filter, uint64_t bias,
    const ConvFixBitModeParams& convFixParams)
{
    if (convFixParams.GetBiasMatrixBroadcast()) {
        convolution(dst, fm, filter, (__cbuf__ int32_t*)get_imm(0), bias, convFixParams.GetConfig0());
    } else {
        convolution(dst, fm, filter, (__cbuf__ int32_t*)bias, (uint64_t)0, convFixParams.GetConfig0());
    }
}

template <typename DstT, typename Src0T, typename Src1T, typename BiasT>
__aicore__ inline void ConvFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* fm, __cb__ Src1T* filter, uint64_t bias, const ConvFixParams& convFixParams)
{
    constexpr uint8_t POS_BIAS_MATRIX_INIT = 0;
    constexpr uint8_t POS_BIAS_MATRIX_BRC = 1;
    constexpr uint8_t POS_PRE_QUANT_MODE = 2;
    constexpr uint8_t POS_PRE_RELU_MODE = 7;
    constexpr uint8_t POS_POST_QUANT_MODE = 10;
    constexpr uint8_t POS_POST_RELU_MODE = 15;
    constexpr uint8_t POS_PRE_CLIP_RELU_MODE = 18;
    constexpr uint8_t POS_ELTWISE_OP = 20;
    constexpr uint8_t POS_ELTWISE_ANTIQ_EN = 23;
    constexpr uint8_t POS_ELTWISE_M_BRC = 24;
    constexpr uint8_t POS_MASK_CTRL = 28;
    constexpr uint8_t POS_DUAL_SRC_EN = 34;
    constexpr uint8_t POS_COMPACT_EN = 35;
    constexpr uint8_t POS_DEPEND_EN = 36;
    constexpr uint8_t POS_INSTR_ID = 37;
    constexpr uint8_t POS_POST_PROC_HALF = 39;
    constexpr uint8_t POS_NBRC_BIAS_CTRL = 40;
    constexpr uint8_t POS_CIN = 48;

    uint64_t config0 = 0;
    config0 |= (uint64_t)(convFixParams.biasMatrixInitVal ? 1 : 0) << POS_BIAS_MATRIX_INIT;
    config0 |= (uint64_t)(convFixParams.biasMatrixBroadcast ? 1 : 0) << POS_BIAS_MATRIX_BRC;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.preQuantMode)) << POS_PRE_QUANT_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.preReluMode)) << POS_PRE_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.postQuantMode)) << POS_POST_QUANT_MODE;
    config0 |= (uint64_t)(convFixParams.actPostMode) << POS_POST_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.preClipReluMode)) << POS_PRE_CLIP_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.eltwiseOp)) << POS_ELTWISE_OP;
    config0 |= (uint64_t)(convFixParams.eltwiseAntiqEnable ? 1 : 0) << POS_ELTWISE_ANTIQ_EN;
    config0 |= (uint64_t)(convFixParams.eltwiseBroadcastEnable ? 1 : 0) << POS_ELTWISE_M_BRC;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.lsbMask)) << POS_MASK_CTRL;
    config0 |= (uint64_t)(convFixParams.dualSrcEnable ? 1 : 0) << POS_DUAL_SRC_EN;
    config0 |= (uint64_t)(convFixParams.compactEnable ? 1 : 0) << POS_COMPACT_EN;
    config0 |= (uint64_t)(convFixParams.dependEnable ? 1 : 0) << POS_DEPEND_EN;
    config0 |= (uint64_t)(static_cast<uint8_t>(convFixParams.instrId)) << POS_INSTR_ID;
    config0 |= (uint64_t)(convFixParams.postProcHalfEnable ? 1 : 0) << POS_POST_PROC_HALF;
    config0 |= (uint64_t)(convFixParams.nbrcBiasCtrl ? 1 : 0) << POS_NBRC_BIAS_CTRL;
    config0 |= (uint64_t)(convFixParams.cin) << POS_CIN;

    if (convFixParams.biasMatrixBroadcast) {
        convolution(dst, fm, filter, (__cbuf__ int32_t*)get_imm(0), bias, config0);
    } else {
        convolution(dst, fm, filter, (__cbuf__ int32_t*)bias, (uint64_t)0, config0);
    }
}

template <typename Src0T, typename Src1T>
__aicore__ inline void matmul(
    __cbuf__ void* dst, __cbuf__ Src0T* left, __cb__ Src1T* right, __cbuf__ int32_t* bias, uint64_t btBias,
    uint64_t config)
{
    if constexpr (
        (IsSameType<Src0T, int8_t>::value && IsSameType<Src1T, int8_t>::value) ||
        (IsSameType<Src0T, int16_t>::value && IsSameType<Src1T, int8_t>::value) ||
        (IsSameType<Src0T, half>::value && IsSameType<Src1T, half>::value) ||
        (IsSameType<Src0T, float8_e4m3_t>::value && IsSameType<Src1T, float8_e4m3_t>::value) ||
        (IsSameType<Src0T, float8_e4m3_t>::value && IsSameType<Src1T, int8_t>::value)) {
        matmul_to_cbuf(dst, left, right, bias, btBias, config);
    } else if constexpr (IsSameType<Src0T, float8_e4m3_t>::value) {
        matmul_to_cbuf_e4m3s4(dst, left, (__cb__ void*)right, bias, btBias, config);
    } else if constexpr (IsSameType<Src0T, int8_t>::value) {
        matmul_to_cbuf_s8s4(dst, left, (__cb__ void*)right, bias, btBias, config);
    } else {
        matmul_to_cbuf_s4s4(dst, (__cbuf__ void*)left, (__cb__ void*)right, bias, btBias, config);
    }
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* left, __cb__ Src1T* right, const MmadFixBitModeParams& mmadFixParams)
{
    matmul(dst, left, right, (__cbuf__ int32_t*)get_imm(0), (uint64_t)0, mmadFixParams.GetConfig0());
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* left, __cb__ Src1T* right, const MmadFixParams& mmadFixParams)
{
    constexpr uint8_t POS_BIAS_MATRIX_INITVAL = 0;
    constexpr uint8_t POS_BIAS_MATRIX_BROADCAST = 1;
    constexpr uint8_t POS_PRE_QUANT_MODE = 2;
    constexpr uint8_t POS_PRE_RELU_MODE = 7;
    constexpr uint8_t POS_POST_QUANT_MODE = 10;
    constexpr uint8_t POS_ACT_POST_ENABLE = 15;
    constexpr uint8_t POS_PRE_CLIP_RELU_MODE = 18;
    constexpr uint8_t POS_ELTWISE_OP = 20;
    constexpr uint8_t POS_ELTWISE_ANTIQ_ENABLE = 23;
    constexpr uint8_t POS_ELTWISE_BROADCAST_ENABLE = 24;
    constexpr uint8_t POS_ITA_ENABLE = 25;
    constexpr uint8_t POS_BIAS_S32_ENABLE = 26;
    constexpr uint8_t POS_HMAX_ENABLE = 27;
    constexpr uint8_t POS_LSB_MASK = 28;
    constexpr uint8_t POS_GEMV_CTRL = 31;
    constexpr uint8_t POS_GROUP_MODE = 32;
    constexpr uint8_t POS_DEPEND_ENABLE = 36;
    constexpr uint8_t POS_INSTRI_D = 37;
    constexpr uint8_t POS_POST_PROC_HALF_ENABLE = 39;
    constexpr uint8_t POS_LAYOUT_CTRL = 40;
    constexpr uint8_t POS_N = 48;

    uint64_t config0 = 0;
    config0 |= (uint64_t)(mmadFixParams.biasMatrixInitVal ? 1 : 0) << POS_BIAS_MATRIX_INITVAL;
    config0 |= (uint64_t)(mmadFixParams.biasMatrixBroadcast ? 1 : 0) << POS_BIAS_MATRIX_BROADCAST;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preQuantMode)) << POS_PRE_QUANT_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preReluMode)) << POS_PRE_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.postQuantMode)) << POS_POST_QUANT_MODE;
    config0 |= (uint64_t)(mmadFixParams.actPostMode) << POS_ACT_POST_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preClipReluMode)) << POS_PRE_CLIP_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.eltwiseOp)) << POS_ELTWISE_OP;
    config0 |= (uint64_t)(mmadFixParams.eltwiseAntiqEnable ? 1 : 0) << POS_ELTWISE_ANTIQ_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.eltwiseBroadcastEnable ? 1 : 0) << POS_ELTWISE_BROADCAST_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.itaEnable ? 1 : 0) << POS_ITA_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.biasS32Enable ? 1 : 0) << POS_BIAS_S32_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.hmaxEnable ? 1 : 0) << POS_HMAX_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.lsbMask)) << POS_LSB_MASK;
    config0 |= (uint64_t)(mmadFixParams.gemvCtrl ? 1 : 0) << POS_GEMV_CTRL;
    config0 |= (uint64_t)(mmadFixParams.groupMode) << POS_GROUP_MODE;
    config0 |= (uint64_t)(mmadFixParams.dependEnable ? 1 : 0) << POS_DEPEND_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.instrId)) << POS_INSTRI_D;
    config0 |= (uint64_t)(mmadFixParams.postProcHalfEnable ? 1 : 0) << POS_POST_PROC_HALF_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.layoutCtrl ? 1 : 0) << POS_LAYOUT_CTRL;
    config0 |= (uint64_t)(mmadFixParams.n) << POS_N;

    matmul(dst, left, right, (__cbuf__ int32_t*)get_imm(0), (uint64_t)0, config0);
}

template <typename DstT, typename Src0T, typename Src1T, typename BiasT>
__aicore__ inline void MmadFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* left, __cb__ Src1T* right, uint64_t bias,
    const MmadFixBitModeParams& mmadFixParams)
{
    if (mmadFixParams.GetBiasMatrixBroadcast()) {
        matmul(dst, left, right, (__cbuf__ int32_t*)get_imm(0), bias, mmadFixParams.GetConfig0());
    } else {
        matmul(dst, left, right, (__cbuf__ int32_t*)bias, (uint64_t)0, mmadFixParams.GetConfig0());
    }
}

template <typename DstT, typename Src0T, typename Src1T, typename BiasT>
__aicore__ inline void MmadFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* left, __cb__ Src1T* right, uint64_t bias, const MmadFixParams& mmadFixParams)
{
    constexpr uint8_t POS_BIAS_MATRIX_INITVAL = 0;
    constexpr uint8_t POS_BIAS_MATRIX_BROADCAST = 1;
    constexpr uint8_t POS_PRE_QUANT_MODE = 2;
    constexpr uint8_t POS_PRE_RELU_MODE = 7;
    constexpr uint8_t POS_POST_QUANT_MODE = 10;
    constexpr uint8_t POS_ACT_POST_ENABLE = 15;
    constexpr uint8_t POS_PRE_CLIP_RELU_MODE = 18;
    constexpr uint8_t POS_ELTWISE_OP = 20;
    constexpr uint8_t POS_ELTWISE_ANTIQ_ENABLE = 23;
    constexpr uint8_t POS_ELTWISE_BROADCAST_ENABLE = 24;
    constexpr uint8_t POS_ITA_ENABLE = 25;
    constexpr uint8_t POS_BIAS_S32_ENABLE = 26;
    constexpr uint8_t POS_HMAX_ENABLE = 27;
    constexpr uint8_t POS_LSB_MASK = 28;
    constexpr uint8_t POS_GEMV_CTRL = 31;
    constexpr uint8_t POS_GROUP_MODE = 32;
    constexpr uint8_t POS_DEPEND_ENABLE = 36;
    constexpr uint8_t POS_INSTRI_D = 37;
    constexpr uint8_t POS_POST_PROC_HALF_ENABLE = 39;
    constexpr uint8_t POS_LAYOUT_CTRL = 40;
    constexpr uint8_t POS_N = 48;

    uint64_t config0 = 0;
    config0 |= (uint64_t)(mmadFixParams.biasMatrixInitVal ? 1 : 0) << POS_BIAS_MATRIX_INITVAL;
    config0 |= (uint64_t)(mmadFixParams.biasMatrixBroadcast ? 1 : 0) << POS_BIAS_MATRIX_BROADCAST;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preQuantMode)) << POS_PRE_QUANT_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preReluMode)) << POS_PRE_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.postQuantMode)) << POS_POST_QUANT_MODE;
    config0 |= (uint64_t)(mmadFixParams.actPostMode) << POS_ACT_POST_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preClipReluMode)) << POS_PRE_CLIP_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.eltwiseOp)) << POS_ELTWISE_OP;
    config0 |= (uint64_t)(mmadFixParams.eltwiseAntiqEnable ? 1 : 0) << POS_ELTWISE_ANTIQ_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.eltwiseBroadcastEnable ? 1 : 0) << POS_ELTWISE_BROADCAST_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.itaEnable ? 1 : 0) << POS_ITA_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.biasS32Enable ? 1 : 0) << POS_BIAS_S32_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.hmaxEnable ? 1 : 0) << POS_HMAX_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.lsbMask)) << POS_LSB_MASK;
    config0 |= (uint64_t)(mmadFixParams.gemvCtrl ? 1 : 0) << POS_GEMV_CTRL;
    config0 |= (uint64_t)(mmadFixParams.groupMode) << POS_GROUP_MODE;
    config0 |= (uint64_t)(mmadFixParams.dependEnable ? 1 : 0) << POS_DEPEND_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.instrId)) << POS_INSTRI_D;
    config0 |= (uint64_t)(mmadFixParams.postProcHalfEnable ? 1 : 0) << POS_POST_PROC_HALF_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.layoutCtrl ? 1 : 0) << POS_LAYOUT_CTRL;
    config0 |= (uint64_t)(mmadFixParams.n) << POS_N;

    if (mmadFixParams.biasMatrixBroadcast) {
        matmul(dst, left, right, (__cbuf__ int32_t*)get_imm(0), bias, config0);
    } else {
        matmul(dst, left, right, (__cbuf__ int32_t*)bias, (uint64_t)0, config0);
    }
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadMxFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* left, __cb__ Src1T* right, const MmadFixBitModeParams& mmadFixParams)
{
    matmul_mx_to_cbuf(dst, left, right, (__cbuf__ int32_t*)get_imm(0), (uint64_t)0, mmadFixParams.GetConfig0());
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadMxFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* left, __cb__ Src1T* right, const MmadFixParams& mmadFixParams)
{
    constexpr uint8_t POS_BIAS_MATRIX_INITVAL = 0;
    constexpr uint8_t POS_BIAS_MATRIX_BROADCAST = 1;
    constexpr uint8_t POS_PRE_QUANT_MODE = 2;
    constexpr uint8_t POS_PRE_RELU_MODE = 7;
    constexpr uint8_t POS_POST_QUANT_MODE = 10;
    constexpr uint8_t POS_ACT_POST_ENABLE = 15;
    constexpr uint8_t POS_PRE_CLIP_RELU_MODE = 18;
    constexpr uint8_t POS_ELTWISE_OP = 20;
    constexpr uint8_t POS_ELTWISE_ANTIQ_ENABLE = 23;
    constexpr uint8_t POS_ELTWISE_BROADCAST_ENABLE = 24;
    constexpr uint8_t POS_ITA_ENABLE = 25;
    constexpr uint8_t POS_BIAS_S32_ENABLE = 26;
    constexpr uint8_t POS_HMAX_ENABLE = 27;
    constexpr uint8_t POS_LSB_MASK = 28;
    constexpr uint8_t POS_GEMV_CTRL = 31;
    constexpr uint8_t POS_GROUP_MODE = 32;
    constexpr uint8_t POS_DEPEND_ENABLE = 36;
    constexpr uint8_t POS_INSTRI_D = 37;
    constexpr uint8_t POS_POST_PROC_HALF_ENABLE = 39;
    constexpr uint8_t POS_LAYOUT_CTRL = 40;
    constexpr uint8_t POS_N = 48;

    uint64_t config0 = 0;
    config0 |= (uint64_t)(mmadFixParams.biasMatrixInitVal ? 1 : 0) << POS_BIAS_MATRIX_INITVAL;
    config0 |= (uint64_t)(mmadFixParams.biasMatrixBroadcast ? 1 : 0) << POS_BIAS_MATRIX_BROADCAST;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preQuantMode)) << POS_PRE_QUANT_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preReluMode)) << POS_PRE_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.postQuantMode)) << POS_POST_QUANT_MODE;
    config0 |= (uint64_t)(mmadFixParams.actPostMode) << POS_ACT_POST_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preClipReluMode)) << POS_PRE_CLIP_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.eltwiseOp)) << POS_ELTWISE_OP;
    config0 |= (uint64_t)(mmadFixParams.eltwiseAntiqEnable ? 1 : 0) << POS_ELTWISE_ANTIQ_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.eltwiseBroadcastEnable ? 1 : 0) << POS_ELTWISE_BROADCAST_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.itaEnable ? 1 : 0) << POS_ITA_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.biasS32Enable ? 1 : 0) << POS_BIAS_S32_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.hmaxEnable ? 1 : 0) << POS_HMAX_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.lsbMask)) << POS_LSB_MASK;
    config0 |= (uint64_t)(mmadFixParams.gemvCtrl ? 1 : 0) << POS_GEMV_CTRL;
    config0 |= (uint64_t)(mmadFixParams.groupMode) << POS_GROUP_MODE;
    config0 |= (uint64_t)(mmadFixParams.dependEnable ? 1 : 0) << POS_DEPEND_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.instrId)) << POS_INSTRI_D;
    config0 |= (uint64_t)(mmadFixParams.postProcHalfEnable ? 1 : 0) << POS_POST_PROC_HALF_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.layoutCtrl ? 1 : 0) << POS_LAYOUT_CTRL;
    config0 |= (uint64_t)(mmadFixParams.n) << POS_N;

    matmul_mx_to_cbuf(dst, left, right, (__cbuf__ int32_t*)get_imm(0), (uint64_t)0, config0);
}

template <typename DstT, typename Src0T, typename Src1T, typename BiasT>
__aicore__ inline void MmadMxFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* left, __cb__ Src1T* right, uint64_t bias,
    const MmadFixBitModeParams& mmadFixParams)
{
    if (mmadFixParams.GetBiasMatrixBroadcast()) {
        matmul_mx_to_cbuf(dst, left, right, (__cbuf__ int32_t*)get_imm(0), bias, mmadFixParams.GetConfig0());
    } else {
        matmul_mx_to_cbuf(dst, left, right, (__cbuf__ int32_t*)bias, (uint64_t)0, mmadFixParams.GetConfig0());
    }
}

template <typename DstT, typename Src0T, typename Src1T, typename BiasT>
__aicore__ inline void MmadMxFixCal(
    __cbuf__ DstT* dst, __cbuf__ Src0T* left, __cb__ Src1T* right, uint64_t bias, const MmadFixParams& mmadFixParams)
{
    constexpr uint8_t POS_BIAS_MATRIX_INITVAL = 0;
    constexpr uint8_t POS_BIAS_MATRIX_BROADCAST = 1;
    constexpr uint8_t POS_PRE_QUANT_MODE = 2;
    constexpr uint8_t POS_PRE_RELU_MODE = 7;
    constexpr uint8_t POS_POST_QUANT_MODE = 10;
    constexpr uint8_t POS_ACT_POST_ENABLE = 15;
    constexpr uint8_t POS_PRE_CLIP_RELU_MODE = 18;
    constexpr uint8_t POS_ELTWISE_OP = 20;
    constexpr uint8_t POS_ELTWISE_ANTIQ_ENABLE = 23;
    constexpr uint8_t POS_ELTWISE_BROADCAST_ENABLE = 24;
    constexpr uint8_t POS_ITA_ENABLE = 25;
    constexpr uint8_t POS_BIAS_S32_ENABLE = 26;
    constexpr uint8_t POS_HMAX_ENABLE = 27;
    constexpr uint8_t POS_LSB_MASK = 28;
    constexpr uint8_t POS_GEMV_CTRL = 31;
    constexpr uint8_t POS_GROUP_MODE = 32;
    constexpr uint8_t POS_DEPEND_ENABLE = 36;
    constexpr uint8_t POS_INSTRI_D = 37;
    constexpr uint8_t POS_POST_PROC_HALF_ENABLE = 39;
    constexpr uint8_t POS_LAYOUT_CTRL = 40;
    constexpr uint8_t POS_N = 48;

    uint64_t config0 = 0;
    config0 |= (uint64_t)(mmadFixParams.biasMatrixInitVal ? 1 : 0) << POS_BIAS_MATRIX_INITVAL;
    config0 |= (uint64_t)(mmadFixParams.biasMatrixBroadcast ? 1 : 0) << POS_BIAS_MATRIX_BROADCAST;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preQuantMode)) << POS_PRE_QUANT_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preReluMode)) << POS_PRE_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.postQuantMode)) << POS_POST_QUANT_MODE;
    config0 |= (uint64_t)(mmadFixParams.actPostMode) << POS_ACT_POST_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.preClipReluMode)) << POS_PRE_CLIP_RELU_MODE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.eltwiseOp)) << POS_ELTWISE_OP;
    config0 |= (uint64_t)(mmadFixParams.eltwiseAntiqEnable ? 1 : 0) << POS_ELTWISE_ANTIQ_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.eltwiseBroadcastEnable ? 1 : 0) << POS_ELTWISE_BROADCAST_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.itaEnable ? 1 : 0) << POS_ITA_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.biasS32Enable ? 1 : 0) << POS_BIAS_S32_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.hmaxEnable ? 1 : 0) << POS_HMAX_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.lsbMask)) << POS_LSB_MASK;
    config0 |= (uint64_t)(mmadFixParams.gemvCtrl ? 1 : 0) << POS_GEMV_CTRL;
    config0 |= (uint64_t)(mmadFixParams.groupMode) << POS_GROUP_MODE;
    config0 |= (uint64_t)(mmadFixParams.dependEnable ? 1 : 0) << POS_DEPEND_ENABLE;
    config0 |= (uint64_t)(static_cast<uint8_t>(mmadFixParams.instrId)) << POS_INSTRI_D;
    config0 |= (uint64_t)(mmadFixParams.postProcHalfEnable ? 1 : 0) << POS_POST_PROC_HALF_ENABLE;
    config0 |= (uint64_t)(mmadFixParams.layoutCtrl ? 1 : 0) << POS_LAYOUT_CTRL;
    config0 |= (uint64_t)(mmadFixParams.n) << POS_N;

    if (mmadFixParams.biasMatrixBroadcast) {
        matmul_mx_to_cbuf(dst, left, right, (__cbuf__ int32_t*)get_imm(0), bias, config0);
    } else {
        matmul_mx_to_cbuf(dst, left, right, (__cbuf__ int32_t*)bias, (uint64_t)0, config0);
    }
}
} // namespace AscendC

#endif // __NPU_ARCH__
#endif // ASCENDC_MODULE_OPERATOR_MACRO_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_IMPL_H__
#endif
