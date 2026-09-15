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
 * \file kernel_operator_mm_bitmode_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_mm_bitmode_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_mm_bitmode_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_BITMODE_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_MM_BITMODE_INTF_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MM_BITMODE_INTF_IMPL_H

namespace AscendC {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510))
__aicore__ inline Load2DBitModeParam::Load2DBitModeParam()
{
    config0 = 0;
    config1 = 0;
    ifTranspose = false;
};
__aicore__ inline Load2DBitModeParam::Load2DBitModeParam(const LoadData2DParamsV2& loadData2DParams_)
{
    config0BitMode.Init(loadData2DParams_);
    config1BitMode.Init(loadData2DParams_);
    ifTranspose = loadData2DParams_.ifTranspose;
};

__aicore__ inline void Load2DBitModeParam::SetMStartPosition(uint32_t mStartPosition_)
{
    config0BitMode.SetMStartPosition(mStartPosition_);
};
__aicore__ inline void Load2DBitModeParam::SetKStartPosition(uint32_t kStartPosition_)
{
    config0BitMode.SetKStartPosition(kStartPosition_);
};
__aicore__ inline void Load2DBitModeParam::SetMStep(uint16_t mStep_) { config0BitMode.SetMStep(mStep_); };
__aicore__ inline void Load2DBitModeParam::SetKStep(uint16_t kStep_) { config0BitMode.SetKStep(kStep_); };
__aicore__ inline void Load2DBitModeParam::SetSrcStride(int32_t srcStride_)
{
    config1BitMode.SetSrcStride(srcStride_);
};
__aicore__ inline void Load2DBitModeParam::SetDstStride(uint16_t dstStride_)
{
    config1BitMode.SetDstStride(dstStride_);
};
__aicore__ inline void Load2DBitModeParam::SetIfTranspose(bool ifTranspose_) { ifTranspose = ifTranspose_; };

__aicore__ inline uint32_t Load2DBitModeParam::GetMStartPosition() const { return config0BitMode.GetMStartPosition(); };
__aicore__ inline uint32_t Load2DBitModeParam::GetKStartPosition() const { return config0BitMode.GetKStartPosition(); };
__aicore__ inline uint16_t Load2DBitModeParam::GetMStep() const { return config0BitMode.GetMStep(); };
__aicore__ inline uint16_t Load2DBitModeParam::GetKStep() const { return config0BitMode.GetKStep(); };
__aicore__ inline int32_t Load2DBitModeParam::GetSrcStride() const { return config1BitMode.GetSrcStride(); };
__aicore__ inline uint16_t Load2DBitModeParam::GetDstStride() const { return config1BitMode.GetDstStride(); };

__aicore__ inline Load3DBitModeParam::Load3DBitModeParam()
{
    config0 = 0;
    config1 = 0;
};
template <typename T>
__aicore__ inline Load3DBitModeParam::Load3DBitModeParam(const LoadData3DParamsV2<T>& loadData3DParams_)
{
    config0BitMode.Init(loadData3DParams_);
    config1BitMode.Init(loadData3DParams_);
};

__aicore__ inline void Load3DBitModeParam::SetKExtension(uint16_t kStep_) { config0BitMode.SetKExtension(kStep_); };
__aicore__ inline void Load3DBitModeParam::SetMExtension(uint16_t mStep_) { config0BitMode.SetMExtension(mStep_); };
__aicore__ inline void Load3DBitModeParam::SetKStartPt(uint16_t kPos_) { config0BitMode.SetKStartPt(kPos_); };
__aicore__ inline void Load3DBitModeParam::SetMStartPt(uint16_t mPos_) { config0BitMode.SetMStartPt(mPos_); };
__aicore__ inline void Load3DBitModeParam::SetStrideW(uint8_t strideW_) { config1BitMode.SetStrideW(strideW_); };
__aicore__ inline void Load3DBitModeParam::SetStrideH(uint8_t strideH_) { config1BitMode.SetStrideH(strideH_); };
__aicore__ inline void Load3DBitModeParam::SetFilterW(uint8_t Wk_) { config1BitMode.SetFilterW(Wk_); };
__aicore__ inline void Load3DBitModeParam::SetFilterH(uint8_t Hk_) { config1BitMode.SetFilterH(Hk_); };
__aicore__ inline void Load3DBitModeParam::SetDilationFilterW(uint8_t dilationW_)
{
    config1BitMode.SetDilationFilterW(dilationW_);
};
__aicore__ inline void Load3DBitModeParam::SetDilationFilterH(uint8_t dilationH_)
{
    config1BitMode.SetDilationFilterH(dilationH_);
};
__aicore__ inline void Load3DBitModeParam::SetFilterSizeW(bool filterSizeW_)
{
    config1BitMode.SetFilterSizeW(filterSizeW_);
};
__aicore__ inline void Load3DBitModeParam::SetFilterSizeH(bool filterSizeH_)
{
    config1BitMode.SetFilterSizeH(filterSizeH_);
};
__aicore__ inline void Load3DBitModeParam::SetTranspose(bool transpose_) { config1BitMode.SetTranspose(transpose_); };
__aicore__ inline void Load3DBitModeParam::SetFMatrixCtrl(bool fmatrixCtrl_)
{
    config1BitMode.SetFMatrixCtrl(fmatrixCtrl_);
};
__aicore__ inline void Load3DBitModeParam::SetChannelSize(uint16_t sizeChannel_)
{
    config1BitMode.SetChannelSize(sizeChannel_);
};

