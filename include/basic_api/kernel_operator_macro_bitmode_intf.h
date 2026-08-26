/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 *\file kernel_operator_macro_bitmode_intf.h
 *\brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_macro_bitmode_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BITMODE_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MACRO_BITMODE_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_MACRO_BITMODE_INTERFACE_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "kernel_struct_macro.h"
#include "../../impl/basic_api/kernel_operator_macro_bitmode_struct.h"

namespace AscendC {
class ConvKernelBitModeParams {
public:
    __aicore__ inline ConvKernelBitModeParams();
    __aicore__ inline ConvKernelBitModeParams(const ConvKernelParams& convKernelParams);

    __aicore__ inline void SetWStride(uint8_t wStride_);
    __aicore__ inline void SetHStride(uint8_t hStride_);
    __aicore__ inline void SetWKernel(uint16_t wKernel_);
    __aicore__ inline void SetHKernel(uint16_t hKernel_);
    __aicore__ inline void SetWDilation(uint8_t wDilation_);
    __aicore__ inline void SetHDilation(uint8_t hDilation_);
    __aicore__ inline void SetKernelNum(uint16_t kernelNum_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint8_t GetWStride() const;
    __aicore__ inline uint8_t GetHStride() const;
    __aicore__ inline uint16_t GetWKernel() const;
    __aicore__ inline uint16_t GetHKernel() const;
    __aicore__ inline uint8_t GetWDilation() const;
    __aicore__ inline uint8_t GetHDilation() const;
    __aicore__ inline uint16_t GetKernelNum() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct ConvKernelBitModeConfig0 config0BitMode;
    };
};

class ConvFMatrixBitModeParams {
public:
    __aicore__ inline ConvFMatrixBitModeParams();
    __aicore__ inline ConvFMatrixBitModeParams(const ConvFMatrixParams& convFMatrixParams);

    __aicore__ inline void SetWFmap(uint16_t wFmap_);
    __aicore__ inline void SetHFmap(uint16_t hFmap_);
    __aicore__ inline void SetLeftPaddingSize(uint8_t leftPaddingSize_);
    __aicore__ inline void SetRightPaddingSize(uint8_t rightPaddingSize_);
    __aicore__ inline void SetTopPaddingSize(uint8_t topPaddingSize_);
    __aicore__ inline void SetBottomPaddingSize(uint8_t bottomPaddingSize_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint16_t GetWFmap() const;
    __aicore__ inline uint16_t GetHFmap() const;
    __aicore__ inline uint8_t GetLeftPaddingSize() const;
    __aicore__ inline uint8_t GetRightPaddingSize() const;
    __aicore__ inline uint8_t GetTopPaddingSize() const;
    __aicore__ inline uint8_t GetBottomPaddingSize() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct ConvFMatrixBitModeConfig0 config0BitMode;
    };
};

class ConvFMatrixStepPosBitModeParams {
public:
    __aicore__ inline ConvFMatrixStepPosBitModeParams();
    __aicore__ inline ConvFMatrixStepPosBitModeParams(const ConvFMatrixStepPosParams& convFMatrixStepPosParams);

    __aicore__ inline void SetWStartPos(uint16_t wStartPos_);
    __aicore__ inline void SetHStartPos(uint16_t hStartPos_);
    __aicore__ inline void SetWStep(uint16_t wStep_);
    __aicore__ inline void SetHStep(uint16_t hStep_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint16_t GetWStartPos() const;
    __aicore__ inline uint16_t GetHStartPos() const;
    __aicore__ inline uint16_t GetWStep() const;
    __aicore__ inline uint16_t GetHStep() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct ConvFMatrixStepPosBitModeConfig0 config0BitMode;
    };
};

class ConvFMatrixDual0BitModeParams {
public:
    __aicore__ inline ConvFMatrixDual0BitModeParams();
    __aicore__ inline ConvFMatrixDual0BitModeParams(const ConvFMatrixDual0Params& convFMatrixDual0Params);

    __aicore__ inline void SetSecondAddr(uint32_t secondAddr_);
    __aicore__ inline void SetSecondH(uint16_t secondH_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint32_t GetSecondAddr() const;
    __aicore__ inline uint16_t GetSecondH() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct ConvFMatrixDual0BitModeConfig0 config0BitMode;
    };
};

class ConvFMatrixDual1BitModeParams {
public:
    __aicore__ inline ConvFMatrixDual1BitModeParams();
    __aicore__ inline ConvFMatrixDual1BitModeParams(const ConvFMatrixDual1Params& convFMatrixDual1Params);

