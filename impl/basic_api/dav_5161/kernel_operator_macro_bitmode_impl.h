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
 *\file kernel_operator_macro_bitmode_impl.h
 *\brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_5161/kernel_operator_macro_bitmode_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BITMODE_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MACRO_BITMODE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MACRO_BITMODE_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "../../../include/basic_api/kernel_struct_macro.h"

namespace AscendC {
struct ConvKernelBitModeConfig0 {
    uint64_t wStride : 6;
    uint64_t hStride : 6;
    uint64_t wKernel : 9;
    uint64_t hKernel : 9;
    uint64_t wDilation : 8;
    uint64_t hDilation : 8;
    uint64_t kernelNum : 12;

    __aicore__ inline void Init(const ConvKernelParams& params)
    {
        wStride = params.wStride;
        hStride = params.hStride;
        wKernel = params.wKernel;
        hKernel = params.hKernel;
        wDilation = params.wDilation;
        hDilation = params.hDilation;
        kernelNum = params.kernelNum;
    }

    __aicore__ inline void SetWStride(uint8_t wStride_) { wStride = wStride_; };
    __aicore__ inline void SetHStride(uint8_t hStride_) { hStride = hStride_; };
    __aicore__ inline void SetWKernel(uint16_t wKernel_) { wKernel = wKernel_; };
    __aicore__ inline void SetHKernel(uint16_t hKernel_) { hKernel = hKernel_; };
    __aicore__ inline void SetWDilation(uint8_t wDilation_) { wDilation = wDilation_; };
    __aicore__ inline void SetHDilation(uint8_t hDilation_) { hDilation = hDilation_; };
    __aicore__ inline void SetKernelNum(uint16_t kernelNum_) { kernelNum = kernelNum_; };

    __aicore__ inline uint8_t GetWStride() const { return wStride; };
    __aicore__ inline uint8_t GetHStride() const { return hStride; };
    __aicore__ inline uint16_t GetWKernel() const { return wKernel; };
    __aicore__ inline uint16_t GetHKernel() const { return hKernel; };
    __aicore__ inline uint8_t GetWDilation() const { return wDilation; };
    __aicore__ inline uint8_t GetHDilation() const { return hDilation; };
    __aicore__ inline uint16_t GetKernelNum() const { return kernelNum; };
};

struct ConvFMatrixBitModeConfig0 {
    uint64_t wFmap : 16;
    uint64_t hFmap : 16;
    uint64_t leftPaddingSize : 8;
    uint64_t rightPaddingSize : 8;
    uint64_t topPaddingSize : 8;
    uint64_t bottomPaddingSize : 8;

    __aicore__ inline void Init(const ConvFMatrixParams& params)
    {
        wFmap = params.wFmap;
        hFmap = params.hFmap;
        leftPaddingSize = params.leftPaddingSize;
        rightPaddingSize = params.rightPaddingSize;
        topPaddingSize = params.topPaddingSize;
        bottomPaddingSize = params.bottomPaddingSize;
    }

    __aicore__ inline void SetWFmap(uint16_t wFmap_) { wFmap = wFmap_; };
    __aicore__ inline void SetHFmap(uint16_t hFmap_) { hFmap = hFmap_; };
    __aicore__ inline void SetLeftPaddingSize(uint8_t leftPaddingSize_) { leftPaddingSize = leftPaddingSize_; };
    __aicore__ inline void SetRightPaddingSize(uint8_t rightPaddingSize_) { rightPaddingSize = rightPaddingSize_; };
    __aicore__ inline void SetTopPaddingSize(uint8_t topPaddingSize_) { topPaddingSize = topPaddingSize_; };
    __aicore__ inline void SetBottomPaddingSize(uint8_t bottomPaddingSize_) { bottomPaddingSize = bottomPaddingSize_; };

