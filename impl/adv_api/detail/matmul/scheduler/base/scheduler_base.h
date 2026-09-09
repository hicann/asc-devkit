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
 * \file scheduler_base.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/scheduler/base/scheduler_base.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_BASE_H__
#endif

#ifndef IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_BASE_H
#define IMPL_MATMUL_SCHEDULER_BASE_SCHEDULER_BASE_H

#include "../../utils/matmul_module.h"
#include "../../utils/matmul_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {

/*
    MatmulSchedulerBase is considered entirely experimental.
    We retain the freedom to make incompatible changes, but do not guarantee the stability.
    MatmulSchedulerBase is only for internal usage, does not support extension or customized specialization!
*/

/*
    MatmulSchedulerBase is the base class for other specialized MatmulScheduler,
    it implements the common GetResult methods.
*/
template <
    typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE, const auto& MM_CFG,
    PolicyType POLICY_TYPE = PolicyType::MATMUL_DEFAULT, typename = void>
class MatmulSchedulerBase {
public:
    MATMUL_USE_MODULE(MLoop);
    MATMUL_USE_MODULE(NLoop);
    MATMUL_USE_MODULE(KLoop);
    MATMUL_USE_MODULE(CopyCubeOut);
    MATMUL_USE_MODULE(CubeOutBuffer);
    MATMUL_USE_MODULE(CopyCubeInA);
    MATMUL_USE_MODULE(CopyCubeInB);
    MATMUL_USE_MODULE(BiasScheduler);
    MATMUL_USE_MODULE(TBufPoolL0);
    MATMUL_USE_MODULE(MatmulSubBlockInfo);
    MATMUL_USE_MODULE(MatmulQuantProcessor);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulCrossCoreSync);
    MATMUL_USE_MODULE(QtableProcessor);

    using DstT = typename C_TYPE::T;
    using L0cT = typename GetMmDstType<typename A_TYPE::T>::Type;
    using SrcAT = typename A_TYPE::T;
    using SrcBT = typename B_TYPE::T;

    __aicore__ inline void Init(const TCubeTiling* __restrict cubeTiling, TPipe* tpipe)
    {
        if constexpr (!NormInitScene<MM_CFG> && !MdlInitScene<MM_CFG> && !DoMatmulIBShareNorm(MM_CFG)) {
            ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Unsupported matmul version."); });
            return;
        }
        static_assert(
            !(ToMatmulConfig(MM_CFG).doNorm && ToMatmulConfig(MM_CFG).doIBShareNorm),
            "Neither Norm nor IBShareNorm can be enabled simultaneously!");
        MATMUL_MODULE(MatmulShapeTiling)->SetTiling(cubeTiling);
        MATMUL_MODULE(MatmulShapeTiling)->template CheckTiling<SrcAT, L0cT>();
        auto& var = MATMUL_PARAM_VAR;
        var.tpipe_ = tpipe;

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3002)
        MATMUL_MODULE(MatmulSubBlockInfo)->SetSubBlockIdx(0);
#endif

        auto shapeInfo = MATMUL_MODULE(MatmulShapeInfo);
        shapeInfo->InitParams();
        if (shapeInfo->GetSingleCoreM() > 0 && shapeInfo->GetSingleCoreN() > 0 && shapeInfo->GetSingleCoreK() > 0) {
            MATMUL_MODULE(MLoop)->Init(shapeInfo->GetSingleCoreM());
            MATMUL_MODULE(NLoop)->Init(shapeInfo->GetSingleCoreN());
            MATMUL_MODULE(KLoop)->Init(shapeInfo->GetSingleCoreK());
        }

        MATMUL_MODULE(TBufPoolL0)->Init();

        if constexpr (unlikely(Impl::Detail::MatmulFeatureTrait<MM_CFG>::IsUnitFlagEnabled())) {
            SetMMLayoutTransform(C_TYPE::format == CubeFormat::ND);
        }

        const auto& tiling = MATMUL_MODULE(MatmulShapeTiling)->GetTiling();
        uint32_t shareUbSize = static_cast<uint32_t>(tiling.GetShareUbSize());
        // shareL1Size, shareL0CSize, shareUbSize
        int32_t sharedL0cSize = PhyPosIsL0C(C_TYPE::pos) ? 0 : tiling.GetShareL0CSize();
        uint32_t shareLens[SHARE_LEN_SIZE] = {
            static_cast<uint32_t>(tiling.GetShareL1Size()), static_cast<uint32_t>(sharedL0cSize), shareUbSize};
        InitShareBufStart(
            var.tpipe_, tiling.GetShareMode(), shareLens, SHARE_LEN_SIZE,
            MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx());
        MATMUL_MODULE(CopyCubeInA)->Init();
        MATMUL_MODULE(CopyCubeInB)->Init();
        auto baseMN = IsBasicBlockEnable<MM_CFG> ? ToMatmulConfig(MM_CFG).basicM * ToMatmulConfig(MM_CFG).basicN :
                                                   tiling.GetBaseM() * tiling.GetBaseN();

        uint32_t lenFactor = 1;
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) || (__NPU_ARCH__ == 5102)
        if constexpr (MdlInitScene<MM_CFG> && ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
            lenFactor = DOUBLE_SIZE;
        }
