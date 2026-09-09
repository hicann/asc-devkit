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
 * \file copy_cube_out_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_out/copy_cube_out_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_UTILS_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_UTILS_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_UTILS_H

namespace AscendC {
namespace Impl {
namespace Detail {

#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) || (__NPU_ARCH__ == 5102)
const static uint8_t FIX_PIPE_UNIT_FLAG = 3;

template <class A_TYPE, class C_TYPE, const auto& MM_CFG, FixpipeParamsType version>
struct FixpipeParamsUtil {
    using DstT = typename C_TYPE::T;
    using SrcT = typename GetMmDstType<typename A_TYPE::T>::Type;
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
    using TYPE = typename AscendC::Conditional<
        C_TYPE::format == CubeFormat::NZ, FixpipeParamsArch3510<CO2Layout::NZ>,
        typename AscendC::Conditional<
            C_TYPE::format == CubeFormat::COLUMN_MAJOR, FixpipeParamsArch3510<CO2Layout::COLUMN_MAJOR>,
            FixpipeParamsArch3510<CO2Layout::ROW_MAJOR>>::type>::type;
#else
    using TYPE = FixpipeParamsV220;
#endif
public:
    __aicore__ inline ~FixpipeParamsUtil() = default;

    __aicore__ inline FixpipeParamsUtil(
        int32_t nSize, int32_t mSize, int32_t nSizeBlock, int32_t mSizeBlock, int32_t baseHeight, int32_t dstStride)
    {
        if constexpr (C_TYPE::format == CubeFormat::ND || C_TYPE::format == CubeFormat::COLUMN_MAJOR) {
            params_.nSize = static_cast<uint16_t>(nSize);
        } else if constexpr (C_TYPE::format == CubeFormat::ND_ALIGN) {
            constexpr uint32_t blockCount = ONE_BLK_SIZE / sizeof(DstT);
            params_.nSize = static_cast<uint16_t>(Ceil(nSize, blockCount) * blockCount);
            if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB()) {
                dstStride = static_cast<uint16_t>(Ceil(dstStride, blockCount) * blockCount);
            }
        } else if constexpr (C_TYPE::format == CubeFormat::NZ) {
            if constexpr (!ToMatmulConfig(MM_CFG).isEnableChannelSplit) {
                params_.nSize = static_cast<uint16_t>(nSizeBlock * BLOCK_CUBE);
            } else {
                params_.nSize = static_cast<uint16_t>(nSize);
                params_.isChannelSplit = true;
            }
        }
        params_.mSize = static_cast<uint16_t>(mSize);
        params_.srcStride = CeilAlign((IsStaticPaddingEnable(MM_CFG) ? baseHeight : mSize), BLOCK_CUBE);
        params_.dstStride = dstStride;
        if constexpr (EnUnitFlag(MM_CFG)) {
            params_.unitFlag = FIX_PIPE_UNIT_FLAG;
        }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        if constexpr (C_TYPE::format == CubeFormat::COLUMN_MAJOR) {
            params_.params = {1, 0, 0, 1};
        }
#endif

#if __NPU_ARCH__ == 5102
        if constexpr (C_TYPE::format == CubeFormat::COLUMN_MAJOR) {
            params_.params = {1, params_.mSize, params_.mSize, 1};
        } else if constexpr (C_TYPE::format == CubeFormat::ND || C_TYPE::format == CubeFormat::ND_ALIGN) {
            params_.params = {1, params_.mSize, params_.mSize};
        }
#endif
        if constexpr (IsEnableRelu(MM_CFG)) {
            params_.reluEn = true;
        }
    }

#if __NPU_ARCH__ == 5102
    __aicore__ inline void SetFixShiftValue(uint8_t value) { params_.fixShiftVal = value; }
#endif

    __aicore__ inline void SetQuantMode(QuantMode_t quantMode) { params_.quantPre = quantMode; }

    __aicore__ inline void SetQuantScalar(uint64_t scalar) { params_.deqScalar = scalar; }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
    __aicore__ inline void SetMcgShfMode(McgShfMode mode) { params_.dualDstCtl = static_cast<uint8_t>(mode); }

    __aicore__ inline void SetSubBlockId(uint8_t id) { params_.subBlockId = id; }
#endif