    __aicore__ inline uint16_t GetWFmap() const { return wFmap; };
    __aicore__ inline uint16_t GetHFmap() const { return hFmap; };
    __aicore__ inline uint8_t GetLeftPaddingSize() const { return leftPaddingSize; };
    __aicore__ inline uint8_t GetRightPaddingSize() const { return rightPaddingSize; };
    __aicore__ inline uint8_t GetTopPaddingSize() const { return topPaddingSize; };
    __aicore__ inline uint8_t GetBottomPaddingSize() const { return bottomPaddingSize; };
};

struct ConvFMatrixStepPosBitModeConfig0 {
    uint64_t wStartPos : 16;
    uint64_t hStartPos : 16;
    uint64_t wStep : 16;
    uint64_t hStep : 16;

    __aicore__ inline void Init(const ConvFMatrixStepPosParams& params)
    {
        wStartPos = params.wStartPos;
        hStartPos = params.hStartPos;
        wStep = params.wStep;
        hStep = params.hStep;
    }

    __aicore__ inline void SetWStartPos(uint16_t wStartPos_) { wStartPos = wStartPos_; };
    __aicore__ inline void SetHStartPos(uint16_t hStartPos_) { hStartPos = hStartPos_; };
    __aicore__ inline void SetWStep(uint16_t wStep_) { wStep = wStep_; };
    __aicore__ inline void SetHStep(uint16_t hStep_) { hStep = hStep_; };

    __aicore__ inline uint16_t GetWStartPos() const { return wStartPos; };
    __aicore__ inline uint16_t GetHStartPos() const { return hStartPos; };
    __aicore__ inline uint16_t GetWStep() const { return wStep; };
    __aicore__ inline uint16_t GetHStep() const { return hStep; };
};

struct ConvFMatrixDual0BitModeConfig0 {
    uint64_t secondAddr : 32;
    uint64_t secondH : 16;

    __aicore__ inline void Init(const ConvFMatrixDual0Params& params)
    {
        secondAddr = params.secondAddr;
        secondH = params.secondH;
    }

    __aicore__ inline void SetSecondAddr(uint32_t secondAddr_) { secondAddr = secondAddr_; };
    __aicore__ inline void SetSecondH(uint16_t secondH_) { secondH = secondH_; };

    __aicore__ inline uint32_t GetSecondAddr() const { return secondAddr; };
    __aicore__ inline uint16_t GetSecondH() const { return secondH; };
};

struct ConvFMatrixDual1BitModeConfig0 {
    uint64_t firstStride : 16;
    uint64_t secondStride : 16;

    __aicore__ inline void Init(const ConvFMatrixDual1Params& params)
    {
        firstStride = params.firstStride;
        secondStride = params.secondStride;
    }

    __aicore__ inline void SetFirstStride(uint16_t firstStride_) { firstStride = firstStride_; };
    __aicore__ inline void SetSecondStride(uint16_t secondStride_) { secondStride = secondStride_; };

    __aicore__ inline uint16_t GetFirstStride() const { return firstStride; };
    __aicore__ inline uint16_t GetSecondStride() const { return secondStride; };
};

template <typename T>
struct ConvPaddingBitModeConfig0 {
    uint64_t paddingValue : 16;
    uint64_t paddingMode : 2;
    uint64_t paddingAddr : 16;

    __aicore__ inline void Init(const ConvPaddingParams<T>& params)
    {
        union {
            T t;
            uint16_t u;
        } cvt;
        cvt.t = params.paddingValue;
        paddingValue = cvt.u;
        paddingMode = params.paddingMode;
        paddingAddr = params.paddingAddr;
    }

    __aicore__ inline void SetPaddingValue(T paddingValue_)
    {
        union {
            T t;
            uint16_t u;
        } cvt;
        cvt.t = paddingValue_;
        paddingValue = cvt.u;
    }

    __aicore__ inline void SetPaddingMode(uint8_t paddingMode_) { paddingMode = paddingMode_; };
    __aicore__ inline void SetPaddingAddr(uint16_t paddingAddr_) { paddingAddr = paddingAddr_; };

