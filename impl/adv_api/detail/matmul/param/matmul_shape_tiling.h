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
 * \file matmul_shape_tiling.h
 * \brief matmul variable manager
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/param/matmul_shape_tiling.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_SHAPE_TILING_H__
#endif

#ifndef IMPL_MATMUL_PARAM_MATMUL_SHAPE_TILING_H
#define IMPL_MATMUL_PARAM_MATMUL_SHAPE_TILING_H

#include "../utils/matmul_module.h"
#include "../utils/matmul_utils.h"
#include "../../../tiling/matmul/matmul_constant_tiling_struct.h"

namespace AscendC {
namespace Impl {
namespace Detail {

template <typename IMPL, const auto& MM_CFG>
class MatmulShapeTiling {
public:
    __aicore__ inline void SetTiling(const TCubeTiling* __restrict tiling) { tiling_.SetTiling(tiling); }

    __aicore__ inline const MatmulTiling<MM_CFG>& GetTiling() const { return tiling_; }

    template <typename SrcT, typename L0cT>
    __aicore__ inline void CheckTiling()
    {
#ifdef ASCENDC_CPU_DEBUG
        NumericalValidCheck();
        ShareInfoCheck();
        if constexpr (
            !HasScalePosition<typename IMPL::AType>::value && !HasScalePosition<typename IMPL::BType>::value) {
            ShapeValidCheck<SrcT, L0cT>();
            DepthCheck();
            ConfigCommonCheck();
            ConfigSpecificCheck();
        } else {
            MxShapeValidCheck<SrcT, L0cT>();
            DepthCheck();
            MxTypeParaCheck();
            MxConfigSpecificCheck();
        }
#else
        ConfigCommonStaticCheck<L0cT>();
        using CFG_TYPE = typename std::remove_cv<typename std::remove_reference<decltype(MM_CFG)>::type>::type;
        if constexpr (IsSameTypeV<CFG_TYPE, MatmulApiStaticTiling>) {
            StaticTilingCheck<SrcT, L0cT>();
        }
#endif
    }

private:
    template <typename SrcT, typename L0cT>
    __aicore__ inline void StaticTilingCheck()
    {
        const auto bitSize = AscendC::GetBitSize<SrcT>();
        const auto l0ABUseSizeFactor = (MM_CFG.dbL0A - 1) & (MM_CFG.dbL0B - 1) ? Impl::DB_FACTOR : 1;
        const auto l0CUseSizeFactor = (MM_CFG.dbL0C == Impl::DB_FACTOR) ? Impl::DB_FACTOR : 1;
        static_assert(
            MM_CFG.baseM * MM_CFG.baseK * bitSize / ONE_BYTE_BIT_SIZE * l0ABUseSizeFactor <= L0ASize_,
            "BaseM * baseK should be no larger than L0ASize.");
        static_assert(
            MM_CFG.baseN * MM_CFG.baseK * bitSize / ONE_BYTE_BIT_SIZE * l0ABUseSizeFactor <= L0BSize_,
            "BaseN * baseK should be no larger than L0BSize.");
        static_assert(
            MM_CFG.baseM * MM_CFG.baseN * sizeof(L0cT) * l0CUseSizeFactor <= L0CSize_,
            "BaseM * baseN should be no larger than L0CSize.");

        if constexpr ((DoMatmulNorm(MM_CFG) || DoMatmulMDL(MM_CFG)) && ToMatmulConfig(MM_CFG).isA2B2Shared) {
            static_assert(
                MM_CFG.baseM * MM_CFG.baseK * bitSize / ONE_BYTE_BIT_SIZE <= L0ASize_ / Impl::DB_FACTOR,
                "BaseM * baseK should be no larger than L0ASize / 2 when isA2B2Shared is enable.");
            static_assert(
                MM_CFG.baseN * MM_CFG.baseK * bitSize / ONE_BYTE_BIT_SIZE <= L0BSize_ / Impl::DB_FACTOR,
                "BaseN * baseK should be no larger than L0BSize / 2 when isA2B2Shared is enable.");
        }

        if constexpr (MM_CFG.shareMode == 1) {
            static_assert(
                MM_CFG.baseM * MM_CFG.baseK * bitSize / ONE_BYTE_BIT_SIZE <= L0ASize_ / HALF_FACTOR,
                "BaseM * baseK should be less than half l0a when in mode 1.");
            static_assert(
                MM_CFG.baseN * MM_CFG.baseK * bitSize / ONE_BYTE_BIT_SIZE <= L0BSize_ / HALF_FACTOR,
                "BaseN * baseK should be less than half l0b when in mode 1.");
            static_assert(
                MM_CFG.baseM * MM_CFG.baseN * sizeof(L0cT) * l0CUseSizeFactor <= L0CSize_ / HALF_FACTOR,
                "BaseM * baseN should be less than half l0c when in mode 1.");
        }
    }

#ifdef ASCENDC_CPU_DEBUG
    __aicore__ inline void NumericalValidCheck()
    {
        ASCENDC_ASSERT((tiling_.GetDepthA1() > 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "tiling_.GetDepthA1() is %d , which should be larger than 0", tiling_.GetDepthA1());
        });
        ASCENDC_ASSERT((tiling_.GetDepthB1() > 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "tiling_.GetDepthB1() is %d , which should be larger than 0", tiling_.GetDepthB1());
        });
        ASCENDC_ASSERT((tiling_.GetStepM() > 0), {
            KERNEL_LOG(KERNEL_ERROR, "tiling_.GetStepM() is %d , which should be larger than 0", tiling_.GetStepM());
        });
        ASCENDC_ASSERT((tiling_.GetStepN() > 0), {
            KERNEL_LOG(KERNEL_ERROR, "tiling_.GetStepN() is %d , which should be larger than 0", tiling_.GetStepN());
        });
        ASCENDC_ASSERT((tiling_.IsBias() >= 0), {
            KERNEL_LOG(KERNEL_ERROR, "tiling_.IsBias() is %d , which should be not less than 0", tiling_.IsBias());
        });

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
        ASCENDC_ASSERT((tiling_.GetTransLength() > 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "tiling_.GetTransLength() is %d , which should be larger than 0",
                tiling_.GetTransLength());
        });
        if constexpr (!ToMatmulConfig(MM_CFG).enableUBReuse) {
            ASCENDC_ASSERT(tiling_.GetTransLength() * 4 <= UBSize_, {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "When enableUBReuse is false, tiling_.GetTransLength() * 4 should be less than UB size");
            });
        }
#endif
        ASCENDC_ASSERT((tiling_.GetIterateOrder() >= 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "tiling_.GetIterateOrder() is %d , which should be not less than 0",
                tiling_.GetIterateOrder());
        });
    }

    __aicore__ inline void ShareInfoCheck()
    {
        ASCENDC_ASSERT((tiling_.GetShareMode() >= 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "tiling_.GetShareMode() is %d , which should be not less than 0", tiling_.GetShareMode());
        });
        ASCENDC_ASSERT((tiling_.GetShareL1Size() >= 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "tiling_.GetShareL1Size() is %d , which should be not less than 0",
                tiling_.GetShareL1Size());
        });
        ASCENDC_ASSERT((tiling_.GetShareL0CSize() >= 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "tiling_.GetShareL0CSize() is %d , which should be not less than 0",
                tiling_.GetShareL0CSize());
        });
        ASCENDC_ASSERT((tiling_.GetShareUbSize() >= 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "tiling_.GetShareUbSize() is %d , which should be not less than 0",
                tiling_.GetShareUbSize());
        });
    }

    template <typename SrcT, typename L0cT>
    __aicore__ inline void ShapeValidCheck()
    {
        const auto l0ABUseSizeFactor = (tiling_.GetDbL0A() - 1) & (tiling_.GetDbL0B() - 1) ? Impl::DB_FACTOR : 1;
        const auto l0CUseSizeFactor = (tiling_.GetDbL0C() == Impl::DB_FACTOR) ? Impl::DB_FACTOR : 1;
        ASCENDC_ASSERT(
            (tiling_.GetBaseM() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE *
                 l0ABUseSizeFactor <=
             L0ASize_),
            {
                KERNEL_LOG(
                    KERNEL_ERROR, "baseM * baseK is %d , which should be no larger than L0ASize_ %d.",
                    tiling_.GetBaseM() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE *
                        l0ABUseSizeFactor,
                    L0ASize_);
            });
        ASCENDC_ASSERT(
            (tiling_.GetBaseN() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE *
                 l0ABUseSizeFactor <=
             L0BSize_),
            {
                KERNEL_LOG(
                    KERNEL_ERROR, "baseN * baseK is %d , which should be no larger than L0BSize_ %d.",
                    tiling_.GetBaseN() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE *
                        l0ABUseSizeFactor,
                    L0BSize_);
            });
        ASCENDC_ASSERT((tiling_.GetBaseM() * tiling_.GetBaseN() * sizeof(L0cT) * l0CUseSizeFactor <= L0CSize_), {
            KERNEL_LOG(
                KERNEL_ERROR, "baseM * baseN is %d , which should be no larger than L0CSize_ %d.",
                tiling_.GetBaseM() * tiling_.GetBaseN() * sizeof(L0cT) * l0CUseSizeFactor, L0CSize_);
        });
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201))
        if constexpr ((DoMatmulNorm(MM_CFG) || DoMatmulMDL(MM_CFG)) && ToMatmulConfig(MM_CFG).isA2B2Shared) {
            ASCENDC_ASSERT(
                (tiling_.GetBaseM() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE <=
                 L0ASize_ / Impl::DB_FACTOR),
                {
                    KERNEL_LOG(
                        KERNEL_ERROR,
                        "baseM * baseK is %d , which should be no larger than A2 Size / 2 when isA2B2Shared is enable "
                        "%d.",
                        tiling_.GetBaseM() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE,
                        L0ASize_ / Impl::DB_FACTOR);
                });
            ASCENDC_ASSERT(
                (tiling_.GetBaseN() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE <=
                 L0BSize_ / Impl::DB_FACTOR),
                {
                    KERNEL_LOG(
                        KERNEL_ERROR,
                        "baseN * baseK is %d , which should be no larger than B2 Size / 2 when isA2B2Shared is enable "
                        "%d.",
                        tiling_.GetBaseN() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE,
                        L0BSize_ / Impl::DB_FACTOR);
                });
        }