__aicore__ inline uint16_t Load3DBitModeParam::GetKExtension() const { return config0BitMode.GetKExtension(); };
__aicore__ inline uint16_t Load3DBitModeParam::GetMExtension() const { return config0BitMode.GetMExtension(); };
__aicore__ inline uint16_t Load3DBitModeParam::GetKStartPt() const { return config0BitMode.GetKStartPt(); };
__aicore__ inline uint16_t Load3DBitModeParam::GetMStartPt() const { return config0BitMode.GetMStartPt(); };
__aicore__ inline uint8_t Load3DBitModeParam::GetStrideW() const { return config1BitMode.GetStrideW(); };
__aicore__ inline uint8_t Load3DBitModeParam::GetStrideH() const { return config1BitMode.GetStrideH(); };
__aicore__ inline uint8_t Load3DBitModeParam::GetFilterW() const { return config1BitMode.GetFilterW(); };
__aicore__ inline uint8_t Load3DBitModeParam::GetFilterH() const { return config1BitMode.GetFilterH(); };
__aicore__ inline uint8_t Load3DBitModeParam::GetDilationFilterW() const
{
    return config1BitMode.GetDilationFilterW();
};
__aicore__ inline uint8_t Load3DBitModeParam::GetDilationFilterH() const
{
    return config1BitMode.GetDilationFilterH();
};
__aicore__ inline bool Load3DBitModeParam::GetFilterSizeW() const { return config1BitMode.GetFilterSizeW(); };
__aicore__ inline bool Load3DBitModeParam::GetFilterSizeH() const { return config1BitMode.GetFilterSizeH(); };
__aicore__ inline bool Load3DBitModeParam::GetTranspose() const { return config1BitMode.GetTranspose(); };
__aicore__ inline bool Load3DBitModeParam::GetFMatrixCtrl() const { return config1BitMode.GetFMatrixCtrl(); };
__aicore__ inline uint16_t Load3DBitModeParam::GetChannelSize() const { return config1BitMode.GetChannelSize(); };

__aicore__ inline SetFMatrixBitModeParams::SetFMatrixBitModeParams() { config0 = 0; };
template <typename T>
__aicore__ inline SetFMatrixBitModeParams::SetFMatrixBitModeParams(const LoadData3DParamsV2<T>& loadData3DParams_)
{
    config0BitMode.Init(loadData3DParams_);
};

__aicore__ inline void SetFMatrixBitModeParams::SetL1H(uint16_t l1H_) { config0BitMode.SetL1H(l1H_); };
__aicore__ inline void SetFMatrixBitModeParams::SetL1W(uint16_t l1W_) { config0BitMode.SetL1W(l1W_); };
__aicore__ inline void SetFMatrixBitModeParams::SetPadList(const uint8_t padList_[4])
{
    config0BitMode.SetPadList(padList_);
};
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
__aicore__ inline MmadBitModeParams::MmadBitModeParams() { config0 = 0; };
__aicore__ inline MmadBitModeParams::MmadBitModeParams(const MmadParams& mmadParams_)
{
    config0BitMode.Init(mmadParams_);
};

__aicore__ inline void MmadBitModeParams::SetM(uint16_t m_) { config0BitMode.SetM(m_); };
__aicore__ inline void MmadBitModeParams::SetK(uint16_t k_) { config0BitMode.SetK(k_); };
__aicore__ inline void MmadBitModeParams::SetN(uint16_t n_) { config0BitMode.SetN(n_); };
__aicore__ inline void MmadBitModeParams::SetUnitFlag(uint8_t unitFlag_) { config0BitMode.SetUnitFlag(unitFlag_); };
__aicore__ inline void MmadBitModeParams::SetDisableGemv(bool disableGemv_)
{
    config0BitMode.SetDisableGemv(disableGemv_);
};
__aicore__ inline void MmadBitModeParams::SetCmatrixSource(bool cmatrixSource_)
{
    config0BitMode.SetCmatrixSource(cmatrixSource_);
};
__aicore__ inline void MmadBitModeParams::SetCmatrixInitVal(bool cmatrixInitVal_)
{
    config0BitMode.SetCmatrixInitVal(cmatrixInitVal_);
};

__aicore__ inline uint16_t MmadBitModeParams::GetM() const { return config0BitMode.GetM(); };
__aicore__ inline uint16_t MmadBitModeParams::GetK() const { return config0BitMode.GetK(); };
__aicore__ inline uint16_t MmadBitModeParams::GetN() const { return config0BitMode.GetN(); };
__aicore__ inline uint8_t MmadBitModeParams::GetUnitFlag() const { return config0BitMode.GetUnitFlag(); };
__aicore__ inline bool MmadBitModeParams::GetDisableGemv() const { return config0BitMode.GetDisableGemv(); };
__aicore__ inline bool MmadBitModeParams::GetCmatrixSource() const { return config0BitMode.GetCmatrixSource(); };
__aicore__ inline bool MmadBitModeParams::GetCmatrixInitVal() const { return config0BitMode.GetCmatrixInitVal(); };
#endif
} // namespace AscendC

#endif // ASCENDC_MODULE_OPERATOR_MM_BITMODE_INTF_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_BITMODE_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_BITMODE_INTF_IMPL_H__
#endif