    __aicore__ inline T GetPaddingValue() const
    {
        union {
            T t;
            uint16_t u;
        } cvt;
        cvt.u = static_cast<uint16_t>(paddingValue);
        return cvt.t;
    }
    __aicore__ inline uint8_t GetPaddingMode() const { return paddingMode; };
    __aicore__ inline uint16_t GetPaddingAddr() const { return paddingAddr; };
};

struct FixPipeConfigBitModeConfig0 {
    uint64_t preReluAddr : 8;
    uint64_t preQuantAddr : 8;
    uint64_t postQuantAddr : 8;
    uint64_t antiquantAddr : 8;
    uint64_t shiftValue : 7;
    uint64_t restorationCtrl : 1;

    __aicore__ inline void Init(const FixPipeConfigParams& params)
    {
        preReluAddr = params.preReluAddr;
        preQuantAddr = params.preQuantAddr;
        postQuantAddr = params.postQuantAddr;
        antiquantAddr = params.antiquantAddr;
        shiftValue = params.shiftValue;
        restorationCtrl = params.restorationCtrl;
    }

    __aicore__ inline void SetPreReluAddr(uint8_t preReluAddr_) { preReluAddr = preReluAddr_; };
    __aicore__ inline void SetPreQuantAddr(uint8_t preQuantAddr_) { preQuantAddr = preQuantAddr_; };
    __aicore__ inline void SetPostQuantAddr(uint8_t postQuantAddr_) { postQuantAddr = postQuantAddr_; };
    __aicore__ inline void SetAntiquantAddr(uint8_t antiquantAddr_) { antiquantAddr = antiquantAddr_; };
    __aicore__ inline void SetShiftValue(uint8_t shiftValue_) { shiftValue = shiftValue_; };
    __aicore__ inline void SetRestorationCtrl(uint8_t restorationCtrl_) { restorationCtrl = restorationCtrl_; };

    __aicore__ inline uint8_t GetPreReluAddr() const { return preReluAddr; };
    __aicore__ inline uint8_t GetPreQuantAddr() const { return preQuantAddr; };
    __aicore__ inline uint8_t GetPostQuantAddr() const { return postQuantAddr; };
    __aicore__ inline uint8_t GetAntiquantAddr() const { return antiquantAddr; };
    __aicore__ inline uint8_t GetShiftValue() const { return shiftValue; };
    __aicore__ inline uint8_t GetRestorationCtrl() const { return restorationCtrl; };
};

struct FixPipeReluAlphaBitModeConfig0 {
    uint64_t lut : 3;
    uint64_t preValue : 19;

    __aicore__ inline void Init(const FixPipeReluAlphaParams& params)
    {
        lut = static_cast<uint8_t>(params.lut);
        preValue = params.preValue;
    }

    __aicore__ inline void SetLut(lut_mode_t lut_) { lut = static_cast<uint8_t>(lut_); };
    __aicore__ inline void SetPreValue(uint32_t preValue_) { preValue = preValue_; };

    __aicore__ inline lut_mode_t GetLut() const { return static_cast<lut_mode_t>(lut); };
    __aicore__ inline uint32_t GetPreValue() const { return preValue; };
};

struct FixPipeAddrBitModeConfig0 {
    uint64_t c0Indicator : 1;
    uint64_t eltSrcAddr : 16;
    uint64_t srcStride1 : 16;
    uint64_t srcStride2 : 16;

    __aicore__ inline void Init(const FixPipeAddrParams& params)
    {
        c0Indicator = params.c0Indicator ? 1 : 0;
        eltSrcAddr = params.eltSrcAddr;
        srcStride1 = params.srcStride1;
        srcStride2 = params.srcStride2;
    }

    __aicore__ inline void SetC0Indicator(bool c0Indicator_) { c0Indicator = c0Indicator_ ? 1 : 0; };
    __aicore__ inline void SetEltSrcAddr(uint32_t eltSrcAddr_) { eltSrcAddr = eltSrcAddr_; };
    __aicore__ inline void SetSrcStride1(uint32_t srcStride1_) { srcStride1 = srcStride1_; };
    __aicore__ inline void SetSrcStride2(uint32_t srcStride2_) { srcStride2 = srcStride2_; };