#endif
        if (tiling_.GetShareMode() == 1) {
            ASCENDC_ASSERT(
                (tiling_.GetBaseM() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE <=
                 L0ASize_ / HALF_FACTOR),
                {
                    KERNEL_LOG(
                        KERNEL_ERROR,
                        "baseM is %d , baseK is %d, baseM * baseK should be less than half l0a when in mode 1.",
                        tiling_.GetBaseM(), tiling_.GetBaseK());
                });
            ASCENDC_ASSERT(
                (tiling_.GetBaseN() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE <=
                 L0BSize_ / HALF_FACTOR),
                {
                    KERNEL_LOG(
                        KERNEL_ERROR,
                        "baseN is %d , baseK is %d, baseN * baseK should be less than half l0b when in mode 1.",
                        tiling_.GetBaseN(), tiling_.GetBaseK());
                });
            ASCENDC_ASSERT((tiling_.GetBaseM() * tiling_.GetBaseN() * sizeof(L0cT) <= L0CSize_ / HALF_FACTOR), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "baseM is %d , baseN is %d, baseM * baseN should be less than half l0c when in mode 1.",
                    tiling_.GetBaseM(), tiling_.GetBaseN());
            });
        }
    }

    template <typename SrcT, typename L0cT>
    __aicore__ inline void MxShapeValidCheck()
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        const auto l0ABUseSizeFactor = (tiling_.GetDbL0A() - 1) & (tiling_.GetDbL0B() - 1) ? Impl::DB_FACTOR : 1;
        const auto l0CUseSizeFactor = (tiling_.GetDbL0C() == Impl::DB_FACTOR) ? Impl::DB_FACTOR : 1;
        // A  < l0aSize;
        ASCENDC_ASSERT(
            (tiling_.GetBaseM() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE *
                 l0ABUseSizeFactor <=
             L0ASize_),
            {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "baseM * baseK * l0ABUseSizeFactor is %d , which should be no larger than L0ASize_ %d",
                    tiling_.GetBaseM() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE *
                        l0ABUseSizeFactor,
                    L0ASize_);
            });
        // scaleA  < l0aMxSize;
        ASCENDC_ASSERT(
            ((tiling_.GetBaseM() * tiling_.GetBaseK() / 32 * sizeof(fp8_e8m0_t) * l0ABUseSizeFactor) <= L0AMxSize_), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "baseM * baseK * l0ABUseSizeFactor / 32 is %d , which should be no larger than L0AMxSize_ %d",
                    tiling_.GetBaseM() * tiling_.GetBaseK() / 32 * sizeof(fp8_e8m0_t) * l0ABUseSizeFactor, L0AMxSize_);
            });
        // B  < l0bSize;
        ASCENDC_ASSERT(
            (tiling_.GetBaseN() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE *
                 l0ABUseSizeFactor <=
             L0BSize_),
            {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "baseN * baseK * l0ABUseSizeFactor is %d , which should be no larger than L0BSize_ %d",
                    tiling_.GetBaseN() * tiling_.GetBaseK() * AscendC::GetBitSize<SrcT>() / ONE_BYTE_BIT_SIZE *
                        l0ABUseSizeFactor,
                    L0BSize_);
            });
        // scaleB  < l0bMxSize;
        ASCENDC_ASSERT(
            (tiling_.GetBaseN() * tiling_.GetBaseK() / 32 * sizeof(fp8_e8m0_t) * l0ABUseSizeFactor <= L0BMxSize_), {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "baseN * baseK * l0ABUseSizeFactor / 32 is %d , which should be no larger than L0BMxSize_ %d",
                    tiling_.GetBaseN() * tiling_.GetBaseK() / 32 * sizeof(fp8_e8m0_t) * l0ABUseSizeFactor, L0BMxSize_);
            });
        // C  < l0cSize;
        ASCENDC_ASSERT((tiling_.GetBaseM() * tiling_.GetBaseN() * sizeof(L0cT) * l0CUseSizeFactor <= L0CSize_), {
            KERNEL_LOG(
                KERNEL_ERROR, "baseM * baseN * l0CUseSizeFactor is %d , which should be no larger than L0CSize_ %d",
                tiling_.GetBaseM() * tiling_.GetBaseN() * sizeof(L0cT) * l0CUseSizeFactor, L0CSize_);
        });