#endif
        MATMUL_MODULE(CubeOutBuffer)->Init(baseMN, lenFactor);
        if constexpr (NormInitScene<MM_CFG>) {
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) || (__NPU_ARCH__ == 5102)
            MATMUL_MODULE(BiasScheduler)->Init();
#endif
        } else {
            MATMUL_MODULE(BiasScheduler)->Init();
        }
        MATMUL_MODULE(MatmulQuantProcessor)->Init(tiling.GetBaseN());
#if __NPU_ARCH__ == 5102
        if constexpr (IsSameTypeV<SrcAT, half> && IsSameTypeV<SrcBT, half> && IsTypeOneOfV<DstT, half, bfloat16_t>) {
            constexpr float FIX_VAL_RECIPROCAL = 1.0f;
            const uint64_t quantScalar =
                static_cast<const uint64_t>(*reinterpret_cast<const int32_t*>(&FIX_VAL_RECIPROCAL));
            MATMUL_MODULE(MatmulQuantProcessor)->SetQuantScalar(quantScalar);
        }
        if constexpr (IsDecompMode<MM_CFG>()) {
            InitQtableProcessor();
        }
#endif

        if constexpr (MdlInitScene<MM_CFG>) {
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 1001
            var.tpipe_->InitBuffer(var.qidA2_, 1, L0ASize_);
            var.tpipe_->InitBuffer(var.qidB2_, 1, L0BSize_);
#endif
        }
        if constexpr (!HasScalePosition<A_TYPE>::value) {
            InitShareBufEnd(var.tpipe_);
        }
    }
#if !defined(ASCENDC_CPU_DEBUG) && defined(__CCE_IS_AICORE__)
    __aicore__ inline void Init(const __gm__ TCubeTiling* gmCubeTiling, TPipe* tpipe)
    {
        TCubeTiling cubeTiling;
        CopyTiling<A_TYPE, B_TYPE, MM_CFG>(gmCubeTiling, cubeTiling);
        Init(&cubeTiling, tpipe);
    }
