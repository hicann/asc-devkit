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
 * \file copy_cube_out_fixpipe.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_out/copy_cube_out_fixpipe.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_FIXPIPE_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_FIXPIPE_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_FIXPIPE_H

#include "../../utils/matmul_module.h"
#include "../../utils/matmul_param.h"
#include "../../feature_trait/matmul_feature_trait.h"
#include "quant/quant_processor_utils.h"
#include "copy_cube_out_intf.h"
#include "copy_cube_out_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {
/*
 * CopyCubeOut is considered entirely experimental.
 * We retain the freedom to make incompatible changes, but do not guarantee the stability.
 * CopyCubeOut is only for internal usage, does not support extension or customized specialization!
 */
template <typename IMPL, class A_TYPE, class B_TYPE, class C_TYPE, const auto& MM_CFG, McgShfMode FIXPIPE_MODE>
class CopyCubeOut<
    IMPL, A_TYPE, B_TYPE, C_TYPE, MM_CFG, FIXPIPE_MODE, enable_if_t<(!MatmulFeatureTrait<MM_CFG>::IsNeedUB())>> {
    using DstT = typename C_TYPE::T;
    using SrcT = typename GetMmDstType<typename A_TYPE::T>::Type;
    using FixpipeAdaptor =
        FixpipeParamsUtil<A_TYPE, C_TYPE, MM_CFG, MatmulFeatureTrait<MM_CFG>::GetFixpipeParamsType()>;

    MATMUL_USE_MODULE(Context);
    MATMUL_USE_MODULE(MatmulQuantProcessor);
    MATMUL_USE_MODULE(MatmulShapeInfo);
    MATMUL_USE_MODULE(MatmulShapeTiling);
    MATMUL_USE_MODULE(MatmulUserDefineInfo);
    MATMUL_USE_MODULE(MatmulSubBlockInfo);
    MATMUL_USE_MODULE(BatchLoop);

public:
    __aicore__ inline CopyCubeOut() = default;

    template <bool enSequentialWrite = false, typename ScheduleContext = int>
    __aicore__ inline void Copy(
        const GlobalTensor<DstT>& gm, const LocalTensor<SrcT>& co1Local, int32_t curRow, int32_t curCol,
        int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth,
        const ScheduleContext& context = 0)
    {
#ifdef ASCENDC_CPU_DEBUG
        if (IMPL::CallBack::DataCopyOutPtr == nullptr) {
#else
        if constexpr (IMPL::CallBack::DataCopyOutPtr == nullptr) {
#endif
            if constexpr (ToMatmulConfig(MM_CFG).intraBlockPartSum) {
                if (!MATMUL_MODULE(MatmulSubBlockInfo)->GetFakeMsg()) {
                    CopyOutImpl<enSequentialWrite, const GlobalTensor<DstT>, true>(
                        gm, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth);
                    return;
                }
            }
            CopyOutImpl<enSequentialWrite, const GlobalTensor<DstT>, false>(
                gm, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth);
        } else {
            CopyOutImplCB<enSequentialWrite>(
                gm, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth);
        }
    }

    template <bool enSequentialWrite = false, typename ScheduleContext = int>
    __aicore__ inline void Copy(
        const LocalTensor<DstT>& co2Local, const LocalTensor<SrcT>& co1Local, int32_t curRow, int32_t curCol,
        int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth,
        const ScheduleContext& context = 0)
    {
        CopyOutImpl<enSequentialWrite>(
            co2Local, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth);
    }

private:
    template <bool enSequentialWrite, class T, bool isIntraBlock = false>
    __aicore__ inline void CopyOutImpl(
        const T& dst, const LocalTensor<SrcT>& co1Local, int32_t curRow, int32_t curCol, int32_t baseHeight,
        int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth)
    {
        SplitMNParamsCheck(baseWidth);
        if constexpr (
            (FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_M || (A_TYPE::ibShare && B_TYPE::ibShare)) &&
            PhyPosIsUB(C_TYPE::pos) && MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB() &&
            FIXPIPE_MODE != McgShfMode::DUAL_DST_SPLIT_N) {
            baseHeight = Align(baseHeight, DOUBLE_SIZE);
        }
        if constexpr (C_TYPE::format == CubeFormat::ND || C_TYPE::format == CubeFormat::ND_ALIGN) {
            CopyOutNZ2ND<enSequentialWrite, T, isIntraBlock>(
                dst, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth);
        } else if constexpr (C_TYPE::format == CubeFormat::NZ) {
            ASCENDC_ASSERT(
                !(FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_N && (A_TYPE::ibShare && B_TYPE::ibShare) &&
                  PhyPosIsUB(C_TYPE::pos) && MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB()),
                { KERNEL_LOG(KERNEL_ERROR, "if split N when copy cube out, NZ is not supported"); });
            CopyOutNZ2NZ<enSequentialWrite, T, isIntraBlock>(
                dst, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
        } else if constexpr (C_TYPE::format == CubeFormat::COLUMN_MAJOR) {
            CopyOutNZ2DN<enSequentialWrite, T, isIntraBlock>(
                dst, co1Local, curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth);
#endif
        } else {
            ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Copy: unsupport Matmul format type."); });
        }
    }

    __aicore__ inline void SplitMNParamsCheck(int32_t baseWidth)
    {
        if constexpr (PhyPosIsGM(C_TYPE::pos)) {
            ASCENDC_ASSERT(
                !((FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_M || FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_N) &&
                  MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB()),
                { KERNEL_LOG(KERNEL_ERROR, "if split M or N when copy cube out, GM is not supported on C310."); });
        } else if constexpr (PhyPosIsUB(C_TYPE::pos)) {
            ASCENDC_ASSERT(
                !((FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_M || FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_N) &&
                  !(A_TYPE::ibShare && B_TYPE::ibShare) && MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB()),
                {
                    KERNEL_LOG(
                        KERNEL_ERROR,
                        "if split M or N when copy cube out, the ibShare of A and B must be true on C310.");
                });
            ASCENDC_ASSERT(!(FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_N && IsBasic(MM_CFG) && baseWidth % 16 == 0), {
                KERNEL_LOG(
                    KERNEL_ERROR, "if split N when copy cube out, only singleCoreM = baseM, singleCoreN = baseN "
                                  "singleCoreK = baseK and N % 16 == 0 are supported on C310.");
            });
        }
    }

    template <bool enSequentialWrite, class T, bool isIntraBlock = false>
    __aicore__ inline void CopyOutNZ2ND(
        const T& dst, const LocalTensor<SrcT>& co1Local, int32_t curRow, int32_t curCol, int32_t baseHeight,
        int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth)
    {
        auto stride = baseWidth;
        int64_t dstOffset = 0;
        if constexpr (!enSequentialWrite) {
            stride = GetDstStride<isIntraBlock>();
            if constexpr (!IsBasic(MM_CFG)) {
                dstOffset = GetDstOffset(curRow, curCol, baseHeight, stride);
            }
        }
        if constexpr (FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_N && PhyPosIsUB(C_TYPE::pos)) {
            if (IsBasic(MM_CFG) && !NoTailN(MM_CFG)) {
                stride = baseWidth >> 1;
            } else {
                stride = stride >> 1;
            }
        }

        FixpipeAdaptor fixpipe(
            baseWidth, baseHeight, baseBlockWidth, baseBlockHeight,
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(), stride);
        if constexpr (ToMatmulConfig(MM_CFG).bmmOutMode != BatchOutMode::SINGLE_BATCH) {
            fixpipe.SetNdParams(
                MATMUL_MODULE(BatchLoop)->GetBatchOutCacheNum(), baseHeight, baseWidth, baseBlockWidth,
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(),
                MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN());
        }
        SetFixpipeParams(fixpipe);
        AlignHeightWithTrans(fixpipe, baseHeight);
        CopyTensor(dst[dstOffset], co1Local, fixpipe, curCol, baseWidth);
    }

    __aicore__ inline void AlignHeightWithTrans(FixpipeAdaptor& fixpipe, int32_t mSize)
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        if constexpr (HasScalePosition<A_TYPE>::value) {
            if constexpr (IsMxDisableUnitFlag<A_TYPE, B_TYPE, MM_CFG>) {
                fixpipe.params_.unitFlag = 0;
            }
        }
        if constexpr (HasScalePosition<A_TYPE>::value) {
            constexpr int32_t c0Size = AuxGetC0Size<typename A_TYPE::T>();
            if (MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA()) {
                fixpipe.params_.srcStride = Align(mSize, c0Size);
            }
        }
#endif
    }

    __aicore__ inline int64_t GetDstOffset(int32_t curRow, int32_t curCol, int32_t baseHeight, int32_t stride)
    {
        int64_t dstOffset = 0;
        if constexpr (
            MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB() && PhyPosIsUB(C_TYPE::pos) &&
            (A_TYPE::ibShare && B_TYPE::ibShare)) {
            if constexpr (FIXPIPE_MODE != McgShfMode::DUAL_DST_SPLIT_N) {
                dstOffset =
                    (static_cast<int64_t>(static_cast<int64_t>(
                         static_cast<int64_t>(curRow * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM()) *
                         stride)) >>
                     1) +
                    static_cast<int64_t>(curCol * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN());
            } else {
                dstOffset =
                    static_cast<int64_t>(
                        static_cast<int64_t>(curRow * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM()) *
                        stride) +
                    static_cast<int64_t>(
                        static_cast<int64_t>(curCol * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN()) *
                        baseHeight);
                dstOffset = dstOffset >> 1;
            }
        } else {
            dstOffset =
                static_cast<int64_t>(
                    static_cast<int64_t>(curRow * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM()) * stride) +
                static_cast<int64_t>(curCol * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN());
        }
        return dstOffset;
    }

    __aicore__ inline void SetFixpipeParams(FixpipeAdaptor& fixpipe)
    {
        if constexpr (PhyPosIsUB(C_TYPE::pos) && MatmulFeatureTrait<MM_CFG>::IsSupportL0CToUB()) {
            fixpipe.SetSubBlockId(MATMUL_MODULE(MatmulSubBlockInfo)->GetSubBlockIdx());
            if constexpr (A_TYPE::ibShare && B_TYPE::ibShare && FIXPIPE_MODE == McgShfMode::SINGLE_DST_MODE) {
                fixpipe.SetMcgShfMode(McgShfMode::DUAL_DST_SPLIT_M);
            } else {
                fixpipe.SetMcgShfMode(FIXPIPE_MODE);
            }
        }
#if __NPU_ARCH__ == 5102
        if constexpr (IsSameTypeV<typename A_TYPE::T, half> && IsSameTypeV<typename B_TYPE::T, half>) {
            uint8_t fixShiftValue = 58 - MATMUL_CONST_PARAM_VAR.fixShiftValue;
            fixpipe.SetFixShiftValue(fixShiftValue);
        }
#endif
    }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
    template <bool enSequentialWrite, class T, bool isIntraBlock = false>
    __aicore__ inline void CopyOutNZ2NZ(
        const T& dst, const LocalTensor<SrcT>& co1Local, int32_t curRow, int32_t curCol, int32_t baseHeight,
        int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth)
    {
        int64_t dstOffset = 0;
        uint32_t stride = 0;
        constexpr uint32_t DST_C0SIZE =
            (ToMatmulConfig(MM_CFG).isEnableChannelSplit) ?
                B32_C0SIZE :
                (IsSupportB4<DstT>() ? B4_C0SIZE :
                                       ((IsTypeOneOfV<DstT, uint8_t> || IsSupportB8<DstT>()) ? B8_C0SIZE : BLOCK_CUBE));
        if constexpr (!enSequentialWrite) {
            if constexpr (!ToMatmulConfig(MM_CFG).isEnableChannelSplit) {
                stride = static_cast<uint32_t>(GetOrgM<isIntraBlock>() * DST_C0SIZE);
                if constexpr (!IsBasic(MM_CFG)) {
                    dstOffset =
                        static_cast<int64_t>(
                            static_cast<int64_t>(curCol * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN()) *
                            GetOrgM<isIntraBlock>()) +
                        static_cast<int64_t>(curRow * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM()) *
                            DST_C0SIZE;
                }

                if constexpr (
                    PhyPosIsUB(C_TYPE::pos) &&
                    ((A_TYPE::ibShare && B_TYPE::ibShare) || FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_M ||
                     FIXPIPE_MODE == McgShfMode::DUAL_DST_SPLIT_N)) {
                    dstOffset = dstOffset >> 1;
                }
            } else {
                stride = static_cast<uint32_t>(CeilAlign(GetOrgM<isIntraBlock>(), BLOCK_CUBE) * DST_C0SIZE);
                if constexpr (!IsBasic(MM_CFG)) {
                    dstOffset =
                        static_cast<int64_t>(
                            static_cast<int64_t>(curCol * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN()) *
                            CeilAlign(GetOrgM<isIntraBlock>(), BLOCK_CUBE)) +
                        static_cast<int64_t>(curRow * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM()) *
                            DST_C0SIZE;
                }
            }
        } else {
            stride = static_cast<uint32_t>((baseBlockHeight * BLOCK_CUBE) * DST_C0SIZE);
        }
        if constexpr (ToMatmulConfig(MM_CFG).isEnableChannelSplit) {
            baseWidth = CeilAlign(baseWidth, DST_C0SIZE);
            MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA() ? baseHeight = CeilAlign(baseHeight, DST_C0SIZE) :
                                                             baseHeight = CeilAlign(baseHeight, BLOCK_CUBE);
        }
        if constexpr (ToMatmulConfig(MM_CFG).bmmOutMode != BatchOutMode::SINGLE_BATCH) {
            baseBlockWidth *= MATMUL_MODULE(BatchLoop)->GetBatchOutCacheNum();
            baseWidth *= MATMUL_MODULE(BatchLoop)->GetBatchOutCacheNum();
        }
        FixpipeAdaptor fixpipe(
            baseWidth, baseHeight, baseBlockWidth, baseBlockHeight,
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(), stride);
        SetFixpipeParams(fixpipe);
        CheckUnitFlagStatus(fixpipe);
        CopyTensor(dst[dstOffset], co1Local, fixpipe, curCol, baseWidth);
    }

    __aicore__ inline void CheckUnitFlagStatus(FixpipeAdaptor& fixpipe)
    {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
        if constexpr (HasScalePosition<A_TYPE>::value) {
            if constexpr (IsMxDisableUnitFlag<A_TYPE, B_TYPE, MM_CFG>) {
                fixpipe.params_.unitFlag = 0;
            }
        }
#endif
    }

    template <class T>
    __aicore__ inline void CopyTensor(
        const T& dst, const LocalTensor<SrcT>& co1Local, FixpipeAdaptor& fixpipe, const int32_t curN = 0,
        const int32_t baseUseN = 0)
    {
        if (MATMUL_MODULE(MatmulQuantProcessor)->IsQuantSenario()) {
            fixpipe.SetQuantMode(MATMUL_MODULE(MatmulQuantProcessor)->GetMatmulQuantMode());
            if (MATMUL_MODULE(MatmulQuantProcessor)->IsPerChannelSenario()) {
                LocalTensor<uint64_t> quantTensor;
                MATMUL_MODULE(MatmulQuantProcessor)->CopyQuantTensor(quantTensor, curN, baseUseN);
                fixpipe.template FixpipeOut<T>(dst, co1Local, quantTensor);
                MATMUL_MODULE(MatmulQuantProcessor)->FreeQuantTensor(quantTensor);
            } else {
                fixpipe.SetQuantScalar(MATMUL_MODULE(MatmulQuantProcessor)->GetQuantScalarValue());
                fixpipe.template FixpipeOut<T>(dst, co1Local);
            }
        } else {
            fixpipe.SetCastMode();
            fixpipe.template FixpipeOut<T>(dst, co1Local);
        }
    }

    template <bool isIntraBlock = false>
    __aicore__ inline uint32_t GetOrgHeight()
    {
        return GetOrgM<isIntraBlock>();
    }

    template <bool enSequentialWrite, class T, bool isIntraBlock = false>
    __aicore__ inline void CopyOutNZ2DN(
        const T& dst, const LocalTensor<SrcT>& co1Local, int32_t curRow, int32_t curCol, int32_t baseHeight,
        int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth)
    {
        ASCENDC_ASSERT(
            PhyPosIsGM(C_TYPE::pos), { KERNEL_LOG(KERNEL_ERROR, "Unsupported TPosition for output matrix."); });
        auto stride = baseHeight;
        int64_t dstOffset = 0;
        if constexpr (!enSequentialWrite) {
            stride = GetOrgHeight<isIntraBlock>();
            if constexpr (!IsBasic(MM_CFG)) {
                dstOffset =
                    static_cast<int64_t>(
                        static_cast<int64_t>(curCol * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN()) *
                        stride) +
                    static_cast<int64_t>(curRow * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM());
            }
        }
        FixpipeAdaptor fixpipe(
            baseWidth, baseHeight, baseBlockWidth, baseBlockHeight,
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(), stride);
        SetFixpipeParams(fixpipe);
        CopyTensor(dst[dstOffset], co1Local, fixpipe, curCol, baseWidth);
    }
#else
    template <bool enSequentialWrite, class T, bool isIntraBlock = false>
    __aicore__ inline void CopyOutNZ2NZ(
        const T& dst, const LocalTensor<SrcT>& co1Local, int32_t curRow, int32_t curCol, int32_t baseHeight,
        int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth)
    {
        int64_t dstOffset = 0;
        uint32_t stride;
        constexpr uint32_t DST_C0SIZE =
            (ToMatmulConfig(MM_CFG).isEnableChannelSplit) ?
                B32_C0SIZE :
                (IsSupportB4<DstT>() ? B4_C0SIZE :
                                       ((IsTypeOneOfV<DstT, uint8_t> || IsSupportB8<DstT>()) ? B8_C0SIZE : BLOCK_CUBE));
        if constexpr (!enSequentialWrite) {
            if constexpr (!ToMatmulConfig(MM_CFG).isEnableChannelSplit) {
                dstOffset =
                    static_cast<int64_t>(
                        static_cast<int64_t>(curCol * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN()) *
                        GetOrgM<isIntraBlock>()) +
                    static_cast<int64_t>(curRow * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM()) *
                        DST_C0SIZE;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
                stride = static_cast<uint32_t>(GetOrgM<isIntraBlock>() * DST_C0SIZE);
#else
                stride = static_cast<uint32_t>(GetOrgM<isIntraBlock>() * DST_C0SIZE * sizeof(DstT) / ONE_BLK_SIZE);
#endif
            } else {
                dstOffset =
                    static_cast<int64_t>(
                        static_cast<int64_t>(curCol * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN()) *
                        Ceil(GetOrgM<isIntraBlock>(), BLOCK_CUBE)) *
                        BLOCK_CUBE +
                    static_cast<int64_t>(curRow * MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM()) *
                        DST_C0SIZE;
                stride = static_cast<uint32_t>(
                    Ceil(GetOrgM<isIntraBlock>(), BLOCK_CUBE) * BLOCK_CUBE * DST_C0SIZE * sizeof(DstT) / ONE_BLK_SIZE);
            }
        } else {
            stride = static_cast<uint32_t>((baseBlockHeight * BLOCK_CUBE) * DST_C0SIZE * sizeof(DstT) / ONE_BLK_SIZE);
        }
        if constexpr (ToMatmulConfig(MM_CFG).isEnableChannelSplit) {
            baseWidth = CeilAlign(baseWidth, DST_C0SIZE);
            if (MATMUL_MODULE(MatmulShapeInfo)->IsTransposeA()) {
                baseHeight = CeilAlign(baseHeight, DST_C0SIZE);
            } else {
                baseHeight = CeilAlign(baseHeight, BLOCK_CUBE);
            }
        }
        if constexpr (ToMatmulConfig(MM_CFG).bmmOutMode != BatchOutMode::SINGLE_BATCH) {
            baseWidth *= MATMUL_MODULE(BatchLoop)->GetBatchOutCacheNum();
            baseBlockWidth *= MATMUL_MODULE(BatchLoop)->GetBatchOutCacheNum();
        }
        FixpipeAdaptor fixpipe(
            baseWidth, baseHeight, baseBlockWidth, baseBlockHeight,
            MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM(), stride);
        CopyTensor(dst[dstOffset], co1Local, fixpipe, curCol, baseWidth);
    }

    template <class T>
    __aicore__ inline void CopyTensor(
        const T& dst, const LocalTensor<SrcT>& co1Local, FixpipeAdaptor& fixpipe, const int32_t curN = 0,
        const int32_t baseUseN = 0)
    {
        fixpipe.SetCastMode();
        if constexpr (IsQuantSenario<DstT, typename A_TYPE::T>()) {
            fixpipe.SetQuantMode(MATMUL_MODULE(MatmulQuantProcessor)->GetMatmulQuantMode());
            LocalTensor<uint64_t> quantTensor;
            if (MATMUL_MODULE(MatmulQuantProcessor)->IsPerChannelSenario()) {
                MATMUL_MODULE(MatmulQuantProcessor)->CopyQuantTensor(quantTensor, curN, baseUseN);
                fixpipe.template FixpipeOut<T>(dst, co1Local, quantTensor);
                MATMUL_MODULE(MatmulQuantProcessor)->FreeQuantTensor(quantTensor);
            } else {
                fixpipe.SetQuantScalar(MATMUL_MODULE(MatmulQuantProcessor)->GetQuantScalarValue());
                fixpipe.template FixpipeOut<T>(dst, co1Local);
            }
        } else {
            fixpipe.template FixpipeOut<T>(dst, co1Local);
        }
    }
#endif
    template <bool isIntraBlock = false>
    __aicore__ inline uint32_t GetOrgWidth()
    {
        uint32_t dimN = MATMUL_MODULE(MatmulShapeInfo)->template GetOrgN<isIntraBlock>();
        if (GetOrgKc<isIntraBlock>() != 0) {
            dimN = GetOrgKc<isIntraBlock>();
        }
        if constexpr (C_TYPE::format == CubeFormat::ND_ALIGN) {
            constexpr uint32_t blockCount = ONE_BLK_SIZE / sizeof(DstT);
            dimN = Ceil(dimN, blockCount) * blockCount;
        }
        return dimN;
    }

    template <bool isIntraBlock = false>
    __aicore__ inline uint32_t GetDstBNGStride()
    {
        int32_t alignedSingleCoreN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetCLayoutInfoS2();
        if constexpr (C_TYPE::format == CubeFormat::ND_ALIGN) {
            constexpr uint32_t blockCount = ONE_BLK_SIZE / sizeof(DstT);

            alignedSingleCoreN = Ceil(alignedSingleCoreN, blockCount) * blockCount;
        }
        if constexpr (C_TYPE::layout == LayoutMode::SBNGD) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetCLayoutInfoG() * alignedSingleCoreN *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetCLayoutInfoN() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetCLayoutInfoB();
        } else if constexpr (C_TYPE::layout == LayoutMode::BSNGD) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetCLayoutInfoG() * alignedSingleCoreN *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetCLayoutInfoN();
        }
    }

    template <bool IS_INTRA_BLOCK = false>
    __aicore__ inline uint32_t GetDstStride()
    {
        uint32_t dimN;
        if constexpr (C_TYPE::layout != LayoutMode::SBNGD && C_TYPE::layout != LayoutMode::BSNGD) { // Scenario 1:
                                                                                                    // Continuous copy
                                                                                                    // on M/N axis
            dimN = GetOrgWidth();
        } else { // Scenario 2: disconsecutive copy on M/N axis for SBNGD/BSNGD
            dimN = GetDstBNGStride();
        }
        return dimN;
    }

    template <bool isIntraBlock = false>
    __aicore__ inline uint32_t GetOrgKc()
    {
        if constexpr ((C_TYPE::layout == LayoutMode::SBNGD) || (C_TYPE::layout == LayoutMode::BSNGD)) {
            return 0;
        } else {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgKc<isIntraBlock>();
        }
    }

    template <bool isIntraBlock = false>
    __aicore__ inline uint32_t GetOrgM()
    {
        if constexpr (C_TYPE::layout == LayoutMode::SBNGD) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetCLayoutInfoB() *
                   MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetCLayoutInfoS1();
        } else if constexpr (C_TYPE::layout == LayoutMode::BSNGD) {
            return MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetCLayoutInfoS1();
        } else if constexpr (
            ToMatmulConfig(MM_CFG).isEnableChannelSplit && A_TYPE::format == CubeFormat::ND &&
            C_TYPE::format == CubeFormat::NZ) {
            return Ceil(MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<isIntraBlock>(), BLOCK_CUBE) * BLOCK_CUBE;
        } else {
            return MATMUL_MODULE(MatmulShapeInfo)->template GetOrgM<isIntraBlock>();
        }
    }

    template <bool enSequentialWrite>
    __aicore__ inline void CopyOutImplCB(
        const GlobalTensor<DstT>& dst, const LocalTensor<SrcT>& co1Local, int32_t curRow, int32_t curCol,
        int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight, int32_t baseBlockWidth)
    {
        // Get stride of gm addr and basewith for DataCopyOutParams params
        uint32_t nSize;
        uint32_t dstStrideIn;
        int64_t dstOffset;
        GetCBCopyOutParams<enSequentialWrite>(
            curRow, curCol, baseHeight, baseWidth, baseBlockHeight, baseBlockWidth, nSize, dstStrideIn, dstOffset);

        // DataCopyOut params for callback function
        DataCopyOutParams param(
            baseBlockWidth, static_cast<uint16_t>(baseHeight * BLOCK_CUBE * sizeof(SrcT) / ONE_BLK_SIZE), 0,
            dstStrideIn, nSize, EnUnitFlag(MM_CFG), curRow, curCol);

        // Update quant params
        if constexpr (IsQuantSenario<DstT, typename A_TYPE::T>()) {
            param.quantMode = MATMUL_MODULE(MatmulQuantProcessor)->GetMatmulQuantMode();
            if (MATMUL_MODULE(MatmulQuantProcessor)->IsPerChannelSenario()) {
                LocalTensor<uint64_t> l1TmpForQuant;
                MATMUL_MODULE(MatmulQuantProcessor)->CopyQuantTensor(l1TmpForQuant, curCol, baseWidth);
                param.cbufWorkspaceAddr = reinterpret_cast<uint64_t>(l1TmpForQuant.GetPhyAddr());
                MATMUL_MODULE(MatmulQuantProcessor)->FreeQuantTensor(l1TmpForQuant);
            } else {
                param.quantScalar = MATMUL_MODULE(MatmulQuantProcessor)->GetQuantScalarValue();
            }
        }

        // CallBack with user define
        LocalTensor<int8_t> co1LocalInt8 = co1Local.template ReinterpretCast<int8_t>();
        (IMPL::CallBack::DataCopyOutPtr)(
            reinterpret_cast<__gm__ void*>(dst[dstOffset].address_), co1LocalInt8, reinterpret_cast<void*>(&param),
            MATMUL_MODULE(MatmulUserDefineInfo)->GetUserDefineInfo(),
            MATMUL_MODULE(MatmulUserDefineInfo)->GetSelfDefineData());
    }

    template <bool enSequentialWrite>
    __aicore__ inline void GetCBCopyOutParams(
        int32_t curRow, int32_t curCol, int32_t baseHeight, int32_t baseWidth, int32_t baseBlockHeight,
        int32_t baseBlockWidth, uint32_t& nSize, uint32_t& dstStrideIn, int64_t& dstOffset)
    {
        if constexpr (enSequentialWrite) {
            dstOffset = 0;
            if constexpr (C_TYPE::format == CubeFormat::ND || C_TYPE::format == CubeFormat::ND_ALIGN) {
                dstStrideIn = baseWidth;
                nSize = static_cast<uint16_t>(baseWidth);
            } else if constexpr (C_TYPE::format == CubeFormat::NZ) {
                dstStrideIn =
                    static_cast<uint32_t>(baseBlockHeight * BLOCK_CUBE * BLOCK_CUBE * sizeof(DstT) / ONE_BLK_SIZE);
                nSize = 0;
            }
        } else {
            auto baseM = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseM();
            auto baseN = MATMUL_MODULE(MatmulShapeTiling)->GetTiling().GetBaseN();
            if constexpr (C_TYPE::format == CubeFormat::ND || C_TYPE::format == CubeFormat::ND_ALIGN) {
                dstStrideIn = GetDstStride();
                nSize = static_cast<uint16_t>(baseWidth);
                dstOffset = static_cast<int64_t>(static_cast<int64_t>(curRow * baseM) * dstStrideIn) +
                            static_cast<int64_t>(curCol * baseN);
            } else if constexpr (C_TYPE::format == CubeFormat::NZ) {
                dstStrideIn = static_cast<uint32_t>(
                    (MATMUL_MODULE(MatmulShapeInfo)->GetOrgM()) * BLOCK_CUBE * sizeof(DstT) / ONE_BLK_SIZE);
                nSize = 0;
                dstOffset = static_cast<int64_t>(
                                static_cast<int64_t>(curCol * baseN) * MATMUL_MODULE(MatmulShapeInfo)->GetOrgM()) +
                            static_cast<int64_t>(curRow * baseM * BLOCK_CUBE);
            }
        }
    }
};
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_FIXPIPE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_FIXPIPE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_OUT_COPY_CUBE_OUT_FIXPIPE_H__
#endif