#endif
    }

    __aicore__ inline void DepthCheck()
    {
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) || (__NPU_ARCH__ == 5102)
        if constexpr (DoMatmulMDL(MM_CFG) || DoMatmulSpecialMDL(MM_CFG)) {
            ASCENDC_ASSERT((tiling_.GetDepthA1() % (tiling_.GetStepM() * tiling_.GetStepKa()) == 0), {
                KERNEL_LOG(
                    KERNEL_ERROR, "depthA1 is %d , which should be divided exactly by stepM * stepKa(%d * %d)",
                    tiling_.GetDepthA1(), tiling_.GetStepM(), tiling_.GetStepKa());
            });
            ASCENDC_ASSERT((tiling_.GetDepthB1() % (tiling_.GetStepN() * tiling_.GetStepKb()) == 0), {
                KERNEL_LOG(
                    KERNEL_ERROR, "depthB1 is %d , which should be divided exactly by stepN * stepKb(%d * %d)",
                    tiling_.GetDepthB1(), tiling_.GetStepN(), tiling_.GetStepKb());
            });
            ASCENDC_ASSERT((tiling_.GetDepthA1() / (tiling_.GetStepM() * tiling_.GetStepKa()) <= 2), {
                KERNEL_LOG(
                    KERNEL_ERROR, "depthA1 is %d , stepM %d, stepKa %d, depthA1 <= 2 * (stepM * stepKa)",
                    tiling_.GetDepthA1(), tiling_.GetStepM(), tiling_.GetStepKa());
            });
            ASCENDC_ASSERT((tiling_.GetDepthB1() / (tiling_.GetStepN() * tiling_.GetStepKb()) <= 2), {
                KERNEL_LOG(
                    KERNEL_ERROR, "depthB1 is %d , stepN %d, stepKb %d, depthB1 <= 2 * (stepN * stepKb)",
                    tiling_.GetDepthB1(), tiling_.GetStepN(), tiling_.GetStepKb());
            });
        }
        if constexpr (DoMatmulSpecialMDL(MM_CFG)) {
            if (tiling_.GetSingleCoreK() / tiling_.GetBaseK() > tiling_.GetStepKb()) {
                ASCENDC_ASSERT(tiling_.GetStepN() <= 2, {
                    KERNEL_LOG(
                        KERNEL_ERROR, "In SpecialMDL scene, when k-axis isn't full loaded, stepN should be <= 2.");
                });
            }
        }