    __aicore__ inline bool GetC0Indicator() const { return c0Indicator != 0; };
    __aicore__ inline uint32_t GetEltSrcAddr() const { return eltSrcAddr; };
    __aicore__ inline uint32_t GetSrcStride1() const { return srcStride1; };
    __aicore__ inline uint32_t GetSrcStride2() const { return srcStride2; };
};

struct ConvStrideBitModeConfig0 {
    uint64_t loop3DstStride : 16;
    uint64_t loop4DstStride : 16;

    __aicore__ inline void Init(const ConvStrideParams& params)
    {
        loop3DstStride = params.loop3DstStride;
        loop4DstStride = params.loop4DstStride;
    }

    __aicore__ inline void SetLoop3DstStride(uint16_t loop3DstStride_) { loop3DstStride = loop3DstStride_; };
    __aicore__ inline void SetLoop4DstStride(uint16_t loop4DstStride_) { loop4DstStride = loop4DstStride_; };

    __aicore__ inline uint16_t GetLoop3DstStride() const { return loop3DstStride; };
    __aicore__ inline uint16_t GetLoop4DstStride() const { return loop4DstStride; };
};

struct MatrixParaBitModeConfig0 {
    uint64_t m : 16;
    uint64_t k : 16;
    uint64_t groupK : 16;

    __aicore__ inline void Init(const MatrixParaParams& params)
    {
        m = params.m;
        k = params.k;
        groupK = params.groupK;
    }

    __aicore__ inline void SetM(uint16_t m_) { m = m_; };
    __aicore__ inline void SetK(uint16_t k_) { k = k_; };
    __aicore__ inline void SetGroupK(uint16_t groupK_) { groupK = groupK_; };

    __aicore__ inline uint16_t GetM() const { return m; };
    __aicore__ inline uint16_t GetK() const { return k; };
    __aicore__ inline uint16_t GetGroupK() const { return groupK; };
};

struct MmadStrideParaBitModeConfig0 {
    uint64_t loop4SrcStride : 16;
    uint64_t loop4DstStride : 16;

    __aicore__ inline void Init(const MmadStrideParaParams& params)
    {
        loop4SrcStride = params.loop4SrcStride;
        loop4DstStride = params.loop4DstStride;
    }

    __aicore__ inline void SetLoop4SrcStride(uint16_t loop4SrcStride_) { loop4SrcStride = loop4SrcStride_; };
    __aicore__ inline void SetLoop4DstStride(uint16_t loop4DstStride_) { loop4DstStride = loop4DstStride_; };

    __aicore__ inline uint16_t GetLoop4SrcStride() const { return loop4SrcStride; };
    __aicore__ inline uint16_t GetLoop4DstStride() const { return loop4DstStride; };
};

struct ItaMaxAddrBitModeConfig0 {
    uint64_t hmaxAddr : 16;
    uint64_t tmaxAddr : 16;
    uint64_t dmaxAddr : 16;
    uint64_t m0nStride : 16;

    __aicore__ inline void Init(const ItaMaxAddrParaParams& params)
    {
        hmaxAddr = params.hmaxAddr;
        tmaxAddr = params.tmaxAddr;
        dmaxAddr = params.dmaxAddr;
        m0nStride = params.m0nStride;
    }

    __aicore__ inline void SetHmaxAddr(uint16_t hmaxAddr_) { hmaxAddr = hmaxAddr_; };
    __aicore__ inline void SetTmaxAddr(uint16_t tmaxAddr_) { tmaxAddr = tmaxAddr_; };
    __aicore__ inline void SetDmaxAddr(uint16_t dmaxAddr_) { dmaxAddr = dmaxAddr_; };
    __aicore__ inline void SetM0nStride(uint16_t m0nStride_) { m0nStride = m0nStride_; };

    __aicore__ inline uint16_t GetHmaxAddr() const { return hmaxAddr; };
    __aicore__ inline uint16_t GetTmaxAddr() const { return tmaxAddr; };
    __aicore__ inline uint16_t GetDmaxAddr() const { return dmaxAddr; };
    __aicore__ inline uint16_t GetM0nStride() const { return m0nStride; };
};

struct MatrixMxParaBitModeConfig0 {
    uint64_t aMxAddr : 32;
    uint64_t bMxAddr : 32;

