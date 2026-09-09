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
 * \file quant_processor_fixpipe.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_out/quant/quant_processor_fixpipe.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_FIXPIPE_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_FIXPIPE_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_FIXPIPE_H

#include "../../../utils/matmul_module.h"
#include "../../../utils/matmul_param.h"
#include "quant_processor_intf.h"
#include "quant_processor_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, class A_TYPE, class C_TYPE, const auto& MM_CFG>
class MatmulQuantProcessor<
    IMPL, A_TYPE, C_TYPE, MM_CFG,
    enable_if_t<(
        IsQuantSenario<typename C_TYPE::T, typename A_TYPE::T>() && !MatmulFeatureTrait<MM_CFG>::IsNeedUB() &&
        !HasScalePosition<A_TYPE>::value)>> {
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(L1Manager);

    using SrcT = typename A_TYPE::T;
    using DstT = typename C_TYPE::T;
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;

public:
    __aicore__ inline MatmulQuantProcessor() {}
    __aicore__ inline ~MatmulQuantProcessor() {}

    __aicore__ inline void Init(const int32_t baseN)
    {
        baseN_ = baseN;
        isPerChannel_ = false;
        isPerTensor_ = false;
        // l1 split to upper and lower half
        if constexpr (ToMatmulConfig(MM_CFG).enableL1BankConflictOptimise) {
            auto initBufferSize = baseN_ * sizeof(int64_t);
            auto addr0 = AscendC::Std::make_tuple(MATMUL_MODULE(L1Manager)->GetCurrentLowerHalfAddr(), initBufferSize);
            GetTPipePtr()->InitBuffer(qidFixPipe_, addr0);
            MATMUL_MODULE(L1Manager)->SetCurrentAddress(0, initBufferSize);
        } else {
            GetTPipePtr()->InitBuffer(qidFixPipe_, 1, baseN_ * sizeof(int64_t));
        }
    }

    __aicore__ inline QuantMode_t GetMatmulQuantMode() { return quantMode_; }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
    __aicore__ inline bool IsQuantSenario() { return isPerChannel_ || isPerTensor_; }
#endif

    __aicore__ inline void SetQuantVector(const GlobalTensor<uint64_t>& quantTensor)
    {
        isPerChannel_ = true;
        quantTensor_ = quantTensor;
        SetQuantVectorMode();
    }

    __aicore__ inline void SetQuantVector(const LocalTensor<uint64_t>& quantTensor)
    {
        isPerChannel_ = true;
        quantTensorL1_ = quantTensor;
        isQuantL1_ = true;
        SetQuantVectorMode();
    }

    __aicore__ inline void SetQuantScalar(const uint64_t quantScalar)
    {
        isPerTensor_ = true;
        quantScalar_ = quantScalar;

        if constexpr (IsSameTypeV<L0cT, int32_t> && IsSameTypeV<DstT, half>) {
            quantMode_ = QuantMode_t::DEQF16;
        } else if constexpr (IsSameTypeV<L0cT, int32_t> && IsTypeOneOfV<DstT, int8_t, uint8_t>) {
            quantMode_ = QuantMode_t::REQ8;
        } else if constexpr (IsSameTypeV<L0cT, float> && IsTypeOneOfV<DstT, int8_t, uint8_t>) {
            quantMode_ = QuantMode_t::QF322B8_PRE;
        }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        else if constexpr (IsSameTypeV<L0cT, int32_t> && IsSameTypeV<DstT, bfloat16_t>) {
            quantMode_ = QuantMode_t::QS322BF16_PRE;
        } else if constexpr (IsSameTypeV<L0cT, float> && IsTypeOneOfV<DstT, fp8_e4m3fn_t, fp8_e5m2_t>) {
            quantMode_ = QuantMode_t::QF322FP8_PRE;
        } else if constexpr (IsSameTypeV<L0cT, float> && IsSameTypeV<DstT, hifloat8_t>) {
            quantMode_ = QuantMode_t::QF322HIF8_PRE;
        } else if constexpr (IsTypeOneOfV<SrcT, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t> && IsSameTypeV<DstT, half>) {
            quantMode_ = QuantMode_t::QF322F16_PRE;
        } else if constexpr (
            IsTypeOneOfV<SrcT, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t> && IsSameTypeV<DstT, bfloat16_t>) {
            quantMode_ = QuantMode_t::QF322BF16_PRE;
        } else if (IsTypeOneOfV<SrcT, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t> && IsSameTypeV<DstT, float>) {
            quantMode_ = QuantMode_t::QF322F32_PRE;
        }
#endif
    }

    __aicore__ inline void CopyQuantTensor(
        LocalTensor<uint64_t>& quantTensor, const int32_t curN, const int32_t baseUseN)
    {
        if (isPerChannel_) {
            int64_t quantTensorGMOffset;
            if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB() && ToMatmulConfig(MM_CFG).isPartialOutput) {
                quantTensorGMOffset =
                    (curN * MATMUL_MODULE(KLoop)->GetInnerIter() + MATMUL_MODULE(KLoop)->GetInnerIdx()) * baseN_;
            } else {
                quantTensorGMOffset = curN * baseN_;
            }
            int32_t quantLen;
            if constexpr (C_TYPE::format == CubeFormat::ND || C_TYPE::format == CubeFormat::ND_ALIGN) {
                quantLen = baseUseN;
            } else {
                quantLen = DivCeil(baseUseN, BLOCK_CUBE) * BLOCK_CUBE;
            }
            if (isQuantL1_) {
                quantTensor = quantTensorL1_[quantTensorGMOffset];
                quantTensor.SetSize(quantLen);
            } else {
                quantTensor = qidFixPipe_.template AllocTensor<uint64_t>();
                CopyDeqTensorToL1(quantTensor, quantTensor_[quantTensorGMOffset], quantLen);
                qidFixPipe_.EnQue(quantTensor);
                qidFixPipe_.DeQue();
            }
        }
    }

    __aicore__ inline uint64_t GetQuantScalarValue() { return quantScalar_; }

    __aicore__ inline void UpdateQuantTensor(int32_t idx) { quantTensor_ = quantTensor_[idx]; }

    __aicore__ inline bool IsPerChannelSenario() { return isPerChannel_; }

    __aicore__ inline void FreeQuantTensor(LocalTensor<uint64_t>& quantTensor)
    {
        if (isPerChannel_ && !isQuantL1_) {
            qidFixPipe_.FreeTensor(quantTensor);
        }
    }

    __aicore__ inline void Destroy()
    {
        if constexpr (
            ((IsSameType<SrcT, int8_t>::value || IsSameType<SrcT, int4b_t>::value) && IsSameType<DstT, half>::value) ||
            (IsSameType<SrcT, int8_t>::value &&
             (IsSameType<DstT, int8_t>::value || IsSameType<DstT, uint8_t>::value))) {
            qidFixPipe_.FreeAllEvent();
        }
    }