#endif
    }

    __aicore__ inline void MxTypeParaCheck()
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        if constexpr (DoMatmulMDL(MM_CFG)) {
            int32_t mxTypePara = tiling_.GetMxTypePara();
            // 0x01010101 is scaleFactorM, scaleFactorN, scaleFactorKa, scaleFactorKb min val
            ASCENDC_ASSERT((mxTypePara >= 0x01010101), {
                KERNEL_LOG(
                    KERNEL_ERROR, "mxTypePara value should be greater than or equal to 0x01010101, current is %d, ",
                    mxTypePara);
            });
        }
#endif
    }

    template <
        typename IMPL_ALIAS = IMPL, const auto& MM_CFG_ALIAS = MM_CFG,
        enable_if_t<NormInitScene<MM_CFG_ALIAS>, bool> = false>
    __aicore__ inline void ConfigSpecificCheck()
    {
        if constexpr (DoMatmulNorm(MM_CFG) && IMPL::AType::layout != LayoutMode::NONE) {
            if constexpr (
                ToMatmulConfig(MM_CFG).batchMode == BatchMode::SINGLE_LARGE_THAN_L1 &&
                !ToMatmulConfig(MM_CFG).isBiasBatch) {
                ASCENDC_ASSERT(false, {
                    KERNEL_LOG(KERNEL_ERROR, "Bias reuse does not support BatchMode::SINGLE_LARGE_THAN_L1");
                });
            }

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
            if constexpr (ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
                ASCENDC_ASSERT(tiling_.GetSingleCoreK() <= tiling_.GetBaseK(), {
                    KERNEL_LOG(
                        KERNEL_ERROR, "When singleCoreK is larger than baseK, the parameter scheduleType of "
                                      "MM_CFG should not be OUTER_PRODUCT");
                });
            }
#endif
        }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
        // when output is int8 and ND format, do not support on the fly trans nd2nz
        if constexpr (
            IMPL::CType::format == CubeFormat::ND && !ToMatmulConfig(MM_CFG).enVecND2NZ &&
            (IsSameType<typename IMPL::CType::T, int8_t>::value ||
             IsSameType<typename IMPL::CType::T, uint8_t>::value)) {
            ASCENDC_ASSERT(false, {
                KERNEL_LOG(
                    KERNEL_ERROR, "Norm Scene, When output's data format is ND and data type is int8_t or uint8_t,"
                                  " the parameter enVecND2NZ of MM_CFG should be true");
            });
        }
#endif
    }

    template <
        typename IMPL_ALIAS = IMPL, const auto& MM_CFG_ALIAS = MM_CFG,
        enable_if_t<MdlInitScene<MM_CFG_ALIAS>, bool> = false>
    __aicore__ inline void ConfigSpecificCheck()
    {
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 1001
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "MatmulVersion MULTI_DATA_LOAD is valid only in v220."); });
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
        // when output is int8 and ND format, do not support on the fly trans nd2nz
        if constexpr (
            IMPL::CType::format == CubeFormat::ND && !ToMatmulConfig(MM_CFG).enVecND2NZ &&
            (IsSameType<typename IMPL::CType::T, int8_t>::value ||
             IsSameType<typename IMPL::CType::T, uint8_t>::value)) {
            ASCENDC_ASSERT(false, {
                KERNEL_LOG(
                    KERNEL_ERROR, "MDL Scene, When output's data format is ND and data type is int8_t or uint8_t,"
                                  " the parameter enVecND2NZ of MM_CFG should be true");
            });
        }
