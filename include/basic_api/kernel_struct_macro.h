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
 * \file kernel_struct_macro.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_struct_macro.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_MACRO_H__
#endif

#ifndef ASCENDC_MODULE_STRUCT_MACRO_H
#define ASCENDC_MODULE_STRUCT_MACRO_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/utils/kernel_utils_constants.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

namespace AscendC {
struct ConvKernelParams {
    __aicore__ ConvKernelParams() {}

    __aicore__ ConvKernelParams(
        const uint8_t wStrideIn, const uint8_t hStrideIn, const uint16_t wKernelIn, const uint16_t hKernelIn,
        const uint8_t wDilationIn, const uint8_t hDilationIn, const uint16_t kernelNumIn)
        : wStride(wStrideIn),
          hStride(hStrideIn),
          wKernel(wKernelIn),
          hKernel(hKernelIn),
          wDilation(wDilationIn),
          hDilation(hDilationIn),
          kernelNum(kernelNumIn)
    {}

    uint8_t wStride = 0;
    uint8_t hStride = 0;
    uint16_t wKernel = 0;
    uint16_t hKernel = 0;
    uint8_t wDilation = 0;
    uint8_t hDilation = 0;
    uint16_t kernelNum = 0;
};

struct ConvFMatrixParams {
    __aicore__ ConvFMatrixParams() {}

    __aicore__ ConvFMatrixParams(
        const uint16_t wFmapIn, const uint16_t hFmapIn, const uint8_t leftPaddingSizeIn,
        const uint8_t rightPaddingSizeIn, const uint8_t topPaddingSizeIn, const uint8_t bottomPaddingSizeIn)
        : wFmap(wFmapIn),
          hFmap(hFmapIn),
          leftPaddingSize(leftPaddingSizeIn),
          rightPaddingSize(rightPaddingSizeIn),
          topPaddingSize(topPaddingSizeIn),
          bottomPaddingSize(bottomPaddingSizeIn)
    {}

    uint16_t wFmap = 0;
    uint16_t hFmap = 0;
    uint8_t leftPaddingSize = 0;
    uint8_t rightPaddingSize = 0;
    uint8_t topPaddingSize = 0;
    uint8_t bottomPaddingSize = 0;
};

struct ConvFMatrixStepPosParams {
    __aicore__ ConvFMatrixStepPosParams() {}

    __aicore__ ConvFMatrixStepPosParams(
        const uint16_t wStartPosIn, const uint16_t hStartPosIn, const uint16_t wStepIn, const uint16_t hStepIn)
        : wStartPos(wStartPosIn), hStartPos(hStartPosIn), wStep(wStepIn), hStep(hStepIn)
    {}

    uint16_t wStartPos = 0;
    uint16_t hStartPos = 0;
    uint16_t wStep = 0;
    uint16_t hStep = 0;
};

struct ConvFMatrixDual0Params {
    __aicore__ ConvFMatrixDual0Params() {}

    __aicore__ ConvFMatrixDual0Params(const uint32_t secondAddrIn, const uint16_t secondHIn)
        : secondAddr(secondAddrIn), secondH(secondHIn)
    {}

    uint32_t secondAddr = 0;
    uint16_t secondH = 0;
};

struct ConvFMatrixDual1Params {
    __aicore__ ConvFMatrixDual1Params() {}

    __aicore__ ConvFMatrixDual1Params(const uint16_t firstStrideIn, const uint16_t secondStrideIn)
        : firstStride(firstStrideIn), secondStride(secondStrideIn)
    {}

    uint16_t firstStride = 0;
    uint16_t secondStride = 0;
};

template <typename T>
struct ConvPaddingParams {
    __aicore__ ConvPaddingParams() {}

    __aicore__ ConvPaddingParams(const T paddingValueIn, const uint8_t paddingModeIn, const uint16_t paddingAddrIn)
        : paddingValue(paddingValueIn), paddingMode(paddingModeIn), paddingAddr(paddingAddrIn)
    {}

    T paddingValue = 0;
    uint8_t paddingMode = 0;
    uint16_t paddingAddr = 0;
};

struct FixPipeConfigParams {
    __aicore__ FixPipeConfigParams() {}

    __aicore__ FixPipeConfigParams(
        const uint8_t preReluAddrIn, const uint8_t preQuantAddrIn, const uint8_t postQuantAddrIn,
        const uint8_t antiquantAddrIn, const uint8_t shiftValueIn, const uint8_t restorationCtrlIn)
        : preReluAddr(preReluAddrIn),
          preQuantAddr(preQuantAddrIn),
          postQuantAddr(postQuantAddrIn),
          antiquantAddr(antiquantAddrIn),
          shiftValue(shiftValueIn),
          restorationCtrl(restorationCtrlIn)
    {}