private:
    __aicore__ inline void CopyDeqTensorToL1(
        const LocalTensor<uint64_t>& dst, const GlobalTensor<uint64_t>& src, int32_t calNSize)
    {
        event_t eventIDFixToMte2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::FIX_MTE2));
        SetFlag<HardEvent::FIX_MTE2>(eventIDFixToMte2);
        WaitFlag<HardEvent::FIX_MTE2>(eventIDFixToMte2);
        constexpr int DEQ_SIZE = 128;
        uint16_t deqDataSize = DivCeil(calNSize * sizeof(uint64_t), DEQ_SIZE) * DEQ_SIZE;
        // GM -> L1
        if (calNSize % BLOCK_CUBE) {
            // nd2nz pad to 32Bytes align
            uint16_t dValue = calNSize * FLOAT_FACTOR;
            Nd2NzParams intriParams{1, 1, dValue, 0, dValue, 1, 1, 0};
            GlobalTensor<uint32_t> srcTmp;
            srcTmp.SetGlobalBuffer((__gm__ uint32_t*)src.GetPhyAddr(), src.GetSize());
            DataCopy(dst.ReinterpretCast<uint32_t>(), srcTmp, intriParams);
        } else {
            DataCopyParams intriParams{1, static_cast<uint16_t>(deqDataSize / ONE_BLK_SIZE), 0, 0};
            DataCopy(dst, src, intriParams);
        }
        event_t eventIDMte2ToFix = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_FIX));
        SetFlag<HardEvent::MTE2_FIX>(eventIDMte2ToFix);
        WaitFlag<HardEvent::MTE2_FIX>(eventIDMte2ToFix);
    }

    __aicore__ inline void SetQuantVectorMode()
    {
        if constexpr (IsSameTypeV<L0cT, int32_t> && IsSameTypeV<DstT, half>) {
            quantMode_ = QuantMode_t::VDEQF16;
        } else if constexpr (IsSameTypeV<L0cT, int32_t> && IsTypeOneOfV<DstT, int8_t, uint8_t>) {
            quantMode_ = QuantMode_t::VREQ8;
        } else if constexpr (IsSameTypeV<L0cT, float> && IsTypeOneOfV<DstT, int8_t, uint8_t>) {
            quantMode_ = QuantMode_t::VQF322B8_PRE;
        }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        else if constexpr (IsSameTypeV<L0cT, int32_t> && IsSameTypeV<DstT, bfloat16_t>) {
            quantMode_ = QuantMode_t::VQS322BF16_PRE;
        } else if constexpr (IsSameTypeV<L0cT, float> && IsTypeOneOfV<DstT, fp8_e4m3fn_t, fp8_e5m2_t>) {
            quantMode_ = QuantMode_t::VQF322FP8_PRE;
        } else if constexpr (IsSameTypeV<L0cT, float> && IsSameTypeV<DstT, hifloat8_t>) {
            quantMode_ = QuantMode_t::VQF322HIF8_PRE;
        } else if constexpr (IsTypeOneOfV<SrcT, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t> && IsSameTypeV<DstT, half>) {
            quantMode_ = QuantMode_t::VQF322F16_PRE;
        } else if constexpr (
            IsTypeOneOfV<SrcT, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t> && IsSameTypeV<DstT, bfloat16_t>) {
            quantMode_ = QuantMode_t::VQF322BF16_PRE;
        } else if (IsTypeOneOfV<SrcT, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t> && IsSameTypeV<DstT, float>) {
            quantMode_ = QuantMode_t::VQF322F32_PRE;
        }
#endif
    }

private:
    bool isPerTensor_ = false;
    bool isPerChannel_ = false;
    bool isQuantL1_ = false;
    QuantMode_t quantMode_ = QuantMode_t::NoQuant;
    TQue<TPosition::C1, QUEUE_DEPTH> qidFixPipe_;
    GlobalTensor<uint64_t> quantTensor_;
    LocalTensor<uint64_t> quantTensorL1_;
    uint64_t quantScalar_ = 0;
    int32_t baseN_ = 0;
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_FIXPIPE_H

#if defined( \
    __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_FIXPIPE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_QUANT_QUANT_PROCESSOR_FIXPIPE_H__
#endif