    __aicore__ inline void Init(const MatrixMxParaParams& params)
    {
        aMxAddr = params.aMxAddr;
        bMxAddr = params.bMxAddr;
    }

    __aicore__ inline void SetAMxAddr(uint32_t aMxAddr_) { aMxAddr = aMxAddr_; };
    __aicore__ inline void SetBMxAddr(uint32_t bMxAddr_) { bMxAddr = bMxAddr_; };

    __aicore__ inline uint32_t GetAMxAddr() const { return aMxAddr; };
    __aicore__ inline uint32_t GetBMxAddr() const { return bMxAddr; };
};

struct ConvFixBitModeConfig0 {
    uint64_t biasMatrixInitVal : 1;
    uint64_t biasMatrixBroadcast : 1;
    uint64_t preQuantMode : 5;
    uint64_t preReluMode : 3;
    uint64_t postQuantMode : 5;
    uint64_t actPostMode : 3;
    uint64_t preClipReluMode : 1;
    uint64_t eltwiseOp : 3;
    uint64_t eltwiseAntiqEnable : 1;
    uint64_t eltwiseBroadcastEnable : 1;
    uint64_t lsbMask : 3;
    uint64_t dualSrcEnable : 1;
    uint64_t compactEnable : 1;
    uint64_t dependEnable : 1;
    uint64_t instrId : 2;
    uint64_t postProcHalfEnable : 1;
    uint64_t nbrcBiasCtrl : 1;
    uint64_t cin : 16;

    __aicore__ inline void Init(const ConvFixParams& params)
    {
        biasMatrixInitVal = params.biasMatrixInitVal ? 1 : 0;
        biasMatrixBroadcast = params.biasMatrixBroadcast ? 1 : 0;
        preQuantMode = static_cast<uint8_t>(params.preQuantMode);
        preReluMode = static_cast<uint8_t>(params.preReluMode);
        postQuantMode = static_cast<uint8_t>(params.postQuantMode);
        actPostMode = params.actPostMode;
        preClipReluMode = static_cast<uint8_t>(params.preClipReluMode);
        eltwiseOp = static_cast<uint8_t>(params.eltwiseOp);
        eltwiseAntiqEnable = params.eltwiseAntiqEnable ? 1 : 0;
        eltwiseBroadcastEnable = params.eltwiseBroadcastEnable ? 1 : 0;
        lsbMask = static_cast<uint8_t>(params.lsbMask);
        dualSrcEnable = params.dualSrcEnable ? 1 : 0;
        compactEnable = params.compactEnable ? 1 : 0;
        dependEnable = params.dependEnable ? 1 : 0;
        instrId = static_cast<uint8_t>(params.instrId);
        postProcHalfEnable = params.postProcHalfEnable ? 1 : 0;
        nbrcBiasCtrl = params.nbrcBiasCtrl ? 1 : 0;
        cin = params.cin;
    }