#endif
    __aicore__ inline void End()
    {
        if constexpr (!NormInitScene<MM_CFG> && !MdlInitScene<MM_CFG> && !DoMatmulIBShareNorm(MM_CFG)) {
            ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Unsupported matmul version."); });
            return;
        }
        MATMUL_MODULE(CopyCubeInA)->Destroy();
        MATMUL_MODULE(CopyCubeInB)->Destroy();
        MATMUL_MODULE(BiasScheduler)->End();
        MATMUL_MODULE(TBufPoolL0)->ResetCache();
        MATMUL_MODULE(CubeOutBuffer)->Destroy();
        MATMUL_MODULE(MatmulQuantProcessor)->Destroy();
        if constexpr (MatmulFeatureTrait<MM_CFG>::IsSupportUBToL1Singleshape()) {
            MATMUL_MODULE(MatmulCrossCoreSync)->End();
        }
        if constexpr (
            unlikely(Impl::Detail::MatmulFeatureTrait<MM_CFG>::IsUnitFlagEnabled()) &&
            C_TYPE::format == CubeFormat::ND) {
            SetMMLayoutTransform(0);
        }
    }

    __aicore__ inline int64_t GetL0cOffset()
    {
        auto baseHeight = MATMUL_MODULE(MLoop)->GetBaseShape();
        auto baseWidth = MATMUL_MODULE(NLoop)->GetBaseShape();
        baseWidth = CeilAlign(baseWidth, BLOCK_CUBE);
        baseHeight = CeilAlign(baseHeight, BLOCK_CUBE);
        return baseHeight * baseWidth;
    }

    __aicore__ inline bool ScheduleOnce(bool enPartialSum)
    {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Matching error. This is an empty implementation."); });
        return false;
    }

    __aicore__ inline void Schedule(
        const GlobalTensor<DstT>& gm, uint8_t enAtomic, bool enSequentialWrite, bool fakeMsg)
    {}

    template <class T>
    __aicore__ inline bool Schedule(const T& dst, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Matching error. This is an empty implementation."); });
        return false;
    }

    __aicore__ inline void Reset()
    {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Matching error. This is an empty implementation."); });
    }

    __aicore__ inline void GetResult(
        const LocalTensor<DstT>& co2Local, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        static_assert(ToMatmulConfig(MM_CFG).scheduleType != ScheduleType::OUTER_PRODUCT, "Unsupported scheduleType");
        GetResultImpl(co2Local, enAtomic, enSequentialWrite);
    }

    __aicore__ inline void GetResult(const GlobalTensor<DstT>& gm, uint8_t enAtomic = 0, bool enSequentialWrite = false)
    {
        GetResultImpl(gm, enAtomic, enSequentialWrite);
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
    __aicore__ inline void GetResult(
        const GlobalTensor<DstT>& gm, const LocalTensor<DstT>& co2Local, uint8_t enAtomic = 0,
        bool enSequentialWrite = false)
    {
        static_assert(ToMatmulConfig(MM_CFG).scheduleType != ScheduleType::OUTER_PRODUCT, "Unsupported scheduleType");
        GetResultImpl(gm, co2Local, enAtomic, enSequentialWrite);
    }
#endif
private:
    constexpr static uint8_t multiOfB16b8 = 2;
    constexpr static uint8_t multiOfB16b4 = 4;

protected:
    __aicore__ inline void StaticPadCommon(
        const LocalTensor<uint16_t>& padTensor, const int32_t repeatTimes, const int32_t blockNum, const int32_t dstGap,
        const int32_t offset)
    {
        InitConstValueParams<uint16_t> initConstValueParams;
        initConstValueParams.repeatTimes = repeatTimes;
        initConstValueParams.blockNum = blockNum;
        initConstValueParams.dstGap = dstGap;
        initConstValueParams.initValue = 0;
        InitConstValue(padTensor[offset], initConstValueParams);
    }

    __aicore__ inline void PadZeroForAInL1(const LocalTensor<SrcAT>& a1)
    {
        int32_t aSingleHeight;
        int32_t aSingleWidth;
        int32_t aSingleCeilHeight;
        int32_t aSingleCeilWidth;
        constexpr static int32_t c0Size_ = AuxGetC0Size<SrcAT>();
        if constexpr (A_TYPE::isTrans) {
            aSingleHeight = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            aSingleCeilHeight = CeilAlign(aSingleHeight, MX_BASEK_FACTOR);
            aSingleWidth = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM();
            int32_t tileWidthC0 = Ceil(aSingleWidth, c0Size_);
            auto padTensor = a1.template ReinterpretCast<uint16_t>();
            if (aSingleCeilHeight > aSingleHeight) {
                if constexpr (IsSupportB8<SrcAT>() && !IsSameTypeV<SrcAT, int8_t>) {
                    StaticPadCommon(
                        padTensor, tileWidthC0, aSingleCeilHeight - aSingleHeight, aSingleHeight,
                        aSingleHeight * c0Size_ / multiOfB16b8);
                } else if constexpr (IsSupportB4<SrcAT>() && !IsSameTypeV<SrcAT, int4b_t>) {
                    StaticPadCommon(
                        padTensor, tileWidthC0, aSingleCeilHeight - aSingleHeight, aSingleHeight,
                        aSingleHeight * c0Size_ / multiOfB16b4);
                }
            }
        } else {
            aSingleHeight = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreM();
            aSingleCeilHeight = CeilAlign(aSingleHeight, BLOCK_CUBE);
            aSingleWidth = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            aSingleCeilWidth = CeilAlign(aSingleWidth, MX_BASEK_FACTOR);
            int32_t tileWidthC0 = Ceil(aSingleWidth, c0Size_);
            int32_t staticWidthC0 = Ceil(aSingleCeilWidth, c0Size_);
            auto padTensor = a1.template ReinterpretCast<uint16_t>();
            if (staticWidthC0 > tileWidthC0) {
                if constexpr (IsSupportB8<SrcAT>() && !IsSameTypeV<SrcAT, int8_t>) {
                    StaticPadCommon(
                        padTensor, 1, (staticWidthC0 - tileWidthC0) * aSingleHeight, 0,
                        aSingleCeilHeight * tileWidthC0 * c0Size_ / multiOfB16b8);
                } else if constexpr (IsSupportB4<SrcAT>() && !IsSameTypeV<SrcAT, int4b_t>) {
                    StaticPadCommon(
                        padTensor, 1, (staticWidthC0 - tileWidthC0) * aSingleHeight, 0,
                        aSingleCeilHeight * tileWidthC0 * c0Size_ / multiOfB16b4);
                }
            }
        }
    }

    __aicore__ inline void PadZeroForBInL1(const LocalTensor<SrcBT>& b1)
    {
        int32_t bSingleHeight;
        int32_t bSingleWidth;
        int32_t bSingleCeilHeight;
        int32_t bSingleCeilWidth;
        constexpr static int32_t c0Size_ = AuxGetC0Size<SrcBT>();
        if constexpr (!B_TYPE::isTrans) {
            bSingleHeight = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            bSingleCeilHeight = CeilAlign(bSingleHeight, MX_BASEK_FACTOR);
            bSingleWidth = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
            int32_t tileWidthC0 = Ceil(bSingleWidth, c0Size_);
            auto padTensor = b1.template ReinterpretCast<uint16_t>();
            if (bSingleCeilHeight > bSingleHeight) {
                if constexpr (IsSupportB8<SrcBT>() && !IsSameTypeV<SrcBT, int8_t>) {
                    StaticPadCommon(
                        padTensor, tileWidthC0, bSingleCeilHeight - bSingleHeight, bSingleHeight,
                        bSingleHeight * c0Size_ / multiOfB16b8);
                } else if constexpr (IsSupportB4<SrcBT>() && !IsSameTypeV<SrcBT, int4b_t>) {
                    StaticPadCommon(
                        padTensor, tileWidthC0, bSingleCeilHeight - bSingleHeight, bSingleHeight,
                        bSingleHeight * c0Size_ / multiOfB16b4);
                }
            }
        } else {
            bSingleHeight = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreN();
            bSingleCeilHeight = CeilAlign(bSingleHeight, BLOCK_CUBE);
            bSingleWidth = MATMUL_MODULE(MatmulShapeInfo)->GetSingleCoreK();
            bSingleCeilWidth = CeilAlign(bSingleWidth, MX_BASEK_FACTOR);
            int32_t tileWidthC0 = Ceil(bSingleWidth, c0Size_);
            int32_t staticWidthC0 = Ceil(bSingleCeilWidth, c0Size_);
            auto padTensor = b1.template ReinterpretCast<uint16_t>();
            if (staticWidthC0 > tileWidthC0) {
                if constexpr (IsSupportB8<SrcBT>() && !IsSameTypeV<SrcBT, int8_t>) {
                    StaticPadCommon(
                        padTensor, 1, (staticWidthC0 - tileWidthC0) * bSingleHeight, 0,
                        bSingleCeilHeight * tileWidthC0 * c0Size_ / multiOfB16b8);
                } else if constexpr (IsSupportB4<SrcBT>() && !IsSameTypeV<SrcBT, int4b_t>) {
                    StaticPadCommon(
                        padTensor, 1, (staticWidthC0 - tileWidthC0) * bSingleHeight, 0,
                        bSingleCeilHeight * tileWidthC0 * c0Size_ / multiOfB16b4);
                }
            }
        }
    }

    __aicore__ inline void PadZeroForABL1(const LocalTensor<SrcAT>& a1, const LocalTensor<SrcBT>& b1)
    {
        if constexpr (PhyPosIsUB(A_TYPE::pos)) {
            PadZeroForAInL1(a1);
            event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE1));
            SetFlag<HardEvent::MTE3_MTE1>(eventID);
            WaitFlag<HardEvent::MTE3_MTE1>(eventID);
        }
        if constexpr (PhyPosIsUB(B_TYPE::pos)) {
            PadZeroForBInL1(b1);
            event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE3_MTE1));
            SetFlag<HardEvent::MTE3_MTE1>(eventID);
            WaitFlag<HardEvent::MTE3_MTE1>(eventID);
        }
    }