    __aicore__ inline void SetNdParams(
        int32_t ndNum, int32_t baseHeight, int32_t baseWidth, int32_t baseBlockWidth, int32_t baseM, int32_t baseN)
    {
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
        params_.params.ndNum = static_cast<uint16_t>(ndNum);
        params_.params.srcNdStride = static_cast<uint16_t>(baseM * baseBlockWidth);
        if constexpr ((C_TYPE::layout == LayoutMode::BSNGD) || (C_TYPE::layout == LayoutMode::SBNGD)) {
            params_.params.dstNdStride = static_cast<uint16_t>(baseWidth);
        } else {
            params_.params.dstNdStride = static_cast<uint16_t>(baseHeight * baseWidth);
        }
#else
        constexpr static int32_t fractalSize = 1024;
        params_.ndNum = static_cast<uint16_t>(ndNum);
        params_.srcNdStride = static_cast<uint16_t>(baseM * baseN * sizeof(SrcT) / fractalSize);
        if constexpr ((C_TYPE::layout == LayoutMode::BSNGD) || (C_TYPE::layout == LayoutMode::SBNGD)) {
            params_.dstNdStride = static_cast<uint16_t>(baseWidth);
        } else {
            params_.dstNdStride = static_cast<uint16_t>(baseHeight * baseWidth);
        }
#endif
    }

    template <typename T>
    __aicore__ inline void FixpipeOut(
        const T& dst, const LocalTensor<SrcT>& colLocal, const LocalTensor<uint64_t>& quantTensor)
    {
        if constexpr (C_TYPE::format == CubeFormat::NZ) {
            if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB() && PhyPosIsUB(C_TYPE::pos)) {
                Fixpipe<DstT, SrcT, CFG_NZ_UB>(dst, colLocal, quantTensor, params_);
            } else {
                Fixpipe<DstT, SrcT, CFG_NZ>(dst, colLocal, quantTensor, params_);
            }
        } else if constexpr (C_TYPE::format == CubeFormat::COLUMN_MAJOR) {
            Fixpipe<DstT, SrcT, CFG_COLUMN_MAJOR>(dst, colLocal, quantTensor, params_);
        } else {
            if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB() && PhyPosIsUB(C_TYPE::pos)) {
                Fixpipe<DstT, SrcT, CFG_ROW_MAJOR_UB>(dst, colLocal, quantTensor, params_);
            } else {
                Fixpipe<DstT, SrcT, CFG_ROW_MAJOR>(dst, colLocal, quantTensor, params_);
            }
        }
    }

    template <typename T>
    __aicore__ inline void FixpipeOut(const T& dst, const LocalTensor<SrcT>& colLocal)
    {
        if constexpr (C_TYPE::format == CubeFormat::NZ) {
            if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB() && PhyPosIsUB(C_TYPE::pos)) {
                Fixpipe<DstT, SrcT, CFG_NZ_UB>(dst, colLocal, params_);
            } else {
                Fixpipe<DstT, SrcT, CFG_NZ>(dst, colLocal, params_);
            }
        } else if constexpr (C_TYPE::format == CubeFormat::COLUMN_MAJOR) {
            Fixpipe<DstT, SrcT, CFG_COLUMN_MAJOR>(dst, colLocal, params_);
        } else {
            if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB() && PhyPosIsUB(C_TYPE::pos)) {
                Fixpipe<DstT, SrcT, CFG_ROW_MAJOR_UB>(dst, colLocal, params_);
            } else {
                Fixpipe<DstT, SrcT, CFG_ROW_MAJOR>(dst, colLocal, params_);
            }
        }
    }

    __aicore__ inline constexpr void SetCastMode()
    {
        if constexpr (IsSameType<DstT, half>::value && IsSameType<SrcT, float>::value) {
            params_.quantPre = QuantMode_t::F322F16;
        } else if constexpr (IsSameType<DstT, bfloat16_t>::value && IsSameType<SrcT, float>::value) {
            params_.quantPre = QuantMode_t::F322BF16;
        }
    }

public:
    TYPE params_;
};
#endif

__aicore__ inline void CopyOutEnQue()
{
    event_t eventIDMte3ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_V));
    SetFlag<HardEvent::MTE3_V>(eventIDMte3ToV);
    WaitFlag<HardEvent::MTE3_V>(eventIDMte3ToV);
}

__aicore__ inline void CopyOutDeQue()
{
    event_t eventIDVToMte3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
    SetFlag<HardEvent::V_MTE3>(eventIDVToMte3);
    WaitFlag<HardEvent::V_MTE3>(eventIDVToMte3);
}

__aicore__ inline void CopyLocal2GMNZ2NDEnQue()
{
    event_t eventIDSToMte3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_MTE3));
    SetFlag<HardEvent::S_MTE3>(eventIDSToMte3);
    WaitFlag<HardEvent::S_MTE3>(eventIDSToMte3);
}

__aicore__ inline void CopyLocal2GMNZ2NDDeQue()
{
    event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_S));
    SetFlag<HardEvent::MTE3_S>(eventID);
    WaitFlag<HardEvent::MTE3_S>(eventID);
}

__aicore__ inline void CopyTrans2GMEnQue()
{
    auto eventIDVToS = GetTPipePtr()->FetchEventID(HardEvent::V_S);
    SetFlag<HardEvent::V_S>(eventIDVToS);
    WaitFlag<HardEvent::V_S>(eventIDVToS);
}
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_UTILS_H__
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_UTILS_H