    __aicore__ inline void SetFirstStride(uint16_t firstStride_);
    __aicore__ inline void SetSecondStride(uint16_t secondStride_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint16_t GetFirstStride() const;
    __aicore__ inline uint16_t GetSecondStride() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct ConvFMatrixDual1BitModeConfig0 config0BitMode;
    };
};

template <typename T>
class ConvPaddingBitModeParams {
public:
    __aicore__ inline ConvPaddingBitModeParams();
    __aicore__ inline ConvPaddingBitModeParams(const ConvPaddingParams<T>& convPaddingParams);

    __aicore__ inline void SetPaddingValue(T paddingValue_);
    __aicore__ inline void SetPaddingMode(uint8_t paddingMode_);
    __aicore__ inline void SetPaddingAddr(uint16_t paddingAddr_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline T GetPaddingValue() const;
    __aicore__ inline uint8_t GetPaddingMode() const;
    __aicore__ inline uint16_t GetPaddingAddr() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct ConvPaddingBitModeConfig0<T> config0BitMode;
    };
};

class FixPipeConfigBitModeParams {
public:
    __aicore__ inline FixPipeConfigBitModeParams();
    __aicore__ inline FixPipeConfigBitModeParams(const FixPipeConfigParams& fixPipeConfigParams);

    __aicore__ inline void SetPreReluAddr(uint8_t preReluAddr_);
    __aicore__ inline void SetPreQuantAddr(uint8_t preQuantAddr_);
    __aicore__ inline void SetPostQuantAddr(uint8_t postQuantAddr_);
    __aicore__ inline void SetAntiquantAddr(uint8_t antiquantAddr_);
    __aicore__ inline void SetShiftValue(uint8_t shiftValue_);
    __aicore__ inline void SetRestorationCtrl(uint8_t restorationCtrl_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint8_t GetPreReluAddr() const;
    __aicore__ inline uint8_t GetPreQuantAddr() const;
    __aicore__ inline uint8_t GetPostQuantAddr() const;
    __aicore__ inline uint8_t GetAntiquantAddr() const;
    __aicore__ inline uint8_t GetShiftValue() const;
    __aicore__ inline uint8_t GetRestorationCtrl() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct FixPipeConfigBitModeConfig0 config0BitMode;
    };
};

class FixPipeReluAlphaBitModeParams {
public:
    __aicore__ inline FixPipeReluAlphaBitModeParams();
    __aicore__ inline FixPipeReluAlphaBitModeParams(const FixPipeReluAlphaParams& fixPipeReluAlphaParams);

    __aicore__ inline void SetLut(lut_mode_t lut_);
    __aicore__ inline void SetPreValue(uint32_t preValue_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline lut_mode_t GetLut() const;
    __aicore__ inline uint32_t GetPreValue() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct FixPipeReluAlphaBitModeConfig0 config0BitMode;
    };
};

class FixPipeAddrBitModeParams {
public:
    __aicore__ inline FixPipeAddrBitModeParams();
    __aicore__ inline FixPipeAddrBitModeParams(const FixPipeAddrParams& fixPipeAddrParams);

    __aicore__ inline void SetC0Indicator(bool c0Indicator_);
    __aicore__ inline void SetEltSrcAddr(uint32_t eltSrcAddr_);
    __aicore__ inline void SetSrcStride1(uint32_t srcStride1_);
    __aicore__ inline void SetSrcStride2(uint32_t srcStride2_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline bool GetC0Indicator() const;
    __aicore__ inline uint32_t GetEltSrcAddr() const;
    __aicore__ inline uint32_t GetSrcStride1() const;
    __aicore__ inline uint32_t GetSrcStride2() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct FixPipeAddrBitModeConfig0 config0BitMode;
    };
};

class ConvStrideBitModeParams {
public:
    __aicore__ inline ConvStrideBitModeParams();
    __aicore__ inline ConvStrideBitModeParams(const ConvStrideParams& convStrideParams);

    __aicore__ inline void SetLoop3DstStride(uint16_t loop3DstStride_);
    __aicore__ inline void SetLoop4DstStride(uint16_t loop4DstStride_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint16_t GetLoop3DstStride() const;
    __aicore__ inline uint16_t GetLoop4DstStride() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct ConvStrideBitModeConfig0 config0BitMode;
    };
};

class MatrixParaBitModeParams {
public:
    __aicore__ inline MatrixParaBitModeParams();
    __aicore__ inline MatrixParaBitModeParams(const MatrixParaParams& matrixParaParams);