#if __NPU_ARCH__ == 5102
    __aicore__ inline void InitQtableProcessor()
    {
        auto shapeInfo = MATMUL_MODULE(MatmulShapeInfo);
        uint32_t qtableNum =
            Ceil(shapeInfo->GetSingleCoreK(), MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseK()) *
            Ceil(shapeInfo->GetSingleCoreN(), MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN());

        constexpr int32_t B42B8_TWO = 2;
        qtableNum = DecompMode(MM_CFG) == DecompressionMode::DECOMP_4bitTo8bit ? qtableNum * B42B8_TWO : qtableNum;
        MATMUL_MODULE(QtableProcessor)->Init(qtableNum);
    }
#endif

    __aicore__ inline void CheckSupportTrianMatmul()
    {
        if constexpr (!MatmulFeatureTrait<MM_CFG>::IsSupportTrianMatmul()) {
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "Triangular Matmul is not supported on current device."); });
        }
    }

    __aicore__ inline constexpr bool IsInTrianMatmul()
    {
        if constexpr (POLICY_TYPE == PolicyType::MATMUL_UPPER_TRIANGULAR) {
            CheckSupportTrianMatmul();
            if (MATMUL_MODULE(MLoop)->GetInnerIdx() > MATMUL_MODULE(NLoop)->GetInnerIdx()) {
                return false;
            }
        } else if constexpr (POLICY_TYPE == PolicyType::MATMUL_LOWER_TRIANGULAR) {
            CheckSupportTrianMatmul();
            if (MATMUL_MODULE(MLoop)->GetInnerIdx() < MATMUL_MODULE(NLoop)->GetInnerIdx()) {
                return false;
            }
        }
        return true;
    }

    __aicore__ inline void GetResultImpl(const LocalTensor<DstT>& co2Local, uint8_t enAtomic, bool enSequentialWrite)
    {
        (void)(enAtomic);
        auto co1Local = MATMUL_MODULE(CubeOutBuffer)->GetTensor();
        MATMUL_MODULE(CubeOutBuffer)->EnQue(co1Local);
        MATMUL_MODULE(CubeOutBuffer)->DeQue();
        if (!IsInTrianMatmul()) {
            MATMUL_MODULE(CubeOutBuffer)->FreeTensor(co1Local);
            return;
        }
        if (enSequentialWrite) {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<true>(
                    co2Local, co1Local, MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
                    MATMUL_MODULE(MLoop)->GetBaseShape(), MATMUL_MODULE(NLoop)->GetBaseShape(),
                    MATMUL_MODULE(MLoop)->GetBaseBlockShape(), MATMUL_MODULE(NLoop)->GetBaseBlockShape());
        } else {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<false>(
                    co2Local, co1Local, MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
                    MATMUL_MODULE(MLoop)->GetBaseShape(), MATMUL_MODULE(NLoop)->GetBaseShape(),
                    MATMUL_MODULE(MLoop)->GetBaseBlockShape(), MATMUL_MODULE(NLoop)->GetBaseBlockShape());
        }
        MATMUL_MODULE(CubeOutBuffer)->FreeTensor(co1Local);
    }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
    __aicore__ inline void SetAtomic(uint8_t enAtomic)
    {
        if (enAtomic == ATOMIC_ADD) {
            SetAtomicAdd<DstT>();
        }
    }

    __aicore__ inline void GetResultImpl(const GlobalTensor<DstT>& gm, uint8_t enAtomic, bool enSequentialWrite)
    {
        auto co1Local = MATMUL_MODULE(CubeOutBuffer)->GetTensor();
        MATMUL_MODULE(CubeOutBuffer)->EnQue(co1Local);
        MATMUL_MODULE(CubeOutBuffer)->DeQue();
        SetAtomic(enAtomic);

        if (enSequentialWrite) {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<true>(
                    gm, co1Local, MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
                    MATMUL_MODULE(MLoop)->GetBaseShape(), MATMUL_MODULE(NLoop)->GetBaseShape(),
                    MATMUL_MODULE(MLoop)->GetBaseBlockShape(), MATMUL_MODULE(NLoop)->GetBaseBlockShape());
        } else {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<false>(
                    gm, co1Local, MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
                    MATMUL_MODULE(MLoop)->GetBaseShape(), MATMUL_MODULE(NLoop)->GetBaseShape(),
                    MATMUL_MODULE(MLoop)->GetBaseBlockShape(), MATMUL_MODULE(NLoop)->GetBaseBlockShape());
        }

        ClearAtomic(enAtomic);
        MATMUL_MODULE(CubeOutBuffer)->FreeTensor(co1Local);
    }

    __aicore__ inline void GetResultImpl(
        const GlobalTensor<DstT>& gm, const LocalTensor<DstT>& co2Local, uint8_t enAtomic, bool enSequentialWrite)
    {
        auto co1Local = MATMUL_MODULE(CubeOutBuffer)->GetTensor();
        MATMUL_MODULE(CubeOutBuffer)->EnQue(co1Local);
        MATMUL_MODULE(CubeOutBuffer)->DeQue();
        SetAtomic(enAtomic);

        if (enSequentialWrite) {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<true>(
                    gm, co2Local, co1Local, MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
                    MATMUL_MODULE(MLoop)->GetBaseShape(), MATMUL_MODULE(NLoop)->GetBaseShape(),
                    MATMUL_MODULE(MLoop)->GetBaseBlockShape(), MATMUL_MODULE(NLoop)->GetBaseBlockShape());
        } else {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<false>(
                    gm, co2Local, co1Local, MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
                    MATMUL_MODULE(MLoop)->GetBaseShape(), MATMUL_MODULE(NLoop)->GetBaseShape(),
                    MATMUL_MODULE(MLoop)->GetBaseBlockShape(), MATMUL_MODULE(NLoop)->GetBaseBlockShape());
        }

        ClearAtomic(enAtomic);
        MATMUL_MODULE(CubeOutBuffer)->FreeTensor(co1Local);
    }