#endif
#if (__NPU_ARCH__ != 2201) && (__NPU_ARCH__ != 3510) && (__NPU_ARCH__ != 5102)
        if constexpr (ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
            ASCENDC_ASSERT(
                false, { KERNEL_LOG(KERNEL_ERROR, "ScheduleType is OUTER_PRODUCT only supported on A2/A3/A5."); });
        }
#endif
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
        if constexpr (ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
            ASCENDC_ASSERT(tiling_.GetSingleCoreK() <= tiling_.GetBaseK(), {
                KERNEL_LOG(
                    KERNEL_ERROR, "When singleCoreK is larger than baseK, the parameter scheduleType of "
                                  "MM_CFG should not be OUTER_PRODUCT");
            });
            ASCENDC_ASSERT((ToMatmulConfig(MM_CFG).iterateOrder != IterateOrder::UNDEF), {
                KERNEL_LOG(
                    KERNEL_ERROR, "When scheduleType is OUTER_PRODUCT, iterateOrder of MM_CFG should not be UNDEF.");
            });
            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_M) {
                ASCENDC_ASSERT((tiling_.GetStepN() > 1), {
                    KERNEL_LOG(
                        KERNEL_ERROR, "When scheduleType is OUTER_PRODUCT and iterateOrder is ORDER_M, "
                                      "stepN should be larger than 1");
                });
            }

            if constexpr (ToMatmulConfig(MM_CFG).iterateOrder == IterateOrder::ORDER_N) {
                ASCENDC_ASSERT((tiling_.GetStepM() > 1), {
                    KERNEL_LOG(
                        KERNEL_ERROR, "When scheduleType is OUTER_PRODUCT and iterateOrder is ORDER_N, "
                                      "stepM should be larger than 1");
                });
            }
        }