    __aicore__ inline void SetBiasMatrixInitVal(bool biasMatrixInitVal_)
    {
        biasMatrixInitVal = biasMatrixInitVal_ ? 1 : 0;
    };
    __aicore__ inline void SetBiasMatrixBroadcast(bool biasMatrixBroadcast_)
    {
        biasMatrixBroadcast = biasMatrixBroadcast_ ? 1 : 0;
    };
    __aicore__ inline void SetPreQuantMode(pre_quant_t preQuantMode_)
    {
        preQuantMode = static_cast<uint8_t>(preQuantMode_);
    };
    __aicore__ inline void SetPreReluMode(pre_relu_t preReluMode_)
    {
        preReluMode = static_cast<uint8_t>(preReluMode_);
    };
    __aicore__ inline void SetPostQuantMode(post_quant_t postQuantMode_)
    {
        postQuantMode = static_cast<uint8_t>(postQuantMode_);
    };
    __aicore__ inline void SetactPostMode(uint8_t actPostMode_) { actPostMode = actPostMode_; };
    __aicore__ inline void SetPreClipReluMode(uint8_t preClipReluMode_)
    {
        preClipReluMode = static_cast<uint8_t>(preClipReluMode_);
    };
    __aicore__ inline void SetEltwiseOp(eltwise_op_t eltwiseOp_) { eltwiseOp = static_cast<uint8_t>(eltwiseOp_); };
    __aicore__ inline void SetEltwiseAntiqEnable(bool eltwiseAntiqEnable_)
    {
        eltwiseAntiqEnable = eltwiseAntiqEnable_ ? 1 : 0;
    };
    __aicore__ inline void SetEltwiseBroadcastEnable(bool eltwiseBroadcastEnable_)
    {
        eltwiseBroadcastEnable = eltwiseBroadcastEnable_ ? 1 : 0;
    };
    __aicore__ inline void SetLsbMask(lsb_mask_t lsbMask_) { lsbMask = static_cast<uint8_t>(lsbMask_); };
    __aicore__ inline void SetDualSrcEnable(bool dualSrcEnable_) { dualSrcEnable = dualSrcEnable_ ? 1 : 0; };
    __aicore__ inline void SetCompactEnable(bool compactEnable_) { compactEnable = compactEnable_ ? 1 : 0; };
    __aicore__ inline void SetDependEnable(bool dependEnable_) { dependEnable = dependEnable_ ? 1 : 0; };
    __aicore__ inline void SetInstrId(instr_id_t instrId_) { instrId = static_cast<uint8_t>(instrId_); };
    __aicore__ inline void SetPostProcHalfEnable(bool postProcHalfEnable_)
    {
        postProcHalfEnable = postProcHalfEnable_ ? 1 : 0;
    };
    __aicore__ inline void SetNbrcBiasCtrl(bool nbrcBiasCtrl_) { nbrcBiasCtrl = nbrcBiasCtrl_ ? 1 : 0; };
    __aicore__ inline void SetCin(uint16_t cin_) { cin = cin_; };

    __aicore__ inline bool GetBiasMatrixInitVal() const { return biasMatrixInitVal != 0; };
    __aicore__ inline bool GetBiasMatrixBroadcast() const { return biasMatrixBroadcast != 0; };
    __aicore__ inline pre_quant_t GetPreQuantMode() const { return static_cast<pre_quant_t>(preQuantMode); };
    __aicore__ inline pre_relu_t GetPreReluMode() const { return static_cast<pre_relu_t>(preReluMode); };
    __aicore__ inline post_quant_t GetPostQuantMode() const { return static_cast<post_quant_t>(postQuantMode); };
    __aicore__ inline uint8_t GetactPostMode() const { return actPostMode; };
    __aicore__ inline uint8_t GetPreClipReluMode() const { return static_cast<uint8_t>(preClipReluMode); };
    __aicore__ inline eltwise_op_t GetEltwiseOp() const { return static_cast<eltwise_op_t>(eltwiseOp); };
    __aicore__ inline bool GetEltwiseAntiqEnable() const { return eltwiseAntiqEnable != 0; };
    __aicore__ inline bool GetEltwiseBroadcastEnable() const { return eltwiseBroadcastEnable != 0; };
    __aicore__ inline lsb_mask_t GetLsbMask() const { return static_cast<lsb_mask_t>(lsbMask); };
    __aicore__ inline bool GetDualSrcEnable() const { return dualSrcEnable != 0; };
    __aicore__ inline bool GetCompactEnable() const { return compactEnable != 0; };
    __aicore__ inline bool GetDependEnable() const { return dependEnable != 0; };
    __aicore__ inline instr_id_t GetInstrId() const { return static_cast<instr_id_t>(instrId); };
    __aicore__ inline bool GetPostProcHalfEnable() const { return postProcHalfEnable != 0; };
    __aicore__ inline bool GetNbrcBiasCtrl() const { return nbrcBiasCtrl != 0; };
    __aicore__ inline uint16_t GetCin() const { return cin; };
};

struct MmadFixBitModeConfig0 {
    uint64_t biasMatrixInitVal : 1;
    uint64_t biasMatrixBroadcast : 1;
    uint64_t preQuantMode : 5;
    uint64_t preReluMode : 3;
    uint64_t postQuantMode : 5;
    uint64_t actPostMode : 3;
    uint64_t preClipReluMode : 1;
    uint64_t eltwiseOp : 3;
    uint64_t eltwiseAntiqEnable : 1;
    uint64_t eltwiseBroadcastEnable : 1;
    uint64_t itaEnable : 1;
    uint64_t biasS32Enable : 1;
    uint64_t hmaxEnable : 1;
    uint64_t lsbMask : 3;
    uint64_t gemvCtrl : 1;
    uint64_t groupMode : 3;
    uint64_t dependEnable : 1;
    uint64_t instrId : 2;
    uint64_t postProcHalfEnable : 1;
    uint64_t layoutCtrl : 1;
    uint64_t n : 16;