#else
    __aicore__ inline void SetAtomic(uint8_t enAtomic)
    {
        if constexpr (SupportType<DstT, float, half, int16_t, int32_t, int8_t, bfloat16_t>()) {
            if (enAtomic == ATOMIC_ADD) {
                SetAtomicAdd<DstT>();
            } else if (enAtomic == ATOMIC_MAX) {
                SetAtomicMax<DstT>();
            } else if (enAtomic == ATOMIC_MIN) {
                SetAtomicMin<DstT>();
            }
        }
    }

    __aicore__ inline void GetResultImpl(const GlobalTensor<DstT>& gm, uint8_t enAtomic, bool enSequentialWrite)
    {
        if constexpr (
            C_TYPE::format != CubeFormat::ND && C_TYPE::format != CubeFormat::ND_ALIGN &&
            C_TYPE::format != CubeFormat::NZ && C_TYPE::format != CubeFormat::COLUMN_MAJOR) {
            ASCENDC_ASSERT((false), {
                KERNEL_LOG(KERNEL_ERROR, "Data format of C matrix should be ND, ND_ALIGN, COLUMN_MAJOR or NZ.");
            });
        }
        // remove dependency conflicts only for scene which is not db
        auto co1Local = MATMUL_MODULE(CubeOutBuffer)->GetTensor();
        MATMUL_MODULE(CubeOutBuffer)->EnQue(co1Local);
        MATMUL_MODULE(CubeOutBuffer)->DeQue();
        SetAtomic(enAtomic);
        if (!IsInTrianMatmul()) {
            ClearAtomic(enAtomic);
            MATMUL_MODULE(CubeOutBuffer)->FreeTensor(co1Local);
            return;
        }

        if (enSequentialWrite) {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<true>(
                    gm, co1Local, MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
                    MATMUL_MODULE(MLoop)->GetBaseShape(), MATMUL_MODULE(NLoop)->GetBaseShape(),
                    MATMUL_MODULE(MLoop)->GetBaseBlockShape(), MATMUL_MODULE(NLoop)->GetBaseBlockShape());
        } else {
            MATMUL_MODULE(CopyCubeOut)
                ->template Copy<false>(
                    gm, co1Local, MATMUL_MODULE(MLoop)->GetInnerIdx(), MATMUL_MODULE(NLoop)->GetInnerIdx(),
                    MATMUL_MODULE(MLoop)->GetBaseShape(), MATMUL_MODULE(NLoop)->GetBaseShape(),
                    MATMUL_MODULE(MLoop)->GetBaseBlockShape(), MATMUL_MODULE(NLoop)->GetBaseBlockShape());
        }

        ClearAtomic(enAtomic);
        MATMUL_MODULE(CubeOutBuffer)->FreeTensor(co1Local);
    }
#endif

    __aicore__ inline void ClearAtomic(uint8_t enAtomic)
    {
        if (enAtomic != 0) {
            SetAtomicNone();
        }
    }
};

} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_BASE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_SCHEDULER_BASE_SCHEDULER_BASE_H__
#endif