#endif
    }

    __aicore__ inline void MxConfigSpecificCheck()
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        if constexpr (ToMatmulConfig(MM_CFG).scheduleType == ScheduleType::OUTER_PRODUCT) {
            ASCENDC_ASSERT(DoMatmulMDL(MM_CFG), {
                KERNEL_LOG(KERNEL_ERROR, "when scheduleType is OUTER_PRODUCT, MxMatmul only support mdl");
            });

            ASCENDC_ASSERT((ToMatmulConfig(MM_CFG).iterateOrder != IterateOrder::UNDEF), {
                KERNEL_LOG(
                    KERNEL_ERROR, "When scheduleType is OUTER_PRODUCT, iterateOrder of MM_CFG should not be UNDEF.");
            });

            ASCENDC_ASSERT(tiling_.GetSingleCoreK() <= tiling_.GetBaseK(), {
                KERNEL_LOG(
                    KERNEL_ERROR, "When singleCoreK is larger than baseK, the parameter scheduleType of "
                                  "MM_CFG should not be OUTER_PRODUCT");
            });
        }
#endif
    }

    template <
        typename IMPL_ALIAS = IMPL, const auto& MM_CFG_ALIAS = MM_CFG,
        enable_if_t<DoMatmulIBShareNorm(MM_CFG_ALIAS), bool> = false>
    __aicore__ inline void ConfigSpecificCheck()
    {
        if constexpr (IMPL::AType::ibShare) {
            ASCENDC_ASSERT((IMPL::BType::ibShare == false), {
                KERNEL_LOG(KERNEL_ERROR, "When A is ibShare, B should not be ibShare");
            });
            ASCENDC_ASSERT((!PhyPosIsL1(IMPL::AType::pos)), {
                KERNEL_LOG(KERNEL_ERROR, "When A is ibShare, A pos should be GM");
            });
        } else {
            ASCENDC_ASSERT((IMPL::BType::ibShare == true), {
                KERNEL_LOG(KERNEL_ERROR, "When A is not ibShare, B should be ibShare");
            });
            ASCENDC_ASSERT((!PhyPosIsL1(IMPL::BType::pos)), {
                KERNEL_LOG(KERNEL_ERROR, "When B is ibShare, B pos should be GM");
            });
        }
    }

    template <
        typename IMPL_ALIAS = IMPL, const auto& MM_CFG_ALIAS = MM_CFG,
        enable_if_t<!NormInitScene<MM_CFG_ALIAS> && !MdlInitScene<MM_CFG_ALIAS> && !DoMatmulIBShareNorm(MM_CFG), bool> =
            false>
    __aicore__ inline void ConfigSpecificCheck()
    {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Unsupported matmul config."); });
    }

    __aicore__ inline void ConfigCommonCheck()
    {
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
        if (IMPL::CType::format == CubeFormat::ND &&
            (tiling_.GetN() * sizeof(typename IMPL::CType::T) % ONE_BLK_SIZE != 0)) {
            ASCENDC_ASSERT((false), {
                KERNEL_LOG(KERNEL_ERROR, "N dims need to be aligned to 32B when ND format output in v200.");
            });
        }
#endif
        if constexpr (IMPL::AType::layout == LayoutMode::NONE && !ToMatmulConfig(MM_CFG).isBiasBatch) {
            ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "Bias reuse is only valid in BMM."); });
        }
    }