    __aicore__ inline void Init(const MmadFixParams& params)
    {
        biasMatrixInitVal = params.biasMatrixInitVal ? 1 : 0;
        biasMatrixBroadcast = params.biasMatrixBroadcast ? 1 : 0;
        preQuantMode = static_cast<uint8_t>(params.preQuantMode);
        preReluMode = static_cast<uint8_t>(params.preReluMode);
        postQuantMode = static_cast<uint8_t>(params.postQuantMode);
        actPostMode = params.actPostMode;
        preClipReluMode = static_cast<uint8_t>(params.preClipReluMode);
        eltwiseOp = static_cast<uint8_t>(params.eltwiseOp);
        eltwiseAntiqEnable = params.eltwiseAntiqEnable ? 1 : 0;
        eltwiseBroadcastEnable = params.eltwiseBroadcastEnable ? 1 : 0;
        itaEnable = params.itaEnable ? 1 : 0;
        biasS32Enable = params.biasS32Enable ? 1 : 0;
        hmaxEnable = params.hmaxEnable ? 1 : 0;
        lsbMask = static_cast<uint8_t>(params.lsbMask);
        gemvCtrl = params.gemvCtrl ? 1 : 0;
        groupMode = params.groupMode;
        dependEnable = params.dependEnable ? 1 : 0;
        instrId = static_cast<uint8_t>(params.instrId);
        postProcHalfEnable = params.postProcHalfEnable ? 1 : 0;
        layoutCtrl = params.layoutCtrl ? 1 : 0;
        n = params.n;
    }

    __aicore__ inline void SetBiasMatrixInitVal(bool biasMatrixInitVal_)
    {
        biasMatrixInitVal = biasMatrixInitVal_ ? 1 : 0;
    };
    __aicore__ inline void SetBiasMatrixBroadcast(bool biasMatrixBroadcast_)
    {
        biasMatrixBroadcast = biasMatrixBroadcast_ ? 1 : 0;
    };
    __aicore__ inline void SetPreQuantMode(pre_quant_t preQuantMode_)
    {
        preQuantMode = static_cast<uint8_t>(preQuantMode_);
    };
    __aicore__ inline void SetPreReluMode(pre_relu_t preReluMode_)
    {
        preReluMode = static_cast<uint8_t>(preReluMode_);
    };
    __aicore__ inline void SetPostQuantMode(post_quant_t postQuantMode_)
    {
        postQuantMode = static_cast<uint8_t>(postQuantMode_);
    };
    __aicore__ inline void SetactPostMode(uint8_t actPostMode_) { actPostMode = actPostMode_; };
    __aicore__ inline void SetPreClipReluMode(uint8_t preClipReluMode_)
    {
        preClipReluMode = static_cast<uint8_t>(preClipReluMode_);
    };
    __aicore__ inline void SetEltwiseOp(eltwise_op_t eltwiseOp_) { eltwiseOp = static_cast<uint8_t>(eltwiseOp_); };
    __aicore__ inline void SetEltwiseAntiqEnable(bool eltwiseAntiqEnable_)
    {
        eltwiseAntiqEnable = eltwiseAntiqEnable_ ? 1 : 0;
    };
    __aicore__ inline void SetEltwiseBroadcastEnable(bool eltwiseBroadcastEnable_)
    {
        eltwiseBroadcastEnable = eltwiseBroadcastEnable_ ? 1 : 0;
    };
    __aicore__ inline void SetItaEnable(bool itaEnable_) { itaEnable = itaEnable_ ? 1 : 0; };
    __aicore__ inline void SetBiasS32Enable(bool biasS32Enable_) { biasS32Enable = biasS32Enable_ ? 1 : 0; };
    __aicore__ inline void SetHmaxEnable(bool hmaxEnable_) { hmaxEnable = hmaxEnable_ ? 1 : 0; };
    __aicore__ inline void SetLsbMask(lsb_mask_t lsbMask_) { lsbMask = static_cast<uint8_t>(lsbMask_); };
    __aicore__ inline void SetGemvCtrl(bool gemvCtrl_) { gemvCtrl = gemvCtrl_ ? 1 : 0; };
    __aicore__ inline void SetGroupMode(uint8_t groupMode_) { groupMode = groupMode_; };
    __aicore__ inline void SetDependEnable(bool dependEnable_) { dependEnable = dependEnable_ ? 1 : 0; };
    __aicore__ inline void SetInstrId(instr_id_t instrId_) { instrId = static_cast<uint8_t>(instrId_); };
    __aicore__ inline void SetPostProcHalfEnable(bool postProcHalfEnable_)
    {
        postProcHalfEnable = postProcHalfEnable_ ? 1 : 0;
    };
    __aicore__ inline void SetLayoutCtrl(bool layoutCtrl_) { layoutCtrl = layoutCtrl_ ? 1 : 0; };
    __aicore__ inline void SetN(uint16_t n_) { n = n_; };