    uint8_t preReluAddr = 0;
    uint8_t preQuantAddr = 0;
    uint8_t postQuantAddr = 0;
    uint8_t antiquantAddr = 0;
    uint8_t shiftValue = 0;
    uint8_t restorationCtrl = 0;
};

enum class lut_mode_t : uint8_t { Gelu, Silu, Sigmoid, Tanh };

struct FixPipeReluAlphaParams {
    __aicore__ FixPipeReluAlphaParams() {}

    __aicore__ FixPipeReluAlphaParams(const lut_mode_t lutIn, const uint32_t preValueIn)
        : lut(lutIn), preValue(preValueIn)
    {}

    lut_mode_t lut = lut_mode_t::Gelu;
    uint32_t preValue = 0;
};

struct FixPipeAddrParams {
    __aicore__ FixPipeAddrParams() {}

    __aicore__ FixPipeAddrParams(
        const bool c0IndicatorIn, const uint32_t eltSrcAddrIn, const uint32_t srcStride1In, const uint32_t srcStride2In)
        : c0Indicator(c0IndicatorIn), eltSrcAddr(eltSrcAddrIn), srcStride1(srcStride1In), srcStride2(srcStride2In)
    {}

    bool c0Indicator = false;
    uint32_t eltSrcAddr = 0;
    uint32_t srcStride1 = 0;
    uint32_t srcStride2 = 0;
};

struct ConvStrideParams {
    __aicore__ ConvStrideParams() {}

    __aicore__ ConvStrideParams(const uint16_t loop3DstStrideIn, const uint16_t loop4DstStrideIn)
        : loop3DstStride(loop3DstStrideIn), loop4DstStride(loop4DstStrideIn)
    {}

    uint16_t loop3DstStride = 0;
    uint16_t loop4DstStride = 0;
};

struct MatrixParaParams {
    __aicore__ MatrixParaParams() {}

    __aicore__ MatrixParaParams(const uint16_t mIn, const uint16_t kIn, const uint16_t groupKIn)
        : m(mIn), k(kIn), groupK(groupKIn)
    {}

    uint16_t m = 0;
    uint16_t k = 0;
    uint16_t groupK = 0;
};

struct MmadStrideParaParams {
    __aicore__ MmadStrideParaParams() {}

    __aicore__ MmadStrideParaParams(const uint16_t loop4SrcStrideIn, const uint16_t loop4DstStrideIn)
        : loop4SrcStride(loop4SrcStrideIn), loop4DstStride(loop4DstStrideIn)
    {}

    uint16_t loop4SrcStride = 0;
    uint16_t loop4DstStride = 0;
};

struct ItaMaxAddrParaParams {
    __aicore__ ItaMaxAddrParaParams() {}

    __aicore__ ItaMaxAddrParaParams(
        const uint16_t hmaxAddrIn, const uint16_t tmaxAddrIn, const uint16_t dmaxAddrIn, const uint16_t m0nStrideIn)
        : hmaxAddr(hmaxAddrIn), tmaxAddr(tmaxAddrIn), dmaxAddr(dmaxAddrIn), m0nStride(m0nStrideIn)
    {}

    uint16_t hmaxAddr = 0;
    uint16_t tmaxAddr = 0;
    uint16_t dmaxAddr = 0;
    uint16_t m0nStride = 0;
};

struct MatrixMxParaParams {
    __aicore__ MatrixMxParaParams() {}

    __aicore__ MatrixMxParaParams(const uint32_t aMxAddrIn, const uint32_t bMxAddrIn)
        : aMxAddr(aMxAddrIn), bMxAddr(bMxAddrIn)
    {}

    uint32_t aMxAddr = 0;
    uint32_t bMxAddr = 0;
};

struct ConvFixParams {
    __aicore__ ConvFixParams() {}

    __aicore__ ConvFixParams(
        const bool biasMatrixInitValIn, const bool biasMatrixBroadcastIn, const pre_quant_t preQuantModeIn,
        const pre_relu_t preReluModeIn, const post_quant_t postQuantModeIn, const uint8_t actPostModeIn,
        const uint8_t preClipReluModeIn, const eltwise_op_t eltwiseOpIn, const bool eltwiseAntiqEnableIn,
        const bool eltwiseBroadcastEnableIn, const lsb_mask_t lsbMaskIn, const bool dualSrcEnableIn,
        const bool compactEnableIn, const bool dependEnableIn, const instr_id_t instrIdIn,
        const bool postProcHalfEnableIn, const bool nbrcBiasCtrlIn, const uint16_t cinIn)
        : biasMatrixInitVal(biasMatrixInitValIn),
          biasMatrixBroadcast(biasMatrixBroadcastIn),
          preQuantMode(preQuantModeIn),
          preReluMode(preReluModeIn),
          postQuantMode(postQuantModeIn),
          actPostMode(actPostModeIn),
          preClipReluMode(preClipReluModeIn),
          eltwiseOp(eltwiseOpIn),
          eltwiseAntiqEnable(eltwiseAntiqEnableIn),
          eltwiseBroadcastEnable(eltwiseBroadcastEnableIn),
          lsbMask(lsbMaskIn),
          dualSrcEnable(dualSrcEnableIn),
          compactEnable(compactEnableIn),
          dependEnable(dependEnableIn),
          instrId(instrIdIn),
          postProcHalfEnable(postProcHalfEnableIn),
          nbrcBiasCtrl(nbrcBiasCtrlIn),
          cin(cinIn)
    {}