#else
    template <typename L0cT>
    __aicore__ inline void ConfigCommonStaticCheck()
    {
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || \
    ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)) || (__NPU_ARCH__ == 5102)
        if constexpr (ToMatmulConfig(MM_CFG).isEnableChannelSplit) {
            static_assert(
                (PhyPosIsGM(IMPL::CType::pos) && (IMPL::CType::format == CubeFormat::NZ) &&
                 IsSameType<typename IMPL::CType::T, float>::value && IsSameType<L0cT, float>::value),
                "ChannelSplit only supports GM position, NZ format and float data type output. Besides, L0cT must be "
                "float.");
        }
        if constexpr (
            ToMatmulConfig(MM_CFG).batchMode == BatchMode::BATCH_LARGE_THAN_L1 ||
            ToMatmulConfig(MM_CFG).batchMode == BatchMode::SINGLE_LARGE_THAN_L1) {
            constexpr bool IsNormalLayout = IMPL::AType::layout == LayoutMode::NORMAL &&
                                            IMPL::BType::layout == LayoutMode::NORMAL &&
                                            IMPL::CType::layout == LayoutMode::NORMAL;

            static_assert(
                IsNormalLayout,
                "When BATCH_LARGE_THAN_L1 or SINGLE_LARGE_THAN_L1 BMM mode, layout of A, B and C must be NORMAL.");
        }
        if constexpr (DoMatmulSpecialMDL(MM_CFG)) {
            static_assert(
                MM_CFG.doMultiDataLoad == false, "In SpecialMDL scene, MatmulConfig.doMultiDataLoad must be false.");
        }
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        if constexpr (ToMatmulConfig(MM_CFG).enableL1BankConflictOptimise) {
            static_assert(DoMatmulMDL(MM_CFG), "L1BankConflictOptimise only support MDL config.");

            constexpr bool IsABBiasGMIn =
                (PhyPosIsGM(IMPL::AType::pos) && PhyPosIsGM(IMPL::BType::pos) && PhyPosIsGM(IMPL::BiasType::pos));
            static_assert(IsABBiasGMIn, "L1BankConflictOptimise only support gm in.");

            if constexpr (HasScalePosition<typename IMPL::AType>::value) {
                constexpr bool IsScaleAGMIn = PhyPosIsGM(IMPL::AType::scalePosition);
                static_assert(IsScaleAGMIn, "L1BankConflictOptimise only support gm in.");
            }

            if constexpr (HasScalePosition<typename IMPL::BType>::value) {
                constexpr bool IsScaleBGMIn = PhyPosIsGM(IMPL::BType::scalePosition);
                static_assert(IsScaleBGMIn, "L1BankConflictOptimise only support gm in.");
            }
        }
#endif
    }
#endif // #ifdef ASCENDC_CPU_DEBUG

private:
    MatmulTiling<MM_CFG> tiling_;
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // IMPL_MATMUL_PARAM_MATMUL_SHAPE_TILING_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_SHAPE_TILING_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_PARAM_MATMUL_SHAPE_TILING_H__
#endif
