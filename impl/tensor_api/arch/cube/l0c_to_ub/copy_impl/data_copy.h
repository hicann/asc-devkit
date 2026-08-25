/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/arch/cube/l0c_to_ub/copy_impl/data_copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file data_copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_DATA_COPY_H
#define IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_DATA_COPY_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/arch/cube/l1_to_fb/copy.h"
#include "impl/tensor_api/arch/cube/utils/l0c2out_utils.h"
#include "impl/tensor_api/arch/cube/l0c_to_ub/copy_impl/instruction.h"

namespace AscendC {
namespace Te {

struct CopyL0C2UBTrait {
    __aicore__ constexpr CopyL0C2UBTrait() {}

    __aicore__ constexpr CopyL0C2UBTrait(
        RoundMode roundModeIn, bool enableReluIn, bool enableChannelSplitIn, DualDstMode dualDstCtlIn)
        : roundMode(roundModeIn),
          enableRelu(enableReluIn),
          enableChannelSplit(enableChannelSplitIn),
          dualDstCtl(dualDstCtlIn)
    {}

    RoundMode roundMode = RoundMode::DEFAULT;
    bool enableRelu = false;
    bool enableChannelSplit = false;
    DualDstMode dualDstCtl = DUAL_DST_DISABLE;
};

class DataCopyL0C2UBNoVectorQuant {
public:
    template <const CopyL0C2UBTrait& trait, QuantMode_t quantPre, typename T, typename U>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src, const FixpipeParams& params)
    {
        if constexpr (IsL0COutSrcBatchLayoutV<U> && !IsL0COutBatchNZ2NZV<T, U>) {
            EmitDataCopy<trait, quantPre>(dst, src, Get<1>(dst.Layout()), Get<1>(src.Layout()), params);
        } else {
            EmitDataCopy<trait, quantPre>(dst, src, dst.Layout(), src.Layout(), params);
        }
    }

private:
    template <
        const CopyL0C2UBTrait& trait, QuantMode_t quantPre, typename T, typename U, typename DstLayout,
        typename SrcLayout>
    __aicore__ inline static void EmitDataCopy(
        const T& dst, const U& src, const DstLayout& dstLayout, const SrcLayout& srcLayout, const FixpipeParams& params)
    {
        constexpr bool isNdFormat = IsL0COutNDFormatV<T>;
        constexpr bool isDnFormat = IsL0COutDNFormatV<T>;
        auto copyParams = MakeL0COutCopyParams<T, U>(dstLayout, srcLayout);

        bool reluEn = trait.enableRelu;
        uint8_t unitFlag = params.unitFlag;
        bool nz2ndEn = isNdFormat;
        bool nz2dnEn = isDnFormat;

        bool isChannelSplit = trait.enableChannelSplit;

        uint8_t dualDstCtl = trait.dualDstCtl;
        bool subBlockId = params.subBlockId;
        CopyMatrixCcToUbInstr::DataCopy<quantPre, T, U>(
            dst, src, copyParams.nSize, copyParams.mSize, copyParams.srcStride, copyParams.dstStride, dualDstCtl,
            reluEn, unitFlag, subBlockId, isChannelSplit, nz2ndEn, nz2dnEn);
    }
};

class DataCopyL0C2UBVectorQuant {
public:
    template <const CopyL0C2UBTrait& trait, QuantMode_t quantPre, typename T, typename U, typename V>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src, const V& quant, const FixpipeParams& params)
    {
        if constexpr (IsL0COutBatchNZ2NZV<T, U>) {
            EmitBatchNZ2NZDataCopy<trait, quantPre>(dst, src, quant, params);
        } else if constexpr (IsL0COutSrcBatchLayoutV<U>) {
            EmitDataCopy<trait, quantPre>(dst, src, quant, Get<1>(dst.Layout()), Get<1>(src.Layout()), params);
        } else {
            EmitDataCopy<trait, quantPre>(dst, src, quant, dst.Layout(), src.Layout(), params);
        }
    }

private:
    template <const CopyL0C2UBTrait& trait, QuantMode_t quantPre, typename T, typename U, typename V>
    __aicore__ inline static void EmitBatchNZ2NZDataCopy(
        const T& dst, const U& src, const V& quant, const FixpipeParams& params)
    {
        auto dstLayout = dst.Layout();
        auto srcLayout = src.Layout();
        for (uint32_t batchIndex = 0; batchIndex < Get<0>(srcLayout.Shape()); ++batchIndex) {
            EmitDataCopy<trait, quantPre>(
                dst(MakeCoord(batchIndex, MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0)))),
                src(MakeCoord(batchIndex, MakeCoord(MakeCoord(0, 0), MakeCoord(0, 0)))), quant, Get<1>(dstLayout),
                Get<1>(srcLayout), params);
        }
    }

    template <
        const CopyL0C2UBTrait& trait, QuantMode_t quantPre, typename T, typename U, typename V, typename DstLayout,
        typename SrcLayout>
    __aicore__ inline static void EmitDataCopy(
        const T& dst, const U& src, const V& quant, const DstLayout& dstLayout, const SrcLayout& srcLayout,
        const FixpipeParams& params)
    {
        uint32_t nSize = Std::min(GetTotalColumnShape(srcLayout), GetTotalColumnShape(dstLayout));
        uint16_t nIterNum = 1;
        uint32_t calNSize = nSize;
        uint32_t tailNSize = 0;
        if (calNSize > MAIN_LOOP_N_SIZE) {
            nIterNum = nSize / MAIN_LOOP_N_SIZE;
            tailNSize = nSize % MAIN_LOOP_N_SIZE;
            calNSize = MAIN_LOOP_N_SIZE;
        }
        ExecuteDataCopy<trait, quantPre>(dst, src, quant, nIterNum, calNSize, tailNSize, dstLayout, srcLayout, params);
    }

    template <const CopyL0C2UBTrait& trait, typename T, bool IsTail, typename DstLayout, typename SrcLayout>
    __aicore__ inline static auto GenerateParams(
        const DstLayout& dstLayout, const SrcLayout& srcLayout, const FixpipeParams& params)
    {
        constexpr bool isNdFormat = IsL0COutNDFormatV<T>;
        constexpr bool isDnFormat = IsL0COutDNFormatV<T>;

        uint32_t nSize = Std::min(GetTotalColumnShape(srcLayout), GetTotalColumnShape(dstLayout));
        uint32_t mSize = Std::min(GetTotalRowShape(srcLayout), GetTotalRowShape(dstLayout));
        if constexpr (IsTail) {
            nSize %= MAIN_LOOP_N_SIZE;
        } else {
            if (nSize > MAIN_LOOP_N_SIZE) {
                nSize = MAIN_LOOP_N_SIZE;
            }
        }

        const uint32_t srcStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(srcLayout) / FRACTAL_FIXED;
        uint32_t dstStride = 0;
        if constexpr (isNdFormat) {
            dstStride = GetL0COutNDStride<T>(dstLayout);
        } else if constexpr (isDnFormat) {
            dstStride = GetL0COutDNStride<T>(dstLayout);
        } else {
            dstStride = GetElement<AttrInfo::Stride, AttrInfo::Column, 1>(dstLayout);
        }

        const bool reluEn = trait.enableRelu;
        const uint8_t unitFlag = params.unitFlag;

        constexpr bool nz2ndEn = isNdFormat;
        constexpr bool nz2dnEn = isDnFormat;

        const bool channelSplit = trait.enableChannelSplit;
        bool subBlockId = params.subBlockId;
        uint8_t dualDstCtl = trait.dualDstCtl;

        return Std::make_tuple(
            nSize, mSize, srcStride, dstStride, dualDstCtl, reluEn, unitFlag, subBlockId, channelSplit, nz2ndEn,
            nz2dnEn);
    }

    template <typename T>
    __aicore__ inline static void CopyL12Fb(const T& src, uint16_t calNSize, uint16_t nIterIndex)
    {
        auto dstAddr = reinterpret_cast<__fbuf__ uint64_t*>(AllocFbTempBuf(calNSize));
        auto dst = MakeTensor(MakeMemPtr<Location::FIXBUF>(dstAddr), src.Layout());
        auto coord = MakeCoord(_0{}, nIterIndex * MAIN_LOOP_N_SIZE);
        auto shape = MakeShape(_1{}, calNSize);
        auto tileSrc = src.Slice(coord, shape);
        CopyL12FBND::Run<DEFAULT_COPY_L1_FB_TRAIT>(dst, tileSrc);
        SetFpc(dstAddr);
    }

    template <QuantMode_t quantPre, typename T, typename U, typename ParamTuple, size_t... Is>
    __aicore__ inline static void DataCopyWrapper(
        const T& dst, const U& src, const ParamTuple& paramTuple, Std::index_sequence<Is...>)
    {
        CopyMatrixCcToUbInstr::DataCopy<quantPre>(dst, src, Std::get<Is>(paramTuple)...);
    }

    template <typename T>
    __aicore__ inline static constexpr auto MakeDstCoord(uint32_t nOffset)
    {
        using LayoutType = typename T::layoutType;
        if constexpr (LayoutType::depth == FIVE_DIM_DATA) {
            return MakeCoord(_0{}, MakeCoord(MakeCoord(0, 0), MakeCoord(0, nOffset)));
        } else if constexpr (LayoutType::depth == FOUR_DIM_DATA) {
            return MakeCoord(MakeCoord(0, 0), MakeCoord(0, nOffset));
        } else if constexpr (LayoutType::depth == THREE_DIM_DATA) {
            return MakeCoord(_0{}, MakeCoord(0, nOffset));
        } else {
            static_assert(LayoutType::depth == TWO_DIM_DATA, "Only support two-dim or four-dim dst tensor.");
            return MakeCoord(0, nOffset);
        }
    }

    template <typename U>
    __aicore__ inline static constexpr auto MakeSrcCoord(uint32_t nOffset)
    {
        using LayoutType = typename U::layoutType;
        if constexpr (LayoutType::depth == FIVE_DIM_DATA) {
            return MakeCoord(_0{}, MakeCoord(MakeCoord(0, 0), MakeCoord(0, nOffset)));
        } else {
            static_assert(LayoutType::depth == FOUR_DIM_DATA, "Only support four-dim or five-dim src tensor.");
            return MakeCoord(MakeCoord(0, 0), MakeCoord(0, nOffset));
        }
    }

    template <
        const CopyL0C2UBTrait& trait, QuantMode_t quantPre, typename T, typename U, typename V, typename DstLayout,
        typename SrcLayout>
    __aicore__ inline static void ExecuteDataCopy(
        const T& dst, const U& src, const V& quant, uint16_t nIterNum, uint32_t calNSize, uint32_t tailNSize,
        const DstLayout& dstLayout, const SrcLayout& srcLayout, const FixpipeParams& params)
    {
        const auto mainLoopParam = GenerateParams<trait, T, false>(dstLayout, srcLayout, params);

        for (uint16_t i = 0; i < nIterNum; ++i) {
            CopyL12Fb(quant, calNSize, i);
            InsertSync();

            const auto srcCoord = MakeSrcCoord<U>(i * CBURST_NUM);
            const auto dstCoord = MakeDstCoord<T>(i * MAIN_LOOP_N_SIZE);

            DataCopyWrapper<quantPre>(
                dst(dstCoord), src(srcCoord), mainLoopParam,
                Std::make_index_sequence<Std::tuple_size_v<decltype(mainLoopParam)>>{});
        }

        if (tailNSize) {
            const auto tailParam = GenerateParams<trait, T, true>(dstLayout, srcLayout, params);

            CopyL12Fb(quant, tailNSize, nIterNum);
            InsertSync();

            const auto srcCoord = MakeSrcCoord<U>(nIterNum * CBURST_NUM);
            const auto dstCoord = MakeDstCoord<T>(nIterNum * MAIN_LOOP_N_SIZE);

            DataCopyWrapper<quantPre>(
                dst(dstCoord), src(srcCoord), tailParam,
                Std::make_index_sequence<Std::tuple_size_v<decltype(tailParam)>>{});
        }
    }
};

