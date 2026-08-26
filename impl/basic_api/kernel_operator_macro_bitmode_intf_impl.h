
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
 * \file kernel_operator_macro_bitmode_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_macro_bitmode_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_macro_bitmode_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BITMODE_INTF_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MACRO_BITMODE_INTF_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MACRO_BITMODE_INTF_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)

namespace AscendC {
__aicore__ inline ConvKernelBitModeParams::ConvKernelBitModeParams() { config0 = 0; }
__aicore__ inline ConvKernelBitModeParams::ConvKernelBitModeParams(const ConvKernelParams& convKernelParams)
{
    config0BitMode.Init(convKernelParams);
}
__aicore__ inline void ConvKernelBitModeParams::SetWStride(uint8_t wStride_) { config0BitMode.SetWStride(wStride_); }
__aicore__ inline void ConvKernelBitModeParams::SetHStride(uint8_t hStride_) { config0BitMode.SetHStride(hStride_); }
__aicore__ inline void ConvKernelBitModeParams::SetWKernel(uint16_t wKernel_) { config0BitMode.SetWKernel(wKernel_); }
__aicore__ inline void ConvKernelBitModeParams::SetHKernel(uint16_t hKernel_) { config0BitMode.SetHKernel(hKernel_); }
__aicore__ inline void ConvKernelBitModeParams::SetWDilation(uint8_t wDilation_)
{
    config0BitMode.SetWDilation(wDilation_);
}
__aicore__ inline void ConvKernelBitModeParams::SetHDilation(uint8_t hDilation_)
{
    config0BitMode.SetHDilation(hDilation_);
}
__aicore__ inline void ConvKernelBitModeParams::SetKernelNum(uint16_t kernelNum_)
{
    config0BitMode.SetKernelNum(kernelNum_);
}
__aicore__ inline void ConvKernelBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint8_t ConvKernelBitModeParams::GetWStride() const { return config0BitMode.GetWStride(); }
__aicore__ inline uint8_t ConvKernelBitModeParams::GetHStride() const { return config0BitMode.GetHStride(); }
__aicore__ inline uint16_t ConvKernelBitModeParams::GetWKernel() const { return config0BitMode.GetWKernel(); }
__aicore__ inline uint16_t ConvKernelBitModeParams::GetHKernel() const { return config0BitMode.GetHKernel(); }
__aicore__ inline uint8_t ConvKernelBitModeParams::GetWDilation() const { return config0BitMode.GetWDilation(); }
__aicore__ inline uint8_t ConvKernelBitModeParams::GetHDilation() const { return config0BitMode.GetHDilation(); }
__aicore__ inline uint16_t ConvKernelBitModeParams::GetKernelNum() const { return config0BitMode.GetKernelNum(); }
__aicore__ inline uint64_t ConvKernelBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline ConvFMatrixBitModeParams::ConvFMatrixBitModeParams() { config0 = 0; }
__aicore__ inline ConvFMatrixBitModeParams::ConvFMatrixBitModeParams(const ConvFMatrixParams& convFMatrixParams)
{
    config0BitMode.Init(convFMatrixParams);
}
__aicore__ inline void ConvFMatrixBitModeParams::SetWFmap(uint16_t wFmap_) { config0BitMode.SetWFmap(wFmap_); }
__aicore__ inline void ConvFMatrixBitModeParams::SetHFmap(uint16_t hFmap_) { config0BitMode.SetHFmap(hFmap_); }
__aicore__ inline void ConvFMatrixBitModeParams::SetLeftPaddingSize(uint8_t leftPaddingSize_)
{
    config0BitMode.SetLeftPaddingSize(leftPaddingSize_);
}
__aicore__ inline void ConvFMatrixBitModeParams::SetRightPaddingSize(uint8_t rightPaddingSize_)
{
    config0BitMode.SetRightPaddingSize(rightPaddingSize_);
}
__aicore__ inline void ConvFMatrixBitModeParams::SetTopPaddingSize(uint8_t topPaddingSize_)
{
    config0BitMode.SetTopPaddingSize(topPaddingSize_);
}
__aicore__ inline void ConvFMatrixBitModeParams::SetBottomPaddingSize(uint8_t bottomPaddingSize_)
{
    config0BitMode.SetBottomPaddingSize(bottomPaddingSize_);
}
__aicore__ inline void ConvFMatrixBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint16_t ConvFMatrixBitModeParams::GetWFmap() const { return config0BitMode.GetWFmap(); }
__aicore__ inline uint16_t ConvFMatrixBitModeParams::GetHFmap() const { return config0BitMode.GetHFmap(); }
__aicore__ inline uint8_t ConvFMatrixBitModeParams::GetLeftPaddingSize() const
{
    return config0BitMode.GetLeftPaddingSize();
}
__aicore__ inline uint8_t ConvFMatrixBitModeParams::GetRightPaddingSize() const
{
    return config0BitMode.GetRightPaddingSize();
}
__aicore__ inline uint8_t ConvFMatrixBitModeParams::GetTopPaddingSize() const
{
    return config0BitMode.GetTopPaddingSize();
}
__aicore__ inline uint8_t ConvFMatrixBitModeParams::GetBottomPaddingSize() const
{
    return config0BitMode.GetBottomPaddingSize();
}
__aicore__ inline uint64_t ConvFMatrixBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline ConvFMatrixStepPosBitModeParams::ConvFMatrixStepPosBitModeParams() { config0 = 0; }
__aicore__ inline ConvFMatrixStepPosBitModeParams::ConvFMatrixStepPosBitModeParams(
    const ConvFMatrixStepPosParams& convFMatrixStepPosParams)
{
    config0BitMode.Init(convFMatrixStepPosParams);
}
__aicore__ inline void ConvFMatrixStepPosBitModeParams::SetWStartPos(uint16_t wStartPos_)
{
    config0BitMode.SetWStartPos(wStartPos_);
}
__aicore__ inline void ConvFMatrixStepPosBitModeParams::SetHStartPos(uint16_t hStartPos_)
{
    config0BitMode.SetHStartPos(hStartPos_);
}
__aicore__ inline void ConvFMatrixStepPosBitModeParams::SetWStep(uint16_t wStep_) { config0BitMode.SetWStep(wStep_); }
__aicore__ inline void ConvFMatrixStepPosBitModeParams::SetHStep(uint16_t hStep_) { config0BitMode.SetHStep(hStep_); }
__aicore__ inline void ConvFMatrixStepPosBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint16_t ConvFMatrixStepPosBitModeParams::GetWStartPos() const
{
    return config0BitMode.GetWStartPos();
}
__aicore__ inline uint16_t ConvFMatrixStepPosBitModeParams::GetHStartPos() const
{
    return config0BitMode.GetHStartPos();
}
__aicore__ inline uint16_t ConvFMatrixStepPosBitModeParams::GetWStep() const { return config0BitMode.GetWStep(); }
__aicore__ inline uint16_t ConvFMatrixStepPosBitModeParams::GetHStep() const { return config0BitMode.GetHStep(); }
__aicore__ inline uint64_t ConvFMatrixStepPosBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline ConvFMatrixDual0BitModeParams::ConvFMatrixDual0BitModeParams() { config0 = 0; }
__aicore__ inline ConvFMatrixDual0BitModeParams::ConvFMatrixDual0BitModeParams(
    const ConvFMatrixDual0Params& convFMatrixDual0Params)
{
    config0BitMode.Init(convFMatrixDual0Params);
}
__aicore__ inline void ConvFMatrixDual0BitModeParams::SetSecondAddr(uint32_t secondAddr_)
{
    config0BitMode.SetSecondAddr(secondAddr_);
}
__aicore__ inline void ConvFMatrixDual0BitModeParams::SetSecondH(uint16_t secondH_)
{
    config0BitMode.SetSecondH(secondH_);
}
__aicore__ inline void ConvFMatrixDual0BitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint32_t ConvFMatrixDual0BitModeParams::GetSecondAddr() const
{
    return config0BitMode.GetSecondAddr();
}
__aicore__ inline uint16_t ConvFMatrixDual0BitModeParams::GetSecondH() const { return config0BitMode.GetSecondH(); }
__aicore__ inline uint64_t ConvFMatrixDual0BitModeParams::GetConfig0() const { return config0; };

__aicore__ inline ConvFMatrixDual1BitModeParams::ConvFMatrixDual1BitModeParams() { config0 = 0; }
__aicore__ inline ConvFMatrixDual1BitModeParams::ConvFMatrixDual1BitModeParams(
    const ConvFMatrixDual1Params& convFMatrixDual1Params)
{
    config0BitMode.Init(convFMatrixDual1Params);
}
__aicore__ inline void ConvFMatrixDual1BitModeParams::SetFirstStride(uint16_t firstStride_)
{
    config0BitMode.SetFirstStride(firstStride_);
}
__aicore__ inline void ConvFMatrixDual1BitModeParams::SetSecondStride(uint16_t secondStride_)
{
    config0BitMode.SetSecondStride(secondStride_);
}
__aicore__ inline void ConvFMatrixDual1BitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint16_t ConvFMatrixDual1BitModeParams::GetFirstStride() const
{
    return config0BitMode.GetFirstStride();
}
__aicore__ inline uint16_t ConvFMatrixDual1BitModeParams::GetSecondStride() const
{
    return config0BitMode.GetSecondStride();
}
__aicore__ inline uint64_t ConvFMatrixDual1BitModeParams::GetConfig0() const { return config0; };

template <typename T>
__aicore__ inline ConvPaddingBitModeParams<T>::ConvPaddingBitModeParams()
{
    config0 = 0;
}
template <typename T>
__aicore__ inline ConvPaddingBitModeParams<T>::ConvPaddingBitModeParams(const ConvPaddingParams<T>& convPaddingParams)
{
    config0BitMode.Init(convPaddingParams);
}
template <typename T>
__aicore__ inline void ConvPaddingBitModeParams<T>::SetPaddingValue(T paddingValue_)
{
    config0BitMode.SetPaddingValue(paddingValue_);
}
template <typename T>
__aicore__ inline void ConvPaddingBitModeParams<T>::SetPaddingMode(uint8_t paddingMode_)
{
    config0BitMode.SetPaddingMode(paddingMode_);
}
template <typename T>
__aicore__ inline void ConvPaddingBitModeParams<T>::SetPaddingAddr(uint16_t paddingAddr_)
{
    config0BitMode.SetPaddingAddr(paddingAddr_);
}
template <typename T>
__aicore__ inline void ConvPaddingBitModeParams<T>::SetConfig0(uint64_t config0_)
{
    config0 = config0_;
};
template <typename T>
__aicore__ inline T ConvPaddingBitModeParams<T>::GetPaddingValue() const
{
    return config0BitMode.GetPaddingValue();
}
template <typename T>
__aicore__ inline uint8_t ConvPaddingBitModeParams<T>::GetPaddingMode() const
{
    return config0BitMode.GetPaddingMode();
}
template <typename T>
__aicore__ inline uint16_t ConvPaddingBitModeParams<T>::GetPaddingAddr() const
{
    return config0BitMode.GetPaddingAddr();
}
template <typename T>
__aicore__ inline uint64_t ConvPaddingBitModeParams<T>::GetConfig0() const
{
    return config0;
};

__aicore__ inline FixPipeConfigBitModeParams::FixPipeConfigBitModeParams() { config0 = 0; }
__aicore__ inline FixPipeConfigBitModeParams::FixPipeConfigBitModeParams(const FixPipeConfigParams& fixPipeConfigParams)
{
    config0BitMode.Init(fixPipeConfigParams);
}
__aicore__ inline void FixPipeConfigBitModeParams::SetPreReluAddr(uint8_t preReluAddr_)
{
    config0BitMode.SetPreReluAddr(preReluAddr_);
}
__aicore__ inline void FixPipeConfigBitModeParams::SetPreQuantAddr(uint8_t preQuantAddr_)
{
    config0BitMode.SetPreQuantAddr(preQuantAddr_);
}
__aicore__ inline void FixPipeConfigBitModeParams::SetPostQuantAddr(uint8_t postQuantAddr_)
{
    config0BitMode.SetPostQuantAddr(postQuantAddr_);
}
__aicore__ inline void FixPipeConfigBitModeParams::SetAntiquantAddr(uint8_t antiquantAddr_)
{
    config0BitMode.SetAntiquantAddr(antiquantAddr_);
}
__aicore__ inline void FixPipeConfigBitModeParams::SetShiftValue(uint8_t shiftValue_)
{
    config0BitMode.SetShiftValue(shiftValue_);
}
__aicore__ inline void FixPipeConfigBitModeParams::SetRestorationCtrl(uint8_t restorationCtrl_)
{
    config0BitMode.SetRestorationCtrl(restorationCtrl_);
}
__aicore__ inline void FixPipeConfigBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint8_t FixPipeConfigBitModeParams::GetPreReluAddr() const { return config0BitMode.GetPreReluAddr(); }
__aicore__ inline uint8_t FixPipeConfigBitModeParams::GetPreQuantAddr() const
{
    return config0BitMode.GetPreQuantAddr();
}
__aicore__ inline uint8_t FixPipeConfigBitModeParams::GetPostQuantAddr() const
{
    return config0BitMode.GetPostQuantAddr();
}
__aicore__ inline uint8_t FixPipeConfigBitModeParams::GetAntiquantAddr() const
{
    return config0BitMode.GetAntiquantAddr();
}
__aicore__ inline uint8_t FixPipeConfigBitModeParams::GetShiftValue() const { return config0BitMode.GetShiftValue(); }
__aicore__ inline uint8_t FixPipeConfigBitModeParams::GetRestorationCtrl() const
{
    return config0BitMode.GetRestorationCtrl();
}
__aicore__ inline uint64_t FixPipeConfigBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline FixPipeReluAlphaBitModeParams::FixPipeReluAlphaBitModeParams() { config0 = 0; }
__aicore__ inline FixPipeReluAlphaBitModeParams::FixPipeReluAlphaBitModeParams(
    const FixPipeReluAlphaParams& fixPipeReluAlphaParams)
{
    config0BitMode.Init(fixPipeReluAlphaParams);
}
__aicore__ inline void FixPipeReluAlphaBitModeParams::SetLut(lut_mode_t lut_) { config0BitMode.SetLut(lut_); }
__aicore__ inline void FixPipeReluAlphaBitModeParams::SetPreValue(uint32_t preValue_)
{
    config0BitMode.SetPreValue(preValue_);
}
__aicore__ inline void FixPipeReluAlphaBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline lut_mode_t FixPipeReluAlphaBitModeParams::GetLut() const { return config0BitMode.GetLut(); }
__aicore__ inline uint32_t FixPipeReluAlphaBitModeParams::GetPreValue() const { return config0BitMode.GetPreValue(); }
__aicore__ inline uint64_t FixPipeReluAlphaBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline FixPipeAddrBitModeParams::FixPipeAddrBitModeParams() { config0 = 0; }
__aicore__ inline FixPipeAddrBitModeParams::FixPipeAddrBitModeParams(const FixPipeAddrParams& fixPipeAddrParams)
{
    config0BitMode.Init(fixPipeAddrParams);
}
__aicore__ inline void FixPipeAddrBitModeParams::SetC0Indicator(bool c0Indicator_)
{
    config0BitMode.SetC0Indicator(c0Indicator_);
}
__aicore__ inline void FixPipeAddrBitModeParams::SetEltSrcAddr(uint32_t eltSrcAddr_)
{
    config0BitMode.SetEltSrcAddr(eltSrcAddr_);
}
__aicore__ inline void FixPipeAddrBitModeParams::SetSrcStride1(uint32_t srcStride1_)
{
    config0BitMode.SetSrcStride1(srcStride1_);
}
__aicore__ inline void FixPipeAddrBitModeParams::SetSrcStride2(uint32_t srcStride2_)
{
    config0BitMode.SetSrcStride2(srcStride2_);
}
__aicore__ inline void FixPipeAddrBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline bool FixPipeAddrBitModeParams::GetC0Indicator() const { return config0BitMode.GetC0Indicator(); }
__aicore__ inline uint32_t FixPipeAddrBitModeParams::GetEltSrcAddr() const { return config0BitMode.GetEltSrcAddr(); }
__aicore__ inline uint32_t FixPipeAddrBitModeParams::GetSrcStride1() const { return config0BitMode.GetSrcStride1(); }
__aicore__ inline uint32_t FixPipeAddrBitModeParams::GetSrcStride2() const { return config0BitMode.GetSrcStride2(); }
__aicore__ inline uint64_t FixPipeAddrBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline ConvStrideBitModeParams::ConvStrideBitModeParams() { config0 = 0; }
__aicore__ inline ConvStrideBitModeParams::ConvStrideBitModeParams(const ConvStrideParams& convStrideParams)
{
    config0BitMode.Init(convStrideParams);
}
__aicore__ inline void ConvStrideBitModeParams::SetLoop3DstStride(uint16_t loop3DstStride_)
{
    config0BitMode.SetLoop3DstStride(loop3DstStride_);
}
__aicore__ inline void ConvStrideBitModeParams::SetLoop4DstStride(uint16_t loop4DstStride_)
{
    config0BitMode.SetLoop4DstStride(loop4DstStride_);
}
__aicore__ inline void ConvStrideBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint16_t ConvStrideBitModeParams::GetLoop3DstStride() const
{
    return config0BitMode.GetLoop3DstStride();
}
__aicore__ inline uint16_t ConvStrideBitModeParams::GetLoop4DstStride() const
{
    return config0BitMode.GetLoop4DstStride();
}
__aicore__ inline uint64_t ConvStrideBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline MatrixParaBitModeParams::MatrixParaBitModeParams() { config0 = 0; }
__aicore__ inline MatrixParaBitModeParams::MatrixParaBitModeParams(const MatrixParaParams& matrixParaParams)
{
    config0BitMode.Init(matrixParaParams);
}
__aicore__ inline void MatrixParaBitModeParams::SetM(uint16_t m_) { config0BitMode.SetM(m_); }
__aicore__ inline void MatrixParaBitModeParams::SetK(uint16_t k_) { config0BitMode.SetK(k_); }
__aicore__ inline void MatrixParaBitModeParams::SetGroupK(uint16_t groupK_) { config0BitMode.SetGroupK(groupK_); }
__aicore__ inline void MatrixParaBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint16_t MatrixParaBitModeParams::GetM() const { return config0BitMode.GetM(); }
__aicore__ inline uint16_t MatrixParaBitModeParams::GetK() const { return config0BitMode.GetK(); }
__aicore__ inline uint16_t MatrixParaBitModeParams::GetGroupK() const { return config0BitMode.GetGroupK(); }
__aicore__ inline uint64_t MatrixParaBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline MmadStrideParaBitModeParams::MmadStrideParaBitModeParams() { config0 = 0; }
__aicore__ inline MmadStrideParaBitModeParams::MmadStrideParaBitModeParams(
    const MmadStrideParaParams& mmadStrideParaParams)
{
    config0BitMode.Init(mmadStrideParaParams);
}
__aicore__ inline void MmadStrideParaBitModeParams::SetLoop4SrcStride(uint16_t loop4SrcStride_)
{
    config0BitMode.SetLoop4SrcStride(loop4SrcStride_);
}
__aicore__ inline void MmadStrideParaBitModeParams::SetLoop4DstStride(uint16_t loop4DstStride_)
{
    config0BitMode.SetLoop4DstStride(loop4DstStride_);
}
__aicore__ inline void MmadStrideParaBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint16_t MmadStrideParaBitModeParams::GetLoop4SrcStride() const
{
    return config0BitMode.GetLoop4SrcStride();
}
__aicore__ inline uint16_t MmadStrideParaBitModeParams::GetLoop4DstStride() const
{
    return config0BitMode.GetLoop4DstStride();
}
__aicore__ inline uint64_t MmadStrideParaBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline ItaMaxAddrBitModeParams::ItaMaxAddrBitModeParams() { config0 = 0; }
__aicore__ inline ItaMaxAddrBitModeParams::ItaMaxAddrBitModeParams(const ItaMaxAddrParaParams& itaMaxAddrParaParams)
{
    config0BitMode.Init(itaMaxAddrParaParams);
}
__aicore__ inline void ItaMaxAddrBitModeParams::SetHmaxAddr(uint16_t hmaxAddr_)
{
    config0BitMode.SetHmaxAddr(hmaxAddr_);
}
__aicore__ inline void ItaMaxAddrBitModeParams::SetTmaxAddr(uint16_t tmaxAddr_)
{
    config0BitMode.SetTmaxAddr(tmaxAddr_);
}
__aicore__ inline void ItaMaxAddrBitModeParams::SetDmaxAddr(uint16_t dmaxAddr_)
{
    config0BitMode.SetDmaxAddr(dmaxAddr_);
}
__aicore__ inline void ItaMaxAddrBitModeParams::SetM0nStride(uint16_t m0nStride_)
{
    config0BitMode.SetM0nStride(m0nStride_);
}
__aicore__ inline void ItaMaxAddrBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint16_t ItaMaxAddrBitModeParams::GetHmaxAddr() const { return config0BitMode.GetHmaxAddr(); }
__aicore__ inline uint16_t ItaMaxAddrBitModeParams::GetTmaxAddr() const { return config0BitMode.GetTmaxAddr(); }
__aicore__ inline uint16_t ItaMaxAddrBitModeParams::GetDmaxAddr() const { return config0BitMode.GetDmaxAddr(); }
__aicore__ inline uint16_t ItaMaxAddrBitModeParams::GetM0nStride() const { return config0BitMode.GetM0nStride(); }
__aicore__ inline uint64_t ItaMaxAddrBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline MatrixMxParaBitModeParams::MatrixMxParaBitModeParams() { config0 = 0; }
__aicore__ inline MatrixMxParaBitModeParams::MatrixMxParaBitModeParams(const MatrixMxParaParams& matrixMxParaParams)
{
    config0BitMode.Init(matrixMxParaParams);
}
__aicore__ inline void MatrixMxParaBitModeParams::SetAMxAddr(uint32_t aMxAddr_) { config0BitMode.SetAMxAddr(aMxAddr_); }
__aicore__ inline void MatrixMxParaBitModeParams::SetBMxAddr(uint32_t bMxAddr_) { config0BitMode.SetBMxAddr(bMxAddr_); }
__aicore__ inline void MatrixMxParaBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline uint32_t MatrixMxParaBitModeParams::GetAMxAddr() const { return config0BitMode.GetAMxAddr(); }
__aicore__ inline uint32_t MatrixMxParaBitModeParams::GetBMxAddr() const { return config0BitMode.GetBMxAddr(); }
__aicore__ inline uint64_t MatrixMxParaBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline ConvFixBitModeParams::ConvFixBitModeParams() { config0 = 0; }
__aicore__ inline ConvFixBitModeParams::ConvFixBitModeParams(const ConvFixParams& convFixParams)
{
    config0BitMode.Init(convFixParams);
}
__aicore__ inline void ConvFixBitModeParams::SetBiasMatrixInitVal(bool biasMatrixInitVal_)
{
    config0BitMode.SetBiasMatrixInitVal(biasMatrixInitVal_);
}
__aicore__ inline void ConvFixBitModeParams::SetBiasMatrixBroadcast(bool biasMatrixBroadcast_)
{
    config0BitMode.SetBiasMatrixBroadcast(biasMatrixBroadcast_);
}
__aicore__ inline void ConvFixBitModeParams::SetPreQuantMode(pre_quant_t preQuantMode_)
{
    config0BitMode.SetPreQuantMode(preQuantMode_);
}
__aicore__ inline void ConvFixBitModeParams::SetPreReluMode(pre_relu_t preReluMode_)
{
    config0BitMode.SetPreReluMode(preReluMode_);
}
__aicore__ inline void ConvFixBitModeParams::SetPostQuantMode(post_quant_t postQuantMode_)
{
    config0BitMode.SetPostQuantMode(postQuantMode_);
}
__aicore__ inline void ConvFixBitModeParams::SetactPostMode(uint8_t actPostMode_)
{
    config0BitMode.SetactPostMode(actPostMode_);
}
__aicore__ inline void ConvFixBitModeParams::SetPreClipReluMode(uint8_t preClipReluMode_)
{
    config0BitMode.SetPreClipReluMode(preClipReluMode_);
}
__aicore__ inline void ConvFixBitModeParams::SetEltwiseOp(eltwise_op_t eltwiseOp_)
{
    config0BitMode.SetEltwiseOp(eltwiseOp_);
}
__aicore__ inline void ConvFixBitModeParams::SetEltwiseAntiqEnable(bool eltwiseAntiqEnable_)
{
    config0BitMode.SetEltwiseAntiqEnable(eltwiseAntiqEnable_);
}
__aicore__ inline void ConvFixBitModeParams::SetEltwiseBroadcastEnable(bool eltwiseBroadcastEnable_)
{
    config0BitMode.SetEltwiseBroadcastEnable(eltwiseBroadcastEnable_);
}
__aicore__ inline void ConvFixBitModeParams::SetLsbMask(lsb_mask_t lsbMask_) { config0BitMode.SetLsbMask(lsbMask_); }
__aicore__ inline void ConvFixBitModeParams::SetDualSrcEnable(bool dualSrcEnable_)
{
    config0BitMode.SetDualSrcEnable(dualSrcEnable_);
}
__aicore__ inline void ConvFixBitModeParams::SetCompactEnable(bool compactEnable_)
{
    config0BitMode.SetCompactEnable(compactEnable_);
}
__aicore__ inline void ConvFixBitModeParams::SetDependEnable(bool dependEnable_)
{
    config0BitMode.SetDependEnable(dependEnable_);
}
__aicore__ inline void ConvFixBitModeParams::SetInstrId(instr_id_t instrId_) { config0BitMode.SetInstrId(instrId_); }
__aicore__ inline void ConvFixBitModeParams::SetPostProcHalfEnable(bool postProcHalfEnable_)
{
    config0BitMode.SetPostProcHalfEnable(postProcHalfEnable_);
}
__aicore__ inline void ConvFixBitModeParams::SetNbrcBiasCtrl(bool nbrcBiasCtrl_)
{
    config0BitMode.SetNbrcBiasCtrl(nbrcBiasCtrl_);
}
__aicore__ inline void ConvFixBitModeParams::SetCin(uint16_t cin_) { config0BitMode.SetCin(cin_); }
__aicore__ inline void ConvFixBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline bool ConvFixBitModeParams::GetBiasMatrixInitVal() const
{
    return config0BitMode.GetBiasMatrixInitVal();
}
__aicore__ inline bool ConvFixBitModeParams::GetBiasMatrixBroadcast() const
{
    return config0BitMode.GetBiasMatrixBroadcast();
}
__aicore__ inline pre_quant_t ConvFixBitModeParams::GetPreQuantMode() const { return config0BitMode.GetPreQuantMode(); }
__aicore__ inline pre_relu_t ConvFixBitModeParams::GetPreReluMode() const { return config0BitMode.GetPreReluMode(); }
__aicore__ inline post_quant_t ConvFixBitModeParams::GetPostQuantMode() const
{
    return config0BitMode.GetPostQuantMode();
}
__aicore__ inline uint8_t ConvFixBitModeParams::GetactPostMode() const { return config0BitMode.GetactPostMode(); }
__aicore__ inline uint8_t ConvFixBitModeParams::GetPreClipReluMode() const
{
    return config0BitMode.GetPreClipReluMode();
}
__aicore__ inline eltwise_op_t ConvFixBitModeParams::GetEltwiseOp() const { return config0BitMode.GetEltwiseOp(); }
__aicore__ inline bool ConvFixBitModeParams::GetEltwiseAntiqEnable() const
{
    return config0BitMode.GetEltwiseAntiqEnable();
}
__aicore__ inline bool ConvFixBitModeParams::GetEltwiseBroadcastEnable() const
{
    return config0BitMode.GetEltwiseBroadcastEnable();
}
__aicore__ inline lsb_mask_t ConvFixBitModeParams::GetLsbMask() const { return config0BitMode.GetLsbMask(); }
__aicore__ inline bool ConvFixBitModeParams::GetDualSrcEnable() const { return config0BitMode.GetDualSrcEnable(); }
__aicore__ inline bool ConvFixBitModeParams::GetCompactEnable() const { return config0BitMode.GetCompactEnable(); }
__aicore__ inline bool ConvFixBitModeParams::GetDependEnable() const { return config0BitMode.GetDependEnable(); }
__aicore__ inline instr_id_t ConvFixBitModeParams::GetInstrId() const { return config0BitMode.GetInstrId(); }
__aicore__ inline bool ConvFixBitModeParams::GetPostProcHalfEnable() const
{
    return config0BitMode.GetPostProcHalfEnable();
}
__aicore__ inline bool ConvFixBitModeParams::GetNbrcBiasCtrl() const { return config0BitMode.GetNbrcBiasCtrl(); }
__aicore__ inline uint16_t ConvFixBitModeParams::GetCin() const { return config0BitMode.GetCin(); }
__aicore__ inline uint64_t ConvFixBitModeParams::GetConfig0() const { return config0; };

__aicore__ inline MmadFixBitModeParams::MmadFixBitModeParams() { config0 = 0; }
__aicore__ inline MmadFixBitModeParams::MmadFixBitModeParams(const MmadFixParams& mmadFixParams)
{
    config0BitMode.Init(mmadFixParams);
}
__aicore__ inline void MmadFixBitModeParams::SetBiasMatrixInitVal(bool biasMatrixInitVal_)
{
    config0BitMode.SetBiasMatrixInitVal(biasMatrixInitVal_);
}
__aicore__ inline void MmadFixBitModeParams::SetBiasMatrixBroadcast(bool biasMatrixBroadcast_)
{
    config0BitMode.SetBiasMatrixBroadcast(biasMatrixBroadcast_);
}
__aicore__ inline void MmadFixBitModeParams::SetPreQuantMode(pre_quant_t preQuantMode_)
{
    config0BitMode.SetPreQuantMode(preQuantMode_);
}
__aicore__ inline void MmadFixBitModeParams::SetPreReluMode(pre_relu_t preReluMode_)
{
    config0BitMode.SetPreReluMode(preReluMode_);
}
__aicore__ inline void MmadFixBitModeParams::SetPostQuantMode(post_quant_t postQuantMode_)
{
    config0BitMode.SetPostQuantMode(postQuantMode_);
}
__aicore__ inline void MmadFixBitModeParams::SetactPostMode(uint8_t actPostMode_)
{
    config0BitMode.SetactPostMode(actPostMode_);
}
__aicore__ inline void MmadFixBitModeParams::SetPreClipReluMode(uint8_t preClipReluMode_)
{
    config0BitMode.SetPreClipReluMode(preClipReluMode_);
}
__aicore__ inline void MmadFixBitModeParams::SetEltwiseOp(eltwise_op_t eltwiseOp_)
{
    config0BitMode.SetEltwiseOp(eltwiseOp_);
}
__aicore__ inline void MmadFixBitModeParams::SetEltwiseAntiqEnable(bool eltwiseAntiqEnable_)
{
    config0BitMode.SetEltwiseAntiqEnable(eltwiseAntiqEnable_);
}
__aicore__ inline void MmadFixBitModeParams::SetEltwiseBroadcastEnable(bool eltwiseBroadcastEnable_)
{
    config0BitMode.SetEltwiseBroadcastEnable(eltwiseBroadcastEnable_);
}
__aicore__ inline void MmadFixBitModeParams::SetItaEnable(bool itaEnable_) { config0BitMode.SetItaEnable(itaEnable_); }
__aicore__ inline void MmadFixBitModeParams::SetBiasS32Enable(bool biasS32Enable_)
{
    config0BitMode.SetBiasS32Enable(biasS32Enable_);
}
__aicore__ inline void MmadFixBitModeParams::SetHmaxEnable(bool hmaxEnable_)
{
    config0BitMode.SetHmaxEnable(hmaxEnable_);
}
__aicore__ inline void MmadFixBitModeParams::SetLsbMask(lsb_mask_t lsbMask_) { config0BitMode.SetLsbMask(lsbMask_); }
__aicore__ inline void MmadFixBitModeParams::SetGemvCtrl(bool gemvCtrl_) { config0BitMode.SetGemvCtrl(gemvCtrl_); }
__aicore__ inline void MmadFixBitModeParams::SetGroupMode(uint8_t groupMode_)
{
    config0BitMode.SetGroupMode(groupMode_);
}
__aicore__ inline void MmadFixBitModeParams::SetDependEnable(bool dependEnable_)
{
    config0BitMode.SetDependEnable(dependEnable_);
}
__aicore__ inline void MmadFixBitModeParams::SetInstrId(instr_id_t instrId_) { config0BitMode.SetInstrId(instrId_); }
__aicore__ inline void MmadFixBitModeParams::SetPostProcHalfEnable(bool postProcHalfEnable_)
{
    config0BitMode.SetPostProcHalfEnable(postProcHalfEnable_);
}
__aicore__ inline void MmadFixBitModeParams::SetLayoutCtrl(bool layoutCtrl_)
{
    config0BitMode.SetLayoutCtrl(layoutCtrl_);
}
__aicore__ inline void MmadFixBitModeParams::SetN(uint16_t n_) { config0BitMode.SetN(n_); }
__aicore__ inline void MmadFixBitModeParams::SetConfig0(uint64_t config0_) { config0 = config0_; };
__aicore__ inline bool MmadFixBitModeParams::GetBiasMatrixInitVal() const
{
    return config0BitMode.GetBiasMatrixInitVal();
}
__aicore__ inline bool MmadFixBitModeParams::GetBiasMatrixBroadcast() const
{
    return config0BitMode.GetBiasMatrixBroadcast();
}
__aicore__ inline pre_quant_t MmadFixBitModeParams::GetPreQuantMode() const { return config0BitMode.GetPreQuantMode(); }
__aicore__ inline pre_relu_t MmadFixBitModeParams::GetPreReluMode() const { return config0BitMode.GetPreReluMode(); }
__aicore__ inline post_quant_t MmadFixBitModeParams::GetPostQuantMode() const
{
    return config0BitMode.GetPostQuantMode();
}
__aicore__ inline uint8_t MmadFixBitModeParams::GetactPostMode() const { return config0BitMode.GetactPostMode(); }
__aicore__ inline uint8_t MmadFixBitModeParams::GetPreClipReluMode() const
{
    return config0BitMode.GetPreClipReluMode();
}
__aicore__ inline eltwise_op_t MmadFixBitModeParams::GetEltwiseOp() const { return config0BitMode.GetEltwiseOp(); }
__aicore__ inline bool MmadFixBitModeParams::GetEltwiseAntiqEnable() const
{
    return config0BitMode.GetEltwiseAntiqEnable();
}
__aicore__ inline bool MmadFixBitModeParams::GetEltwiseBroadcastEnable() const
{
    return config0BitMode.GetEltwiseBroadcastEnable();
}
__aicore__ inline bool MmadFixBitModeParams::GetItaEnable() const { return config0BitMode.GetItaEnable(); }
__aicore__ inline bool MmadFixBitModeParams::GetBiasS32Enable() const { return config0BitMode.GetBiasS32Enable(); }
__aicore__ inline bool MmadFixBitModeParams::GetHmaxEnable() const { return config0BitMode.GetHmaxEnable(); }
__aicore__ inline lsb_mask_t MmadFixBitModeParams::GetLsbMask() const { return config0BitMode.GetLsbMask(); }
__aicore__ inline bool MmadFixBitModeParams::GetGemvCtrl() const { return config0BitMode.GetGemvCtrl(); }
__aicore__ inline uint8_t MmadFixBitModeParams::GetGroupMode() const { return config0BitMode.GetGroupMode(); }
__aicore__ inline bool MmadFixBitModeParams::GetDependEnable() const { return config0BitMode.GetDependEnable(); }
__aicore__ inline instr_id_t MmadFixBitModeParams::GetInstrId() const { return config0BitMode.GetInstrId(); }
__aicore__ inline bool MmadFixBitModeParams::GetPostProcHalfEnable() const
{
    return config0BitMode.GetPostProcHalfEnable();
}
__aicore__ inline bool MmadFixBitModeParams::GetLayoutCtrl() const { return config0BitMode.GetLayoutCtrl(); }
__aicore__ inline uint16_t MmadFixBitModeParams::GetN() const { return config0BitMode.GetN(); }
__aicore__ inline uint64_t MmadFixBitModeParams::GetConfig0() const { return config0; };
} // namespace AscendC

#endif // __NPU_ARCH__
#endif // ASCENDC_MODULE_OPERATOR_MACRO_BITMODE_INTF_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BITMODE_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BITMODE_INTF_IMPL_H__
#endif