    __aicore__ inline bool GetBiasMatrixInitVal() const { return biasMatrixInitVal != 0; };
    __aicore__ inline bool GetBiasMatrixBroadcast() const { return biasMatrixBroadcast != 0; };
    __aicore__ inline pre_quant_t GetPreQuantMode() const { return static_cast<pre_quant_t>(preQuantMode); };
    __aicore__ inline pre_relu_t GetPreReluMode() const { return static_cast<pre_relu_t>(preReluMode); };
    __aicore__ inline post_quant_t GetPostQuantMode() const { return static_cast<post_quant_t>(postQuantMode); };
    __aicore__ inline uint8_t GetactPostMode() const { return actPostMode; };
    __aicore__ inline uint8_t GetPreClipReluMode() const { return static_cast<uint8_t>(preClipReluMode); };
    __aicore__ inline eltwise_op_t GetEltwiseOp() const { return static_cast<eltwise_op_t>(eltwiseOp); };
    __aicore__ inline bool GetEltwiseAntiqEnable() const { return eltwiseAntiqEnable != 0; };
    __aicore__ inline bool GetEltwiseBroadcastEnable() const { return eltwiseBroadcastEnable != 0; };
    __aicore__ inline bool GetItaEnable() const { return itaEnable != 0; };
    __aicore__ inline bool GetBiasS32Enable() const { return biasS32Enable != 0; };
    __aicore__ inline bool GetHmaxEnable() const { return hmaxEnable != 0; };
    __aicore__ inline lsb_mask_t GetLsbMask() const { return static_cast<lsb_mask_t>(lsbMask); };
    __aicore__ inline bool GetGemvCtrl() const { return gemvCtrl != 0; };
    __aicore__ inline uint8_t GetGroupMode() const { return groupMode; };
    __aicore__ inline bool GetDependEnable() const { return dependEnable != 0; };
    __aicore__ inline instr_id_t GetInstrId() const { return static_cast<instr_id_t>(instrId); };
    __aicore__ inline bool GetPostProcHalfEnable() const { return postProcHalfEnable != 0; };
    __aicore__ inline bool GetLayoutCtrl() const { return layoutCtrl != 0; };
    __aicore__ inline uint16_t GetN() const { return n; };
};
} // namespace AscendC

#endif // __NPU_ARCH__
#endif // ASCENDC_MODULE_OPERATOR_MACRO_BITMODE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BITMODE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MACRO_BITMODE_IMPL_H__
#endif