class DataCopyL0C2UB {
public:
    template <const CopyL0C2UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src, const FixpipeParams& params)
    {
        constexpr QuantMode_t quantPre = GetQuantMode<trait.roundMode, T, U>();
        CheckDataType::CheckL0C2UbDataType<quantPre, T, U>();
        SetRegisterImpl<T, U>(dst, src);
        DataCopyL0C2UBNoVectorQuant::DataCopyImpl<trait, quantPre, T, U>(dst, src, params);
    }

    template <const CopyL0C2UBTrait& trait, typename T, typename U>
    __aicore__ inline static void Run(const T& dst, const U& src, uint64_t quant, const FixpipeParams& params)
    {
        constexpr QuantMode_t quantPre = GetQuantMode<trait.roundMode, T, U, uint64_t>();
        SetRegisterImpl<T, U>(dst, src, quant);
        DataCopyL0C2UBNoVectorQuant::DataCopyImpl<trait, quantPre, T, U>(dst, src, params);
    }

    template <const CopyL0C2UBTrait& trait, typename T, typename U, typename V>
    __aicore__ inline static void Run(const T& dst, const U& src, const V& quant, const FixpipeParams& params)
    {
        constexpr QuantMode_t quantPre = GetQuantMode<trait.roundMode, T, U, V>();
        constexpr bool quantBatched = CheckVectorQuantBatchConsistency<T, U, V>();

        if constexpr (quantBatched) {
            // Fixpipe hardware reads a single quant slice from FB per instruction and reuses it
            // across the ndNum batch repetition; the FB quant address does not auto-step per
            // batch. To honor per-batch quant we must split on the host: each batch issues its
            // own fixpipe instruction with its own L1->FB quant copy.
            uint32_t batchSize = Get<0>(src.Layout().Shape());
            for (uint32_t b = 0; b < batchSize; ++b) {
                auto subDst = MakeSingleBatchSubTensor(dst, b);
                auto subSrc = MakeSingleBatchSubTensor(src, b);
                auto subQuant = MakeSingleBatchSubTensor(quant, b);
                SetRegisterImpl<decltype(subDst), decltype(subSrc)>(subDst, subSrc);
                DataCopyL0C2UBVectorQuant::DataCopyImpl<
                    trait, quantPre, decltype(subDst), decltype(subSrc), decltype(subQuant)>(
                    subDst, subSrc, subQuant, params);
            }
        } else {
            SetRegisterImpl<T, U>(dst, src);
            DataCopyL0C2UBVectorQuant::DataCopyImpl<trait, quantPre, T, U, V>(dst, src, quant, params);
        }
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_DATA_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