    bool biasMatrixInitVal = false;
    bool biasMatrixBroadcast = false;
    pre_quant_t preQuantMode = pre_quant_t::No_Quant;
    pre_relu_t preReluMode = pre_relu_t::NoRelu;
    post_quant_t postQuantMode = post_quant_t::No_Quant;
    uint8_t actPostMode = 0;
    uint8_t preClipReluMode = 0;
    eltwise_op_t eltwiseOp = eltwise_op_t::No_Eltwise;
    bool eltwiseAntiqEnable = false;
    bool eltwiseBroadcastEnable = false;
    lsb_mask_t lsbMask = lsb_mask_t::Disable;
    bool dualSrcEnable = false;
    bool compactEnable = false;
    bool dependEnable = false;
    instr_id_t instrId = instr_id_t::ID_0;
    bool postProcHalfEnable = false;
    bool nbrcBiasCtrl = false;
    uint16_t cin = 0;
};

struct MmadFixParams {
    __aicore__ MmadFixParams() {}

    __aicore__ MmadFixParams(
        const bool biasMatrixInitValIn, const bool biasMatrixBroadcastIn, const pre_quant_t preQuantModeIn,
        const pre_relu_t preReluModeIn, const post_quant_t postQuantModeIn, const uint8_t actPostModeIn,
        const uint8_t preClipReluModeIn, const eltwise_op_t eltwiseOpIn, const bool eltwiseAntiqEnableIn,
        const bool eltwiseBroadcastEnableIn, const bool itaEnableIn, const bool biasS32EnableIn,
        const bool hmaxEnableIn, const lsb_mask_t lsbMaskIn, const bool gemvCtrlIn, const uint8_t groupModeIn,
        const bool dependEnableIn, const instr_id_t instrIdIn, const bool postProcHalfEnableIn, const bool layoutCtrlIn,
        const uint16_t nIn)
        : biasMatrixInitVal(biasMatrixInitValIn),
          biasMatrixBroadcast(biasMatrixBroadcastIn),
          preQuantMode(preQuantModeIn),
          preReluMode(preReluModeIn),
          postQuantMode(postQuantModeIn),
          actPostMode(actPostModeIn),
          preClipReluMode(preClipReluModeIn),
          eltwiseOp(eltwiseOpIn),
          eltwiseAntiqEnable(eltwiseAntiqEnableIn),
          eltwiseBroadcastEnable(eltwiseBroadcastEnableIn),
          itaEnable(itaEnableIn),
          biasS32Enable(biasS32EnableIn),
          hmaxEnable(hmaxEnableIn),
          lsbMask(lsbMaskIn),
          gemvCtrl(gemvCtrlIn),
          groupMode(groupModeIn),
          dependEnable(dependEnableIn),
          instrId(instrIdIn),
          postProcHalfEnable(postProcHalfEnableIn),
          layoutCtrl(layoutCtrlIn),
          n(nIn)
    {}

    bool biasMatrixInitVal = false;
    bool biasMatrixBroadcast = false;
    pre_quant_t preQuantMode = pre_quant_t::No_Quant;
    pre_relu_t preReluMode = pre_relu_t::NoRelu;
    post_quant_t postQuantMode = post_quant_t::No_Quant;
    uint8_t actPostMode = 0;
    uint8_t preClipReluMode = 0;
    eltwise_op_t eltwiseOp = eltwise_op_t::No_Eltwise;
    bool eltwiseAntiqEnable = false;
    bool eltwiseBroadcastEnable = false;
    bool itaEnable = false;
    bool biasS32Enable = false;
    bool hmaxEnable = false;
    lsb_mask_t lsbMask = lsb_mask_t::Disable;
    bool gemvCtrl = false;
    uint8_t groupMode = 0;
    bool dependEnable = false;
    instr_id_t instrId = instr_id_t::ID_0;
    bool postProcHalfEnable = false;
    bool layoutCtrl = false;
    uint16_t n = 0;
};
} // namespace AscendC

#endif // __NPU_ARCH__
#endif // ASCENDC_MODULE_STRUCT_MACRO_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_MACRO_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_STRUCT_MACRO_H__
#endif