    __aicore__ inline void SetM(uint16_t m_);
    __aicore__ inline void SetK(uint16_t k_);
    __aicore__ inline void SetGroupK(uint16_t groupK_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint16_t GetM() const;
    __aicore__ inline uint16_t GetK() const;
    __aicore__ inline uint16_t GetGroupK() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct MatrixParaBitModeConfig0 config0BitMode;
    };
};

class MmadStrideParaBitModeParams {
public:
    __aicore__ inline MmadStrideParaBitModeParams();
    __aicore__ inline MmadStrideParaBitModeParams(const MmadStrideParaParams& mmadStrideParaParams);

    __aicore__ inline void SetLoop4SrcStride(uint16_t loop4SrcStride_);
    __aicore__ inline void SetLoop4DstStride(uint16_t loop4DstStride_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint16_t GetLoop4SrcStride() const;
    __aicore__ inline uint16_t GetLoop4DstStride() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct MmadStrideParaBitModeConfig0 config0BitMode;
    };
};

class ItaMaxAddrBitModeParams {
public:
    __aicore__ inline ItaMaxAddrBitModeParams();
    __aicore__ inline ItaMaxAddrBitModeParams(const ItaMaxAddrParaParams& itaMaxAddrParaParams);

    __aicore__ inline void SetHmaxAddr(uint16_t hmaxAddr_);
    __aicore__ inline void SetTmaxAddr(uint16_t tmaxAddr_);
    __aicore__ inline void SetDmaxAddr(uint16_t dmaxAddr_);
    __aicore__ inline void SetM0nStride(uint16_t m0nStride_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint16_t GetHmaxAddr() const;
    __aicore__ inline uint16_t GetTmaxAddr() const;
    __aicore__ inline uint16_t GetDmaxAddr() const;
    __aicore__ inline uint16_t GetM0nStride() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct ItaMaxAddrBitModeConfig0 config0BitMode;
    };
};

class MatrixMxParaBitModeParams {
public:
    __aicore__ inline MatrixMxParaBitModeParams();
    __aicore__ inline MatrixMxParaBitModeParams(const MatrixMxParaParams& matrixMxParaParams);

    __aicore__ inline void SetAMxAddr(uint32_t aMxAddr_);
    __aicore__ inline void SetBMxAddr(uint32_t bMxAddr_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline uint32_t GetAMxAddr() const;
    __aicore__ inline uint32_t GetBMxAddr() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct MatrixMxParaBitModeConfig0 config0BitMode;
    };
};

class ConvFixBitModeParams {
public:
    __aicore__ inline ConvFixBitModeParams();
    __aicore__ inline ConvFixBitModeParams(const ConvFixParams& convFixParams);

    __aicore__ inline void SetBiasMatrixInitVal(bool biasMatrixInitVal_);
    __aicore__ inline void SetBiasMatrixBroadcast(bool biasMatrixBroadcast_);
    __aicore__ inline void SetPreQuantMode(pre_quant_t preQuantMode_);
    __aicore__ inline void SetPreReluMode(pre_relu_t preReluMode_);
    __aicore__ inline void SetPostQuantMode(post_quant_t postQuantMode_);
    __aicore__ inline void SetactPostMode(uint8_t actPostMode_);
    __aicore__ inline void SetPreClipReluMode(uint8_t preClipReluMode_);
    __aicore__ inline void SetEltwiseOp(eltwise_op_t eltwiseOp_);
    __aicore__ inline void SetEltwiseAntiqEnable(bool eltwiseAntiqEnable_);
    __aicore__ inline void SetEltwiseBroadcastEnable(bool eltwiseBroadcastEnable_);
    __aicore__ inline void SetLsbMask(lsb_mask_t lsbMask_);
    __aicore__ inline void SetDualSrcEnable(bool dualSrcEnable_);
    __aicore__ inline void SetCompactEnable(bool compactEnable_);
    __aicore__ inline void SetDependEnable(bool dependEnable_);
    __aicore__ inline void SetInstrId(instr_id_t instrId_);
    __aicore__ inline void SetPostProcHalfEnable(bool postProcHalfEnable_);
    __aicore__ inline void SetNbrcBiasCtrl(bool nbrcBiasCtrl_);
    __aicore__ inline void SetCin(uint16_t cin_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline bool GetBiasMatrixInitVal() const;
    __aicore__ inline bool GetBiasMatrixBroadcast() const;
    __aicore__ inline pre_quant_t GetPreQuantMode() const;
    __aicore__ inline pre_relu_t GetPreReluMode() const;
    __aicore__ inline post_quant_t GetPostQuantMode() const;
    __aicore__ inline uint8_t GetactPostMode() const;
    __aicore__ inline uint8_t GetPreClipReluMode() const;
    __aicore__ inline eltwise_op_t GetEltwiseOp() const;
    __aicore__ inline bool GetEltwiseAntiqEnable() const;
    __aicore__ inline bool GetEltwiseBroadcastEnable() const;
    __aicore__ inline lsb_mask_t GetLsbMask() const;
    __aicore__ inline bool GetDualSrcEnable() const;
    __aicore__ inline bool GetCompactEnable() const;
    __aicore__ inline bool GetDependEnable() const;
    __aicore__ inline instr_id_t GetInstrId() const;
    __aicore__ inline bool GetPostProcHalfEnable() const;
    __aicore__ inline bool GetNbrcBiasCtrl() const;
    __aicore__ inline uint16_t GetCin() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct ConvFixBitModeConfig0 config0BitMode;
    };
};

class MmadFixBitModeParams {
public:
    __aicore__ inline MmadFixBitModeParams();
    __aicore__ inline MmadFixBitModeParams(const MmadFixParams& mmadFixParams);

    __aicore__ inline void SetBiasMatrixInitVal(bool biasMatrixInitVal_);
    __aicore__ inline void SetBiasMatrixBroadcast(bool biasMatrixBroadcast_);
    __aicore__ inline void SetPreQuantMode(pre_quant_t preQuantMode_);
    __aicore__ inline void SetPreReluMode(pre_relu_t preReluMode_);
    __aicore__ inline void SetPostQuantMode(post_quant_t postQuantMode_);
    __aicore__ inline void SetactPostMode(uint8_t actPostMode_);
    __aicore__ inline void SetPreClipReluMode(uint8_t preClipReluMode_);
    __aicore__ inline void SetEltwiseOp(eltwise_op_t eltwiseOp_);
    __aicore__ inline void SetEltwiseAntiqEnable(bool eltwiseAntiqEnable_);
    __aicore__ inline void SetEltwiseBroadcastEnable(bool eltwiseBroadcastEnable_);
    __aicore__ inline void SetItaEnable(bool itaEnable_);
    __aicore__ inline void SetBiasS32Enable(bool biasS32Enable_);
    __aicore__ inline void SetHmaxEnable(bool hmaxEnable_);
    __aicore__ inline void SetLsbMask(lsb_mask_t lsbMask_);
    __aicore__ inline void SetGemvCtrl(bool gemvCtrl_);
    __aicore__ inline void SetGroupMode(uint8_t groupMode_);
    __aicore__ inline void SetDependEnable(bool dependEnable_);
    __aicore__ inline void SetInstrId(instr_id_t instrId_);
    __aicore__ inline void SetPostProcHalfEnable(bool postProcHalfEnable_);
    __aicore__ inline void SetLayoutCtrl(bool layoutCtrl_);
    __aicore__ inline void SetN(uint16_t n_);
    __aicore__ inline void SetConfig0(uint64_t config0_);

    __aicore__ inline bool GetBiasMatrixInitVal() const;
    __aicore__ inline bool GetBiasMatrixBroadcast() const;
    __aicore__ inline pre_quant_t GetPreQuantMode() const;
    __aicore__ inline pre_relu_t GetPreReluMode() const;
    __aicore__ inline post_quant_t GetPostQuantMode() const;
    __aicore__ inline uint8_t GetactPostMode() const;
    __aicore__ inline uint8_t GetPreClipReluMode() const;
    __aicore__ inline eltwise_op_t GetEltwiseOp() const;
    __aicore__ inline bool GetEltwiseAntiqEnable() const;
    __aicore__ inline bool GetEltwiseBroadcastEnable() const;
    __aicore__ inline bool GetItaEnable() const;
    __aicore__ inline bool GetBiasS32Enable() const;
    __aicore__ inline bool GetHmaxEnable() const;
    __aicore__ inline lsb_mask_t GetLsbMask() const;
    __aicore__ inline bool GetGemvCtrl() const;
    __aicore__ inline uint8_t GetGroupMode() const;
    __aicore__ inline bool GetDependEnable() const;
    __aicore__ inline instr_id_t GetInstrId() const;
    __aicore__ inline bool GetPostProcHalfEnable() const;
    __aicore__ inline bool GetLayoutCtrl() const;
    __aicore__ inline uint16_t GetN() const;
    __aicore__ inline uint64_t GetConfig0() const;

private:
    union {
        uint64_t config0;
        struct MmadFixBitModeConfig0 config0BitMode;
    };
};
} // namespace AscendC

#if defined(__NPU_ARCH__)
#include "../../impl/basic_api/kernel_operator_macro_bitmode_intf_impl.h"
#endif

#endif // __NPU_ARCH__
#endif // ASCENDC_MODULE_OPERATOR_MACRO_BITMODE_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BITMODE_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BITMODE_INTF_H__
#endif
